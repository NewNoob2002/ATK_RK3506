#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ButtonGesture.h"
#include "P4App.h"
#include "Resource/ResourcePool.h"
#include "Status/DemoStatus.h"
#include "Utils/I18n/I18n.h"

constexpr int width_px = 294;
constexpr int height_px = 126;
static lv_color_t frame[width_px * height_px];
static lv_color_t dialplate_frame[width_px * height_px];
static lv_color_t draw_pixels[width_px * 20];
static unsigned flushes;

static lv_obj_t* find_label(lv_obj_t* root, const char* text) {
    if (lv_obj_has_flag(root, LV_OBJ_FLAG_HIDDEN))
        return nullptr;
    if (lv_obj_check_type(root, &lv_label_class) && std::strstr(lv_label_get_text(root), text))
        return root;
    for (uint32_t i = 0; i < lv_obj_get_child_cnt(root); ++i)
        if (auto* found = find_label(lv_obj_get_child(root, static_cast<int32_t>(i)), text))
            return found;
    return nullptr;
}

static lv_obj_t* find_image(lv_obj_t* root, const void* source) {
    if (lv_obj_has_flag(root, LV_OBJ_FLAG_HIDDEN))
        return nullptr;
    if (lv_obj_check_type(root, &lv_img_class) && lv_img_get_src(root) == source)
        return root;
    for (uint32_t i = 0; i < lv_obj_get_child_cnt(root); ++i)
        if (auto* found = find_image(lv_obj_get_child(root, static_cast<int32_t>(i)), source))
            return found;
    return nullptr;
}

static void advance(unsigned ms) {
    for (unsigned i = 0; i < ms; i += 10) {
        lv_tick_inc(10);
        lv_timer_handler();
    }
}

static void flush(lv_disp_drv_t* driver, const lv_area_t* area, lv_color_t* pixels) {
    assert(area->x1 >= 0 && area->y1 >= 0 && area->x2 < width_px && area->y2 < height_px);
    const int width = area->x2 - area->x1 + 1;
    for (int y = area->y1; y <= area->y2; ++y)
        std::memcpy(&frame[y * width_px + area->x1],
                    &pixels[static_cast<std::size_t>(y - area->y1) * static_cast<std::size_t>(width)],
                    width * sizeof(lv_color_t));
    ++flushes;
    lv_disp_flush_ready(driver);
}

