#include "P4App.h"

#include "Pages/Dialplate/DialplateView.h"
#include "Pages/RecordConfig/RecordConfigView.h"
#include "Pages/SaveConfig/SaveConfigView.h"
#include "Pages/Shutdown/ShutdownView.h"
#include "Pages/StarMap/StarMapView.h"
#include "Pages/Startup/StartupView.h"
#include "Pages/SystemInfos/SystemInfosView.h"
#include "Pages/SystemLoading/SystemLoadingView.h"
#include "Pages/WorkSettings/WorkSettingsView.h"
#include "Resource/ResourcePool.h"
#include "Utils/I18n/I18n.h"

#include <array>
#include <cstring>
#include <type_traits>

namespace {

class DialplatePage final : public PageBase {
  public:
    void on_custom_attr_config() override {
        set_custom_load_anim_type(PageManager::LOAD_ANIM_MOVE_LEFT, 250, lv_anim_path_ease_out);
    }
    void on_view_load() override {
        view_.create(root);
        lv_obj_t* buttons[] = {view_.ui.btn_cont.btn_map, view_.ui.btn_cont.btn_rec, view_.ui.btn_cont.btn_menu,
                               view_.ui.btn_cont.btn_shutdown, view_.ui.top_info.icon_satellite};
        for (lv_obj_t* button : buttons) {
            lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(button, clicked, LV_EVENT_SHORT_CLICKED, this);
        }
    }
    void on_view_will_appear() override {
        lv_group_t* group = lv_group_get_default();
        lv_group_set_wrap(group, true);
        lv_group_add_obj(group, view_.ui.btn_cont.btn_map);
        lv_group_add_obj(group, view_.ui.btn_cont.btn_rec);
        lv_group_add_obj(group, view_.ui.btn_cont.btn_menu);
        lv_group_add_obj(group, view_.ui.btn_cont.btn_shutdown);
        lv_group_add_obj(group, view_.ui.top_info.icon_satellite);
        lv_group_focus_obj(last_focus_ ? last_focus_ : view_.ui.btn_cont.btn_menu);
        view_.appear_anim_start();
    }
    void on_view_will_disappear() override {
        last_focus_ = lv_group_get_focused(lv_group_get_default());
        lv_group_remove_all_objs(lv_group_get_default());
    }
    void on_view_unload() override {
        view_.destroy();
        last_focus_ = nullptr;
    }

