#include "ButtonGesture.h"

ButtonGesture::Action ButtonGesture::sample(bool pressed, std::uint64_t now_ms) {
    constexpr std::uint64_t debounce_ms = 20;
    constexpr std::uint64_t double_click_ms = 250;
    if (pressed != raw_) {
        raw_ = pressed;
        changed_ms_ = now_ms;
    }
    if (raw_ != stable_ && now_ms - changed_ms_ >= debounce_ms) {
        stable_ = raw_;
        if (!stable_) {
            if (pending_click_ && now_ms - first_release_ms_ <= double_click_ms) {
                pending_click_ = false;
                return Action::NextFocus;
            }
            pending_click_ = true;
            first_release_ms_ = now_ms;
        }
    }
    if (pending_click_ && !stable_ && now_ms - first_release_ms_ > double_click_ms) {
        pending_click_ = false;
        return Action::Confirm;
    }
    return Action::None;
}
