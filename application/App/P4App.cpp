#include "P4App.h"
#include <cstdio>
#include "Common/SystemService.h"
#include "Utils/Log/Log.h"

#include "Pages/Dialplate/Dialplate.h"
#include "Pages/RecordConfig/RecordConfig.h"
#include "Pages/SaveConfig/SaveConfig.h"
#include "Pages/StarMap/StarMap.h"
#include "Pages/Startup/Startup.h"
#include "Pages/SystemDash/SystemDash.h"
#include "Pages/SystemInfos/SystemInfos.h"
#include "Pages/SystemLoading/SystemLoading.h"
#include "Pages/SystemSettings/SystemSettings.h"
#include "Pages/WorkSettings/WorkSettings.h"
#include "Resource/ResourcePool.h"

#include <cstring>

bool P4App::init() {
    resource_pool::init();
    if (!DataProc_Init())
        return false;
    data_ready_ = true;
    group_ = lv_group_create();
    if (!group_)
        return false;
    lv_group_set_default(group_);
    lv_style_init(&root_style_);
    style_ready_ = true;
    lv_style_set_width(&root_style_, LV_HOR_RES);
    lv_style_set_height(&root_style_, LV_VER_RES);
    lv_style_set_bg_color(&root_style_, lv_color_black());
    lv_style_set_pad_all(&root_style_, 0);
    lv_style_set_border_width(&root_style_, 0);
    lv_style_set_radius(&root_style_, 0);

    manager_ = std::make_unique<PageManager>();
    manager_->set_root_default_style(&root_style_);
    // Default forward navigation is a smooth horizontal push; pages override this for boot/modal/context changes.
    manager_->set_global_load_anim_type(PageManager::LOAD_ANIM_MOVE_LEFT, 320, lv_anim_path_ease_in_out);
    auto register_page = [this](PageBase* page, const char* name) {
        if (!manager_->register_page(page, name)) {
            delete page;
            return false;
        }
        page->on_custom_attr_config();
        return true;
    };
    if (!register_page(new page::Dialplate, "Pages/Dialplate")
        || !register_page(new page::WorkSettings, "Pages/WorkSettings")
        || !register_page(new page::RecordConfig, "Pages/RecordConfig")
        || !register_page(new page::SystemInfos, "Pages/SystemInfos")
        || !register_page(new page::SystemDash, "Pages/SystemDash")
        || !register_page(new page::SystemSettings, "Pages/SystemSettings")
        || !register_page(new page::StarMap, "Pages/StarMap") || !register_page(new page::Startup, "Pages/Startup")
        || !register_page(new page::SystemLoading, "Pages/SystemLoading")
        || !register_page(new page::SaveConfig, "Pages/SaveConfig"))
        return false;
    if (!status_bar_.create(lv_layer_top()))
        return false;
    language_ = i18n::get_language();
    account_ = std::make_unique<Account>("Application", DataProc::Center(), 0, this);
    account_->SetEventCallback(on_system_event);
    if (!account_->IsRegistered() || !account_->Subscribe("System"))
        return false;
    return manager_->push("Pages/Startup");
}

P4App::~P4App() {
    manager_.reset();
    account_.reset();
    status_bar_.destroy();
    if (data_ready_ && !DataProc_Deinit())
        APP_LOG_E("Application", "provider cleanup failed");
    if (group_) {
        lv_group_set_default(nullptr);
        lv_group_del(group_);
    }
    if (style_ready_)
        lv_style_reset(&root_style_);
}

int P4App::on_system_event(Account* account, Account::EventParam_t* event) {
    if (!event || event->event != Account::EVENT_PUB_PUBLISH)
        return Account::RES_UNSUPPORTED_REQUEST;
    DataProc::SystemState state;
    const int result = DataProc::ReadPayload(event, state);
    if (result != Account::RES_OK)
        return result;
    auto* self = static_cast<P4App*>(account->UserData);
    if (state.language != self->language_) {
        self->language_ = state.language;
        self->manager_->notify_language_changed();
    }
    return Account::RES_OK;
}

void P4App::update_status(const page::StatusBarState& state) {
    DataProc::StatusSnapshot snapshot;
    std::snprintf(snapshot.position.data(), snapshot.position.size(), "%s", state.position ? state.position : "DEMO");
    snapshot.position_color_rgb = lv_color_to32(state.position_color) & 0xffffffu;
    snapshot.satellites = state.satellites;
    snapshot.satellites_valid = state.satellites_valid;
    snapshot.battery_percent = state.battery_percent;
    snapshot.battery_valid = state.battery_valid;
    snapshot.hour = state.hour;
    snapshot.minute = state.minute;
    snapshot.second = state.second;
    snapshot.clock_valid = state.clock_valid;
    snapshot.recording = state.recording;
    snapshot.wifi = state.wifi;
    if (!StatusService::update(snapshot))
        APP_LOG_E("Application", "status publication rejected");
}

