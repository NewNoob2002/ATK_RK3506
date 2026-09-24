#ifndef SYSTEM_INFOS_VIEW_H
#define SYSTEM_INFOS_VIEW_H

#include "lvgl.h"
#include "Resource/ResourcePool.h"
#include "Utils/I18n/I18n.h"

namespace Page
{
    class SystemInfosView
    {
    public:
        SystemInfosView()
        {
        }

        ~SystemInfosView()
        {
        }

        void Create(lv_obj_t *root);

        void Delete();

        void ApplyLanguage() const;

        typedef struct {
            lv_obj_t *cont;
            lv_obj_t *icon;
            lv_obj_t *labelName;
            lv_obj_t *labelInfo;
            lv_obj_t *labelData;
        } item_t;

        struct {
            item_t work;
            item_t gps;
            item_t wifi;
            item_t battery;
            item_t storage;
            item_t system;
        } ui;

        static void SetScrollToY(lv_obj_t *obj, lv_coord_t y, lv_anim_enable_t en);

        static void onFocus(lv_group_t *e);

    private:
        struct {
            lv_style_t icon;
            lv_style_t focus;
            lv_style_t info;
            lv_style_t data;
        } style;

    private:
        void Group_Init();

        void Style_Init();

        void Style_Reset();

        void Item_Create(
            item_t *item,
            lv_obj_t *par,
            const char *name,
            const char *img_src,
            const char *infos);
    };
}

#endif // SYSTEM_INFOS_VIEW_H
