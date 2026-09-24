#define _POSIX_C_SOURCE 200809L

#include "ButtonGesture.h"
#include "P4App.h"
extern "C" {
#include "linux_button.h"
#include "linux_display.h"
#include "lv_display.h"
}

#include <cerrno>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unistd.h>

static volatile sig_atomic_t stop;
static void on_signal(int) { stop = 1; }

static bool number(const char* text, unsigned long max, unsigned long& value) {
    char* end;
    errno = 0;
    value = std::strtoul(text, &end, 10);
    return !errno && text[0] >= '0' && text[0] <= '9' && !*end && value && value <= max;
}

static std::uint64_t milliseconds() {
    timespec now{};
    if (clock_gettime(CLOCK_MONOTONIC, &now) < 0)
        return 0;
    return std::uint64_t(now.tv_sec) * 1000 + std::uint64_t(now.tv_nsec) / 1000000;
}

int main(int argc, char** argv) {
    const bool display_only = argc > 1 && std::strcmp(argv[1], "--display-only") == 0;
    const char* page = display_only && argc > 3 && std::strcmp(argv[2], "--page") == 0 ? argv[3] : nullptr;
    const int first = page ? 4 : display_only ? 2 : 1;
    unsigned long speed, chunk = 4096;
    if ((argc != first + 3 && argc != first + 4) || !number(argv[first + 2], UINT32_MAX, speed)
        || (argc == first + 4 && !number(argv[first + 3], 32768, chunk)) || chunk < 4 || chunk % 2) {
        std::fprintf(stderr, "Usage: %s [--display-only [--page Pages/NAME]] SPI_DEVICE GPIO1_CHIP VERIFIED_SPEED_HZ [EVEN_CHUNK_BYTES]\n", argv[0]);
        return EXIT_FAILURE;
    }
    struct sigaction action{};
    action.sa_handler = on_signal;
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGINT, &action, nullptr) < 0 || sigaction(SIGTERM, &action, nullptr) < 0) {
        std::perror("sigaction");
        return EXIT_FAILURE;
    }
    // 先申请按键，避免屏初始化后才发现 GPIO 被其他设备占用。
    int button_fd = display_only ? -1 : linux_button_open(argv[first + 1]);
    if (!display_only && button_fd < 0) {
        std::perror("button GPIO1_B4 (offset 12, pull-up)");
        return EXIT_FAILURE;
    }
    linux_display io;
    rm690a0 screen;
    if (linux_display_open(&io, &screen, argv[first], argv[first + 1], speed, chunk) < 0) {
        std::perror("display open");
        if (button_fd >= 0)
            close(button_fd);
        return EXIT_FAILURE;
    }
    int result = EXIT_FAILURE;
    if (rm_init(&screen) < 0) {
        std::perror("RM690A0 init");
    } else if (!stop) {
        lv_init();
        lv_display port{};
        if (lv_display_register(&port, &screen) < 0) {
            std::perror("LVGL display");
        } else {
            {
                P4App app;
                if (!app.Init()) {
                    std::fprintf(stderr, "P4 app init failed\n");
                } else {
                    if (page) {
                        lv_tick_inc(50);
                        lv_timer_handler();
                    }
                    if (page && !app.ShowPage(page)) {
                        std::fprintf(stderr, "Unknown or unavailable page: %s\n", page);
                    } else {
                        ButtonGesture gesture;
                        int last_pressed = -1;
                        std::uint64_t last = milliseconds();
                        while (!stop && !port.error) {
                            std::uint64_t now = milliseconds();
                            if (!now || now < last) {
                                std::fprintf(stderr, "monotonic clock failed\n");
                                break;
                            }
                            lv_tick_inc(std::uint32_t(now - last));
                            last = now;
                            if (button_fd >= 0) {
                                int pressed = linux_button_pressed(button_fd);
                                if (pressed < 0) {
                                    std::perror("button read");
                                    break;
                                }
                                if (pressed != last_pressed) {
                                    std::fprintf(stderr, "button raw %s\n", pressed ? "pressed" : "released");
                                    last_pressed = pressed;
                                }
                                const auto button_action = gesture.Sample(pressed != 0, now);
                                if (button_action != ButtonGesture::Action::None) {
                                    app.OnButton(button_action);
                                    std::fprintf(stderr, "button %s -> %s\n",
                                                 button_action == ButtonGesture::Action::Confirm ? "confirm" : "next-focus",
                                                 app.CurrentPage());
                                }
                            }
                            lv_timer_handler();
                            timespec wait{0, 5000000};
                            if (nanosleep(&wait, nullptr) < 0 && errno != EINTR) {
                                std::perror("nanosleep");
                                break;
                            }
                        }
                        if (stop && !port.error)
                            result = EXIT_SUCCESS;
                    }
                }
            }
            if (port.error) {
                errno = port.error;
                std::perror("display flush");
            }
            lv_disp_remove(port.display);
        }
    }
    if (linux_display_close(&io) < 0) {
        std::perror("display close");
        result = EXIT_FAILURE;
    }
    if (button_fd >= 0 && close(button_fd) < 0) {
        std::perror("button close");
        result = EXIT_FAILURE;
    }
    return result;
}