void P4App::on_input(const InputAction action) {
    if (!group_ || !manager_ || manager_->is_switching())
        return;
    cancel_keys();
    auto* current = manager_->get_current_page();
    if (action == InputAction::Press && pressed_ == focused() && pressed_page_ == current_page())
        return; // Auto-repeat must not restart a hold.
    if (action == InputAction::NextFocus && current->on_next_request())
        return;
    if (action == InputAction::Confirm && current->on_enter_request())
        return;
    if (action == InputAction::Commit) {
        current->on_commit_request();
        return;
    }
    const char* page = current_page();
    if (action == InputAction::Release) {
        lv_obj_t* pressed = pressed_;
        const char* pressed_page = pressed_page_;
        pressed_ = nullptr;
        pressed_page_ = nullptr;
        if (pressed && page && std::strcmp(page, pressed_page) == 0 && lv_obj_is_valid(pressed)) {
            lv_obj_clear_state(pressed, LV_STATE_PRESSED);
            lv_event_send(pressed, LV_EVENT_RELEASED, nullptr);
            if (lv_group_get_focused(group_) == pressed && !current->on_enter_request())
                lv_event_send(pressed, LV_EVENT_SHORT_CLICKED, nullptr);
        }
        return;
    }
    if (pressed_ && action != InputAction::None) {
        if (page && std::strcmp(page, pressed_page_) == 0 && lv_obj_is_valid(pressed_)) {
            lv_obj_clear_state(pressed_, LV_STATE_PRESSED);
            lv_event_send(pressed_, LV_EVENT_PRESS_LOST, nullptr);
        }
        pressed_ = nullptr;
        pressed_page_ = nullptr;
    }
    if (action == InputAction::NextFocus)
        lv_group_focus_next(group_);
    else if (action == InputAction::PreviousFocus)
        lv_group_focus_prev(group_);
    else if (action == InputAction::Confirm || action == InputAction::Press) {
        if (lv_obj_t* focused = lv_group_get_focused(group_)) {
            // Uniform input dispatch; each page decides whether Press is a hold or a normal click.
            if (action == InputAction::Press && page) {
                pressed_ = focused;
                pressed_page_ = page;
                lv_obj_add_state(focused, LV_STATE_PRESSED);
                lv_event_send(focused, LV_EVENT_PRESSED, nullptr);
            } else {
                lv_event_send(focused, LV_EVENT_SHORT_CLICKED, nullptr);
            }
        }
    } else if (action == InputAction::Back) {
        back();
    }
}

void P4App::cancel_input() {
    power_down_ = function_down_ = false;
    cancel_keys();
    auto* pressed = pressed_;
    const auto* page = pressed_page_;
    // Pointer presses are owned by LVGL, not pressed_; reset alone does not emit PRESS_LOST.
    if (auto* focus = focused(); focus && lv_obj_has_state(focus, LV_STATE_PRESSED)) {
        pressed = focus;
        page = current_page();
    }
    pressed_ = nullptr;
    pressed_page_ = nullptr;
    if (pressed && page && current_page() && std::strcmp(page, current_page()) == 0 && lv_obj_is_valid(pressed)) {
        lv_obj_clear_state(pressed, LV_STATE_PRESSED);
        lv_event_send(pressed, LV_EVENT_PRESS_LOST, nullptr);
    }
}

void P4App::cancel_keys() {
    power_gesture_.cancel(power_down_);
    function_gesture_.cancel(function_down_);
    key_page_ = current_page();
}
void P4App::on_key(Key key, bool pressed) {
    if (key == Key::Power)
        power_down_ = pressed;
    else
        function_down_ = pressed;
}
void P4App::poll_keys(std::uint64_t now_ms) {
    if (!manager_ || manager_->is_switching() || key_page_ != current_page()) {
        cancel_keys();
        return;
    }
    const auto power = power_gesture_.sample(power_down_, now_ms);
    const auto function = function_gesture_.sample(function_down_, now_ms);
    if (power != ButtonGesture::Action::None)
        on_input(power == ButtonGesture::Action::Single ? InputAction::Confirm : InputAction::Commit);
    else if (function != ButtonGesture::Action::None)
        on_input(function == ButtonGesture::Action::Single ? InputAction::NextFocus : InputAction::Back);
}

const char* P4App::current_page() const {
    if (!manager_)
        return nullptr;
    PageBase* page = manager_->get_current_page();
    return page ? page->page_name : nullptr;
}

bool P4App::show_page(const char* name) {
    if (!manager_ || !name)
        return false;
    const char* current = current_page();
    cancel_keys();
    return (current && std::strcmp(current, name) == 0) || manager_->push(name);
}

bool P4App::back() {
    if (!manager_ || manager_->is_switching())
        return false;
    cancel_keys();
    if (auto* current = manager_->get_current_page(); current && current->on_back_request())
        return true;
    return manager_->pop();
}

lv_obj_t* P4App::focused() const {
    return group_ ? lv_group_get_focused(group_) : nullptr;
}
