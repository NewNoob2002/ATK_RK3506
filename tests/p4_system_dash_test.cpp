#include <cassert>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <string>
#include "Common/SystemService.h"
#include "P4App.h"
#include "Status/DemoStatus.h"
#include "Utils/Log/Log.h"

namespace {
constexpr int width = 294, height = 126;
lv_color_t frame[width * height], pixels[width * 20];
P4App* active = nullptr;
void flush(lv_disp_drv_t* driver, const lv_area_t* area, lv_color_t* colors) {
    assert(area->x1 >= 0 && area->y1 >= 0 && area->x2 < width && area->y2 < height);
    for (int y = area->y1; y <= area->y2; ++y)
        for (int x = area->x1; x <= area->x2; ++x)
            frame[y * width + x] = *colors++;
    lv_disp_flush_ready(driver);
}
void advance(unsigned ms) {
    for (unsigned i = 0; i < ms; i += 10) {
        lv_tick_inc(10);
        lv_timer_handler();
    }
}
void input(P4App::InputAction action) {
    active->on_input(action);
    advance(400); // Wait beyond the longest page transition; not product gesture timing.
}
void product_click(P4App::Key key) {
    const auto sample = [](unsigned ms) {
        for (unsigned i = 0; i < ms; i += 10) {
            advance(10);
            active->poll_keys(lv_tick_get());
        }
    };
    active->on_key(key, true);
    sample(40);
    active->on_key(key, false);
    sample(700); // Debounce, single-click decision and the resulting page transition.
}
lv_obj_t* label(lv_obj_t* root, const char* text) {
    if (lv_obj_has_flag(root, LV_OBJ_FLAG_HIDDEN))
        return nullptr;
    if (lv_obj_check_type(root, &lv_label_class) && std::strcmp(lv_label_get_text(root), text) == 0)
        return root;
    for (unsigned i = 0; i < lv_obj_get_child_cnt(root); ++i)
        if (auto* found = label(lv_obj_get_child(root, static_cast<int32_t>(i)), text))
            return found;
    return nullptr;
}
void check_layout(lv_obj_t* root) {
    if (lv_obj_has_flag(root, LV_OBJ_FLAG_HIDDEN))
        return;
    if (lv_obj_check_type(root, &lv_label_class)) {
        const auto* text = lv_label_get_text(root);
        const auto* font = lv_obj_get_style_text_font(root, 0);
        for (uint32_t i = 0; text[i];) {
            const auto codepoint = _lv_txt_encoded_next(text, &i);
            if (codepoint == '\n')
                continue;
            lv_font_glyph_dsc_t glyph{};
            assert(lv_font_get_glyph_dsc(font, &glyph, codepoint, 0) && !glyph.is_placeholder);
        }
        auto* parent = lv_obj_get_parent(root);
        assert(root->coords.x1 >= parent->coords.x1 && root->coords.x2 <= parent->coords.x2);
        assert(root->coords.y1 >= parent->coords.y1 && root->coords.y2 <= parent->coords.y2);
        if (root->coords.y1 < 0 || root->coords.y2 >= height)
            std::fprintf(stderr, "outside: %s y=%d..%d\n", text, root->coords.y1, root->coords.y2);
        assert(root->coords.y1 >= 0 && root->coords.y2 < height);
    }
    if (lv_obj_check_type(root, &lv_img_class)) {
        lv_point_t pivot;
        lv_img_get_pivot(root, &pivot);
        lv_area_t rendered;
        _lv_img_buf_get_transformed_area(&rendered, lv_obj_get_width(root), lv_obj_get_height(root),
                                         static_cast<int16_t>(lv_img_get_angle(root)), lv_img_get_zoom(root), &pivot);
        auto* parent = lv_obj_get_parent(root);
        assert(rendered.x1 + root->coords.x1 >= parent->coords.x1
               && rendered.x2 + root->coords.x1 <= parent->coords.x2);
        assert(rendered.y1 + root->coords.y1 >= parent->coords.y1
               && rendered.y2 + root->coords.y1 <= parent->coords.y2);
    }
    for (unsigned i = 0; i < lv_obj_get_child_cnt(root); ++i)
        check_layout(lv_obj_get_child(root, static_cast<int32_t>(i)));
}
bool on_screen(lv_obj_t* obj) {
    // LVGL layer-top has OVERFLOW_VISIBLE: lv_obj_is_visible alone also accepts off-screen objects.
    return obj && lv_obj_is_visible(obj) && obj->coords.y2 >= 0 && obj->coords.y1 < height;
}
void capture(lv_disp_t* display, const std::string& directory, const std::string& name, bool status = true,
             unsigned settle_ms = 400) {
    advance(settle_ms);
    lv_obj_update_layout(lv_scr_act());
    // Inactive information cards are intentionally translated outside the viewport.
    const bool infos = std::strcmp(active->current_page(), "Pages/SystemInfos") == 0;
    check_layout(infos ? lv_obj_get_parent(active->focused()) : lv_scr_act());
    auto* position = label(lv_layer_top(), "DEMO");
    assert(position && on_screen(position) == status);
    lv_refr_now(display);
    if (directory.empty())
        return;
    auto* file = std::fopen((directory + "/" + name + ".ppm").c_str(), "wb");
    assert(file);
    assert(std::fprintf(file, "P6\n%d %d\n255\n", width, height) > 0);
    for (auto pixel : frame) {
        const unsigned char rgb[]{static_cast<unsigned char>(LV_COLOR_GET_R(pixel) * 255 / 31),
                                  static_cast<unsigned char>(LV_COLOR_GET_G(pixel) * 255 / 63),
                                  static_cast<unsigned char>(LV_COLOR_GET_B(pixel) * 255 / 31)};
        assert(std::fwrite(rgb, 1, 3, file) == 3);
    }
    assert(std::fclose(file) == 0);
}
void boot_sequence(lv_disp_t* display, const std::string& output, const std::string& suffix) {
    assert(std::strcmp(active->current_page(), "Pages/SystemLoading") == 0);
    capture(display, output, "SystemLoading-Logo" + suffix, false, 0);
    assert(!label(lv_scr_act(), i18n::text(i18n::TextId::SystemLoadingTitle)));
    unsigned frame_index = 0;
    uint32_t last_frame_ms = lv_tick_get() - 100;
    const auto step_frames = [&](unsigned ms) {
        for (unsigned elapsed = 0; elapsed < ms; elapsed += 10) {
            advance(10);
            if (!output.empty() && lv_tick_elaps(last_frame_ms) >= 100
                && std::strcmp(active->current_page(), "Pages/SystemLoading") == 0) {
                char name[48];
                std::snprintf(name, sizeof(name), "Boot-frame%s-%03u", suffix.c_str(), frame_index++);
                capture(display, output, name, false, 0);
                last_frame_ms = lv_tick_get();
            }
        }
    };
    const auto wait_for = [&](i18n::TextId id) {
        for (unsigned elapsed = 0; elapsed < 3500; elapsed += 10) {
            assert(std::strcmp(active->current_page(), "Pages/SystemLoading") == 0);
            if (label(lv_scr_act(), i18n::text(id)))
                return;
            step_frames(10);
        }
        assert(false && "boot phase deadline exceeded");
    };
    const i18n::TextId stages[] = {i18n::TextId::LoadingConfiguration, i18n::TextId::PreparingServices,
                                   i18n::TextId::StartingGnssService, i18n::TextId::PreparingNetwork,
                                   i18n::TextId::FinalizingStartup};
    for (unsigned i = 0; i < 5; ++i) {
        wait_for(stages[i]);
        step_frames(i == 2 ? 400 : 180); // Stage 3 capture around 60%, like the reference.
        capture(display, output, "SystemLoading-Step" + std::to_string(i + 1) + suffix, false, 0);
        auto* headline = label(lv_scr_act(), i18n::text(stages[i]));
        assert(headline && label(lv_scr_act(), "DEMO"));
        assert(lv_obj_get_style_border_width(lv_obj_get_parent(headline), 0) == 0);
        assert(lv_obj_get_style_outline_width(lv_obj_get_parent(headline), 0) == 0);
        lv_point_t text_size;
        lv_txt_get_size(&text_size, lv_label_get_text(headline), lv_obj_get_style_text_font(headline, 0), 0, 0,
                        LV_COORD_MAX, LV_TEXT_FLAG_NONE);
        assert(text_size.x <= lv_obj_get_width(headline)); // No silently clipped translated headline.
        char step[40];
        std::snprintf(step, sizeof(step), i18n::text(i18n::TextId::InitializationStep), static_cast<int>(i + 1));
        assert(label(lv_scr_act(), step));
    }
    wait_for(i18n::TextId::SystemReady);
    step_frames(260); // Record the orange-to-green/checkmark transition, then its settled endpoint.
    capture(display, output, "SystemLoading-Ready" + suffix, false, 0);
    assert(label(lv_scr_act(), "100%") && label(lv_scr_act(), i18n::text(i18n::TextId::StartupCompleted)));
    assert(label(lv_scr_act(), i18n::text(i18n::TextId::ServicesAvailable)));
    auto* ready_panel = lv_obj_get_parent(label(lv_scr_act(), i18n::text(i18n::TextId::SystemReady)));
    assert(lv_obj_get_style_border_width(ready_panel, 0) == 0);
    assert(lv_obj_get_style_outline_width(ready_panel, 0) == 0);
    step_frames(1200);
    advance(400);
    assert(std::strcmp(active->current_page(), "Pages/Dialplate") == 0);
}
void focused(i18n::TextId id) {
    assert(active->focused() && label(active->focused(), i18n::text(id)));
}
} // namespace
int main(int argc, char** argv) {
    assert(argc <= 2);
    const std::string output = argc == 2 ? argv[1] : "";
    if (!output.empty())
        std::filesystem::create_directories(output);
    app_log_set_level(APP_LOG_WARN);
    lv_init();
    lv_disp_draw_buf_t draw;
    lv_disp_draw_buf_init(&draw, pixels, nullptr, width * 20);
    lv_disp_drv_t driver;
    lv_disp_drv_init(&driver);
    driver.hor_res = width;
    driver.ver_res = height;
    driver.draw_buf = &draw;
    driver.flush_cb = flush;
    auto* display = lv_disp_drv_register(&driver);
    assert(display);
    {
        P4App app;
        active = &app;
        assert(app.init());
        advance(400);
        assert(!on_screen(label(lv_layer_top(), "DEMO")));
        capture(display, output, "Startup-hidden-en", false);
        advance(10000);
        capture(display, output, "Startup-idle-en");
        input(P4App::InputAction::Press); // Host Enter down/up: Startup requires a real hold.
        advance(2100);
        input(P4App::InputAction::Release); // Cross-page release cannot activate SystemLoading.
        boot_sequence(display, output, "-en");
        assert(std::strcmp(app.current_page(), "Pages/Dialplate") == 0);
        assert(!app.back());
        app.update_status(DemoStatus::sample(16000));
        input(P4App::InputAction::NextFocus);
        input(P4App::InputAction::Confirm);
        assert(std::strcmp(app.current_page(), "Pages/SystemDash") == 0);
        assert(!app.show_page("Pages/Shutdown"));
        focused(i18n::TextId::PowerTitle);
        for (auto language : {i18n::Language::English, i18n::Language::Russian}) {
            if (i18n::get_language() != language) {
                input(P4App::InputAction::NextFocus);
                input(P4App::InputAction::Confirm);
                if (!label(app.focused(), i18n::text(i18n::TextId::LanguageTitle)))
                    input(P4App::InputAction::NextFocus);
                focused(i18n::TextId::LanguageTitle);
                input(P4App::InputAction::Confirm);
                assert(i18n::get_language() == language);
                input(P4App::InputAction::Back);      // Back to dashboard, preserving Settings tile focus.
                input(P4App::InputAction::NextFocus); // Return tile.
                input(P4App::InputAction::NextFocus); // Power tile.
            }
            assert(i18n::get_language() == language);
            const std::string suffix = language == i18n::Language::English ? "-en" : "-ru";
            focused(i18n::TextId::PowerTitle);
            auto* tile = app.focused();
            auto* icon = lv_obj_get_child(tile, 0);
            assert(lv_obj_get_style_bg_color(tile, 0).full == lv_color_hex(0x333333).full);
            assert(lv_obj_get_style_border_color(tile, 0).full == lv_color_hex(0xff931e).full);
            assert(lv_obj_get_style_img_recolor(icon, 0).full == lv_color_hex(0xd03c3b).full);
            assert(lv_obj_get_style_img_recolor_opa(icon, 0) == LV_OPA_COVER);
            for (auto id : {i18n::TextId::PowerTitle, i18n::TextId::SettingsTitle, i18n::TextId::Return}) {
                auto* title = label(lv_scr_act(), i18n::text(id));
                assert(title);
                lv_point_t natural_size;
                lv_txt_get_size(&natural_size, i18n::text(id), lv_obj_get_style_text_font(title, 0),
                                lv_obj_get_style_text_letter_space(title, 0), 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
                assert(natural_size.x <= lv_obj_get_width(title)); // No clipped Russian captions on narrow tiles.
                auto* menu_icon = lv_obj_get_child(lv_obj_get_parent(title), 0);
                const auto* bitmap = static_cast<const lv_img_dsc_t*>(lv_img_get_src(menu_icon));
                assert(bitmap && lv_img_src_get_type(bitmap) == LV_IMG_SRC_VARIABLE);
                assert(bitmap->header.cf == LV_IMG_CF_ALPHA_8BIT);
                assert(bitmap->header.w == 40 && bitmap->header.h == 40 && bitmap->data_size == 1600);
                assert(lv_img_get_zoom(menu_icon) == LV_IMG_ZOOM_NONE);
            }
            capture(display, output, "SystemDash" + suffix, false);
            product_click(P4App::Key::Function);
            focused(i18n::TextId::SettingsTitle);
            product_click(P4App::Key::Function);
            focused(i18n::TextId::Return);
            capture(display, output, "SystemDash-Return" + suffix, false);
            input(P4App::InputAction::Commit); // Return is ordinary Enter, not a dangerous Commit action.
            assert(std::strcmp(app.current_page(), "Pages/SystemDash") == 0);
            product_click(P4App::Key::Power);
            assert(std::strcmp(app.current_page(), "Pages/Dialplate") == 0);
            assert(!SystemService::snapshot().power_pending && SystemService::snapshot().power_calls == 0);
            advance(600);
            assert(on_screen(label(lv_layer_top(), "DEMO"))); // Working page's StatusBar is restored.
            auto* home_root = app.focused();
            assert(home_root);
            while (lv_obj_get_parent(home_root) != lv_scr_act())
                home_root = lv_obj_get_parent(home_root);
            assert(lv_obj_get_x(home_root) == 0 && lv_obj_get_y(home_root) == 0);
            assert(lv_obj_get_style_opa(home_root, 0) == LV_OPA_COVER);
            product_click(P4App::Key::Power); // The working page remembers its System Menu entry focus.
            assert(std::strcmp(app.current_page(), "Pages/SystemDash") == 0);
            focused(i18n::TextId::PowerTitle); // Popping the menu unloads it; the fresh menu is Power-first.
            product_click(P4App::Key::Function);
            focused(i18n::TextId::SettingsTitle);
            product_click(P4App::Key::Function);
            focused(i18n::TextId::Return);
            product_click(P4App::Key::Function);
            focused(i18n::TextId::PowerTitle); // Three-item cyclic navigation.
            input(P4App::InputAction::Confirm);
            focused(i18n::TextId::Cancel);
            assert(lv_obj_get_style_bg_color(app.focused(), 0).full == lv_color_hex(0x666666).full);
            auto* shutdown = lv_obj_get_parent(label(lv_scr_act(), i18n::text(i18n::TextId::ShutdownDouble)));
            assert(lv_obj_get_style_bg_color(shutdown, 0).full == lv_color_hex(0xd03c3b).full);
            capture(display, output, "Power" + suffix, false);
            input(P4App::InputAction::NextFocus);
            focused(i18n::TextId::ShutdownDouble);
            input(P4App::InputAction::Confirm); // Select/Enter MUST NOT execute; host Commit is a separate action.
            assert(std::strcmp(app.current_page(), "Pages/SystemDash") == 0
                   && SystemService::snapshot().power_calls == 0);
            input(P4App::InputAction::Back);
            focused(i18n::TextId::PowerTitle);
            input(P4App::InputAction::NextFocus);
            input(P4App::InputAction::Confirm);
            assert(std::strcmp(app.current_page(), "Pages/SystemSettings") == 0);
            focused(i18n::TextId::LanguageTitle);
            assert(!on_screen(label(lv_layer_top(), "DEMO"))); // SystemDash subpages stay full-screen.
            auto* settings_title = label(lv_scr_act(), i18n::text(i18n::TextId::SettingsTitle));
            assert(settings_title && settings_title->coords.y1 >= 0 && settings_title->coords.y2 < height);
            auto* settings_back = label(lv_scr_act(), i18n::text(i18n::TextId::Back));
            assert(settings_back && settings_back->coords.y2 < height);
            capture(display, output, "Settings" + suffix, false);
            auto* wifi = label(lv_scr_act(), i18n::text(i18n::TextId::SystemWifiTitle));
            assert(wifi && lv_obj_has_state(lv_obj_get_parent(wifi), LV_STATE_DISABLED));
            assert(!label(lv_scr_act(), i18n::text(i18n::TextId::DateTimeTitle)));
            const auto original = SystemService::snapshot().date;
            input(P4App::InputAction::Commit); // No hidden date editor/commit path remains.
            const auto unchanged = SystemService::snapshot().date;
            assert(unchanged.year == original.year && unchanged.month == original.month && unchanged.day == original.day
                   && unchanged.hour == original.hour && unchanged.minute == original.minute);
            product_click(P4App::Key::Function);
            focused(i18n::TextId::Back); // Disabled Wi-Fi is skipped.
            capture(display, output, "Settings-Back" + suffix, false);
            product_click(P4App::Key::Function);
            focused(i18n::TextId::LanguageTitle); // Two-item cyclic navigation, no Up key needed.
            product_click(P4App::Key::Function);
            focused(i18n::TextId::Back);
            product_click(P4App::Key::Power); // Visible Back works with a single Enter, not a hidden gesture.
            assert(std::strcmp(app.current_page(), "Pages/SystemDash") == 0);
            assert(!on_screen(label(lv_layer_top(), "DEMO")));
            focused(i18n::TextId::SettingsTitle);
            input(P4App::InputAction::NextFocus);
            focused(i18n::TextId::Return);
            input(P4App::InputAction::NextFocus);
            focused(i18n::TextId::PowerTitle);
            assert(app.show_page("Pages/SystemInfos"));
            advance(400);
            for (unsigned card = 0; card < 6; ++card) {
                capture(display, output, "SystemInfos-" + std::to_string(card) + suffix, false);
                input(P4App::InputAction::NextFocus);
            }
            input(P4App::InputAction::Back);
            assert(!on_screen(label(lv_layer_top(), "DEMO")));
            assert(app.show_page("Pages/StarMap"));
            advance(600);
            assert(app.focused() && lv_obj_has_state(app.focused(), LV_STATE_FOCUSED));
            assert(lv_obj_get_style_outline_width(app.focused(), 0) == 0);
            assert(lv_obj_get_style_border_width(app.focused(), 0) == 0);
            capture(display, output, "StarMap-no-outline" + suffix);
            input(P4App::InputAction::Confirm);
            assert(std::strcmp(app.current_page(), "Pages/SystemDash") == 0);
        }
        input(P4App::InputAction::Confirm);
        input(P4App::InputAction::NextFocus);
        input(P4App::InputAction::Commit);
        assert(std::strcmp(app.current_page(), "Pages/SaveConfig") == 0);
        input(P4App::InputAction::Back); // Cancel before the simulated service runs.
        assert(!SystemService::snapshot().power_pending && SystemService::snapshot().power_calls == 0);
        input(P4App::InputAction::Back); // Close the retained power modal.
        // Both committed actions run only after SaveConfig's timer; cancel also clears the transaction.
        for (auto action : {DataProc::PowerAction::Off, DataProc::PowerAction::Reboot}) {
            input(P4App::InputAction::Confirm);
            input(P4App::InputAction::NextFocus);
            if (action == DataProc::PowerAction::Reboot)
                input(P4App::InputAction::NextFocus);
            const auto calls = SystemService::snapshot().power_calls;
            input(P4App::InputAction::Commit);
            assert(std::strcmp(app.current_page(), "Pages/SaveConfig") == 0
                   && SystemService::snapshot().power_calls == calls);
            input(P4App::InputAction::Commit); // Duplicate commit cannot create another request.
            advance(12000);
            assert(SystemService::snapshot().power_calls == calls + 1
                   && SystemService::snapshot().power_action == action);
            assert(!SystemService::snapshot().power_pending);
            if (action == DataProc::PowerAction::Off) {
                assert(std::strcmp(app.current_page(), "Pages/Startup") == 0 && !app.back());
                assert(label(lv_scr_act(), "0%"));
                assert(!DataProc::Center()->SearchAccount("DialplateModel"));
                assert(!DataProc::Center()->SearchAccount("SystemDashModel"));
                assert(!DataProc::Center()->SearchAccount("SaveConfigModel"));
                capture(display, output, "PowerOff-Startup", false);
                input(P4App::InputAction::Confirm); // Short click cannot restart after shutdown.
                assert(std::strcmp(app.current_page(), "Pages/Startup") == 0);
                input(P4App::InputAction::Press);
                advance(2100);
                input(P4App::InputAction::Release);
                boot_sequence(display, output, "-ru");
            }
            assert(std::strcmp(app.current_page(), "Pages/Dialplate") == 0 && !app.back());
            auto* restored_root = app.focused();
            assert(restored_root);
            while (lv_obj_get_parent(restored_root) != lv_scr_act())
                restored_root = lv_obj_get_parent(restored_root);
            assert(lv_obj_get_x(restored_root) == 0 && lv_obj_get_y(restored_root) == 0);
            assert(lv_obj_get_style_opa(restored_root, 0) == LV_OPA_COVER);
            assert(app.show_page("Pages/SystemDash"));
            advance(400);
        }
        assert(app.show_page("Pages/SystemInfos"));
        advance(400);
        for (unsigned card = 0; card < 6; ++card) {
            capture(display, output, "SystemInfos-" + std::to_string(card), false);
            input(P4App::InputAction::NextFocus);
        }
        input(P4App::InputAction::Back);
        advance(400);
        assert(!on_screen(label(lv_layer_top(), "DEMO")));
        input(P4App::InputAction::Back);
        advance(300);
        assert(std::strcmp(app.current_page(), "Pages/Dialplate") == 0 && !app.back());
        assert(on_screen(label(lv_layer_top(), "DEMO"))); // Returning to the working root restores the bar.
        active = nullptr;
    }
    assert(DataProc::Center()->GetAccountLen() == 0);
    lv_disp_remove(display);
}
