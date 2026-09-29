#pragma once

#include "Pages/StatusBar/StatusBar.h"
#include "Utils/PageManager/PageManager.h"

#include <memory>

/** P4 页面组合：业务状态尚未迁入，状态栏只显示调用方传入的数据。 */
class P4App {
  public:
    bool Init();
    ~P4App();
    void UpdateStatus(const Page::StatusBarState& state) {
        status_bar_.Update(state);
    }
    enum class InputAction { None, NextFocus, PreviousFocus, Confirm, Back, Press, Release };
    void OnInput(InputAction action);
    /** 显示已注册的页面，供无按键的静态画面预览使用。 */
    bool ShowPage(const char* name);
    bool Back();
    const char* CurrentPage() const;
    lv_obj_t* Focused() const;

  private:
    lv_group_t* group_ = nullptr;
    lv_style_t root_style_{};
    bool style_ready_ = false;
    Page::StatusBar status_bar_;
    std::unique_ptr<PageManager> manager_;
    lv_obj_t* pressed_ = nullptr; // 按键按住期间的焦点；页面切换后绝不解引用
    const char* pressed_page_ = nullptr;
};
