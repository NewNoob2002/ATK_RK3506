#pragma once

#include <array>
#include <cstdint>
#include "Utils/I18n/I18n.h"

// Native in-process Account payloads, not a wire/flash format. Delivery is LVGL-thread-only.
namespace DataProc {
enum class StatusBarStyle { Default = 0, Transparent };
struct StatusBarPresentation {
    bool visible = true;
    StatusBarStyle style{};
};
struct StatusSnapshot {
    std::array<char, 32> position{};
    unsigned satellites = 0, battery_percent = 0, hour = 0, minute = 0, second = 0;
    bool satellites_valid = false, battery_valid = false, clock_valid = false, recording = false, wifi = false;
    bool charging = false;
    float battery_voltage = 7.60f;
    std::uint32_t position_color_rgb = 0xf44336;
};
struct DateTime {
    unsigned year = 2026, month = 9, day = 30, hour = 12, minute = 30;
};
enum class PowerAction { Off, Reboot };
constexpr std::size_t kInitializationStepCount = 5;
enum class InitializationStatus { NotRun, Ok, Failed };
/** Last initialization attempt, retained by System until the next attempt/app teardown. UI thread only.
 * Detail is a NUL-terminated diagnostic message (maximum 95 bytes), not a serialized payload.
 */
struct InitializationResult {
    InitializationStatus status{};
    std::array<char, 96> detail{};
};
struct SystemState {
    i18n::Language language{};
    bool wifi_available = false, wifi_on = false;
    DateTime date{};
    bool simulated = true, power_pending = false;
    PowerAction power_action{};
    std::uint64_t power_token = 0, power_calls = 0;
    bool initialization_started = false;
    // ponytail: retain only the latest attempt in RAM; add persistent history if earlier boots must be inspected.
    std::array<InitializationResult, kInitializationStepCount> initialization{};
};
enum class SystemCommand {
    SetLanguage,
    SetTime,
    PreparePower,
    ExecutePower,
    CancelPower,
    BeginInitialization,
    RecordInitialization
};
struct SystemRequest {
    SystemCommand command{};
    i18n::Language language{};
    DateTime date{};
    PowerAction action{};
    std::uint64_t token = 0;
    unsigned initialization_step = 0;
    InitializationResult initialization{};
};
} // namespace DataProc
