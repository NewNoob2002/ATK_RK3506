#include "SystemInfosView.h"

using namespace page;

#define ITEM_HEIGHT_MIN (LV_VER_RES - 8)
#define ITEM_PAD        ((LV_VER_RES - ITEM_HEIGHT_MIN) / 2)

void SystemInfosView::create(lv_obj_t* root) {
    lv_obj_set_style_pad_ver(root, ITEM_PAD, 0);

    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(root, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

    style_init();

    /* Item workmode */
    item_create(&ui.work, root, i18n::text(i18n::TextId::SystemWorkTitle), "workmode",
                i18n::text(i18n::TextId::SystemWorkInfo));

    /* Item GPS */
    item_create(&ui.gps, root, i18n::text(i18n::TextId::SystemGpsTitle), "map_location",
                i18n::text(i18n::TextId::SystemGpsInfo));

    /* Item Wi-Fi */
    item_create(&ui.wifi, root, i18n::text(i18n::TextId::SystemWifiTitle), "wifi",
                i18n::text(i18n::TextId::SystemWifiInfo));

    /* Item Battery */
    item_create(&ui.battery, root, i18n::text(i18n::TextId::SystemBatteryTitle), "battery_info",
                i18n::text(i18n::TextId::SystemBatteryInfo));

    /* Item Storage */
    item_create(&ui.storage, root, i18n::text(i18n::TextId::SystemStorageTitle), "storage",
                i18n::text(i18n::TextId::SystemStorageInfo));

    /* Item System */
    item_create(&ui.system, root, i18n::text(i18n::TextId::SystemTitle), "system_info",
                i18n::text(i18n::TextId::SystemInfo));
}

void SystemInfosView::group_init() {
    lv_group_t* group = lv_group_get_default();
    lv_group_set_wrap(group, true);
    lv_group_set_focus_cb(group, on_focus);

    const item_t* item_grp = reinterpret_cast<item_t*>(&ui);

    /* Reverse adding to group makes encoder operation more comfortable */
    for (int i = sizeof(ui) / sizeof(item_t) - 1; i >= 0; i--) {
        lv_group_add_obj(group, item_grp[i].icon);
    }

    lv_group_focus_obj(item_grp[0].icon);
}

void SystemInfosView::destroy() {
    lv_group_set_focus_cb(lv_group_get_default(), nullptr);
    style_reset();
}

void SystemInfosView::set_scroll_to_y(lv_obj_t* obj, lv_coord_t y, lv_anim_enable_t en) {
    const lv_coord_t scroll_y = lv_obj_get_scroll_y(obj);
    const lv_coord_t diff = static_cast<lv_coord_t>(-y + scroll_y);

    lv_obj_scroll_by(obj, 0, diff, en);
}

void SystemInfosView::on_focus(lv_group_t* g) {
    const lv_obj_t* icon = lv_group_get_focused(g);
    const lv_obj_t* cont = lv_obj_get_parent(icon);
    const lv_coord_t y = lv_obj_get_y(cont);
    lv_obj_scroll_to_y(lv_obj_get_parent(cont), y, LV_ANIM_ON);
}

void SystemInfosView::style_init() {
    lv_style_init(&style_.icon);
    lv_style_set_width(&style_.icon, 260);
    lv_style_set_height(&style_.icon, ITEM_HEIGHT_MIN);
    lv_style_set_bg_color(&style_.icon, lv_color_black());
    lv_style_set_bg_opa(&style_.icon, LV_OPA_COVER);
    lv_style_set_text_font(&style_.icon, resource_pool::get_font("oswaldBold_18"));
    lv_style_set_text_color(&style_.icon, lv_color_white());

    lv_style_init(&style_.focus);
    lv_style_set_width(&style_.focus, 70);
    lv_style_set_height(&style_.focus, ITEM_HEIGHT_MIN - 16);
    lv_style_set_border_side(&style_.focus, LV_BORDER_SIDE_RIGHT);
    lv_style_set_border_width(&style_.focus, 2);
    lv_style_set_border_color(&style_.focus, lv_color_hex(0xff931e));

    static constexpr lv_style_prop_t style_prop[] = {LV_STYLE_WIDTH, LV_STYLE_PROP_INV};

    static lv_style_transition_dsc_t trans;
    lv_style_transition_dsc_init(&trans, style_prop, lv_anim_path_overshoot, 200, 0, nullptr);
    lv_style_set_transition(&style_.focus, &trans);
    lv_style_set_transition(&style_.icon, &trans);

    lv_style_init(&style_.info);
    lv_style_set_text_font(&style_.info, resource_pool::get_font("oswaldBold_12"));
    lv_style_set_text_color(&style_.info, lv_color_hex(0x999999));

    lv_style_init(&style_.data);
    lv_style_set_text_font(&style_.data, resource_pool::get_font("oswaldBold_12"));
    lv_style_set_text_color(&style_.data, lv_color_white());
}

void SystemInfosView::style_reset() {
    lv_style_reset(&style_.icon);
    lv_style_reset(&style_.info);
    lv_style_reset(&style_.data);
    lv_style_reset(&style_.focus);
}

void SystemInfosView::item_create(item_t* item, lv_obj_t* par, const char* name, const char* img_src,
                                  const char* infos) {
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_enable_style_refresh(false);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, LV_HOR_RES, ITEM_HEIGHT_MIN);

    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    item->cont = cont;

    /* icon */
    lv_obj_t* icon = lv_obj_create(cont);
    lv_obj_enable_style_refresh(false);
    lv_obj_remove_style_all(icon);
    lv_obj_clear_flag(icon, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_style(icon, &style_.icon, 0);
    lv_obj_add_style(icon, &style_.focus, LV_STATE_FOCUSED);
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_set_flex_flow(icon, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(icon, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* img = lv_img_create(icon);
    lv_obj_enable_style_refresh(false);
    lv_img_set_src(img, resource_pool::get_image(img_src));

    lv_obj_t* label = lv_label_create(icon);
    lv_obj_enable_style_refresh(false);
    lv_label_set_text(label, name);
    lv_obj_set_width(label, LV_PCT(100));
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    item->label_name = label;
    item->icon = icon;

    /* infos */
    lv_obj_t* info_label = lv_label_create(cont);
    lv_obj_remove_style_all(info_label);
    lv_obj_enable_style_refresh(false);
    lv_label_set_text(info_label, infos);
    lv_obj_add_style(info_label, &style_.info, 0);
    lv_obj_align(info_label, LV_ALIGN_LEFT_MID, 85, 0);
    item->label_info = info_label;

    /* datas */
    lv_obj_t* data_label = lv_label_create(cont);
    lv_obj_remove_style_all(data_label);
    lv_obj_enable_style_refresh(false);
    lv_label_set_text(data_label, "N/A");
    lv_obj_add_style(data_label, &style_.data, 0);
    lv_obj_align(data_label, LV_ALIGN_LEFT_MID, 175, 0);
    item->label_data = data_label;

    lv_obj_move_foreground(icon);
    lv_obj_enable_style_refresh(true);

    /* get real max height */
    lv_obj_update_layout(item->label_info);
    lv_coord_t height = lv_obj_get_height(item->label_info);
    height = LV_MAX(height, ITEM_HEIGHT_MIN);
    lv_obj_set_height(cont, height);
    lv_obj_set_height(icon, height);
}

void SystemInfosView::apply_language() const {
    lv_label_set_text(ui.work.label_name, i18n::text(i18n::TextId::SystemWorkTitle));
    lv_label_set_text(ui.work.label_info, i18n::text(i18n::TextId::SystemWorkInfo));
    lv_label_set_text(ui.gps.label_name, i18n::text(i18n::TextId::SystemGpsTitle));
    lv_label_set_text(ui.gps.label_info, i18n::text(i18n::TextId::SystemGpsInfo));
    lv_label_set_text(ui.wifi.label_name, i18n::text(i18n::TextId::SystemWifiTitle));
    lv_label_set_text(ui.wifi.label_info, i18n::text(i18n::TextId::SystemWifiInfo));
    lv_label_set_text(ui.battery.label_name, i18n::text(i18n::TextId::SystemBatteryTitle));
    lv_label_set_text(ui.battery.label_info, i18n::text(i18n::TextId::SystemBatteryInfo));
    lv_label_set_text(ui.storage.label_name, i18n::text(i18n::TextId::SystemStorageTitle));
    lv_label_set_text(ui.storage.label_info, i18n::text(i18n::TextId::SystemStorageInfo));
    lv_label_set_text(ui.system.label_name, i18n::text(i18n::TextId::SystemTitle));
    lv_label_set_text(ui.system.label_info, i18n::text(i18n::TextId::SystemInfo));
}
