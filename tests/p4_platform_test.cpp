// Platform-decided power flow: power-on confirmed by the platform, no Reboot, platform shutdown and standby.
#include <cassert>
#include <cstdio>
#include <cstring>
#include "Common/SystemBackend.h"
#include "Common/SystemService.h"
#include "P4App.h"
#include "Resource/ResourcePool.h"
#include "Utils/I18n/I18n.h"

namespace {
constexpr int width = 294, height = 126;
lv_color_t pixels[width * 20];
unsigned power_on_requests = 0, off_calls = 0, reboot_calls = 0;
P4App* active = nullptr;

void flush(lv_disp_drv_t* driver, const lv_area_t*, lv_color_t*) {
    lv_disp_flush_ready(driver);
}
void advance(unsigned ms) {
    for (unsigned i = 0; i < ms; i += 10) {
        lv_tick_inc(10);
        lv_timer_handler();
    }
}
void sample_keys(unsigned ms) {
    for (unsigned i = 0; i < ms; i += 10) {
        advance(10);
        active->poll_keys(lv_tick_get());
    }
}
lv_obj_t* find_text(lv_obj_t* root, const char* text) {
    if (lv_obj_has_flag(root, LV_OBJ_FLAG_HIDDEN))
        return nullptr;
    if (lv_obj_check_type(root, &lv_label_class) && std::strstr(lv_label_get_text(root), text))
        return root;
    for (unsigned i = 0; i < lv_obj_get_child_cnt(root); ++i)
        if (auto* found = find_text(lv_obj_get_child(root, static_cast<int32_t>(i)), text))
            return found;
    return nullptr;
}
bool on_page(const char* name) {
    return active->current_page() && std::strcmp(active->current_page(), name) == 0;
}
void publish_status(bool temperature_valid = true, float temperature = 30.0f) {
    page::StatusBarState status;
    status.position = "RTK FIX"; // Real data: no "DEMO" text anywhere on screen.
    status.battery_valid = true;
    status.battery_percent = 60;
    status.battery_temperature_valid = temperature_valid;
    status.battery_temperature = temperature;
    active->update_status(status);
}
bool banner_shown() {
    return find_text(lv_layer_top(), "BATTERY HOT");
}
void boot_to_dialplate() {
    assert(on_page("Pages/Startup"));
    assert(SystemService::set_powered_on(true)); // Platform decided (e.g. HAL key timing).
    advance(200);
    assert(on_page("Pages/SystemLoading"));
    advance(7000); // Logo, five steps, completion and the fade to the working root.
    assert(on_page("Pages/Dialplate"));
}
} // namespace

// Test platform: power-on is decided outside the UI, Reboot is not offered, data is real.
system_backend::Capabilities system_backend::capabilities() {
    return {.simulated = false,
            .reboot_available = false,
            .wifi_available = false,
            .star_map = false,
            .record_settings = false};
}
bool system_backend::request_power_on() {
    ++power_on_requests;
    return false;
}
bool system_backend::execute_power(DataProc::PowerAction action) {
    ++(action == DataProc::PowerAction::Off ? off_calls : reboot_calls);
    return true; // Completion is reported later with SystemService::enter_standby().
}
i18n::TextId system_backend::initialization_title(unsigned step) {
    constexpr i18n::TextId titles[] = {i18n::TextId::LoadingConfiguration, i18n::TextId::CheckingBatteryGauge,
                                       i18n::TextId::CheckingCharger, i18n::TextId::PoweringHostBoard,
                                       i18n::TextId::FinalizingStartup};
    return titles[step];
}
const char* system_backend::initialization_name(unsigned) {
    return "device";
}
bool system_backend::initialize_step(unsigned step, char* detail, std::size_t size) {
    std::snprintf(detail, size, "device %u online", step + 1);
    return true;
}

