#include <cassert>
#include <cstdint>
#include <cstring>
#include <cstdio>

#include "ButtonGesture.h"
#include "P4App.h"

constexpr int width_px = 294;
constexpr int height_px = 126;
static lv_color_t frame[width_px * height_px];
static lv_color_t dialplate_frame[width_px * height_px];
static lv_color_t draw_pixels[width_px * 20];
static unsigned flushes;

static bool has_label(lv_obj_t* root, const char* text) {
    if (lv_obj_check_type(root, &lv_label_class) && std::strstr(lv_label_get_text(root), text))
        return true;
    for (uint32_t i = 0; i < lv_obj_get_child_cnt(root); ++i)
        if (has_label(lv_obj_get_child(root, i), text))
            return true;
    return false;
}

static void flush(lv_disp_drv_t* driver, const lv_area_t* area, lv_color_t* pixels) {
    assert(area->x1 >= 0 && area->y1 >= 0 && area->x2 < width_px && area->y2 < height_px);
    const int width = area->x2 - area->x1 + 1;
    for (int y = area->y1; y <= area->y2; ++y)
        std::memcpy(&frame[y * width_px + area->x1], &pixels[(y - area->y1) * width], width * sizeof(lv_color_t));
    ++flushes;
    lv_disp_flush_ready(driver);
}

int main(int argc, char** argv) {
    if (argc > 3)
        return 2;
    ButtonGesture gesture;
    using Action = ButtonGesture::Action;
    assert(gesture.Sample(true, 0) == Action::None);
    assert(gesture.Sample(false, 5) == Action::None); // bounce
    assert(gesture.Sample(true, 10) == Action::None);
    assert(gesture.Sample(true, 35) == Action::None);
    assert(gesture.Sample(false, 60) == Action::None);
    assert(gesture.Sample(false, 85) == Action::None);
    assert(gesture.Sample(false, 336) == Action::Confirm);
    assert(gesture.Sample(true, 400) == Action::None);
    assert(gesture.Sample(true, 425) == Action::None);
    assert(gesture.Sample(false, 445) == Action::None);
    assert(gesture.Sample(false, 470) == Action::None);
    assert(gesture.Sample(true, 500) == Action::None);
    assert(gesture.Sample(true, 525) == Action::None);
    assert(gesture.Sample(false, 550) == Action::None);
    assert(gesture.Sample(false, 575) == Action::NextFocus);
    assert(gesture.Sample(false, 900) == Action::None);

    lv_init();
    lv_disp_draw_buf_t draw;
    lv_disp_draw_buf_init(&draw, draw_pixels, nullptr, width_px * 20);
    lv_disp_drv_t driver;
    lv_disp_drv_init(&driver);
    driver.hor_res = width_px;
    driver.ver_res = height_px;
    driver.draw_buf = &draw;
    driver.flush_cb = flush;
    lv_disp_t* display = lv_disp_drv_register(&driver);
    assert(display);
    {
        P4App app;
        assert(app.Init());
        assert(std::strcmp(app.CurrentPage(), "Pages/Dialplate") == 0);
        assert(app.ShowPage("Pages/Dialplate"));
        assert(app.Focused());
        lv_tick_inc(1500);
        lv_timer_handler();
        if (argc == 3) {
            assert(app.ShowPage(argv[1]));
            lv_tick_inc(500);
            lv_timer_handler();
        }
        lv_refr_now(display);
        assert(flushes);
        unsigned lit = 0;
        for (const auto& pixel : frame)
            lit += LV_COLOR_GET_R(pixel) || LV_COLOR_GET_G(pixel) || LV_COLOR_GET_B(pixel);
        assert(lit > 100);
        if (argc != 3)
            std::memcpy(dialplate_frame, frame, sizeof(frame));
        if (argc >= 2) {
            FILE* image = std::fopen(argv[argc - 1], "wb");
            assert(image);
            assert(std::fprintf(image, "P6\n%d %d\n255\n", width_px, height_px) > 0);
            for (const lv_color_t pixel : frame) {
                const std::uint8_t rgb[] = {(std::uint8_t)(LV_COLOR_GET_R(pixel) * 255 / 31),
                                            (std::uint8_t)(LV_COLOR_GET_G(pixel) * 255 / 63),
                                            (std::uint8_t)(LV_COLOR_GET_B(pixel) * 255 / 31)};
                assert(std::fwrite(rgb, 1, sizeof(rgb), image) == sizeof(rgb));
            }
            assert(std::fclose(image) == 0);
        }
        if (argc != 3) {
            const struct { const char* name; const char* label; } pages[] = {
                {"Pages/WorkSettings", "TRIMTALK"},
                {"Pages/RecordConfig", "infinite"},
                {"Pages/SystemInfos", "Firmware"},
                {"Pages/Shutdown", "PRESS"},
                {"Pages/StarMap", "STAR MAP"},
                {"Pages/Startup", "N/A"},
                {"Pages/HardwareCheck", "0%"},
                {"Pages/SaveConfig", "0%"},
            };
            for (const auto& page : pages) {
                assert(app.ShowPage(page.name));
                assert(std::strcmp(app.CurrentPage(), page.name) == 0);
                assert(has_label(lv_scr_act(), page.label));
                lv_tick_inc(500);
                lv_timer_handler();
                lv_refr_now(display);
                assert(std::memcmp(frame, dialplate_frame, sizeof(frame)) != 0);
                assert(app.Back());
                lv_tick_inc(50);
                lv_timer_handler();
                assert(std::strcmp(app.CurrentPage(), "Pages/Dialplate") == 0);
                assert(app.Focused());
            }
        }
    }
    lv_disp_remove(display);
}
