#ifndef SHUTDOWN_VIEW_H
#define SHUTDOWN_VIEW_H

#include "lvgl.h"
#include "Resource/ResourcePool.h"
#include "Utils/I18n/I18n.h"

namespace Page {
class ShutdownView {
public:
    void Create(lv_obj_t* root);

    void Delete();

    void ApplyLanguage() const;

    struct {
        struct {
            lv_obj_t* cont;
            lv_obj_t* hintLabel;
            lv_obj_t* btnLabel;

            struct {
                lv_obj_t* cont;
                lv_obj_t* label;
                lv_obj_t* obj;
                lv_anim_t anim;
            } bar;

            lv_obj_t* btnPress;
            lv_obj_t* btnWifi;
            lv_obj_t* btnWifiLabel;
            lv_obj_t* wifiLoadingLabel;
            lv_obj_t* btnLanguage;
            lv_obj_t* btnLanguageImg;
        } shutdown;
    } ui;

    void SetWifiStatus(bool enabled) const;

    void SetWifiLoading(bool loading, uint8_t step = 0) const;
};
}

#endif // !SHUTDOWN_VIEW_H
