#pragma once

#include "ButtonGesture.h"
#include "Pages/StatusBar/StatusBar.h"
#include "Utils/PageManager/PageManager.h"

#include <memory>

/** P4 页面组合：业务状态尚未迁入，状态栏只显示调用方传入的数据。 */
class P4App {
  public:
    bool init();
    ~P4App();
    void update_status(const page::StatusBarState& state);
    enum class InputAction { None, NextFocus, PreviousFocus, Confirm, Back, Press, Release, Commit };
    enum class Key { Power, Function };
    /** Reserved product GPIO input; Key names are logical roles, not keyboard letters or evdev codes.
     * The platform samples each GPIO HIGH/LOW and normalizes its confirmed active level:
     * on_key(role, level == active_level). No GPIO line or polarity is assumed here.
     * Call on the UI thread; host/test-board adapters do not use this path.
     */
    void on_key(Key key, bool pressed);
    /** Process sampled GPIO states with a monotonic millisecond timestamp on the UI thread. */
    void poll_keys(std::uint64_t now_ms);
    /** Cancel held controls and partial gestures on device/window focus loss; never confirm on cancellation. */
    void cancel_input();
    /** Direct host/test actions: no production debounce or double-click timing. */
    void on_input(InputAction action);
    /** 显示已注册的页面，供无按键的静态画面预览使用。 */
    bool show_page(const char* name);
    bool back();
    const char* current_page() const;
    lv_obj_t* focused() const;

  private:
    lv_group_t* group_ = nullptr;
    lv_style_t root_style_{};
    bool style_ready_ = false;
    bool data_ready_ = false;
    i18n::Language language_{};
    std::unique_ptr<Account> account_;
    static int on_system_event(Account* account, Account::EventParam_t* event);
    page::StatusBar status_bar_;
    std::unique_ptr<PageManager> manager_;
    bool power_down_ = false, function_down_ = false;
    ButtonGesture power_gesture_, function_gesture_;
    const char* key_page_ = nullptr;
    void cancel_keys();
    lv_obj_t* pressed_ = nullptr; // 按键按住期间的焦点；页面切换后绝不解引用
    const char* pressed_page_ = nullptr;
};
