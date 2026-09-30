#pragma once

#include <cstdint>

/** 每次采样返回一个动作。短按要等双击窗口结束，才能与双击互斥。 */
class ButtonGesture {
  public:
    enum class Action { None, Confirm, NextFocus };
    Action sample(bool pressed, std::uint64_t now_ms);

  private:
    bool raw_ = false;
    bool stable_ = false;
    bool pending_click_ = false;
    std::uint64_t changed_ms_ = 0;
    std::uint64_t first_release_ms_ = 0;
};
