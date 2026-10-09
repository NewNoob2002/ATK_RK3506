#include "SystemService.h"
#include <algorithm>
#include <limits>
#include "Utils/Log/Log.h"

namespace {
DataProc::SystemState state;
Account* provider = nullptr;
std::uint64_t executed_token = 0;

bool sender_is(const Account::EventParam_t* event, const char* name) {
    return event->tran && std::strcmp(event->tran->ID, name) == 0;
}
int on_event(Account* account, Account::EventParam_t* event) {
    if (!event || !event->data_p)
        return Account::RES_PARAM_ERROR;
    if (event->event == Account::EVENT_SUB_PULL) {
        if (event->size != sizeof(state))
            return Account::RES_SIZE_MISMATCH;
        std::memcpy(event->data_p, &state, sizeof(state));
        return Account::RES_OK;
    }
    if (event->event != Account::EVENT_NOTIFY)
        return Account::RES_UNSUPPORTED_REQUEST;
    DataProc::SystemRequest request;
    const int decoded = DataProc::ReadPayload(event, request);
    if (decoded != Account::RES_OK)
        return decoded;
    switch (request.command) {
        case DataProc::SystemCommand::BeginInitialization:
            if (!sender_is(event, "SystemLoadingModel"))
                return Account::RES_PARAM_ERROR;
            state.initialization_started = true;
            state.initialization = {}; // A fresh attempt replaces earlier results, including cached-page re-entry.
            break;
        case DataProc::SystemCommand::RecordInitialization:
            if (!sender_is(event, "SystemLoadingModel") || !state.initialization_started
                || request.initialization_step >= state.initialization.size()
                || (request.initialization.status != DataProc::InitializationStatus::Ok
                    && request.initialization.status != DataProc::InitializationStatus::Failed)
                || std::find(request.initialization.detail.begin(), request.initialization.detail.end(), '\0')
                       == request.initialization.detail.end())
                return Account::RES_PARAM_ERROR;
            state.initialization[request.initialization_step] = request.initialization;
            break;
        case DataProc::SystemCommand::SetLanguage:
            if (!sender_is(event, "SystemSettingsModel")
                || (request.language != i18n::Language::English && request.language != i18n::Language::Russian))
                return Account::RES_PARAM_ERROR;
            i18n::set_language(request.language);
            state.language = request.language;
            break;
        case DataProc::SystemCommand::SetTime:
            if (!sender_is(event, "SystemSettingsModel") || !SystemService::valid_date(request.date))
                return Account::RES_PARAM_ERROR;
            state.date = request.date; // Explicit RAM simulation: no system clock or persistent write.
            APP_LOG_I("SystemService", "demo date/time committed to RAM");
            break;
        case DataProc::SystemCommand::PreparePower:
            if (!sender_is(event, "SystemDashModel") || state.power_pending
                || (request.action != DataProc::PowerAction::Off && request.action != DataProc::PowerAction::Reboot)
                || state.power_token == std::numeric_limits<std::uint64_t>::max())
                return Account::RES_PARAM_ERROR;
            ++state.power_token;
            state.power_action = request.action;
            state.power_pending = true;
            break;
        case DataProc::SystemCommand::CancelPower:
            if ((!sender_is(event, "SystemDashModel") && !sender_is(event, "SaveConfigModel"))
                || request.token != state.power_token)
                return Account::RES_PARAM_ERROR;
            state.power_pending = false;
            break;
        case DataProc::SystemCommand::ExecutePower:
            if (!sender_is(event, "SaveConfigModel") || !request.token)
                return Account::RES_PARAM_ERROR;
            if (request.token == executed_token)
                return Account::RES_OK; // Retry is not another power operation.
            if (!state.power_pending || request.token != state.power_token)
                return Account::RES_PARAM_ERROR;
            state.power_pending = false; // Consume before calling the service, guarding reentrant confirmation.
            executed_token = request.token;
            if (!SystemService::power(state.power_action))
                return Account::RES_UNKNOW;
            break;
        default:
            return Account::RES_UNSUPPORTED_REQUEST;
    }
    if (!account->Commit(&state, sizeof(state)))
        return Account::RES_NO_CACHE;
    const int published = account->Publish();
    if (published != Account::RES_OK && published != Account::RES_UNKNOW)
        APP_LOG_W("SystemService", "subscriber rejected state: %d", published);
    return Account::RES_OK;
}
} // namespace
DATA_PROC_INIT_DEF(System) {
    provider = account;
    state = {};
    state.language = i18n::get_language();
    executed_token = 0;
    account->SetEventCallback(on_event);
    return account->Commit(&state, sizeof(state));
}
DataProc::SystemState SystemService::snapshot() {
    return state;
}
bool SystemService::power(DataProc::PowerAction action) {
    if (DataProc::Center()->SearchAccount("System") != provider || !provider
        || (action != DataProc::PowerAction::Off && action != DataProc::PowerAction::Reboot)
        || state.power_calls == std::numeric_limits<std::uint64_t>::max())
        return false;
    ++state.power_calls;
    state.power_action = action;
    APP_LOG_I("SystemService", "simulated power %s: NO host/device power operation",
              action == DataProc::PowerAction::Off ? "OFF" : "REBOOT");
    return true;
}
unsigned SystemService::days_in_month(unsigned year, unsigned month) {
    constexpr unsigned days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month < 1 || month > 12)
        return 0;
    const bool leap = year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
    return days[month - 1] + (month == 2 && leap ? 1 : 0);
}
bool SystemService::valid_date(const DataProc::DateTime& date) {
    return date.year >= 2000 && date.year <= 2099 && date.month >= 1 && date.month <= 12 && date.day >= 1
           && date.day <= days_in_month(date.year, date.month) && date.hour < 24 && date.minute < 60;
}
