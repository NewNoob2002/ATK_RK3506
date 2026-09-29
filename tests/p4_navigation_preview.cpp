#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "ButtonGesture.h"
#include "P4App.h"
#include "Resource/ResourcePool.h"
#include "Utils/I18n/I18n.h"

constexpr int width_px = 294;
constexpr int height_px = 126;
static lv_color_t frame[width_px * height_px];
static lv_color_t dialplate_frame[width_px * height_px];
static lv_color_t draw_pixels[width_px * 20];
static unsigned flushes;

static lv_obj_t* find_label(lv_obj_t* root, const char* text) {
    if (lv_obj_check_type(root, &lv_label_class) && std::strstr(lv_label_get_text(root), text))
        return root;
    for (uint32_t i = 0; i < lv_obj_get_child_cnt(root); ++i)
        if (auto* found = find_label(lv_obj_get_child(root, i), text))
            return found;
    return nullptr;
}

static lv_obj_t* find_image(lv_obj_t* root, const void* source) {
    if (lv_obj_check_type(root, &lv_img_class) && lv_img_get_src(root) == source)
        return root;
    for (uint32_t i = 0; i < lv_obj_get_child_cnt(root); ++i)
        if (auto* found = find_image(lv_obj_get_child(root, i), source))
            return found;
    return nullptr;
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
        using Input = P4App::InputAction;
        if (argc == 3) {
            lv_tick_inc(50);
            lv_timer_handler(); // 初始页面切换完成后才可再次 Push
            assert(app.ShowPage(argv[1]));
            lv_tick_inc(500);
            lv_timer_handler();
        } else {
            assert(std::strcmp(app.CurrentPage(), "Pages/Startup") == 0);
            assert(app.Focused());
            app.OnInput(Input::Press);
            lv_tick_inc(500);
            lv_timer_handler();
            app.OnInput(Input::Release); // 短按不能开机
            lv_tick_inc(400);
            lv_timer_handler();
            assert(std::strcmp(app.CurrentPage(), "Pages/Startup") == 0);
            app.OnInput(Input::Press);
            lv_tick_inc(2100);
            lv_timer_handler();
            assert(std::strcmp(app.CurrentPage(), "Pages/SystemLoading") == 0);
            assert(find_label(lv_scr_act(), "0%"));
            app.OnInput(Input::Release); // 跨页面松开不能误触下一个页面
            lv_tick_inc(2300);
            lv_timer_handler();
            assert(std::strcmp(app.CurrentPage(), "Pages/Dialplate") == 0);
            lv_obj_t* initial_focus = app.Focused();
            assert(initial_focus);
            app.OnInput(Input::NextFocus);
            assert(app.Focused() != initial_focus);
            app.OnInput(Input::PreviousFocus);
            assert(app.Focused() == initial_focus);
            app.OnInput(Input::Confirm);
            assert(std::strcmp(app.CurrentPage(), "Pages/SystemInfos") == 0);
            lv_tick_inc(50);
            lv_timer_handler();
            app.OnInput(Input::Back);
            assert(std::strcmp(app.CurrentPage(), "Pages/Dialplate") == 0);
            assert(app.Focused() == initial_focus);
            lv_tick_inc(1500);
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
            const struct {
                const char* name;
                const char* label;
            } pages[] = {
                {"Pages/WorkSettings", "TRIMTALK"}, {"Pages/RecordConfig", "infinite"},
                {"Pages/SystemInfos", "Firmware"},  {"Pages/Shutdown", "PRESS"},
                {"Pages/StarMap", "STAR MAP"},      {"Pages/SaveConfig", "0%"},
            };
            for (const auto& page : pages) {
                assert(app.ShowPage(page.name));
                assert(std::strcmp(app.CurrentPage(), page.name) == 0);
                assert(find_label(lv_scr_act(), page.label));
                lv_tick_inc(500);
                lv_timer_handler();
                const bool work = std::strcmp(page.name, "Pages/WorkSettings") == 0;
                const bool record = std::strcmp(page.name, "Pages/RecordConfig") == 0;
                const bool infos = std::strcmp(page.name, "Pages/SystemInfos") == 0;
                const bool shutdown = std::strcmp(page.name, "Pages/Shutdown") == 0;
                const bool star_map = std::strcmp(page.name, "Pages/StarMap") == 0;
                if (work || record || infos || shutdown || star_map) {
                    lv_obj_t* first = app.Focused();
                    assert(first);
                    app.OnInput(Input::NextFocus);
                    if (!star_map)
                        assert(app.Focused() != first);
                    if (work || record) {
                        app.OnInput(Input::PreviousFocus);
                        app.OnInput(Input::NextFocus);
                        lv_obj_t* option = find_label(lv_scr_act(), record ? "XYZ" : page.label);
                        assert(option);
                        const lv_coord_t previous_y = lv_obj_get_y(option);
                        app.OnInput(Input::Confirm); // 左滚轮向上，循环到最后一项
                        lv_tick_inc(500);
                        lv_timer_handler();
                        lv_tick_inc(500);
                        lv_timer_handler();
                        assert(lv_obj_get_y(option) != previous_y);
                        assert(app.Focused() != first);
                    } else if (shutdown) {
                        const auto previous = I18n::GetLanguage();
                        app.OnInput(Input::Confirm); // 切换语言，不执行关机/Wi-Fi 操作
                        assert(I18n::GetLanguage() != previous);
                        app.OnInput(Input::Confirm);
                        assert(I18n::GetLanguage() == previous);
                    }
                } else {
                    assert(!app.Focused()); // 无可操作控件的静态页
                }
                if (record) {
                    lv_obj_t* clock = find_image(lv_scr_act(), ResourcePool::GetImage("clock"));
                    lv_obj_t* option = find_label(lv_scr_act(), "infinite");
                    assert(clock && option);
                    lv_obj_t* column = lv_obj_get_parent(option);
                    assert(clock->coords.x1 >= column->coords.x1 - 30);
                    assert(clock->coords.x2 < column->coords.x1);
                    assert(clock->coords.y1 >= column->coords.y1 && clock->coords.y2 <= column->coords.y2);
                }
                if (std::strcmp(page.name, "Pages/SaveConfig") == 0) {
                    lv_obj_t* brand = find_label(lv_scr_act(), "SINGULARXYZ");
                    assert(brand && lv_obj_get_parent(brand)->coords.y1 >= 26);
                }
                lv_refr_now(display);
                assert(std::memcmp(frame, dialplate_frame, sizeof(frame)) != 0);
                if (work || record || shutdown) {
                    app.OnInput(Input::PreviousFocus); // 聚焦返回/重置按钮
                    app.OnInput(Input::Confirm);
                } else if (infos || star_map) {
                    app.OnInput(Input::Confirm); // 点击信息项/星图返回
                } else {
                    assert(app.Back());
                }
                lv_tick_inc(50);
                lv_timer_handler();
                assert(std::strcmp(app.CurrentPage(), "Pages/Dialplate") == 0);
                assert(app.Focused());
                assert(lv_group_get_focus_cb(lv_group_get_default()) == nullptr);
            }
            assert(app.ShowPage("Pages/Shutdown"));
            lv_tick_inc(50);
            lv_timer_handler(); // 先完成页面切换，再开始计长按时间
            app.OnInput(Input::Press);
            lv_tick_inc(500);
            lv_timer_handler();
            app.OnInput(Input::NextFocus); // 离开按钮必须取消长按
            lv_tick_inc(2100);
            lv_timer_handler();
            assert(std::strcmp(app.CurrentPage(), "Pages/Shutdown") == 0);
            app.OnInput(Input::PreviousFocus);
            app.OnInput(Input::Press);
            lv_tick_inc(2100);
            lv_timer_handler();
            assert(std::strcmp(app.CurrentPage(), "Pages/SaveConfig") == 0);
            app.OnInput(Input::Release);
            lv_tick_inc(50);
            lv_timer_handler();
            lv_tick_inc(9000);
            lv_timer_handler();
            assert(std::strcmp(app.CurrentPage(), "Pages/Startup") == 0);
            assert(app.Focused());
            assert(find_label(lv_scr_act(), "0%")); // 下次开机进度必须重置
        }
    }
    lv_disp_remove(display);
}
