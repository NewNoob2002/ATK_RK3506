#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "Pages/Dialplate/DialplateView.h"
#include "Resource/ResourcePool.h"

constexpr int width_px = 294;
constexpr int height_px = 126;
static lv_color_t frame_pixels[width_px * height_px];
static lv_color_t draw_pixels[width_px * 20];
static unsigned flush_count;

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

    ResourcePool::Init();
    assert(ResourcePool::GetImage("settings") && ResourcePool::GetImage("satellite_big"));
    assert(ResourcePool::GetFont("rajdhaniBold_40") && ResourcePool::GetFont("dialplate"));
    Page::DialplateView view{};
    view.Create(lv_scr_act());
    assert(view.ui.topInfo.cont && view.ui.btnCont.btnMap && view.ui.btnCont.btnRec);
    view.AppearAnimStart();
    lv_tick_inc(1500);
    lv_timer_handler();
    lv_refr_now(display);
    assert(flush_count && lv_obj_get_child_cnt(lv_scr_act()) >= 2);
    unsigned nonblack_pixels = 0;
    for (const lv_color_t color : frame_pixels)
        nonblack_pixels += LV_COLOR_GET_R(color) || LV_COLOR_GET_G(color) || LV_COLOR_GET_B(color);
    assert(nonblack_pixels > 100);

    const bool saved = argc == 1 || save_ppm(argv[1]);
    view.Delete();
    lv_obj_clean(lv_scr_act());
    if (!saved) {
        std::perror("save preview");
        return 1;
    }
    return 0;
}
