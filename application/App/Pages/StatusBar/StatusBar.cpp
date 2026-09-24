#include "StatusBar.h"

#include "Resource/ResourcePool.h"

namespace Page {

void StatusBar::Create(lv_obj_t* parent) {
    root_ = lv_obj_create(parent);
    lv_obj_remove_style_all(root_);
    lv_obj_set_size(root_, LV_HOR_RES, 26);
    lv_obj_set_style_bg_color(root_, lv_color_hex(0x333333), 0);
    lv_obj_set_style_bg_opa(root_, LV_OPA_COVER, 0);
    lv_obj_clear_flag(root_, LV_OBJ_FLAG_SCROLLABLE);

    const lv_font_t* small = ResourcePool::GetFont("oswaldBold_12");
    const lv_font_t* large = ResourcePool::GetFont("oswaldBold_18");
    const lv_font_t* symbols = ResourcePool::GetFont("statusbar");

    lv_obj_t* sat_icon = lv_img_create(root_);
    lv_img_set_src(sat_icon, ResourcePool::GetImage("satellite_small"));
    lv_obj_align(sat_icon, LV_ALIGN_TOP_LEFT, 10, 5);
    satellites_ = new numberFlow(small, 2, true);
    satellites_->create(root_);
    satellites_->setAlignTo(sat_icon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    position_icon_ = lv_label_create(root_);
    lv_obj_set_style_text_font(position_icon_, symbols, 0);
    lv_label_set_text(position_icon_, CUSTOM_SYMBOL_LOCATION);
    lv_obj_align_to(position_icon_, satellites_->getCont(), LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    position_label_ = lv_label_create(root_);
    lv_obj_set_style_text_font(position_label_, small, 0);
    lv_obj_set_style_text_color(position_label_, lv_color_white(), 0);
    lv_label_set_text(position_label_, "NONE");
    lv_obj_align_to(position_label_, position_icon_, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    clock_ = new numberFlow_clock(large);
    clock_->create(root_);
    clock_->setPos(LV_ALIGN_TOP_MID, 0, 0);

    sd_icon_ = lv_label_create(root_);
    lv_obj_set_style_text_font(sd_icon_, symbols, 0);
    lv_label_set_text(sd_icon_, CUSTOM_SYMBOL_SD_CARD);
    lv_obj_align_to(sd_icon_, clock_->getCont(), LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    wifi_icon_ = lv_label_create(root_);
    lv_obj_set_style_text_font(wifi_icon_, symbols, 0);
    lv_label_set_text(wifi_icon_, CUSTOM_SYMBOL_WIFI);
    lv_obj_align_to(wifi_icon_, sd_icon_, LV_ALIGN_OUT_RIGHT_MID, 2, 0);

    lv_obj_t* battery = lv_img_create(root_);
    lv_img_set_src(battery, ResourcePool::GetImage("battery"));
    lv_obj_align(battery, LV_ALIGN_TOP_RIGHT, -40, 5);
    battery_fill_ = lv_obj_create(battery);
    lv_obj_remove_style_all(battery_fill_);
    lv_obj_set_style_bg_opa(battery_fill_, LV_OPA_COVER, 0);
    lv_obj_set_size(battery_fill_, 16, 8);
    lv_obj_align(battery_fill_, LV_ALIGN_LEFT_MID, 2, 0);
    battery_percent_ = new numberFlow(large, 3, true);
    battery_percent_->create(root_);
    battery_percent_->setAlignTo(battery, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    Update({});
}

void StatusBar::Update(const StatusBarState& state) {
    if (state.satellites_valid)
        satellites_->setValue(state.satellites > 99 ? 99 : state.satellites);
    if (state.satellites_valid)
        lv_obj_clear_flag(satellites_->getCont(), LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(satellites_->getCont(), LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(position_label_, state.position ? state.position : "DEMO");
    lv_obj_set_style_text_color(position_icon_, state.position_color, 0);
    if (state.clock_valid)
        clock_->setTime(state.hour % 24, state.minute % 60, state.second % 60);
    if (state.clock_valid)
        lv_obj_clear_flag(clock_->getCont(), LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(clock_->getCont(), LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_text_color(sd_icon_, state.recording ? lv_palette_main(LV_PALETTE_BLUE) : lv_color_white(), 0);
    lv_obj_set_style_text_color(wifi_icon_, state.wifi ? lv_palette_main(LV_PALETTE_BLUE) : lv_color_white(), 0);
    const unsigned percent = state.battery_percent > 100 ? 100 : state.battery_percent;
    if (state.battery_valid) {
        battery_percent_->setValue(percent);
        lv_obj_set_width(battery_fill_, lv_map(percent, 0, 100, 0, 16));
        lv_obj_set_style_bg_color(battery_fill_, lv_color_hex(percent > 50 ? 0x4caf50 : percent > 20 ? 0xff9800 : 0xf44336), 0);
        lv_obj_clear_flag(battery_percent_->getCont(), LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_set_width(battery_fill_, 0);
        lv_obj_add_flag(battery_percent_->getCont(), LV_OBJ_FLAG_HIDDEN);
    }
}

void StatusBar::Delete() {
    delete satellites_;
    delete battery_percent_;
    delete clock_;
    satellites_ = nullptr;
    battery_percent_ = nullptr;
    clock_ = nullptr;
    if (root_) {
        lv_obj_del(root_);
        root_ = nullptr;
    }
}

} // namespace Page
