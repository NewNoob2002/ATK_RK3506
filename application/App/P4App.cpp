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
    void onCustomAttrConfig() override {
        SetCustomLoadAnimType(PageManager::LOAD_ANIM_MOVE_LEFT, 250, lv_anim_path_ease_out);
    }
    void onViewLoad() override {
        view_.Create(_root);
        lv_obj_t* buttons[] = {view_.ui.btnCont.btnMap, view_.ui.btnCont.btnRec, view_.ui.btnCont.btnMenu,
                               view_.ui.btnCont.btnShutdown, view_.ui.topInfo.icon_satellite};
        for (lv_obj_t* button : buttons) {
            lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(button, Clicked, LV_EVENT_SHORT_CLICKED, this);
        }
    }
    void onViewWillAppear() override {
        lv_group_t* group = lv_group_get_default();
        lv_group_set_wrap(group, true);
        lv_group_add_obj(group, view_.ui.btnCont.btnMap);
        lv_group_add_obj(group, view_.ui.btnCont.btnRec);
        lv_group_add_obj(group, view_.ui.btnCont.btnMenu);
        lv_group_add_obj(group, view_.ui.btnCont.btnShutdown);
        lv_group_add_obj(group, view_.ui.topInfo.icon_satellite);
        lv_group_focus_obj(last_focus_ ? last_focus_ : view_.ui.btnCont.btnMenu);
        view_.AppearAnimStart();
    }
    void onViewWillDisappear() override {
        last_focus_ = lv_group_get_focused(lv_group_get_default());
        lv_group_remove_all_objs(lv_group_get_default());
    }
    void onViewUnload() override {
        view_.Delete();
        last_focus_ = nullptr;
    }

  private:
    static void Clicked(lv_event_t* event) {
        auto* self = static_cast<DialplatePage*>(lv_event_get_user_data(event));
        const lv_obj_t* button = lv_event_get_target(event);
        const char* destination = nullptr;
        if (button == self->view_.ui.btnCont.btnMap)
            destination = "Pages/WorkSettings";
        else if (button == self->view_.ui.btnCont.btnRec)
            destination = "Pages/RecordConfig";
        else if (button == self->view_.ui.btnCont.btnMenu)
            destination = "Pages/SystemInfos";
        else if (button == self->view_.ui.btnCont.btnShutdown)
            destination = "Pages/Shutdown";
        else if (button == self->view_.ui.topInfo.icon_satellite)
            destination = "Pages/StarMap";
        if (destination)
            self->pageManager->Push(destination);
    }
    Page::DialplateView view_{};
    lv_obj_t* last_focus_ = nullptr;
};

