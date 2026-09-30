#include "Status/DemoStatus.h"

#include <cassert>

int main() {
    const auto start = DemoStatus::sample(0);
    assert(start.position && start.position[0] == 'D');
    assert(start.satellites_valid && start.satellites == 8);
    assert(start.clock_valid && start.hour == 12 && start.minute == 0 && start.second == 0);
    assert(start.battery_valid && start.battery_percent == 85);
    assert(!start.wifi && !start.recording);

    const auto later = DemoStatus::sample(16000);
    assert(later.satellites == 9);
    assert(later.hour == 12 && later.minute == 0 && later.second == 16);
    assert(later.battery_percent == 85);
    assert(later.wifi && later.recording);

    const auto long_running = DemoStatus::sample(std::uint64_t{30} * 16 * 1000);
    assert(long_running.battery_percent >= 70 && long_running.battery_percent <= 85);
    assert(long_running.hour < 24 && long_running.minute < 60 && long_running.second < 60);
    return 0;
}
