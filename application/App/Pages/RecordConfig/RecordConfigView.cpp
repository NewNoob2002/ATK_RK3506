#include "RecordConfigView.h"
#include "Utils/PageStyle.h"
using namespace page;
using i18n::TextId;

void RecordConfigView::create(lv_obj_t* root) {
    const auto* font = resource_pool::get_font("oswaldBold_12");
    state_ = style::label(root, font, 10, 28, 274);
    ui.type = style::box(root, 8, 50, 134, 23, true);
    ui.interval = style::box(root, 150, 50, 136, 23, true);
    ui.record = style::box(root, 8, 78, 134, 25, true);
    ui.back = style::box(root, 150, 78, 136, 25, true);
    for (auto* control : controls())
        style::control(control);
    type_text_ = style::label(ui.type, font, 8, 2, 116);
    interval_text_ = style::label(ui.interval, font, 8, 2, 118);
    record_text_ = style::label(ui.record, font, 8, 3, 116);
    back_text_ = style::label(ui.back, font, 8, 3, 118);
    feedback_ = style::label(root, font, 8, 108, 278);
    lv_obj_set_style_text_align(feedback_, LV_TEXT_ALIGN_CENTER, 0);
}
void RecordConfigView::set_selection(unsigned type, unsigned interval) {
    if (type < DataProc::kRecordTypeCount)
        type_ = type;
    if (interval < DataProc::kRecordIntervalCount)
        interval_ = interval;
}
void RecordConfigView::update(const DataProc::DeviceSnapshot& device, bool configurable, TextId feedback) const {
    lv_label_set_text_fmt(state_, "%s | %.0f MB%s%s",
                          i18n::text(!device.record_valid ? TextId::NotAvailable
                                     : device.recording   ? TextId::Recording
                                                          : TextId::RecordIdle),
                          device.record_space_mb, configurable ? "" : " | ",
                          configurable ? "" : i18n::text(TextId::ReadOnly));
    lv_obj_set_style_text_color(state_, device.recording ? style::kDanger : style::kText, 0);
    char value[64];
    i18n::option_text(TextId::RecordTypeOptions, type_, value, sizeof(value));
    lv_label_set_text(type_text_, value);
    i18n::option_text(TextId::RecordIntervalOptions, interval_, value, sizeof(value));
    lv_label_set_text(interval_text_, value);
    lv_label_set_text(record_text_, i18n::text(device.recording ? TextId::Stop : TextId::StartupStart));
    lv_label_set_text(back_text_, i18n::text(TextId::Back));
    const bool locked = device.recording || device.record_pending || !device.record_valid;
    for (auto* control : {ui.type, ui.interval}) {
        if (configurable && !locked)
            lv_obj_clear_state(control, LV_STATE_DISABLED);
        else
            lv_obj_add_state(control, LV_STATE_DISABLED);
        // 固定参数保持只读数据样式，不呈现灰色的可编辑控件。
        lv_obj_set_style_opa(control, configurable ? LV_OPA_50 : LV_OPA_COVER, LV_STATE_DISABLED);
        if (configurable)
            lv_obj_add_flag(control, LV_OBJ_FLAG_CLICKABLE);
        else
            lv_obj_clear_flag(control, LV_OBJ_FLAG_CLICKABLE);
    }
    if (device.record_pending || !device.record_valid)
        lv_obj_add_state(ui.record, LV_STATE_DISABLED);
    else
        lv_obj_clear_state(ui.record, LV_STATE_DISABLED);
    lv_obj_set_style_border_color(ui.record, device.recording ? style::kDanger : style::kBorder, 0);
    const auto message = feedback != TextId::Count ? feedback
                         : device.recording        ? TextId::RecordStopHint
                                                   : TextId::RecordStartHint;
    lv_label_set_text(feedback_, i18n::text(message));
    lv_obj_set_style_text_color(feedback_,
                                message == TextId::RequestRejected || message == TextId::RequestTimeout ? style::kDanger
                                : message == TextId::RequestReceived ? style::kSuccess
                                                                     : style::kMuted,
                                0);
}
