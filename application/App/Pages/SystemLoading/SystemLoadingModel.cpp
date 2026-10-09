#include "SystemLoadingModel.h"
#include <algorithm>
#include <cstdio>
#include "Common/ModelUtils.h"
#include "Common/SystemService.h"
#include "Utils/Log/Log.h"

using namespace page;

bool SystemLoadingModel::init() {
    if (account_)
        return true;
    account_ = std::make_unique<Account>("SystemLoadingModel", DataProc::Center(), 0, this);
    account_->SetEventCallback(on_event);
    if (!account_->IsRegistered() || !model_utils::subscribe(*account_, status_, state_)) {
        account_.reset();
        APP_LOG_E("SystemLoadingModel", "Account initialization failed");
        return false;
    }
    return true;
}
void SystemLoadingModel::deinit() {
    account_.reset();
}
bool SystemLoadingModel::set_status_bar(bool visible, DataProc::StatusBarStyle style) const {
    const DataProc::StatusBarPresentation request{visible, style};
    const bool ok = account_ && account_->Notify("StatusBar", &request, sizeof(request)) == Account::RES_OK;
    if (!ok)
        APP_LOG_E("SystemLoadingModel", "StatusBar notification rejected");
    return ok;
}
bool SystemLoadingModel::begin_initialization() const {
    DataProc::SystemRequest request{};
    request.command = DataProc::SystemCommand::BeginInitialization;
    const bool ok = account_ && account_->Notify("System", &request, sizeof(request)) == Account::RES_OK;
    if (!ok)
        APP_LOG_E("SystemLoading", "initialization report reset rejected");
    return ok;
}
bool SystemLoadingModel::initialize_step(unsigned step) const {
    struct DemoStep {
        const char* name;
        bool success;
        const char* detail;
    };
    // Placeholder outcomes only; replace with bounded platform initialization and retain these log points.
    static constexpr DemoStep steps[] = {
        {"configuration", true, "Rover, TRIMTALK, 455.05 MHz defaults loaded"},
        {"services", true, "status and logging services prepared"},
        {"gnss", true, "GNSS receiver placeholder prepared"},
        {"network", false, "Wi-Fi backend reserved; continue offline"},
        {"finalization", true, "working UI prepared"},
    };
    static_assert(sizeof(steps) / sizeof(steps[0]) == DataProc::kInitializationStepCount);
    if (!account_ || step >= DataProc::kInitializationStepCount) {
        APP_LOG_E("SystemLoading", "invalid/uninitialized step %u", step);
        return false;
    }
    const auto& node = steps[step];
    APP_LOG_I("SystemLoading", "step %u/5 %s: begin [DEMO]", step + 1, node.name);
    app_log_write(node.success ? APP_LOG_INFO : APP_LOG_WARN, "SystemLoading", "step %u/5 %s: %s [DEMO] (%s)", step + 1,
                  node.name, node.success ? "OK" : "FAILED", node.detail);
    DataProc::SystemRequest request{};
    request.command = DataProc::SystemCommand::RecordInitialization;
    request.initialization_step = step;
    request.initialization.status =
        node.success ? DataProc::InitializationStatus::Ok : DataProc::InitializationStatus::Failed;
    std::snprintf(request.initialization.detail.data(), request.initialization.detail.size(), "%s", node.detail);
    if (account_->Notify("System", &request, sizeof(request)) != Account::RES_OK) {
        APP_LOG_E("SystemLoading", "step %u report rejected", step + 1);
        return false;
    }
    return node.success;
}

int SystemLoadingModel::on_event(Account* account, Account::EventParam_t* event) {
    auto* self = static_cast<SystemLoadingModel*>(account->UserData);
    return model_utils::receive(event, self->status_, self->state_);
}
