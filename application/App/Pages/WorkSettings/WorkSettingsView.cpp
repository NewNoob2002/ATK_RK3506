#include "WorkSettingsView.h"
#include <cmath>
#include <cstdint>

using namespace page;

constexpr lv_coord_t font_height = 26;

int8_t WorkSettingsView::left_roller_index = 0;
int8_t WorkSettingsView::right_roller_index = 0;

static void lv_anim_label_set_y(void* obj, const int32_t y) {
    lv_obj_set_y(static_cast<lv_obj_t*>(obj), y);
}

void WorkSettingsView::create(lv_obj_t* root) {
    lv_obj_set_size(root, 294, 100);
    lv_obj_set_align(root, LV_ALIGN_BOTTOM_MID);
    roller_create(root);
    btn_cont_create(root);
}

void WorkSettingsView::destroy() {}

void WorkSettingsView::roller_create(lv_obj_t* par) {
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, 230, 90);
    lv_obj_set_align(cont, LV_ALIGN_LEFT_MID);
    ui.roller.cont = cont;

    lv_obj_t* cont_left = lv_obj_create(cont);
    lv_obj_remove_style_all(cont_left);
    lv_obj_set_style_border_color(cont_left, lv_color_white(), 0);
    lv_obj_set_style_border_width(cont_left, 1, 0);
    lv_obj_set_size(cont_left, 90, 30);
    lv_obj_align(cont_left, LV_ALIGN_LEFT_MID, 20, -20);
    ui.roller.left_roller.cont = cont_left;

    const lv_font_t* font = resource_pool::get_font("oswaldBold_18");

    lv_obj_t* label_left = lv_label_create(cont_left);
    lv_obj_set_style_text_font(label_left, font, 0);
    lv_obj_set_align(label_left, LV_ALIGN_TOP_MID);
    ui.roller.left_roller.label = label_left;

    lv_obj_t* cont_right = lv_obj_create(cont);
    lv_obj_remove_style_all(cont_right);
    lv_obj_set_style_border_color(cont_right, lv_color_white(), 0);
    lv_obj_set_style_border_width(cont_right, 1, 0);
    lv_obj_set_size(cont_right, 90, 30);
    lv_obj_align_to(cont_right, cont_left, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    ui.roller.right_roller.cont = cont_right;

    lv_obj_t* label_right = lv_label_create(cont_right);
    lv_obj_set_style_text_font(label_right, font, 0);
    lv_obj_set_align(label_right, LV_ALIGN_TOP_MID);
    ui.roller.right_roller.label = label_right;

    lv_obj_t* cont_select_left = lv_obj_create(cont);
    lv_obj_remove_style_all(cont_select_left);
    lv_obj_set_size(cont_select_left, 90, 40);
    lv_obj_align_to(cont_select_left, cont_left, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    ui.roller.left_roller.btn_up = btn_create(cont_select_left, resource_pool::get_image("up"), -20, 0);
    ui.roller.left_roller.btn_down = btn_create(cont_select_left, resource_pool::get_image("down"), 20, 0);

    lv_obj_t* cont_select_right = lv_obj_create(cont);
    lv_obj_remove_style_all(cont_select_right);
    lv_obj_set_size(cont_select_right, 120, 40);
    lv_obj_align_to(cont_select_right, cont_select_left, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

    ui.roller.right_roller.btn_up = btn_create(cont_select_right, resource_pool::get_image("up"), -40, 0);
    ui.roller.right_roller.btn_down = btn_create(cont_select_right, resource_pool::get_image("down"), 0, 0);

    ui.roller.btn_reset = btn_create(cont_select_right, resource_pool::get_image("reset"), 40, 0);
    apply_language();
}

void WorkSettingsView::apply_language() const {
    lv_label_set_text(ui.roller.left_roller.label, i18n::text(i18n::TextId::WorkRadioProtocolOptions));
    lv_label_set_text(ui.roller.right_roller.label, i18n::text(i18n::TextId::WorkRadioChannelOptions));
    roller_to_index(ui.roller.left_roller.label, left_roller_index);
    roller_to_index(ui.roller.right_roller.label, right_roller_index);
}

void WorkSettingsView::scroll(lv_obj_t* label, int delta) {
    if (label != ui.roller.left_roller.label && label != ui.roller.right_roller.label)
        return;
    int8_t& index = label == ui.roller.left_roller.label ? left_roller_index : right_roller_index;
    const int count = label == ui.roller.left_roller.label ? 8 : 10;
    index = static_cast<int8_t>((index + delta + count) % count);
    roller_to_index(label, index);
}

void WorkSettingsView::roller_style_init(lv_obj_t* obj) {
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_width(obj, 45, LV_STATE_PRESSED);
    lv_obj_set_style_height(obj, 25, LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x666666), 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xbbbbbb), LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff931e), LV_STATE_FOCUSED);
    lv_obj_set_style_radius(obj, 9, 0);

    static lv_style_transition_dsc_t tran;
    static constexpr lv_style_prop_t prop[] = {LV_STYLE_WIDTH, LV_STYLE_HEIGHT, LV_STYLE_PROP_INV};
    lv_style_transition_dsc_init(&tran, prop, lv_anim_path_ease_out, 200, 0, nullptr);
    lv_obj_set_style_transition(obj, &tran, LV_STATE_PRESSED);
    lv_obj_set_style_transition(obj, &tran, LV_STATE_FOCUSED);

    lv_obj_update_layout(obj);
}

void WorkSettingsView::btn_cont_create(lv_obj_t* par) {
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, 50, 99);
    lv_obj_align(cont, LV_ALIGN_RIGHT_MID, -10, 0);

    ui.btn_cont.cont = cont;

    ui.btn_cont.btn_base = btn_create(cont, resource_pool::get_image("base"), 0, -33);
    ui.btn_cont.btn_rover = btn_create(cont, resource_pool::get_image("rover"), 0, 0);
    ui.btn_cont.btn_ntrip = btn_create(cont, resource_pool::get_image("ntrip"), 0, 33);
}

lv_obj_t* WorkSettingsView::btn_create(lv_obj_t* par, const void* img_src, const lv_coord_t x_ofs,
                                       const lv_coord_t y_ofs) {
    lv_obj_t* obj = lv_obj_create(par);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 35, 26);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_align(obj, LV_ALIGN_CENTER, x_ofs, y_ofs);
    lv_obj_set_style_bg_img_src(obj, img_src, 0);

    roller_style_init(obj);

    return obj;
}

void WorkSettingsView::roller_to_index(lv_obj_t* obj, const uint8_t index) {
    lv_anim_del(obj, lv_anim_label_set_y);

    const lv_coord_t current_y = lv_obj_get_y(obj);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, current_y, -index * font_height);
    lv_anim_set_time(&a, 300);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_set_exec_cb(&a, lv_anim_label_set_y);
    lv_anim_start(&a);
}
