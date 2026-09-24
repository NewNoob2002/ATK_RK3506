#ifndef DIALPLATE_VIEW_H
#define DIALPLATE_VIEW_H

#include "Utils/lv_anim_label/numberFlow.h"
#include "lvgl.h"

namespace Page {

class DialplateView {

    typedef struct Transform {
        lv_coord_t width_default;
        lv_coord_t height_default;
        lv_coord_t transform_width;
        lv_coord_t transform_height;
    } TransformInfo_t;

  public:
    struct {
        struct {
            lv_obj_t* cont;
            lv_obj_t* icon_satellite;
            lv_obj_t* icon_radio;
            lv_obj_t* icon_mode;
            numberFlow* satellite_used;
            numberFlow* satellite_tacked;
        } topInfo;

        struct {
            lv_obj_t* cont;
            lv_obj_t* btnMap;
            lv_obj_t* btnRec;
            lv_obj_t* btnMenu;
            lv_obj_t* btnShutdown;
        } btnCont;

        lv_anim_timeline_t* anim_timeline;
    } ui;

    /** 在已初始化的 LVGL 中创建主界面；LVGL 子对象由 root 持有。 */
    void Create(lv_obj_t* root);

    /** 释放动画和 C++ 辅助对象；调用方仍须清理 root 下的 LVGL 子对象。 */
    void Delete();

    void AppearAnimStart(bool reverse = false) const;

  private:
    void TopInfo_Create(lv_obj_t* par);

    void BtnCont_Create(lv_obj_t* par);

    static lv_obj_t* Btn_Create(lv_obj_t* par, const void* img_src, lv_coord_t x_ofs, const TransformInfo_t& transform);
};

} // namespace Page

#endif // DIALPLATE_VIEW_H
