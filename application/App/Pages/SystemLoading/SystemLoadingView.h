//
// Created by guoti on 2025/12/14.
//

#ifndef LVGL_SYSTEM_LOADING_VIEW_H
#define LVGL_SYSTEM_LOADING_VIEW_H
#include "Resource/ResourcePool.h"
#include "Utils/I18n/I18n.h"
#include "Utils/lv_ext/lv_anim_timeline_wrapper.h"
#include "lvgl.h"

namespace page {
class SystemLoadingView {
  public:
    void create(lv_obj_t* root);

    void destroy();

    void update() const;

    void apply_language() const;

    struct {
        lv_obj_t* cont;
        lv_obj_t* logo_label;
        lv_obj_t* bar_label;
        lv_anim_t bar_anim;
        lv_obj_t* bar_percent;
        lv_anim_timeline_t* anim_timeline;
        lv_anim_t anim_label;

        lv_obj_t* img_logo;
    } ui;
};
} // namespace page

#endif //LVGL_SYSTEM_LOADING_VIEW_H
