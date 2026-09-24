#include "P4App.h"

#include "Pages/Dialplate/DialplateView.h"
#include "Pages/HardwareCheckView/HardwareCheckView.h"
#include "Pages/RecordConfig/RecordConfigView.h"
#include "Pages/SaveConfig/SaveConfigView.h"
#include "Pages/Shutdown/ShutdownView.h"
#include "Pages/StarMap/StarMapView.h"
#include "Pages/Startup/StartupView.h"
#include "Pages/SystemInfos/SystemInfosView.h"
#include "Pages/WorkSettings/WorkSettingsView.h"
#include "Resource/ResourcePool.h"

#include <cstring>

namespace {

class DialplatePage final : public PageBase {
  public:
    void onCustomAttrConfig() override { SetCustomLoadAnimType(PageManager::LOAD_ANIM_NONE, 0); }
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

template <typename View>
class ViewPage final : public PageBase {
  public:
    void onCustomAttrConfig() override { SetCustomLoadAnimType(PageManager::LOAD_ANIM_NONE, 0); }
    void onViewLoad() override { view_.Create(_root); }
    void onViewWillDisappear() override { lv_group_remove_all_objs(lv_group_get_default()); }
    void onViewUnload() override { view_.Delete(); }
    void onLanguageChanged() override { view_.ApplyLanguage(); }

  private:
    View view_{};
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
    manager_->SetGlobalLoadAnimType(PageManager::LOAD_ANIM_NONE, 0);
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
    if (!register_page(new ViewPage<Page::WorkSettingsView>, "Pages/WorkSettings") ||
        !register_page(new ViewPage<Page::RecordConfigView>, "Pages/RecordConfig") ||
        !register_page(new ViewPage<Page::SystemInfosView>, "Pages/SystemInfos") ||
        !register_page(new ViewPage<Page::ShutdownView>, "Pages/Shutdown") ||
        !register_page(new ViewPage<Page::StarMapView>, "Pages/StarMap") ||
        !register_page(new ViewPage<Page::StartupView>, "Pages/Startup") ||
        !register_page(new ViewPage<Page::HardwareCheckView>, "Pages/HardwareCheck") ||
        !register_page(new ViewPage<Page::SaveConfigView>, "Pages/SaveConfig"))
        return false;
    status_bar_.Create(lv_layer_top());
    return manager_->Push("Pages/Dialplate");
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

void P4App::OnButton(ButtonGesture::Action action) {
    if (!group_)
        return;
    if (action == ButtonGesture::Action::NextFocus)
        lv_group_focus_next(group_);
    else if (action == ButtonGesture::Action::Confirm)
        if (lv_obj_t* focused = lv_group_get_focused(group_))
            lv_event_send(focused, LV_EVENT_SHORT_CLICKED, nullptr);
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

bool P4App::Back() { return manager_ && manager_->Pop(); }

lv_obj_t* P4App::Focused() const { return group_ ? lv_group_get_focused(group_) : nullptr; }
