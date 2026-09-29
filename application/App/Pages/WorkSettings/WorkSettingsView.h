#ifndef WorkSettings_VIEW_H
#define WorkSettings_VIEW_H

#include "Resource/ResourcePool.h"
#include "Utils/I18n/I18n.h"
#include "lvgl.h"

namespace Page {
class WorkSettingsView {
  public:
    struct {
        struct {
            lv_obj_t* cont;

            struct {
                lv_obj_t* cont;
                lv_obj_t* label;

                lv_obj_t* btnCont;
                lv_obj_t* btnUp;
                lv_obj_t* btnDown;
            } left_roller;

            struct {
                lv_obj_t* cont;
                lv_obj_t* label;

                lv_obj_t* btnCont;
                lv_obj_t* btnUp;
                lv_obj_t* btnDown;
            } right_roller;

            lv_obj_t* btnReset;
        } roller;

        struct {
            lv_obj_t* cont;
            lv_obj_t* btnBase;
            lv_obj_t* btnRover;
            lv_obj_t* btnNtrip;
        } btnCont;

    } ui;

    WorkSettingsView() = default;

    ~WorkSettingsView() = default;

    void Create(lv_obj_t* root);

    void Delete();

    void ApplyLanguage() const;
    /** 仅更新本页滚轮预览，不写入电台配置。delta 为 -1（上一项）或 1（下一项）。 */
    void Scroll(lv_obj_t* label, int delta);

    void Roller_Create(lv_obj_t* par);

    static void Roller_Style_Init(lv_obj_t* obj);

    void BtnCont_Create(lv_obj_t* par);

    static lv_obj_t* Btn_Create(lv_obj_t* par, const void* img_src, lv_coord_t x_ofs, lv_coord_t y_ofs);

    static void Roller_toIndex(lv_obj_t* obj, uint8_t index);

  private:
    static int8_t left_roller_index;
    static int8_t right_roller_index;
};

} // namespace Page

#endif // WorkSettings_VIEW_H