template <typename View> class ViewPage final : public PageBase {
  public:
    void onCustomAttrConfig() override {
        SetCustomLoadAnimType(PageManager::LOAD_ANIM_MOVE_LEFT, 250, lv_anim_path_ease_out);
    }
    void onViewLoad() override {
        view_.Create(_root);
        // 尚无业务后端的按钮不能表现为可操作；其余控件只处理界面交互。
        if constexpr (std::is_same_v<View, Page::WorkSettingsView>) {
            lv_obj_add_state(view_.ui.btnCont.btnBase, LV_STATE_DISABLED);
            lv_obj_add_state(view_.ui.btnCont.btnRover, LV_STATE_DISABLED);
            lv_obj_add_state(view_.ui.btnCont.btnNtrip, LV_STATE_DISABLED);
        } else if constexpr (std::is_same_v<View, Page::RecordConfigView>) {
            lv_obj_add_state(view_.ui.btnCont.btnRecord, LV_STATE_DISABLED);
        } else if constexpr (std::is_same_v<View, Page::ShutdownView>) {
            lv_obj_add_state(view_.ui.shutdown.btnWifi, LV_STATE_DISABLED);
        }
        for (lv_obj_t* obj : Controls()) {
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(obj, Clicked, LV_EVENT_SHORT_CLICKED, this);
        }
        if constexpr (std::is_same_v<View, Page::StartupView> || std::is_same_v<View, Page::ShutdownView>) {
            lv_obj_t* progress;
            lv_anim_t* animation;
            if constexpr (std::is_same_v<View, Page::StartupView>) {
                progress = view_.ui.arc;
                animation = &view_.ui.arc_anim;
                lv_obj_add_event_cb(view_.ui.btnPress, HoldEvent, LV_EVENT_ALL, this);
            } else {
                progress = view_.ui.shutdown.bar.obj;
                animation = &view_.ui.shutdown.bar.anim;
                lv_obj_add_event_cb(view_.ui.shutdown.btnPress, HoldEvent, LV_EVENT_ALL, this);
            }
            lv_obj_set_user_data(progress, this);
            lv_anim_set_exec_cb(animation, SetProgress);
        }
    }
    void onViewWillAppear() override {
        lv_group_t* group = lv_group_get_default();
        lv_group_set_wrap(group, true);
        if constexpr (std::is_same_v<View, Page::SystemInfosView>) {
            // 前一页在消失时清空焦点组；信息页必须在此之后注册。
            view_.Group_Init();
        } else {
            for (lv_obj_t* obj : Controls())
                lv_group_add_obj(group, obj);
            if constexpr (std::is_same_v<View, Page::WorkSettingsView>) {
                lv_group_focus_obj(last_focus_ ? last_focus_ : view_.ui.roller.btnReset);
            } else if constexpr (std::is_same_v<View, Page::RecordConfigView>) {
                lv_group_focus_obj(last_focus_ ? last_focus_ : view_.ui.btnCont.btnReturn);
            } else if constexpr (std::is_same_v<View, Page::ShutdownView>) {
                SetProgress(view_.ui.shutdown.bar.obj, 0);
                lv_obj_clear_state(view_.ui.shutdown.btnPress, LV_STATE_PRESSED);
                lv_group_focus_obj(last_focus_ ? last_focus_ : view_.ui.shutdown.btnPress);
            } else if constexpr (std::is_same_v<View, Page::StarMapView>) {
                lv_group_focus_obj(_root);
            } else if constexpr (std::is_same_v<View, Page::StartupView>) {
                SetProgress(view_.ui.arc, 0);
                lv_obj_clear_state(view_.ui.btnPress, LV_STATE_PRESSED);
                lv_group_focus_obj(view_.ui.btnPress);
            }
        }
        if constexpr (std::is_same_v<View, Page::SystemLoadingView>) {
            lv_anim_timeline_start(view_.ui.anim_timeline);
            lv_anim_start(&view_.ui.bar_anim);
            timer_ = lv_timer_create(OnTimeout, lv_anim_get_playtime(&view_.ui.bar_anim) + 200, this);
        } else if constexpr (std::is_same_v<View, Page::SaveConfigView>) {
            lv_anim_timeline_start(view_.ui.anim_timeline);
            lv_anim_start(&view_.ui.sync.bar.anim);
            timer_ = lv_timer_create(OnTimeout, lv_anim_get_playtime(&view_.ui.sync.bar.anim) + 500, this);
        }
        if constexpr (std::is_same_v<View, Page::SystemLoadingView> || std::is_same_v<View, Page::SaveConfigView>) {
            if (!timer_)
                LV_LOG_ERROR("Page %s: preview timer allocation failed", pageName);
        }
    }
    void onViewWillDisappear() override {
        if (timer_) {
            lv_timer_del(timer_);
            timer_ = nullptr;
        }
        if constexpr (std::is_same_v<View, Page::StartupView>)
            lv_anim_del(view_.ui.arc, SetProgress);
        else if constexpr (std::is_same_v<View, Page::ShutdownView>)
            lv_anim_del(view_.ui.shutdown.bar.obj, SetProgress);
        lv_group_t* group = lv_group_get_default();
        if constexpr (std::is_same_v<View, Page::SystemInfosView>)
            lv_group_set_focus_cb(group, nullptr);
        last_focus_ = lv_group_get_focused(group);
        lv_group_remove_all_objs(group);
    }
    void onViewUnload() override {
        if (timer_) {
            lv_timer_del(timer_);
            timer_ = nullptr;
        }
        view_.Delete();
        last_focus_ = nullptr;
    }
    void onLanguageChanged() override {
        view_.ApplyLanguage();
    }

  private:
    auto Controls() const {
        if constexpr (std::is_same_v<View, Page::WorkSettingsView>)
            return std::array{view_.ui.roller.left_roller.btnUp, view_.ui.roller.left_roller.btnDown,
                              view_.ui.roller.right_roller.btnUp, view_.ui.roller.right_roller.btnDown,
                              view_.ui.roller.btnReset};
        else if constexpr (std::is_same_v<View, Page::RecordConfigView>)
            return std::array{view_.ui.roller.left_roller.btnUp, view_.ui.roller.left_roller.btnDown,
                              view_.ui.roller.right_roller.btnUp, view_.ui.roller.right_roller.btnDown,
                              view_.ui.btnCont.btnReturn};
        else if constexpr (std::is_same_v<View, Page::ShutdownView>)
            return std::array{view_.ui.shutdown.btnPress, view_.ui.shutdown.btnLanguage};
        else if constexpr (std::is_same_v<View, Page::SystemInfosView>)
            return std::array{view_.ui.work.icon,    view_.ui.gps.icon,     view_.ui.wifi.icon,
                              view_.ui.battery.icon, view_.ui.storage.icon, view_.ui.system.icon};
        else if constexpr (std::is_same_v<View, Page::StarMapView>)
            return std::array{_root};
        else if constexpr (std::is_same_v<View, Page::StartupView>)
            return std::array{view_.ui.btnPress};
        else
            return std::array<lv_obj_t*, 0>{};
    }
    static void Clicked(lv_event_t* event) {
        auto* self = static_cast<ViewPage*>(lv_event_get_user_data(event));
        lv_obj_t* obj = lv_event_get_current_target(event);
        if constexpr (std::is_same_v<View, Page::WorkSettingsView> || std::is_same_v<View, Page::RecordConfigView>) {
            auto& r = self->view_.ui.roller;
            if (obj == r.left_roller.btnUp || obj == r.left_roller.btnDown)
                self->view_.Scroll(r.left_roller.label, obj == r.left_roller.btnUp ? -1 : 1);
            else if (obj == r.right_roller.btnUp || obj == r.right_roller.btnDown)
                self->view_.Scroll(r.right_roller.label, obj == r.right_roller.btnUp ? -1 : 1);
            else
                self->pageManager->Pop();
        } else if constexpr (std::is_same_v<View, Page::ShutdownView>) {
            if (obj == self->view_.ui.shutdown.btnLanguage) {
                const auto next =
                    I18n::GetLanguage() == I18n::Language::English ? I18n::Language::Russian : I18n::Language::English;
                if (I18n::SetLanguage(next))
                    self->pageManager->NotifyLanguageChanged();
            } else {
                self->pageManager->Pop();
            }
        } else if constexpr (!std::is_same_v<View, Page::StartupView>) {
            self->pageManager->Pop();
        }
    }
    static void SetProgress(void* obj, int32_t value) {
        auto* self = static_cast<ViewPage*>(lv_obj_get_user_data(static_cast<lv_obj_t*>(obj)));
        if constexpr (std::is_same_v<View, Page::StartupView>) {
            lv_arc_set_value(static_cast<lv_obj_t*>(obj), value);
            lv_label_set_text_fmt(self->view_.ui.arc_percent, "%ld%%", static_cast<long>(value));
        } else if constexpr (std::is_same_v<View, Page::ShutdownView>) {
            lv_obj_set_width(static_cast<lv_obj_t*>(obj), value);
            lv_label_set_text_fmt(self->view_.ui.shutdown.bar.label, "%ld%%", static_cast<long>(value));
        }
    }
    static void HoldEvent(lv_event_t* event) {
        auto* self = static_cast<ViewPage*>(lv_event_get_user_data(event));
        const auto code = lv_event_get_code(event);
        if (code != LV_EVENT_PRESSED && code != LV_EVENT_RELEASED && code != LV_EVENT_PRESS_LOST
            && code != LV_EVENT_DEFOCUSED)
            return;
        lv_obj_t* obj;
        lv_anim_t* animation;
        if constexpr (std::is_same_v<View, Page::StartupView>) {
            obj = self->view_.ui.arc;
            animation = &self->view_.ui.arc_anim;
        } else if constexpr (std::is_same_v<View, Page::ShutdownView>) {
            obj = self->view_.ui.shutdown.bar.obj;
            animation = &self->view_.ui.shutdown.bar.anim;
        } else {
            return;
        }
        if (code == LV_EVENT_PRESSED) {
            if (self->timer_)
                return;
            lv_anim_del(obj, SetProgress);
            const int32_t value =
                std::is_same_v<View, Page::StartupView> ? lv_arc_get_value(obj) : lv_obj_get_width(obj);
            lv_anim_set_values(animation, value, 100);
            constexpr uint32_t hold_ms = 2000; // 旧版开关机长按阈值；这里只控制演示页面
            const uint32_t remaining_ms = hold_ms * (100 - value) / 100;
            lv_anim_set_time(animation, remaining_ms);
            self->timer_ = lv_timer_create(OnTimeout, remaining_ms, self);
            if (self->timer_)
                lv_anim_start(animation);
            else
                LV_LOG_ERROR("Page %s: hold timer allocation failed", self->pageName);
        } else if (self->timer_) {
            lv_timer_del(self->timer_);
            self->timer_ = nullptr;
            lv_anim_del(obj, SetProgress);
            const int32_t value =
                std::is_same_v<View, Page::StartupView> ? lv_arc_get_value(obj) : lv_obj_get_width(obj);
            lv_anim_set_values(animation, value, 0);
            lv_anim_set_time(animation, 300);
            lv_anim_start(animation);
        }
    }
    static void OnTimeout(lv_timer_t* timer) {
        auto* self = static_cast<ViewPage*>(timer->user_data);
        self->timer_ = nullptr;
        lv_timer_del(timer);
        bool switched = false;
        if constexpr (std::is_same_v<View, Page::StartupView>) {
            switched = self->pageManager->Push("Pages/SystemLoading");
        } else if constexpr (std::is_same_v<View, Page::SystemLoadingView>) {
            switched = self->pageManager->Push("Pages/Dialplate");
        } else if constexpr (std::is_same_v<View, Page::ShutdownView>) {
            switched = self->pageManager->Push("Pages/SaveConfig");
        } else if constexpr (std::is_same_v<View, Page::SaveConfigView>) {
            switched = self->pageManager->BackHome();
        }
        if (!switched)
            LV_LOG_WARN("Page %s: preview transition rejected", self->pageName);
    }
    View view_{};
    lv_obj_t* last_focus_ = nullptr;
    lv_timer_t* timer_ = nullptr;
};

} // namespace