int main(int argc, char** argv) {
    if (argc > 3)
        return 2;
    ButtonGesture gesture;
    using Action = ButtonGesture::Action;
    assert(gesture.sample(true, 0) == Action::None);
    assert(gesture.sample(false, 5) == Action::None); // bounce
    assert(gesture.sample(true, 10) == Action::None);
    assert(gesture.sample(true, 35) == Action::None);
    assert(gesture.sample(false, 60) == Action::None);
    assert(gesture.sample(false, 85) == Action::None);
    assert(gesture.sample(false, 336) == Action::Single);
    assert(gesture.sample(true, 400) == Action::None);
    assert(gesture.sample(true, 425) == Action::None);
    assert(gesture.sample(false, 445) == Action::None);
    assert(gesture.sample(false, 470) == Action::None);
    assert(gesture.sample(true, 500) == Action::None);
    assert(gesture.sample(true, 525) == Action::None);
    assert(gesture.sample(false, 550) == Action::None);
    assert(gesture.sample(false, 575) == Action::Double);
    assert(gesture.sample(false, 900) == Action::None);
    ButtonGesture boundary;
    boundary.sample(true, 1000);
    boundary.sample(true, 1020);
    boundary.sample(false, 1040);
    boundary.sample(false, 1060);
    assert(boundary.sample(true, 1305) == Action::None); // Press within 250ms, debounce past deadline.
    assert(boundary.sample(true, 1325) == Action::None);
    boundary.sample(false, 1340);
    assert(boundary.sample(false, 1360) == Action::Double);
    assert(boundary.sample(false, 1700) == Action::None);
    boundary.cancel(true);
    assert(boundary.sample(false, 1800) == Action::None);
    assert(boundary.sample(false, 1820) == Action::None);
    assert(boundary.sample(false, 2200) == Action::None); // A cancelled held key is not a click.

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
        assert(app.init());
        app.update_status(DemoStatus::sample(0));
        using Input = P4App::InputAction;
        if (argc == 3) {
            lv_tick_inc(400);
            lv_timer_handler(); // 初始页面切换完成后才可再次 Push
            app.update_status(DemoStatus::sample(16000));
            assert(app.show_page(argv[1]));
            lv_tick_inc(500);
            lv_timer_handler();
        } else {
            assert(std::strcmp(app.current_page(), "Pages/Startup") == 0);
            assert(app.focused());
            advance(400); // Finish initial transition before testing actual input.
            auto* startup_status = find_label(lv_layer_top(), "DEMO");
            assert(startup_status && startup_status->coords.y2 < 0);
            app.on_input(Input::Confirm);
            advance(300);
            assert(std::strcmp(app.current_page(), "Pages/Startup") == 0);
            app.on_input(Input::Press);
            advance(500);
            assert(!find_label(lv_scr_act(), "0%"));
            app.on_input(Input::Release); // A short press must not start.
            advance(400);
            assert(std::strcmp(app.current_page(), "Pages/Startup") == 0);
            assert(find_label(lv_scr_act(), "0%"));
            app.on_input(Input::Press);
            advance(500);
            app.cancel_input(); // SDL focus loss cancels rather than confirming the held control.
            app.on_input(Input::Release);
            advance(2500);
            assert(std::strcmp(app.current_page(), "Pages/Startup") == 0);
            assert(find_label(lv_scr_act(), "0%"));
            // LVGL pointer presses bypass P4App::pressed_; window loss must still cancel their hold.
            lv_obj_add_state(app.focused(), LV_STATE_PRESSED);
            lv_event_send(app.focused(), LV_EVENT_PRESSED, nullptr);
            advance(500);
            app.cancel_input();
            assert(!lv_obj_has_state(app.focused(), LV_STATE_PRESSED));
            advance(2500);
            assert(std::strcmp(app.current_page(), "Pages/Startup") == 0);
            assert(find_label(lv_scr_act(), "0%"));
            advance(2000);
            assert(startup_status->coords.y2 < 0); // Still below the ten-second Startup idle deadline.
            advance(1500);
            assert(startup_status->coords.y1 >= 0); // Delay expired; slide-in completed.
            app.on_input(Input::Press);
            advance(1000);
            app.on_input(Input::Press); // Repeated keydown cannot restart the timer.
            advance(1100);
            assert(std::strcmp(app.current_page(), "Pages/SystemLoading") == 0);
#if defined(RGK_LOGO_USE)
            const auto* loading_logo = resource_pool::get_image("RGKLogo");
#elif defined(MIDDLE_LOGO_USE)
            const auto* loading_logo = resource_pool::get_image("MiddleLogo");
#else
            const auto* loading_logo = resource_pool::get_image("startupLogo");
#endif
            assert(loading_logo && find_image(lv_scr_act(), loading_logo));
            assert(!find_label(lv_scr_act(), i18n::text(i18n::TextId::SystemLoadingTitle)));
            lv_tick_inc(5000); // A stalled host frame must not lose the loading completion request.
            lv_timer_handler();
            assert(std::strcmp(app.current_page(), "Pages/SystemLoading") == 0);
            advance(300);
            assert(startup_status->coords.y2 < 0); // SystemLoading slides a previously shown bar off-screen.
            assert(!app.back());                   // Loading replaced Startup, rather than retaining it underneath.
            app.on_input(Input::Release);          // Cross-page release must not activate the next page.
            advance(800);
            assert(!find_image(lv_scr_act(), loading_logo));
            assert(find_label(lv_scr_act(), i18n::text(i18n::TextId::SystemLoadingTitle)));
            assert(find_label(lv_scr_act(), i18n::text(i18n::TextId::LoadingConfiguration)));
            assert(find_label(lv_scr_act(), i18n::text(i18n::TextId::PreparingModules)));
            advance(1600);
            auto* progress = find_label(lv_scr_act(), "%");
            assert(progress && std::atoi(lv_label_get_text(progress)) >= 50);
            advance(1400);
            assert(find_label(lv_scr_act(), i18n::text(i18n::TextId::ReadyWithWarnings)));
            advance(1400);
            assert(std::strcmp(app.current_page(), "Pages/Dialplate") == 0);
            assert(!DataProc::Center()->SearchAccount("StartupModel"));
            assert(!DataProc::Center()->SearchAccount("SystemLoadingModel"));
            assert(!app.back()); // Dialplate is the root, not a child of the boot pages.
            lv_obj_t* initial_focus = app.focused();
            assert(initial_focus);
            app.on_input(Input::NextFocus);
            assert(app.focused() != initial_focus);
            app.on_input(Input::PreviousFocus);
            assert(app.focused() == initial_focus);
            app.on_input(Input::Confirm);
            lv_tick_inc(300);
            lv_timer_handler();
            assert(std::strcmp(app.current_page(), "Pages/SystemInfos") == 0);
            lv_tick_inc(50);
            lv_timer_handler();
            app.on_input(Input::Back);
            lv_tick_inc(300);
            lv_timer_handler();
            assert(std::strcmp(app.current_page(), "Pages/Dialplate") == 0);
            assert(app.focused() == initial_focus);
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
                {"Pages/SystemInfos", "Firmware"},  {"Pages/SystemDash", "System Menu"},
                {"Pages/StarMap", "STAR MAP"},      {"Pages/SaveConfig", "0%"},
            };
            for (const auto& page : pages) {
                lv_tick_inc(300);
                lv_timer_handler();
                app.update_status(DemoStatus::sample(16000));
                assert(app.show_page(page.name));
                assert(std::strcmp(app.current_page(), page.name) == 0);
                auto* page_root = find_label(lv_scr_act(), page.label);
                assert(page_root);
                while (lv_obj_get_parent(page_root) != lv_scr_act())
                    page_root = lv_obj_get_parent(page_root);
                const auto expected_anim =
                    std::strcmp(page.name, "Pages/SystemInfos") == 0 ? PageManager::LOAD_ANIM_FADE_ON
                    : (std::strcmp(page.name, "Pages/StarMap") == 0 || std::strcmp(page.name, "Pages/SaveConfig") == 0)
                        ? PageManager::LOAD_ANIM_MOVE_TOP
                        : PageManager::LOAD_ANIM_MOVE_LEFT;
                const auto* controller = static_cast<PageBase*>(lv_obj_get_user_data(page_root));
                assert(controller && controller->priv.anim.attr.type == expected_anim);
                advance(120);
                if (expected_anim == PageManager::LOAD_ANIM_FADE_ON) {
                    const auto opa = lv_obj_get_style_opa(page_root, 0);
                    assert(opa > LV_OPA_TRANSP && opa < LV_OPA_COVER);
                } else if (expected_anim == PageManager::LOAD_ANIM_MOVE_LEFT) {
                    assert(lv_obj_get_x(page_root) > 0 && lv_obj_get_x(page_root) < width_px);
                } else {
                    assert(lv_obj_get_y(page_root) > 0 && lv_obj_get_y(page_root) < height_px);
                }
                advance(480); // Page and StatusBar animations both need frame delivery.
                assert(lv_obj_get_style_x(page_root, 0) == 0 && lv_obj_get_style_y(page_root, 0) == 0);
                assert(lv_obj_get_y(page_root) == (std::strcmp(page.name, "Pages/StarMap") == 0 ? 26 : 0));
                assert(lv_obj_get_style_opa(page_root, 0) == LV_OPA_COVER);
                const bool work = std::strcmp(page.name, "Pages/WorkSettings") == 0;
                const bool record = std::strcmp(page.name, "Pages/RecordConfig") == 0;
                const bool infos = std::strcmp(page.name, "Pages/SystemInfos") == 0;
                const bool system_dash = std::strcmp(page.name, "Pages/SystemDash") == 0;
                const bool star_map = std::strcmp(page.name, "Pages/StarMap") == 0;
                auto* position = find_label(lv_layer_top(), "DEMO");
                assert(position && (position->coords.y2 >= 0) == !(infos || system_dash));
                if (work || record || infos || system_dash || star_map) {
                    lv_obj_t* first = app.focused();
                    assert(first);
                    if (star_map) {
                        assert(lv_obj_has_state(page_root, LV_STATE_FOCUSED));
                        assert(lv_obj_get_style_outline_width(page_root, 0) == 0);
                        assert(lv_obj_get_style_border_width(page_root, 0) == 0);
                    }
                    app.on_input(Input::NextFocus);
                    if (!star_map)
                        assert(app.focused() != first);
                    if (work || record) {
                        app.on_input(Input::PreviousFocus);
                        app.on_input(Input::NextFocus);
                        lv_obj_t* option = find_label(lv_scr_act(), record ? "XYZ" : page.label);
                        assert(option);
                        const lv_coord_t previous_y = lv_obj_get_y(option);
                        app.on_input(Input::Press);
                        assert(lv_obj_has_state(app.focused(), LV_STATE_PRESSED));
                        assert(lv_obj_get_y(option) == previous_y); // Normal buttons act on release.
                        app.on_input(Input::Release);               // 左滚轮向上，循环到最后一项
                        assert(!lv_obj_has_state(app.focused(), LV_STATE_PRESSED));
                        lv_tick_inc(500);
                        lv_timer_handler();
                        lv_tick_inc(500);
                        lv_timer_handler();
                        assert(lv_obj_get_y(option) != previous_y);
                        assert(app.focused() != first);
                    }
                } else {
                    assert(!app.focused()); // 无可操作控件的静态页
                }
                if (record) {
                    lv_obj_t* clock = find_image(lv_scr_act(), resource_pool::get_image("clock"));
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
                if (work || record) {
                    app.on_input(Input::PreviousFocus); // 聚焦返回按钮
                    app.on_input(Input::Confirm);
                } else if (infos || star_map) {
                    app.on_input(Input::Confirm); // 点击信息项/星图返回
                } else {
                    assert(app.back());
                }
                if (expected_anim == PageManager::LOAD_ANIM_MOVE_TOP)
                    assert(lv_obj_get_style_y(page_root, 0) == 0); // No bottom-alignment jump at pop start.
                lv_tick_inc(50);
                lv_timer_handler();
                assert(std::strcmp(app.current_page(), "Pages/Dialplate") == 0);
                lv_obj_update_layout(
                    page_root); // Animation setters dirty layout; getters otherwise see the last frame.
                if (expected_anim == PageManager::LOAD_ANIM_FADE_ON)
                    assert(lv_obj_get_style_opa(page_root, 0) < LV_OPA_COVER);
                else if (expected_anim == PageManager::LOAD_ANIM_MOVE_LEFT)
                    assert(lv_obj_get_x(page_root) > 0); // Back reverses the direction, not another left wipe.
                else
                    assert(lv_obj_get_style_y(page_root, 0) > 0);
                assert(app.focused());
                assert(lv_group_get_focus_cb(lv_group_get_default()) == nullptr);
            }
            // Cancelling Logo, Initialization or Ready must remove timers/animations, not navigate later.
            for (const auto phase_ms : {400u, 1600u, 4450u}) {
                advance(300);
                assert(app.show_page("Pages/SystemLoading"));
                advance(phase_ms);
                assert(app.show_page("Pages/SystemInfos")); // Loading stays cached beneath this page.
                advance(400);
                assert(app.back());
                advance(400);
                assert(std::strcmp(app.current_page(), "Pages/SystemLoading") == 0);
                advance(1200);
                auto* restarted = find_label(lv_scr_act(), i18n::text(i18n::TextId::LoadingConfiguration));
                assert(restarted && lv_obj_get_style_opa(restarted, 0) == LV_OPA_COVER);
                assert(!find_label(lv_scr_act(), i18n::text(i18n::TextId::ReadyWithWarnings)));
                assert(app.back());
                advance(4000);
                assert(std::strcmp(app.current_page(), "Pages/Dialplate") == 0);
                assert(!DataProc::Center()->SearchAccount("SystemLoadingModel"));
                assert(!app.back());
            }
            lv_tick_inc(300);
            lv_timer_handler();
            assert(app.show_page("Pages/SystemDash"));
            lv_tick_inc(500);
            lv_timer_handler();
            app.on_input(Input::Confirm); // Power-first tile; confirmation opens with Cancel focused
            assert(std::strcmp(app.current_page(), "Pages/SystemDash") == 0);
            app.on_input(Input::NextFocus); // Select Shutdown, but single Enter cannot execute.
            app.on_input(Input::Commit);
            assert(std::strcmp(app.current_page(), "Pages/SaveConfig") == 0);
            app.on_input(Input::Release);
            lv_tick_inc(50);
            lv_timer_handler();
            // Step real UI frames so save completion cannot race a still-busy page animation.
            for (int elapsed = 0; elapsed < 9000; elapsed += 10) {
                lv_tick_inc(10);
                lv_timer_handler();
            }
            assert(std::strcmp(app.current_page(), "Pages/Startup") == 0);
            assert(app.focused() && find_label(lv_scr_act(), "0%"));
            assert(!app.back()); // Off resets the root; no previous working/boot history remains.
        }
    }
    lv_disp_remove(display);
}
