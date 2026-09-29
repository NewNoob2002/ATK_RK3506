#include "DemoStatus.h"

Page::StatusBarState DemoStatus::Sample(const std::uint64_t elapsed_ms) {
    const std::uint64_t seconds = elapsed_ms / 1000;
    Page::StatusBarState state;
    state.position = "DEMO";
    state.position_color = lv_color_hex(0xf44336);
    state.satellites_valid = true;
    state.satellites = 8U + static_cast<unsigned>(seconds % 5U);
    state.clock_valid = true;
    state.hour = static_cast<unsigned>((12U + (seconds / 3600U)) % 24U);
    state.minute = static_cast<unsigned>((seconds / 60U) % 60U);
    state.second = static_cast<unsigned>(seconds % 60U);
    state.battery_valid = true;
    state.battery_percent = 85U - static_cast<unsigned>((seconds / 30U) % 16U);
    state.wifi = (seconds / 10U) % 2U != 0U;
    state.recording = (seconds / 16U) % 2U != 0U;
    return state;
}