  private:
    static void clicked(lv_event_t* event) {
        auto* self = static_cast<DialplatePage*>(lv_event_get_user_data(event));
        const lv_obj_t* button = lv_event_get_target(event);
        const char* destination = nullptr;
        if (button == self->view_.ui.btn_cont.btn_map)
            destination = "Pages/WorkSettings";
        else if (button == self->view_.ui.btn_cont.btn_rec)
            destination = "Pages/RecordConfig";
        else if (button == self->view_.ui.btn_cont.btn_menu)
            destination = "Pages/SystemInfos";
        else if (button == self->view_.ui.btn_cont.btn_shutdown)
            destination = "Pages/Shutdown";
        else if (button == self->view_.ui.top_info.icon_satellite)
            destination = "Pages/StarMap";
        if (destination)
            self->page_manager->push(destination);
    }
    page::DialplateView view_{};
    lv_obj_t* last_focus_ = nullptr;
};

template <typename View> class ViewPage final : public PageBase {
  public:
    void on_custom_attr_config() override {
        set_custom_load_anim_type(PageManager::LOAD_ANIM_MOVE_LEFT, 250, lv_anim_path_ease_out);
    }
    void on_view_load() override {
        view_.create(root);
        // 尚无业务后端的按钮不能表现为可操作；其余控件只处理界面交互。
        if constexpr (std::is_same_v<View, page::WorkSettingsView>) {
            lv_obj_add_state(view_.ui.btn_cont.btn_base, LV_STATE_DISABLED);
            lv_obj_add_state(view_.ui.btn_cont.btn_rover, LV_STATE_DISABLED);
            lv_obj_add_state(view_.ui.btn_cont.btn_ntrip, LV_STATE_DISABLED);
        } else if constexpr (std::is_same_v<View, page::RecordConfigView>) {
            lv_obj_add_state(view_.ui.btn_cont.btn_record, LV_STATE_DISABLED);
        } else if constexpr (std::is_same_v<View, page::ShutdownView>) {
            lv_obj_add_state(view_.ui.shutdown.btn_wifi, LV_STATE_DISABLED);
        }
        for (lv_obj_t* obj : controls()) {
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(obj, clicked, LV_EVENT_SHORT_CLICKED, this);
        }
        if constexpr (std::is_same_v<View, page::StartupView> || std::is_same_v<View, page::ShutdownView>) {
            lv_obj_t* progress;
            lv_anim_t* animation;
            if constexpr (std::is_same_v<View, page::StartupView>) {
                progress = view_.ui.arc;
                animation = &view_.ui.arc_anim;
                lv_obj_add_event_cb(view_.ui.btn_press, hold_event, LV_EVENT_ALL, this);
            } else {
                progress = view_.ui.shutdown.bar.obj;
                animation = &view_.ui.shutdown.bar.anim;
                lv_obj_add_event_cb(view_.ui.shutdown.btn_press, hold_event, LV_EVENT_ALL, this);
            }
            lv_obj_set_user_data(progress, this);
            lv_anim_set_exec_cb(animation, set_progress);
        }
    }
    void on_view_will_appear() override {
        lv_group_t* group = lv_group_get_default();
        lv_group_set_wrap(group, true);
        if constexpr (std::is_same_v<View, page::SystemInfosView>) {
            // 前一页在消失时清空焦点组；信息页必须在此之后注册。
            view_.group_init();
        } else {
            for (lv_obj_t* obj : controls())
                lv_group_add_obj(group, obj);
            if constexpr (std::is_same_v<View, page::WorkSettingsView>) {
                lv_group_focus_obj(last_focus_ ? last_focus_ : view_.ui.roller.btn_reset);
            } else if constexpr (std::is_same_v<View, page::RecordConfigView>) {
                lv_group_focus_obj(last_focus_ ? last_focus_ : view_.ui.btn_cont.btn_return);
            } else if constexpr (std::is_same_v<View, page::ShutdownView>) {
                set_progress(view_.ui.shutdown.bar.obj, 0);
                lv_obj_clear_state(view_.ui.shutdown.btn_press, LV_STATE_PRESSED);
                lv_group_focus_obj(last_focus_ ? last_focus_ : view_.ui.shutdown.btn_press);
            } else if constexpr (std::is_same_v<View, page::StarMapView>) {
                lv_group_focus_obj(root);
            } else if constexpr (std::is_same_v<View, page::StartupView>) {
                set_progress(view_.ui.arc, 0);
                lv_obj_clear_state(view_.ui.btn_press, LV_STATE_PRESSED);
                lv_group_focus_obj(view_.ui.btn_press);
            }
        }
        if constexpr (std::is_same_v<View, page::SystemLoadingView>) {
            lv_anim_timeline_start(view_.ui.anim_timeline);
            lv_anim_start(&view_.ui.bar_anim);
            timer_ = lv_timer_create(on_timeout, lv_anim_get_playtime(&view_.ui.bar_anim) + 200, this);
        } else if constexpr (std::is_same_v<View, page::SaveConfigView>) {
            lv_anim_timeline_start(view_.ui.anim_timeline);
            lv_anim_start(&view_.ui.sync.bar.anim);
            timer_ = lv_timer_create(on_timeout, lv_anim_get_playtime(&view_.ui.sync.bar.anim) + 500, this);
        }
        if constexpr (std::is_same_v<View, page::SystemLoadingView> || std::is_same_v<View, page::SaveConfigView>) {
            if (!timer_)
                LV_LOG_ERROR("Page %s: preview timer allocation failed", page_name);
        }
    }
    void on_view_will_disappear() override {
        if (timer_) {
            lv_timer_del(timer_);
            timer_ = nullptr;
        }
        if constexpr (std::is_same_v<View, page::StartupView>)
            lv_anim_del(view_.ui.arc, set_progress);
        else if constexpr (std::is_same_v<View, page::ShutdownView>)
            lv_anim_del(view_.ui.shutdown.bar.obj, set_progress);
        lv_group_t* group = lv_group_get_default();
        if constexpr (std::is_same_v<View, page::SystemInfosView>)
            lv_group_set_focus_cb(group, nullptr);
        last_focus_ = lv_group_get_focused(group);
        lv_group_remove_all_objs(group);
    }
    void on_view_unload() override {
        if (timer_) {
            lv_timer_del(timer_);
            timer_ = nullptr;
        }
        view_.destroy();
        last_focus_ = nullptr;
    }
    void on_language_changed() override {
        view_.apply_language();
    }

  private:
    auto controls() const {
        if constexpr (std::is_same_v<View, page::WorkSettingsView>)
            return std::array{view_.ui.roller.left_roller.btn_up, view_.ui.roller.left_roller.btn_down,
                              view_.ui.roller.right_roller.btn_up, view_.ui.roller.right_roller.btn_down,
                              view_.ui.roller.btn_reset};
        else if constexpr (std::is_same_v<View, page::RecordConfigView>)
            return std::array{view_.ui.roller.left_roller.btn_up, view_.ui.roller.left_roller.btn_down,
                              view_.ui.roller.right_roller.btn_up, view_.ui.roller.right_roller.btn_down,
                              view_.ui.btn_cont.btn_return};
        else if constexpr (std::is_same_v<View, page::ShutdownView>)
            return std::array{view_.ui.shutdown.btn_press, view_.ui.shutdown.btn_language};
        else if constexpr (std::is_same_v<View, page::SystemInfosView>)
            return std::array{view_.ui.work.icon,    view_.ui.gps.icon,     view_.ui.wifi.icon,
                              view_.ui.battery.icon, view_.ui.storage.icon, view_.ui.system.icon};
        else if constexpr (std::is_same_v<View, page::StarMapView>)
            return std::array{root};
        else if constexpr (std::is_same_v<View, page::StartupView>)
            return std::array{view_.ui.btn_press};
        else
            return std::array<lv_obj_t*, 0>{};
    }
    static void clicked(lv_event_t* event) {
        auto* self = static_cast<ViewPage*>(lv_event_get_user_data(event));
        lv_obj_t* obj = lv_event_get_current_target(event);
        if constexpr (std::is_same_v<View, page::WorkSettingsView> || std::is_same_v<View, page::RecordConfigView>) {
            auto& r = self->view_.ui.roller;
            if (obj == r.left_roller.btn_up || obj == r.left_roller.btn_down)
                self->view_.scroll(r.left_roller.label, obj == r.left_roller.btn_up ? -1 : 1);
            else if (obj == r.right_roller.btn_up || obj == r.right_roller.btn_down)
                self->view_.scroll(r.right_roller.label, obj == r.right_roller.btn_up ? -1 : 1);
            else
                self->page_manager->pop();
        } else if constexpr (std::is_same_v<View, page::ShutdownView>) {
            if (obj == self->view_.ui.shutdown.btn_language) {
                const auto next =
                    i18n::get_language() == i18n::Language::English ? i18n::Language::Russian : i18n::Language::English;
                if (i18n::set_language(next))
                    self->page_manager->notify_language_changed();
            } else {
                self->page_manager->pop();
            }
        } else if constexpr (!std::is_same_v<View, page::StartupView>) {
            self->page_manager->pop();
        }
    }
    static void set_progress(void* obj, int32_t value) {
        auto* self = static_cast<ViewPage*>(lv_obj_get_user_data(static_cast<lv_obj_t*>(obj)));
        if constexpr (std::is_same_v<View, page::StartupView>) {
            lv_arc_set_value(static_cast<lv_obj_t*>(obj), static_cast<int16_t>(value));
            lv_label_set_text_fmt(self->view_.ui.arc_percent, "%ld%%", static_cast<long>(value));
        } else if constexpr (std::is_same_v<View, page::ShutdownView>) {
            lv_obj_set_width(static_cast<lv_obj_t*>(obj), static_cast<lv_coord_t>(value));
            lv_label_set_text_fmt(self->view_.ui.shutdown.bar.label, "%ld%%", static_cast<long>(value));
        }
    }
    static void hold_event(lv_event_t* event) {
        auto* self = static_cast<ViewPage*>(lv_event_get_user_data(event));
        const auto code = lv_event_get_code(event);
        if (code != LV_EVENT_PRESSED && code != LV_EVENT_RELEASED && code != LV_EVENT_PRESS_LOST
            && code != LV_EVENT_DEFOCUSED)
            return;
        lv_obj_t* obj;
        lv_anim_t* animation;
        if constexpr (std::is_same_v<View, page::StartupView>) {
            obj = self->view_.ui.arc;
            animation = &self->view_.ui.arc_anim;
        } else if constexpr (std::is_same_v<View, page::ShutdownView>) {
            obj = self->view_.ui.shutdown.bar.obj;
            animation = &self->view_.ui.shutdown.bar.anim;
        } else {
            return;
        }
        if (code == LV_EVENT_PRESSED) {
            if (self->timer_)
                return;
            lv_anim_del(obj, set_progress);
            const int32_t value =
                std::is_same_v<View, page::StartupView> ? lv_arc_get_value(obj) : lv_obj_get_width(obj);
            lv_anim_set_values(animation, value, 100);
            constexpr uint32_t hold_ms = 2000; // 旧版开关机长按阈值；这里只控制演示页面
            const uint32_t remaining_ms = hold_ms * (100 - value) / 100;
            lv_anim_set_time(animation, remaining_ms);
            self->timer_ = lv_timer_create(on_timeout, remaining_ms, self);
            if (self->timer_)
                lv_anim_start(animation);
            else
                LV_LOG_ERROR("Page %s: hold timer allocation failed", self->page_name);
        } else if (self->timer_) {
            lv_timer_del(self->timer_);
            self->timer_ = nullptr;
            lv_anim_del(obj, set_progress);
            const int32_t value =
                std::is_same_v<View, page::StartupView> ? lv_arc_get_value(obj) : lv_obj_get_width(obj);
            lv_anim_set_values(animation, value, 0);
            lv_anim_set_time(animation, 300);
            lv_anim_start(animation);
        }
    }
    static void on_timeout(lv_timer_t* timer) {
        auto* self = static_cast<ViewPage*>(timer->user_data);
        self->timer_ = nullptr;
        lv_timer_del(timer);
        bool switched = false;
        if constexpr (std::is_same_v<View, page::StartupView>) {
            switched = self->page_manager->push("Pages/SystemLoading");
        } else if constexpr (std::is_same_v<View, page::SystemLoadingView>) {
            switched = self->page_manager->push("Pages/Dialplate");
        } else if constexpr (std::is_same_v<View, page::ShutdownView>) {
            switched = self->page_manager->push("Pages/SaveConfig");
        } else if constexpr (std::is_same_v<View, page::SaveConfigView>) {
            switched = self->page_manager->back_home();
        }
        if (!switched)
            LV_LOG_WARN("Page %s: preview transition rejected", self->page_name);
    }
    View view_{};
    lv_obj_t* last_focus_ = nullptr;
    lv_timer_t* timer_ = nullptr;
};

} // namespace

