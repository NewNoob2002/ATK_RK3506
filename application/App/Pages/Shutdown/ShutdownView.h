#ifndef SHUTDOWN_VIEW_H
#define SHUTDOWN_VIEW_H

#include "Resource/ResourcePool.h"
#include "Utils/I18n/I18n.h"
#include "lvgl.h"

namespace page {
class ShutdownView {
  public:
    void create(lv_obj_t* root);

    void destroy();

    void apply_language() const;

    struct {
        struct {
            lv_obj_t* cont;
            lv_obj_t* hint_label;
            lv_obj_t* btn_label;

            struct {
                lv_obj_t* cont;
                lv_obj_t* label;
                lv_obj_t* obj;
                lv_anim_t anim;
            } bar;

            lv_obj_t* btn_press;
            lv_obj_t* btn_wifi;
            lv_obj_t* btn_wifi_label;
            lv_obj_t* wifi_loading_label;
            lv_obj_t* btn_language;
            lv_obj_t* btn_language_img;
        } shutdown;
    } ui;

    void set_wifi_status(bool enabled) const;

    void set_wifi_loading(bool loading, uint8_t step = 0) const;
};
} // namespace page

#endif // !SHUTDOWN_VIEW_H
