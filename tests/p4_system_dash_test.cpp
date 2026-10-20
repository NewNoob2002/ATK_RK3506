#include <cassert>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <string>
#include <vector>
#include "Common/SystemService.h"
#include "P4App.h"
#include "Pages/SystemLoading/SystemLoadingView.h"
#include "Status/DemoStatus.h"
#include "Utils/Log/Log.h"

namespace {
constexpr int width = 294, height = 126;
constexpr i18n::TextId inventory_titles[] = {i18n::TextId::SystemWorkTitle,    i18n::TextId::SystemGpsTitle,
                                             i18n::TextId::SystemWifiTitle,    i18n::TextId::SystemBatteryTitle,
                                             i18n::TextId::SystemStorageTitle, i18n::TextId::SystemTitle};
lv_color_t frame[width * height], pixels[width * 20];
P4App* active = nullptr;
std::vector<std::string> boot_logs;
void boot_log_sink(AppLogLevel, const char* module, const char* message, void*) {
    if (std::strcmp(module, "SystemLoading") == 0)
        boot_logs.emplace_back(message);
}
unsigned red_nodes(lv_obj_t* root) {
    if (lv_obj_has_flag(root, LV_OBJ_FLAG_HIDDEN))
        return 0;
    unsigned count = lv_obj_get_width(root) == 8 && lv_obj_get_height(root) == 8
                     && lv_obj_get_style_bg_color(root, 0).full == lv_color_hex(0xd03c3b).full;
    for (unsigned i = 0; i < lv_obj_get_child_cnt(root); ++i)
        count += red_nodes(lv_obj_get_child(root, static_cast<int32_t>(i)));
    return count;
}
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
void sample_keys(unsigned ms) {
    for (unsigned i = 0; i < ms; i += 10) {
        advance(10);
        active->poll_keys(lv_tick_get());
    }
}
void product_click(P4App::Key key, bool double_click = false) {
    sample_keys(40); // Synchronize the idle page context, as the board's continuous polling does.
    for (unsigned click = 0; click < (double_click ? 2u : 1u); ++click) {
        active->on_key(key, true);
        sample_keys(40);
        active->on_key(key, false);
        sample_keys(80);
    }
    sample_keys(700); // Debounce, single-click decision and the resulting page transition.
}
void product_start() {
    sample_keys(40);
    active->on_key(P4App::Key::Power, true);
    sample_keys(2100);
    active->on_key(P4App::Key::Power, false);
    sample_keys(400); // The release belongs to Startup, not the loading page.
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
void check_startup() {
    auto* start = active->focused();
    assert(start && lv_obj_get_width(start) >= 100 && lv_obj_get_height(start) >= 90);
    assert(label(lv_scr_act(), "GNSS RTK"));
    for (auto id : {i18n::TextId::StartupStart, i18n::TextId::StartupHoldHint}) {
        auto* caption = label(lv_scr_act(), i18n::text(id));
        assert(caption);
        lv_point_t natural_size;
        lv_txt_get_size(&natural_size, i18n::text(id), lv_obj_get_style_text_font(caption, 0), 0, 0, LV_COORD_MAX,
                        LV_TEXT_FLAG_NONE);
        assert(natural_size.x <= lv_obj_get_width(caption));
        assert(lv_obj_get_parent(caption) == start); // The whole power tile is the hold target.
    }
}
lv_obj_t* battery_fill() {
    auto* caption = label(lv_scr_act(), "BAT");
    if (!caption)
        caption = label(lv_scr_act(), "CHG");
    assert(caption);
    auto* level = lv_obj_get_child(lv_obj_get_parent(caption), -1);
    assert(level && lv_obj_get_width(level) == 94 && lv_obj_get_height(level) == 10);
    auto* fill = lv_obj_get_child(level, 0);
    assert(fill);
    return fill;
}
lv_obj_t* status_battery_fill() {
    auto* bar = lv_obj_get_child(lv_layer_top(), 0);
    assert(bar);
    for (unsigned i = 0; i < lv_obj_get_child_cnt(bar); ++i) {
        auto* slot = lv_obj_get_child(bar, static_cast<int32_t>(i));
        if (lv_obj_get_width(slot) == 20 && lv_obj_get_height(slot) == 12 && lv_obj_get_child_cnt(slot) == 2)
            return lv_obj_get_child(slot, 0);
    }
    assert(false && "StatusBar battery slot missing");
    return nullptr;
}
void random_battery_events(lv_disp_t* display, const std::string& output) {
    auto* fill = battery_fill();
    auto* status_fill = status_battery_fill();
    auto state = DemoStatus::sample(0);
    const auto check = [&] {
        active->update_status(state); // Exercise Application -> Status provider -> Model -> View.
        advance(40);
        const auto percent = state.battery_percent > 100 ? 100 : state.battery_percent;
        char text[24];
        std::snprintf(text, sizeof(text), "%u%%", percent);
        auto* card = lv_obj_get_parent(lv_obj_get_parent(fill));
        const bool charging = state.battery_valid && state.charging;
        assert(label(card, state.battery_valid ? text : "N/A") && label(card, charging ? "CHG" : "BAT"));
        std::snprintf(text, sizeof(text), "%.2f V", state.battery_voltage);
        assert(label(card, state.battery_valid ? text : "N/A"));
        const auto color = !state.battery_valid        ? 0x999999u
                           : charging || percent >= 50 ? 0x28c76fu
                           : percent >= 20             ? 0xff931eu
                           : percent >= 10             ? 0xea5455u
                                                       : 0xd03c3bu;
        auto* icon = lv_obj_get_child(card, 0);
        assert(lv_obj_get_style_img_recolor(icon, 0).full == lv_color_hex(color).full);
        for (auto* current : {fill, status_fill}) {
            const auto full_width = current == fill ? 90 : 16;
            auto* slot = lv_obj_get_parent(current);
            assert(lv_obj_get_style_bg_color(current, 0).full == lv_color_hex(color).full);
            assert(lv_obj_get_style_border_color(slot, 0).full == lv_color_hex(color).full);
            assert(lv_obj_get_style_bg_color(lv_obj_get_child(slot, 1), 0).full == lv_color_hex(color).full);
            const auto target = state.battery_valid ? full_width * percent / 100 : 0;
            auto* animation = lv_anim_get(current, nullptr);
            if (charging) {
                assert(animation && animation->start_value == 0 && animation->end_value == full_width);
                assert(animation->playback_time == 0 && !animation->playback_now);
                assert(lv_obj_get_width(current) >= 0 && lv_obj_get_width(current) <= full_width);
            } else {
                assert(!animation && lv_obj_get_width(current) == static_cast<int32_t>(target));
            }
            assert(current->coords.x1 > slot->coords.x1 && current->coords.x2 < slot->coords.x2);
        }
        auto* numbers = lv_obj_get_child(lv_obj_get_parent(lv_obj_get_parent(status_fill)), -1);
        assert(lv_obj_has_flag(numbers, LV_OBJ_FLAG_HIDDEN) == !state.battery_valid);
        if (state.battery_valid) {
            unsigned divisor = 100;
            for (unsigned digit = 0; digit < 3; ++digit, divisor /= 10) {
                auto* container = lv_obj_get_child(numbers, static_cast<int32_t>(digit));
                const bool hidden = digit == 0 ? percent < 100 : digit == 1 && percent < 10;
                assert(lv_obj_has_flag(container, LV_OBJ_FLAG_HIDDEN) == hidden);
                if (hidden)
                    continue;
                auto* number = lv_obj_get_child(container, 0);
                const auto y = -static_cast<int32_t>((percent / divisor % 10)
                                                     * lv_obj_get_style_text_font(number, 0)->line_height);
                auto* animation = lv_anim_get(number, nullptr);
                assert((animation ? animation->end_value : lv_obj_get_y(number)) == y);
            }
        }
    };
    // Force threshold boundaries/full/empty while charging, independently of the seed.
    for (bool charging : {false, true}) {
        state.charging = charging;
        for (unsigned percent : {0u, 9u, 10u, 19u, 20u, 49u, 50u, 100u, 101u}) {
            state.battery_percent = percent;
            check();
        }
    }
    // At full actual charge, both slots still animate from zero and the number stays 100.
    advance(10000); // Let Startup's idle timer expose StatusBar for the joint captures.
    state.battery_percent = 100;
    state.charging = false;
    check();
    state.charging = true;
    active->update_status(state);
    lv_obj_update_layout(lv_scr_act());
    lv_obj_update_layout(lv_layer_top());
    assert(lv_obj_get_width(fill) == 0 && lv_obj_get_width(status_fill) == 0);
    capture(display, output, "Startup-StatusBar-charge-empty", on_screen(label(lv_layer_top(), "DEMO")), 0);
    advance(600);
    assert(lv_obj_get_width(fill) >= 40 && lv_obj_get_width(fill) <= 50);
    assert(lv_obj_get_width(status_fill) >= 7 && lv_obj_get_width(status_fill) <= 9);
    capture(display, output, "Startup-StatusBar-charge-half", on_screen(label(lv_layer_top(), "DEMO")), 0);
    advance(650);
    assert(lv_obj_get_width(fill) == 90 && lv_obj_get_width(status_fill) == 16);
    capture(display, output, "Startup-StatusBar-charge-full", on_screen(label(lv_layer_top(), "DEMO")), 0);
    advance(350);
    assert(lv_obj_get_width(fill) < 15 && lv_obj_get_width(status_fill) < 3);
    assert(label(lv_obj_get_parent(lv_obj_get_parent(fill)), "100%"));
    capture(display, output, "Startup-StatusBar-charge-reset", on_screen(label(lv_layer_top(), "DEMO")), 0);
    unsigned levels = 0, modes = 0;
    for (unsigned event = 0; event < 128; ++event) {
        const std::uint64_t elapsed = event * std::uint64_t{3000};
        state = DemoStatus::sample_random(elapsed, 42);
        const auto repeat = DemoStatus::sample_random(elapsed + 2999, 42);
        assert(state.battery_percent <= 100 && state.battery_valid && state.satellites <= 20);
        assert(state.battery_voltage >= 6.80f && state.battery_voltage <= 8.40f);
        assert(state.battery_percent == repeat.battery_percent && state.charging == repeat.charging
               && state.battery_voltage == repeat.battery_voltage && state.wifi == repeat.wifi
               && state.recording == repeat.recording && state.satellites == repeat.satellites);
        levels |= 1u << (state.battery_percent >= 50   ? 3
                         : state.battery_percent >= 20 ? 2
                         : state.battery_percent >= 10 ? 1
                                                       : 0);
        modes |= state.charging ? 2u : 1u;
        check();
        bool moved = false, status_moved = false, saw_reset = false;
        auto previous_width = lv_obj_get_width(fill);
        const auto first_status_width = lv_obj_get_width(status_fill);
        // Keep publishing throughout two animation cycles, just like the SDL host loop.
        for (unsigned frame = 0; frame < 15; ++frame) {
            advance(160);
            check();
            const auto current_width = lv_obj_get_width(fill);
            moved |= current_width != previous_width;
            saw_reset |= current_width < previous_width;
            status_moved |= lv_obj_get_width(status_fill) != first_status_width;
            previous_width = current_width;
        }
        if (state.charging)
            assert(moved && status_moved && saw_reset); // Grow from empty to full and restart, even at 100%.
        if (event < 4)
            capture(display, output, "Startup-random-42-" + std::to_string(event),
                    on_screen(label(lv_layer_top(), "DEMO")), 0);
    }
    assert(levels == 15 && modes == 3);
    // Retained/cached or recreated Startup must restart charging after a return.
    state.battery_percent = 40;
    state.charging = true;
    for (unsigned visit = 0; visit < 2; ++visit) {
        check();
        assert(active->show_page("Pages/SystemInfos"));
        assert(!lv_anim_get(fill, nullptr));
        active->update_status(state); // An update during the outgoing transition must not restart its animation.
        assert(!lv_anim_get(fill, nullptr));
        advance(400);
        assert(active->back());
        advance(400);
        fill = battery_fill();
        check();
    }
    state.battery_valid = false;
    check(); // Invalid data cancels both charging animations and clears both slots.
    state.battery_valid = true;
    check();
    state.charging = false;
    check();
    std::puts("Startup/StatusBar random events: seed=42, 128 events, matching colors and 0-100% charging verified");
}
void boot_sequence(lv_disp_t* display, const std::string& output, const std::string& suffix) {
    const auto first_log = boot_logs.size();
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
        assert(red_nodes(lv_scr_act()) == (i >= 3 ? 1u : 0u));
        if (i == 3) {
            assert(lv_obj_get_style_text_color(headline, 0).full == lv_color_hex(0xd03c3b).full);
            assert(label(lv_scr_act(), i18n::text(i18n::TextId::InitializationFailedContinue)));
        }
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
    wait_for(i18n::TextId::ReadyWithWarnings);
    step_frames(260); // Failed nodes stay red throughout the completion animation.
    capture(display, output, "SystemLoading-Ready" + suffix, false, 0);
    assert(label(lv_scr_act(), "100%") && label(lv_scr_act(), i18n::text(i18n::TextId::StartupCompleted)));
    assert(label(lv_scr_act(), i18n::text(i18n::TextId::ServicesLimited)));
    assert(red_nodes(lv_scr_act()) == 1);
    assert(!label(lv_scr_act(), i18n::text(i18n::TextId::SystemReady)));
    auto* ready_panel = lv_obj_get_parent(label(lv_scr_act(), i18n::text(i18n::TextId::ReadyWithWarnings)));
    assert(lv_obj_get_style_border_width(ready_panel, 0) == 0);
    assert(lv_obj_get_style_outline_width(ready_panel, 0) == 0);
    step_frames(1200);
    advance(400);
    assert(std::strcmp(active->current_page(), "Pages/Dialplate") == 0);
    std::size_t cursor = first_log;
    const char* names[] = {"configuration", "services", "gnss", "network", "finalization"};
    for (unsigned i = 0; i < 5; ++i) {
        for (const char* outcome : {"begin", i == 3 ? "FAILED" : "OK"}) {
            char expected[96];
            std::snprintf(expected, sizeof(expected), "step %u/5 %s: %s [DEMO]", i + 1, names[i], outcome);
            while (cursor < boot_logs.size() && boot_logs[cursor].find(expected) == std::string::npos)
                ++cursor;
            assert(cursor < boot_logs.size()); // Each failed result must be followed by later begin/results.
            ++cursor;
        }
    }
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
    app_log_set_level(APP_LOG_INFO);
    app_log_set_sink(boot_log_sink, nullptr);
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
        assert(label(lv_scr_act(), "1.0.0") && label(lv_scr_act(), "85%") && label(lv_scr_act(), "7.60 V"));
        assert(!label(lv_scr_act(), "N/A"));
        assert(!on_screen(label(lv_layer_top(), "DEMO")));
        check_startup();
        capture(display, output, "Startup-hidden-en", false);
        advance(10000);
        capture(display, output, "Startup-idle-en");
        product_click(P4App::Key::Power);
        assert(std::strcmp(app.current_page(), "Pages/Startup") == 0 && label(lv_scr_act(), "0%"));
        app.on_key(P4App::Key::Power, true);
        sample_keys(500);
        assert(!label(lv_scr_act(), "0%"));
        capture(display, output, "Startup-hold-en");
        app.on_key(P4App::Key::Power, false);
        sample_keys(800);
        assert(std::strcmp(app.current_page(), "Pages/Startup") == 0 && label(lv_scr_act(), "0%"));
        app.on_key(P4App::Key::Power, true);
        sample_keys(500);
        app.cancel_input();
        sample_keys(2500);
        assert(std::strcmp(app.current_page(), "Pages/Startup") == 0 && label(lv_scr_act(), "0%"));
        product_start();
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
            assert(label(lv_scr_act(), "31.2304 N\n121.4737 E\n12.5 m"));
            assert(label(lv_scr_act(), "1.0.0\n02:15:30\n1\nDEMO-001"));
            assert(!label(lv_scr_act(), "N/A"));
            assert(!DataProc::Center()->SearchAccount("SystemLoadingModel"));
            assert(!label(lv_scr_act(), i18n::text(i18n::TextId::SystemInitialization)));
            assert(!label(lv_scr_act(), i18n::text(i18n::TextId::InitializationConfig)));
            assert(lv_group_get_obj_count(lv_group_get_default()) == 6);
            focused(i18n::TextId::SystemWorkTitle);
            product_click(P4App::Key::Function);
            focused(i18n::TextId::SystemGpsTitle); // One Next must advance to the next visible inventory section.
            input(P4App::InputAction::PreviousFocus);
            focused(i18n::TextId::SystemWorkTitle);
            auto* scroller = lv_obj_get_parent(lv_obj_get_parent(app.focused()));
            assert(lv_obj_get_height(scroller) == height && lv_obj_get_scroll_y(scroller) == 0);
            lv_coord_t previous_scroll_y = 0;
            for (unsigned card = 0; card < 6; ++card) {
                const auto scroll_y = lv_obj_get_scroll_y(scroller);
                assert(scroll_y >= previous_scroll_y); // Forward traversal must never scroll upward.
                previous_scroll_y = scroll_y;
                focused(inventory_titles[card]);
                capture(display, output, "SystemInfos-" + std::to_string(card) + suffix, false);
                product_click(P4App::Key::Function);
            }
            focused(i18n::TextId::SystemWorkTitle); // All controls cycle back to the start of the visible list.
            assert(lv_obj_get_scroll_y(scroller) == 0);
            input(P4App::InputAction::PreviousFocus);
            focused(i18n::TextId::SystemTitle);
            input(P4App::InputAction::NextFocus);
            focused(i18n::TextId::SystemWorkTitle);
            for (unsigned i = 0; i < 5; ++i)
                product_click(P4App::Key::Function);
            focused(i18n::TextId::SystemTitle);
            if (language == i18n::Language::English)
                product_click(P4App::Key::Power); // Select retains the existing return action.
            else
                product_click(P4App::Key::Function, true);
            assert(std::strcmp(app.current_page(), "Pages/SystemDash") == 0);
            assert(app.show_page("Pages/SystemInfos"));
            advance(400);
            focused(i18n::TextId::SystemWorkTitle);
            assert(label(lv_scr_act(), "1.0.0\n02:15:30\n1\nDEMO-001"));
            assert(lv_group_get_obj_count(lv_group_get_default()) == 6);
            capture(display, output, "SystemInfos-Reopen" + suffix, false);
            input(P4App::InputAction::Back);
            assert(!on_screen(label(lv_layer_top(), "DEMO")));
            assert(app.show_page("Pages/StarMap"));
            advance(600);
            assert(app.focused() && lv_obj_has_state(app.focused(), LV_STATE_FOCUSED));
            assert(lv_obj_get_style_outline_width(app.focused(), 0) == 0);
            assert(lv_obj_get_style_border_width(app.focused(), 0) == 0);
            capture(display, output, "StarMap-no-outline" + suffix);
            assert(label(lv_scr_act(), "4") && label(lv_scr_act(), "3") && label(lv_scr_act(), "DEMO"));
            assert(!label(lv_scr_act(), "-"));
            input(P4App::InputAction::Confirm);
            assert(std::strcmp(app.current_page(), "Pages/SystemDash") == 0);
        }
        product_click(P4App::Key::Power);
        product_click(P4App::Key::Function);
        product_click(P4App::Key::Power, true);
        assert(std::strcmp(app.current_page(), "Pages/SaveConfig") == 0);
        product_click(P4App::Key::Function, true); // Cancel before the simulated service runs.
        assert(!SystemService::snapshot().power_pending && SystemService::snapshot().power_calls == 0);
        product_click(P4App::Key::Function, true); // Close the retained power modal.
        // Both committed actions run only after SaveConfig's timer; cancel also clears the transaction.
        for (auto action : {DataProc::PowerAction::Off, DataProc::PowerAction::Reboot}) {
            product_click(P4App::Key::Power);
            product_click(P4App::Key::Function);
            if (action == DataProc::PowerAction::Reboot)
                product_click(P4App::Key::Function);
            const auto calls = SystemService::snapshot().power_calls;
            product_click(P4App::Key::Power); // A single click selects but must not commit either power action.
            assert(std::strcmp(app.current_page(), "Pages/SystemDash") == 0 && !SystemService::snapshot().power_pending
                   && SystemService::snapshot().power_calls == calls);
            product_click(P4App::Key::Power, true);
            assert(std::strcmp(app.current_page(), "Pages/SaveConfig") == 0
                   && SystemService::snapshot().power_calls == calls);
            product_click(P4App::Key::Power, true); // Duplicate commit cannot create another request.
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
                product_click(P4App::Key::Power); // Short click cannot restart after shutdown.
                assert(std::strcmp(app.current_page(), "Pages/Startup") == 0);
                product_start();
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
    // A fresh application before any startup reports zero initialization errors and six inventory controls.
    for (auto language : {i18n::Language::English, i18n::Language::Russian}) {
        i18n::set_language(language);
        P4App app;
        active = &app;
        assert(app.init());
        advance(400);
        check_startup();
        const std::string suffix = language == i18n::Language::English ? "-en" : "-ru";
        capture(display, output, "Startup-modern" + suffix, false);
        assert(app.show_page("Pages/SystemInfos"));
        advance(400);
        assert(label(lv_scr_act(), "1.0.0\n02:15:30\n0\nDEMO-001"));
        assert(lv_group_get_obj_count(lv_group_get_default()) == 6);
        assert(!label(lv_scr_act(), i18n::text(i18n::TextId::SystemInitialization)));
        for (unsigned section = 0; section < 6; ++section) {
            focused(inventory_titles[section]);
            product_click(P4App::Key::Function);
        }
        focused(i18n::TextId::SystemWorkTitle);
        product_click(P4App::Key::Function, true);
        assert(std::strcmp(app.current_page(), "Pages/Startup") == 0);
        active = nullptr;
    }
    assert(DataProc::Center()->GetAccountLen() == 0);
    app_log_set_sink(nullptr, nullptr);
    // The View also handles all-success and multiple-failure outcomes, then resets on fresh initialization.
    auto* canvas = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(canvas);
    lv_obj_set_size(canvas, width, height);
    page::SystemLoadingView view;
    view.create(canvas);
    for (bool fail : {false, true}) {
        view.show_logo();
        view.show_initialization();
        assert(!view.has_failures() && red_nodes(canvas) == 0);
        for (unsigned i = 0; i < page::SystemLoadingView::kStepCount; ++i) {
            view.show_step(i, !(fail && (i == 1 || i == 3)));
            advance(page::SystemLoadingView::kStepMs);
        }
        view.show_ready();
        advance(260);
        assert(view.has_failures() == fail && red_nodes(canvas) == (fail ? 2u : 0u));
        assert(label(canvas, i18n::text(fail ? i18n::TextId::ReadyWithWarnings : i18n::TextId::SystemReady)));
        view.destroy();
    }
    lv_obj_del(canvas);
    // Test battery levels and charging animations on Startup page
    {
        P4App app;
        active = &app;
        assert(app.init());
        advance(400);
        assert(std::strcmp(app.current_page(), "Pages/Startup") == 0);

        // 1. High battery (≥50%): Green #28C76F, BAT label, no charging
        page::StatusBarState state{};
        state.battery_valid = true;
        state.battery_percent = 85;
        state.charging = false;
        state.battery_voltage = 7.60f;
        app.update_status(state);
        advance(50);
        assert(label(lv_scr_act(), "85%") && label(lv_scr_act(), "7.60 V") && label(lv_scr_act(), "BAT"));
        capture(display, output, "Startup-battery-85-green", false);

        // 2. Medium battery (20%~49%): Amber #FF931E
        state.battery_percent = 35;
        state.battery_voltage = 7.45f;
        app.update_status(state);
        advance(50);
        assert(label(lv_scr_act(), "35%") && label(lv_scr_act(), "7.45 V") && label(lv_scr_act(), "BAT"));
        capture(display, output, "Startup-battery-35-orange", false);

        // 3. Low battery (10%~19%): Red #EA5455
        state.battery_percent = 15;
        state.battery_voltage = 7.20f;
        app.update_status(state);
        advance(50);
        assert(label(lv_scr_act(), "15%") && label(lv_scr_act(), "7.20 V") && label(lv_scr_act(), "BAT"));
        capture(display, output, "Startup-battery-15-red", false);

        // 4. Critical low battery (<10%): Dark red #D03C3B
        state.battery_percent = 5;
        state.battery_voltage = 6.90f;
        app.update_status(state);
        advance(50);
        assert(label(lv_scr_act(), "5%") && label(lv_scr_act(), "6.90 V") && label(lv_scr_act(), "BAT"));
        capture(display, output, "Startup-battery-5-critical", false);

        // 5. Charging state: CHG label, animated fill progressing towards full width
        state.battery_percent = 40;
        state.battery_voltage = 7.80f;
        state.charging = true;
        app.update_status(state);
        advance(50);
        assert(label(lv_scr_act(), "40%") && label(lv_scr_act(), "7.80 V") && label(lv_scr_act(), "CHG"));
        capture(display, output, "Startup-battery-charging-start", false);
        advance(600); // Wait mid-animation
        capture(display, output, "Startup-battery-charging-mid", false);
        advance(600); // Completion of one charge cycle
        capture(display, output, "Startup-battery-charging-full", false);

        // 6. Unplug charger: returns to BAT, static width
        state.charging = false;
        app.update_status(state);
        advance(50);
        assert(label(lv_scr_act(), "40%") && label(lv_scr_act(), "7.80 V") && label(lv_scr_act(), "BAT"));
        capture(display, output, "Startup-battery-unplugged", false);
        random_battery_events(display, output);

        active = nullptr;
    }
    assert(DataProc::Center()->GetAccountLen() == 0);
    lv_disp_remove(display);
}
