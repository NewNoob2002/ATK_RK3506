#ifndef STARTUP_VIEW_H
#define STARTUP_VIEW_H

#include "lvgl.h"
#include "Resource/ResourcePool.h"
#include "Utils/I18n/I18n.h"

namespace Page {
class StartupView {
public:
    void Create(lv_obj_t* root);

    void Delete() const;

    void Update();

    void ApplyLanguage() const;

    struct {
        lv_obj_t* arc;
        lv_obj_t* arc_percent;
        lv_anim_t arc_anim;

        lv_obj_t* btnPress;
        lv_obj_t* btnLabel;
    } ui;

};
}

#endif // STARTUP_VIEW_H