bool P4App::init() {
    resource_pool::init();
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
    manager_->set_global_load_anim_type(PageManager::LOAD_ANIM_MOVE_LEFT, 250, lv_anim_path_ease_out);
    auto register_page_helper = [this](PageBase* page, const char* name) {
        if (!manager_->register_page(page, name)) {
            delete page;
            return false;
        }
        page->on_custom_attr_config();
        return true;
    };
    auto* dialplate = new DialplatePage;
    if (!register_page_helper(dialplate, "Pages/Dialplate"))
        return false;
    if (!register_page_helper(new ViewPage<page::WorkSettingsView>, "Pages/WorkSettings")
        || !register_page_helper(new ViewPage<page::RecordConfigView>, "Pages/RecordConfig")
        || !register_page_helper(new ViewPage<page::SystemInfosView>, "Pages/SystemInfos")
        || !register_page_helper(new ViewPage<page::ShutdownView>, "Pages/Shutdown")
        || !register_page_helper(new ViewPage<page::StarMapView>, "Pages/StarMap")
        || !register_page_helper(new ViewPage<page::StartupView>, "Pages/Startup")
        || !register_page_helper(new ViewPage<page::SystemLoadingView>, "Pages/SystemLoading")
        || !register_page_helper(new ViewPage<page::SaveConfigView>, "Pages/SaveConfig"))
        return false;
    status_bar_.create(lv_layer_top());
    return manager_->push("Pages/Startup");
}