int main() {
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
        assert(app.init({.battery_warning = true}));
        publish_status();
        advance(400);
        assert(on_page("Pages/Startup"));
        assert(!SystemService::snapshot().simulated && !find_text(lv_scr_act(), "DEMO"));

        // A completed hold only requests power-on; without confirmation the page stays and resets on release.
        sample_keys(40);
        app.on_key(P4App::Key::Power, true);
        sample_keys(2200);
        assert(power_on_requests == 1 && on_page("Pages/Startup"));
        app.on_key(P4App::Key::Power, false);
        sample_keys(600);
        assert(on_page("Pages/Startup") && find_text(lv_scr_act(), "0%"));

        // Platform step titles fit the 246px SystemLoading headline in both languages.
        for (auto language : {i18n::Language::English, i18n::Language::Russian}) {
            assert(i18n::set_language(language) || i18n::get_language() == language);
            for (unsigned step = 0; step < DataProc::kInitializationStepCount; ++step) {
                const char* title = i18n::text(system_backend::initialization_title(step));
                const auto* font = resource_pool::get_font("oswaldBold_18");
                assert(lv_txt_get_width(title, std::strlen(title), font, 0, LV_TEXT_FLAG_NONE) <= 246);
            }
        }
        assert(i18n::set_language(i18n::Language::English));

        boot_to_dialplate();
        assert(!find_text(lv_scr_act(), "DEMO"));

        // No StarMap: not registered, and the Dialplate satellite icon is not a focus stop.
        assert(!app.show_page("Pages/StarMap") && on_page("Pages/Dialplate"));
        {
            lv_obj_t* start = app.focused();
            unsigned stops = 0;
            do {
                app.on_input(P4App::InputAction::NextFocus);
                ++stops;
            } while (app.focused() != start && stops < 10);
            assert(stops == 4); // Work, Record, Menu, Power.
        }
        // Display-only record settings: only Return is focusable on RecordConfig.
        assert(app.show_page("Pages/RecordConfig"));
        advance(500);
        {
            lv_obj_t* only = app.focused();
            assert(only);
            app.on_input(P4App::InputAction::NextFocus);
            assert(app.focused() == only);
        }
        assert(app.back());
        advance(500);
        assert(on_page("Pages/Dialplate"));

        // Over-temperature banner with hysteresis: 65 C on, below 60 C off, invalid data off.
        assert(!banner_shown());
        publish_status(true, 66.0f);
        advance(300);
        assert(banner_shown() && find_text(lv_layer_top(), "66.0"));
        publish_status(true, 62.0f);
        advance(100);
        assert(banner_shown() && find_text(lv_layer_top(), "62.0"));
        publish_status(true, 59.5f);
        advance(100);
        assert(!banner_shown());
        publish_status(true, 70.0f);
        advance(100);
        assert(banner_shown());
        publish_status(false, 70.0f);
        advance(100);
        assert(!banner_shown());
        publish_status();

        // Power dialog without Reboot: focus cycles between Shutdown and Cancel only.
        assert(app.show_page("Pages/SystemDash"));
        advance(400);
        app.on_input(P4App::InputAction::Confirm); // Power tile is focused first.
        advance(100);
        assert(find_text(lv_scr_act(), i18n::text(i18n::TextId::ShutdownDouble)));
        assert(!find_text(lv_scr_act(), i18n::text(i18n::TextId::RebootDouble)));
        lv_obj_t* cancel = app.focused(); // The dialog defaults to Cancel.
        assert(find_text(cancel, i18n::text(i18n::TextId::Cancel)));
        app.on_input(P4App::InputAction::NextFocus);
        assert(find_text(app.focused(), i18n::text(i18n::TextId::ShutdownDouble)));
        app.on_input(P4App::InputAction::NextFocus);
        assert(app.focused() == cancel); // Two entries only.
        app.on_input(P4App::InputAction::NextFocus);
        assert(find_text(app.focused(), i18n::text(i18n::TextId::ShutdownDouble)));

        // User shutdown: committed when SaveConfig ends, then the page waits for the platform's standby.
        app.on_input(P4App::InputAction::Commit);
        advance(400);
        assert(on_page("Pages/SaveConfig"));
        advance(10000);
        assert(off_calls == 1 && reboot_calls == 0 && on_page("Pages/SaveConfig"));
        assert(SystemService::begin_shutdown(DataProc::PowerCause::User)); // e.g. HAL ShutdownGoing.
        advance(100);
        assert(app.back() && on_page("Pages/SaveConfig")); // Back is consumed during a shutdown.
        assert(SystemService::enter_standby());
        advance(600);
        assert(on_page("Pages/Startup") && !app.back());

        // Platform shutdown (host request): SaveConfig shows the cause, cannot be left, standby returns.
        boot_to_dialplate();
        assert(SystemService::begin_shutdown(DataProc::PowerCause::Host));
        advance(600);
        assert(on_page("Pages/SaveConfig"));
        assert(find_text(lv_scr_act(), i18n::text(i18n::TextId::SaveConfigPowerOffBoard)));
        assert(app.back() && on_page("Pages/SaveConfig"));
        advance(10000);
        assert(off_calls == 1 && on_page("Pages/SaveConfig")); // Nothing is executed for a platform shutdown.
        assert(SystemService::enter_standby());
        advance(600);
        assert(on_page("Pages/Startup"));
        assert(!SystemService::begin_shutdown(DataProc::PowerCause::Forced)); // Already in standby.
        active = nullptr;
    }
    lv_disp_remove(display);
    std::puts("p4_platform: OK");
    return 0;
}
