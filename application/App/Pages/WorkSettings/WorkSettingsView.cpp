#include "WorkSettingsView.h"
#include "Utils/PageStyle.h"
using namespace page;
using i18n::TextId;

void WorkSettingsView::create(lv_obj_t* root) {
    const auto* font = resource_pool::get_font("oswaldBold_12");
    ui.preset = style::box(root, 8, 28, 278, 23, true);
    ui.protocol = style::box(root, 8, 55, 134, 23, true);
    ui.channel = style::box(root, 150, 55, 136, 23, true);
    ui.apply = style::box(root, 8, 82, 134, 23, true);
    ui.back = style::box(root, 150, 82, 136, 23, true);
    for (auto* control : controls())
        style::control(control);
    preset_text_ = style::label(ui.preset, font, 8, 2, 260);
    protocol_text_ = style::label(ui.protocol, font, 6, 2, 120);
    channel_text_ = style::label(ui.channel, font, 6, 2, 122);
    apply_text_ = style::label(ui.apply, font, 6, 2, 120);
    back_text_ = style::label(ui.back, font, 6, 2, 122);
    feedback_ = style::label(root, font, 8, 108, 278);
    lv_obj_set_style_text_align(feedback_, LV_TEXT_ALIGN_CENTER, 0);
}
void WorkSettingsView::set_selection(unsigned protocol, unsigned channel) {
    if (protocol < DataProc::kRadioProtocolCount)
        protocol_ = protocol;
    if (channel < DataProc::kRadioChannelCount)
        channel_ = channel;
}
void WorkSettingsView::update(DataProc::WorkTarget target, bool pending, TextId feedback) const {
    const auto title = target == DataProc::WorkTarget::Base    ? TextId::RadioBase
                       : target == DataProc::WorkTarget::Rover ? TextId::RadioRover
                                                               : TextId::NetworkBase;
    lv_label_set_text(preset_text_, i18n::text(title));
    char value[64];
    i18n::option_text(TextId::WorkRadioProtocolOptions, protocol_, value, sizeof(value));
    lv_label_set_text(protocol_text_, value);
    i18n::option_text(TextId::WorkRadioChannelOptions, channel_, value, sizeof(value));
    lv_label_set_text(channel_text_, value);
    lv_label_set_text(apply_text_, i18n::text(TextId::Apply));
    lv_label_set_text(back_text_, i18n::text(TextId::Back));
    const bool radio = target != DataProc::WorkTarget::Ntrip;
    for (auto* control : {ui.protocol, ui.channel}) {
        if (radio)
            lv_obj_clear_flag(control, LV_OBJ_FLAG_HIDDEN);
        else
            lv_obj_add_flag(control, LV_OBJ_FLAG_HIDDEN);
    }
    for (auto* control : {ui.preset, ui.protocol, ui.channel, ui.apply}) {
        if (pending)
            lv_obj_add_state(control, LV_STATE_DISABLED);
        else
            lv_obj_clear_state(control, LV_STATE_DISABLED);
    }
    const auto message = feedback == TextId::Count ? TextId::WorkApplyHint : feedback;
    lv_label_set_text(feedback_, i18n::text(message));
    lv_obj_set_style_text_color(feedback_,
                                message == TextId::RequestRejected || message == TextId::RequestTimeout
                                        || message == TextId::SelectChannel
                                    ? style::kDanger
                                : message == TextId::RequestReceived ? style::kSuccess
                                                                     : style::kMuted,
                                0);
}
