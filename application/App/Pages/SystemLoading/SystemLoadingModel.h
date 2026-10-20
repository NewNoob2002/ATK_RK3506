#pragma once
#include <memory>
#include "Common/DataProc/DataProc.h"

namespace page {
/** Page-owned Account and received data. Init on view load, Deinit on unload, LVGL thread only. */
class SystemLoadingModel {
  public:
    bool init();
    void deinit();
    bool set_status_bar(bool visible, DataProc::StatusBarStyle style = {}) const;
    /** UI thread: clear the retained report when a new initialization attempt starts. */
    bool begin_initialization() const;
    /** UI-thread demo initialization/logging hook. step is 0..4; false marks failure, never stops boot.
     * Retains status/detail in System even after this page's Account is unloaded.
     * No hardware operations occur here. Replace placeholders with bounded platform results later.
     */
    bool initialize_step(unsigned step) const;
    const DataProc::StatusSnapshot& status() const {
        return status_;
    }
    const DataProc::SystemState& settings() const {
        return state_;
    }

  private:
    static int on_event(Account* account, Account::EventParam_t* event);
    std::unique_ptr<Account> account_;
    DataProc::StatusSnapshot status_{};
    DataProc::SystemState state_{};
};
} // namespace page
