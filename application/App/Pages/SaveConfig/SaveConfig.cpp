#include "SaveConfig.h"
#include <cstdlib>
#include "Utils/PageManager/PageManager.h"
#include "Utils/PageManager/PageUtils.h"

using namespace page;

void SaveConfig::on_custom_attr_config() {
    set_custom_load_anim_type(PageManager::LOAD_ANIM_MOVE_TOP, 320, lv_anim_path_ease_in_out);
}

void SaveConfig::on_view_load() {
    if (!model_.init())
        std::abort();
    view_.create(root);
}

void SaveConfig::on_view_will_appear() {
    model_.set_status_bar(true);
    lv_anim_timeline_start(view_.ui.anim_timeline);
}

void SaveConfig::on_view_did_appear() {
    // A long host frame cannot execute power completion while navigation is still busy.
    lv_anim_start(&view_.ui.sync.bar.anim);
    timer_ = lv_timer_create(on_timeout, lv_anim_get_playtime(&view_.ui.sync.bar.anim) + 500, this);
    if (!timer_)
        LV_LOG_ERROR("SaveConfig: preview timer allocation failed");
}

void SaveConfig::cancel_timer() {
    if (timer_) {
        lv_timer_del(timer_);
        timer_ = nullptr;
    }
}

void SaveConfig::on_view_will_disappear() {
    cancel_timer();
    if (model_.settings().power_pending && !model_.cancel_power())
        LV_LOG_ERROR("SaveConfig: cancellation failed");
    utils::leave_focus_group();
}

void SaveConfig::on_view_unload() {
    model_.deinit();
    cancel_timer();
    view_.destroy();
}

void SaveConfig::on_language_changed() {
    view_.apply_language();
}

void SaveConfig::on_timeout(lv_timer_t* timer) {
    auto* self = static_cast<SaveConfig*>(timer->user_data);
    self->timer_ = nullptr;
    lv_timer_del(timer);
    const auto request = self->model_.settings(); // Copy before navigation unloads this Model/View.
    if (request.power_pending && !self->model_.finish_power()) {
        LV_LOG_ERROR("SaveConfig: simulated power service failed");
        return;
    }
    // If power remains on, Off returns to standby Startup; Reboot resumes the working root.
    const bool navigated = request.power_pending && request.power_action == DataProc::PowerAction::Off
                               ? self->page_manager->reset_root("Pages/Startup")
                               : self->page_manager->back_home();
    if (!navigated)
        LV_LOG_WARN("SaveConfig: preview transition rejected");
}