P4App::~P4App() {
    manager_.reset();
    status_bar_.destroy();
    if (group_) {
        lv_group_set_default(nullptr);
        lv_group_del(group_);
    }
    if (style_ready_)
        lv_style_reset(&root_style_);
}

void P4App::on_input(const InputAction action) {
    if (!group_)
        return;
    const char* page = current_page();
    if (action == InputAction::Release) {
        lv_obj_t* pressed = pressed_;
        const char* pressed_page = pressed_page_;
        pressed_ = nullptr;
        pressed_page_ = nullptr;
        if (pressed && page && std::strcmp(page, pressed_page) == 0) {
            lv_obj_clear_state(pressed, LV_STATE_PRESSED);
            lv_event_send(pressed, LV_EVENT_RELEASED, nullptr);
            if (lv_group_get_focused(group_) == pressed)
                lv_event_send(pressed, LV_EVENT_SHORT_CLICKED, nullptr);
        }
        return;
    }
    if (pressed_ && action != InputAction::None) {
        if (page && std::strcmp(page, pressed_page_) == 0) {
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
            if (action == InputAction::Press && page
                && (std::strcmp(page, "Pages/Startup") == 0 || std::strcmp(page, "Pages/Shutdown") == 0)) {
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
    return (current && std::strcmp(current, name) == 0) || manager_->push(name);
}

bool P4App::back() {
    return manager_ && manager_->pop();
}

lv_obj_t* P4App::focused() const {
    return group_ ? lv_group_get_focused(group_) : nullptr;
}
