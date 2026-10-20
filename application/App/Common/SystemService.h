#pragma once
#include "DataProc/DataProc.h"

/** Simulator/RAM services only. No filesystem writes, clock_settime, shutdown or reboot syscalls. */
namespace SystemService {
DataProc::SystemState snapshot();
bool power(DataProc::PowerAction action);
bool valid_date(const DataProc::DateTime& date);
unsigned days_in_month(unsigned year, unsigned month);
} // namespace SystemService

namespace StatusService {
/** Feed an owned snapshot to Status and its subscribers, on the LVGL thread. */
bool update(const DataProc::StatusSnapshot& status);
} // namespace StatusService
