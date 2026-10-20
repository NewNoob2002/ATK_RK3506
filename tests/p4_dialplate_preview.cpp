#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "Pages/Dialplate/DialplateView.h"
#include "Resource/ResourcePool.h"
#include "Utils/PageStyle.h"

constexpr int width_px = 294;
constexpr int height_px = 126;
static lv_color_t frame_pixels[width_px * height_px];
static lv_color_t draw_pixels[width_px * 20];
static unsigned flush_count;

static void advance(unsigned ms) {
    for (unsigned elapsed = 0; elapsed < ms; elapsed += 10) {
        lv_tick_inc(10);
        lv_timer_handler();
    }
}

static void check_count(NumberFlow* number, unsigned value) {
    auto* cont = number->get_cont();
    assert(lv_obj_get_child_cnt(cont) == 3);
    unsigned divisor = 100;
    for (int digit = 0; digit < 3; ++digit, divisor /= 10) {
        auto* cell = lv_obj_get_child(cont, digit);
        auto* strip = lv_obj_get_child(cell, 0);
        const auto* font = lv_obj_get_style_text_font(strip, 0);
        assert(lv_obj_has_flag(cell, LV_OBJ_FLAG_HIDDEN) == (digit < 2 && value < divisor));
        assert(lv_obj_get_y(strip) == -static_cast<int>(value / divisor % 10) * font->line_height
               || lv_obj_has_flag(cell, LV_OBJ_FLAG_HIDDEN));
    }
}

static void flush(lv_disp_drv_t* driver, const lv_area_t* area, lv_color_t* pixels) {
    assert(area->x1 >= 0 && area->y1 >= 0 && area->x2 < width_px && area->y2 < height_px);
    const int row_width_px = area->x2 - area->x1 + 1;
    for (int y = area->y1; y <= area->y2; ++y)
        std::memcpy(&frame_pixels[y * width_px + area->x1], &pixels[(y - area->y1) * row_width_px],
                    (std::size_t)row_width_px * sizeof(lv_color_t));
    ++flush_count;
    lv_disp_flush_ready(driver);
}

static bool save_ppm(const char* path) {
    FILE* file = std::fopen(path, "wb");
    if (!file)
        return false;
    bool ok = std::fprintf(file, "P6\n%d %d\n255\n", width_px, height_px) > 0;
    for (const lv_color_t color : frame_pixels) {
        const uint8_t rgb[] = {(uint8_t)(LV_COLOR_GET_R(color) * 255 / 31), (uint8_t)(LV_COLOR_GET_G(color) * 255 / 63),
                               (uint8_t)(LV_COLOR_GET_B(color) * 255 / 31)};
        if (ok && std::fwrite(rgb, 1, sizeof(rgb), file) != sizeof(rgb))
            ok = false;
    }
    return std::fclose(file) == 0 && ok;
}

int main(int argc, char** argv) {
    if (argc > 2)
        return 2;
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

    resource_pool::init();
    assert(resource_pool::get_image("settings") && resource_pool::get_image("satellite_big"));
    assert(resource_pool::get_font("rajdhaniBold_40") && resource_pool::get_font("dialplate"));
    page::DialplateView view{};
    view.create(lv_scr_act());
    assert(view.ui.top_info.cont && view.ui.btn_cont.btn_map && view.ui.btn_cont.btn_rec);
    view.appear_anim_start();
    advance(600);
    const auto& top = view.ui.top_info;
    auto* used = top.satellite_used->get_cont();
    auto* tracked = top.satellite_tracked->get_cont();
    assert(lv_obj_has_flag(used, LV_OBJ_FLAG_HIDDEN) && lv_obj_has_flag(tracked, LV_OBJ_FLAG_HIDDEN));
    assert(!lv_obj_has_flag(top.satellite_missing, LV_OBJ_FLAG_HIDDEN));
    DataProc::DeviceSnapshot device{};
    device.satellites_valid = true;
    for (unsigned count : {0U, 9U, 10U, 99U, 100U, 255U, 999U, 9U}) {
        device.satellites_used = count;
        device.satellites_tracked = count;
        view.update(device);
        advance(400);
        check_count(top.satellite_used, count);
        check_count(top.satellite_tracked, count);
        assert(used->coords.x1 > top.icon_satellite->coords.x2);
        assert(used->coords.x2 < top.satellite_separator->coords.x1);
        assert(top.satellite_separator->coords.x2 < tracked->coords.x1);
        assert(tracked->coords.x2 < top.radio_card->coords.x1);
        assert(used->coords.y2 == tracked->coords.y2);
        const auto* separator_font = lv_obj_get_style_text_font(top.satellite_separator, 0);
        assert(top.satellite_separator->coords.y2 - separator_font->base_line == used->coords.y2);
        assert(used->coords.y2 < top.cont->coords.y2);
    }
    device.satellites_valid = false;
    view.update(device);
    assert(lv_obj_has_flag(used, LV_OBJ_FLAG_HIDDEN) && lv_obj_has_flag(tracked, LV_OBJ_FLAG_HIDDEN));
    assert(!lv_obj_has_flag(top.satellite_missing, LV_OBJ_FLAG_HIDDEN));
    device.satellites_valid = true;
    device.satellites_used = 9;
    device.satellites_tracked = 14;
    device.work_valid = true;
    device.work_mode = DataProc::WorkMode::Base;
    device.radio_on = true;
    view.update(device);
    assert(std::strcmp(lv_label_get_text(top.mode_label), "BASE") == 0);
    assert(std::strcmp(lv_label_get_text(top.radio_label), "TX") == 0);
    assert(lv_obj_get_style_text_color(top.mode_label, 0).full == page::style::kInfo.full);
    device.radio_on = false;
    view.update(device);
    assert(std::strcmp(lv_label_get_text(top.radio_label), "OFF") == 0);
    device.work_valid = false;
    view.update(device);
    assert(std::strcmp(lv_label_get_text(top.mode_label), "--") == 0);
    assert(std::strcmp(lv_label_get_text(top.radio_label), "--") == 0);
    device.work_valid = true;
    device.work_mode = DataProc::WorkMode::Rover;
    device.radio_on = true;
    view.update(device);
    assert(std::strcmp(lv_label_get_text(top.mode_label), "ROVER") == 0);
    assert(std::strcmp(lv_label_get_text(top.radio_label), "RX") == 0);
    assert(lv_obj_get_style_text_color(top.mode_label, 0).full == page::style::kSuccess.full);
    advance(400);
    assert(!lv_obj_has_flag(used, LV_OBJ_FLAG_HIDDEN) && !lv_obj_has_flag(tracked, LV_OBJ_FLAG_HIDDEN));
    assert(lv_obj_has_flag(top.satellite_missing, LV_OBJ_FLAG_HIDDEN));
    lv_refr_now(display);
    assert(flush_count && lv_obj_get_child_cnt(lv_scr_act()) >= 2);
    unsigned nonblack_pixels = 0;
    for (const lv_color_t color : frame_pixels)
        nonblack_pixels += LV_COLOR_GET_R(color) || LV_COLOR_GET_G(color) || LV_COLOR_GET_B(color);
    assert(nonblack_pixels > 100);

    const bool saved = argc == 1 || save_ppm(argv[1]);
    view.destroy();
    lv_obj_clean(lv_scr_act());
    if (!saved) {
        std::perror("save preview");
        return 1;
    }
    return 0;
}