bool P4App::Init() {
    ResourcePool::Init();
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
    manager_->SetRootDefaultStyle(&root_style_);
    manager_->SetGlobalLoadAnimType(PageManager::LOAD_ANIM_MOVE_LEFT, 250, lv_anim_path_ease_out);
    auto register_page = [this](PageBase* page, const char* name) {
        if (!manager_->Register(page, name)) {
            delete page;
            return false;
        }
        page->onCustomAttrConfig();
        return true;
    };
    auto* dialplate = new DialplatePage;
    if (!register_page(dialplate, "Pages/Dialplate"))
        return false;
    if (!register_page(new ViewPage<Page::WorkSettingsView>, "Pages/WorkSettings")
        || !register_page(new ViewPage<Page::RecordConfigView>, "Pages/RecordConfig")
        || !register_page(new ViewPage<Page::SystemInfosView>, "Pages/SystemInfos")
        || !register_page(new ViewPage<Page::ShutdownView>, "Pages/Shutdown")
        || !register_page(new ViewPage<Page::StarMapView>, "Pages/StarMap")
        || !register_page(new ViewPage<Page::StartupView>, "Pages/Startup")
        || !register_page(new ViewPage<Page::SystemLoadingView>, "Pages/SystemLoading")
        || !register_page(new ViewPage<Page::SaveConfigView>, "Pages/SaveConfig"))
        return false;
    status_bar_.Create(lv_layer_top());
    return manager_->Push("Pages/Startup");
}

P4App::~P4App() {
    manager_.reset();
    status_bar_.Delete();
    if (group_) {
        lv_group_set_default(nullptr);
        lv_group_del(group_);
    }
    if (style_ready_)
        lv_style_reset(&root_style_);
}

void P4App::OnInput(const InputAction action) {
    if (!group_)
        return;
    const char* page = CurrentPage();
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
        Back();
    }
}

const char* P4App::CurrentPage() const {
    if (!manager_)
        return nullptr;
    PageBase* page = manager_->GetCurrentPage();
    return page ? page->pageName : nullptr;
}

bool P4App::ShowPage(const char* name) {
    if (!manager_ || !name)
        return false;
    const char* current = CurrentPage();
    return (current && std::strcmp(current, name) == 0) || manager_->Push(name);
}

bool P4App::Back() {
    return manager_ && manager_->Pop();
}

lv_obj_t* P4App::Focused() const {
    return group_ ? lv_group_get_focused(group_) : nullptr;
}
