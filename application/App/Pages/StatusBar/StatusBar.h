#pragma once

#include "Utils/lv_anim_label/numberFlow.h"
#include "Utils/lv_anim_label/numberFlow_clock.h"

namespace Page {

/** 状态由调用方提供；此 View 不读取板级或业务全局变量。数值仅在 LVGL 线程更新。 */
struct StatusBarState {
    unsigned satellites = 0;
    bool satellites_valid = false;
    const char* position = "DEMO";
    lv_color_t position_color = lv_color_hex(0xf44336);
    unsigned hour = 0;
    unsigned minute = 0;
    unsigned second = 0;
    bool clock_valid = false;
    bool recording = false;
    bool wifi = false;
    unsigned battery_percent = 0;
    bool battery_valid = false;
};

class StatusBar {
  public:
    void Create(lv_obj_t* parent);
    void Update(const StatusBarState& state);
    void Delete();
    lv_obj_t* Root() const { return root_; }

  private:
    lv_obj_t* root_ = nullptr;
    lv_obj_t* position_icon_ = nullptr;
    lv_obj_t* position_label_ = nullptr;
    lv_obj_t* sd_icon_ = nullptr;
    lv_obj_t* wifi_icon_ = nullptr;
    lv_obj_t* battery_fill_ = nullptr;
    numberFlow* satellites_ = nullptr;
    numberFlow* battery_percent_ = nullptr;
    numberFlow_clock* clock_ = nullptr;
};

} // namespace Page
