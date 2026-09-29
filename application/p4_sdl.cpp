#define SDL_MAIN_HANDLED
#include "App/P4App.h"

#include <SDL2/SDL.h>
#include <array>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

namespace {
constexpr int width = 294;
constexpr int height = 126;
volatile sig_atomic_t stop = 0;

struct Frame {
    std::array<std::uint32_t, width * height> pixels{};
    bool dirty = false;
};

struct Pointer {
    lv_point_t point{};
    lv_indev_state_t state = LV_INDEV_STATE_RELEASED;
};

void pointer_read(lv_indev_drv_t* driver, lv_indev_data_t* data) {
    auto* pointer = static_cast<Pointer*>(driver->user_data);
    data->point = pointer->point;
    data->state = pointer->state;
}

void on_signal(int) {
    stop = 1;
}

void flush(lv_disp_drv_t* driver, const lv_area_t* area, lv_color_t* colors) {
    auto* frame = static_cast<Frame*>(driver->user_data);
    for (int y = area->y1; y <= area->y2; ++y) {
        for (int x = area->x1; x <= area->x2; ++x) {
            const lv_color_t color = *colors++;
            frame->pixels[y * width + x] = 0xff000000u | (LV_COLOR_GET_R(color) * 255u / 31u << 16)
                                           | (LV_COLOR_GET_G(color) * 255u / 63u << 8)
                                           | (LV_COLOR_GET_B(color) * 255u / 31u);
        }
    }
    frame->dirty = true;
    lv_disp_flush_ready(driver);
}
} // namespace

int main() {
    if (std::signal(SIGINT, on_signal) == SIG_ERR || std::signal(SIGTERM, on_signal) == SIG_ERR) {
        std::perror("signal");
        return EXIT_FAILURE;
    }
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "SDL init: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_Window* window =
        SDL_CreateWindow("P4 preview", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = window ? SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE) : nullptr;
    SDL_Texture* texture =
        renderer ? SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height)
                 : nullptr;
    int result = EXIT_FAILURE;
    if (texture) {
        lv_init();
        static Frame frame;
        static lv_color_t draw_pixels[width * 20];
        lv_disp_draw_buf_t draw;
        lv_disp_draw_buf_init(&draw, draw_pixels, nullptr, width * 20);
        lv_disp_drv_t driver;
        lv_disp_drv_init(&driver);
        driver.hor_res = width;
        driver.ver_res = height;
        driver.draw_buf = &draw;
        driver.flush_cb = flush;
        driver.user_data = &frame;
        lv_disp_t* display = lv_disp_drv_register(&driver);
        if (display) {
            Pointer pointer;
            lv_indev_drv_t pointer_driver;
            lv_indev_drv_init(&pointer_driver);
            pointer_driver.type = LV_INDEV_TYPE_POINTER;
            pointer_driver.read_cb = pointer_read;
            pointer_driver.user_data = &pointer;
            lv_indev_t* indev = lv_indev_drv_register(&pointer_driver);
            if (indev) {
                P4App app;
                if (app.Init()) {
                    lv_tick_inc(50);
                    lv_timer_handler();
                    const char* shown_page = app.CurrentPage();
                    SDL_SetWindowTitle(window, shown_page);
                    std::puts("Hold left click/Enter 2s to start or shut down; wheel/arrows: focus, "
                              "middle click: confirm, right click/Esc: back, Q: quit");
                    std::uint32_t last = SDL_GetTicks();
                    result = EXIT_SUCCESS;
                    bool running = true;
                    while (running && !stop) {
                        SDL_Event event;
                        while (SDL_PollEvent(&event)) {
                            if (event.type == SDL_QUIT) {
                                running = false;
                            } else if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN
                                       || event.type == SDL_MOUSEBUTTONUP) {
                                int x = event.type == SDL_MOUSEMOTION ? event.motion.x : event.button.x;
                                int y = event.type == SDL_MOUSEMOTION ? event.motion.y : event.button.y;
                                pointer.point = {static_cast<lv_coord_t>(x), static_cast<lv_coord_t>(y)};
                                if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT)
                                    app.OnInput(P4App::InputAction::Back);
                                if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_MIDDLE)
                                    app.OnInput(P4App::InputAction::Confirm);
                                if (event.type != SDL_MOUSEMOTION && event.button.button == SDL_BUTTON_LEFT) {
                                    pointer.state = event.type == SDL_MOUSEBUTTONDOWN ? LV_INDEV_STATE_PRESSED
                                                                                      : LV_INDEV_STATE_RELEASED;
                                    lv_timer_ready(indev->driver->read_timer);
                                    lv_timer_handler();
                                }
                            } else if (event.type == SDL_MOUSEWHEEL && event.wheel.y != 0) {
                                app.OnInput(event.wheel.y > 0 ? P4App::InputAction::PreviousFocus
                                                              : P4App::InputAction::NextFocus);
                            } else if (event.type == SDL_KEYDOWN && !event.key.repeat) {
                                const SDL_Keycode key = event.key.keysym.sym;
                                if (key == SDLK_q)
                                    running = false;
                                else if (key == SDLK_RIGHT || key == SDLK_DOWN)
                                    app.OnInput(P4App::InputAction::NextFocus);
                                else if (key == SDLK_LEFT || key == SDLK_UP)
                                    app.OnInput(P4App::InputAction::PreviousFocus);
                                else if (key == SDLK_RETURN || key == SDLK_KP_ENTER)
                                    app.OnInput(P4App::InputAction::Press);
                                else if (key == SDLK_ESCAPE)
                                    app.OnInput(P4App::InputAction::Back);
                            } else if (event.type == SDL_KEYUP
                                       && (event.key.keysym.sym == SDLK_RETURN
                                           || event.key.keysym.sym == SDLK_KP_ENTER)) {
                                app.OnInput(P4App::InputAction::Release);
                            }
                        }
                        const std::uint32_t now = SDL_GetTicks();
                        lv_tick_inc(now - last);
                        last = now;
                        lv_timer_handler();
                        if (const char* page = app.CurrentPage(); page != shown_page) {
                            shown_page = page;
                            SDL_SetWindowTitle(window, page);
                        }
                        if (frame.dirty) {
                            if (SDL_UpdateTexture(texture, nullptr, frame.pixels.data(), width * 4) != 0
                                || SDL_RenderClear(renderer) != 0
                                || SDL_RenderCopy(renderer, texture, nullptr, nullptr) != 0) {
                                std::fprintf(stderr, "SDL render: %s\n", SDL_GetError());
                                result = EXIT_FAILURE;
                                break;
                            }
                            SDL_RenderPresent(renderer);
                            frame.dirty = false;
                        }
                        SDL_Delay(5);
                    }
                } else {
                    std::fprintf(stderr, "P4 app init failed\n");
                }
                lv_indev_delete(indev);
            } else {
                std::fprintf(stderr, "LVGL pointer init failed\n");
            }
            lv_disp_remove(display);
        }
    } else {
        std::fprintf(stderr, "SDL window: %s\n", SDL_GetError());
    }
    if (texture)
        SDL_DestroyTexture(texture);
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
    return result;
}
