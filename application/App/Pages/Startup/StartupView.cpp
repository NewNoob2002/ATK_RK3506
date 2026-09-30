#include "StartupView.h"

using namespace page;

namespace {

constexpr lv_coord_t kStatusBarHeight = 26;
constexpr lv_color_t kBackground = LV_COLOR_MAKE(0x15, 0x15, 0x13);
constexpr lv_color_t kPanel = LV_COLOR_MAKE(0x22, 0x22, 0x1E);
constexpr lv_color_t kAmber = LV_COLOR_MAKE(0xE1, 0xAA, 0x22);
constexpr lv_color_t kAmberBright = LV_COLOR_MAKE(0xF6, 0xD0, 0x62);
constexpr lv_color_t kLineGrey = LV_COLOR_MAKE(0x72, 0x6E, 0x63);
constexpr lv_color_t kTextWarm = LV_COLOR_MAKE(0xE8, 0xE1, 0xCF);

lv_obj_t* create_info_label(lv_obj_t* parent, const lv_font_t* font, const char* title, const char* value,
                            lv_align_t align, lv_coord_t x, lv_coord_t y, lv_color_t value_color = kTextWarm) {
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, 72, 23);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(cont, align, x, y);

    lv_obj_t* title_label = lv_label_create(cont);
    lv_obj_remove_style_all(title_label);
    lv_obj_set_style_text_font(title_label, font, 0);
    lv_obj_set_style_text_color(title_label, kLineGrey, 0);
    lv_obj_set_style_text_letter_space(title_label, 1, 0);
    lv_label_set_text(title_label, title);
    lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t* value_label = lv_label_create(cont);
    lv_obj_remove_style_all(value_label);
    lv_obj_set_width(value_label, 72);
    lv_label_set_long_mode(value_label, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_font(value_label, font, 0);
    lv_obj_set_style_text_color(value_label, value_color, 0);
    lv_obj_set_style_text_letter_space(value_label, 1, 0);
    lv_label_set_text(value_label, value);
    lv_obj_align(value_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    return cont;
}

} // namespace

void StartupView::create(lv_obj_t* root) {
    const lv_font_t* font_small = resource_pool::get_font("oswaldBold_12");
    const lv_font_t* font_main = resource_pool::get_font("oswaldBold_18");

    lv_obj_set_style_bg_color(root, kBackground, 0);
    lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);

    lv_obj_t* content = lv_obj_create(root);
    lv_obj_remove_style_all(content);
    lv_obj_set_size(content, LV_HOR_RES, LV_VER_RES - kStatusBarHeight);
    lv_obj_set_pos(content, 0, kStatusBarHeight);
    lv_obj_clear_flag(content, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(content, kBackground, 0);
    lv_obj_set_style_bg_grad_color(content, kPanel, 0);
    lv_obj_set_style_bg_grad_dir(content, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_opa(content, LV_OPA_COVER, 0);

    create_info_label(content, font_small, "FW", "N/A", LV_ALIGN_TOP_LEFT, 18, 15);
    create_info_label(content, font_small, "HW", "N/A", LV_ALIGN_TOP_LEFT, 18, 54);
    create_info_label(content, font_small, "BAT", "N/A", LV_ALIGN_TOP_RIGHT, -8, 15, kAmberBright);
    create_info_label(content, font_small, "PWR", "N/A", LV_ALIGN_TOP_RIGHT, -8, 54);

    lv_obj_t* accent = lv_obj_create(content);
    lv_obj_remove_style_all(accent);
    lv_obj_set_size(accent, 42, 2);
    lv_obj_set_style_bg_color(accent, kAmber, 0);
    lv_obj_set_style_bg_grad_color(accent, kLineGrey, 0);
    lv_obj_set_style_bg_grad_dir(accent, LV_GRAD_DIR_HOR, 0);
    lv_obj_set_style_bg_opa(accent, LV_OPA_70, 0);
    lv_obj_align(accent, LV_ALIGN_TOP_MID, 0, 5);

    constexpr lv_coord_t arc_size = 62;
    lv_obj_t* arc = lv_arc_create(content);
    lv_obj_set_size(arc, arc_size, arc_size);
    lv_obj_align(arc, LV_ALIGN_TOP_MID, 0, 10);
    lv_arc_set_range(arc, 0, 100);
    lv_arc_set_value(arc, 0);
    lv_arc_set_bg_angles(arc, 135, 45);
    lv_arc_set_rotation(arc, 90);
    lv_obj_remove_style(arc, nullptr, LV_PART_KNOB);
    lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(arc, 5, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 5, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, kLineGrey, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(arc, LV_OPA_50, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, kAmberBright, LV_PART_INDICATOR);
    ui.arc = arc;

    lv_obj_t* label_percent = lv_label_create(content);
    lv_obj_remove_style_all(label_percent);
    lv_label_set_text(label_percent, "0%");
    lv_obj_set_style_text_font(label_percent, font_main, 0);
    lv_obj_set_style_text_color(label_percent, kTextWarm, 0);
    lv_obj_align_to(label_percent, arc, LV_ALIGN_CENTER, 0, -1);
    ui.arc_percent = label_percent;

    lv_anim_init(&ui.arc_anim);
    lv_anim_set_var(&ui.arc_anim, ui.arc);
    lv_anim_set_time(&ui.arc_anim, 200);
    lv_anim_set_playback_time(&ui.arc_anim, 0);
    lv_anim_set_repeat_count(&ui.arc_anim, 0);
    lv_anim_set_path_cb(&ui.arc_anim, lv_anim_path_linear);

    lv_obj_t* btn_press = lv_obj_create(content);
    lv_obj_remove_style_all(btn_press);
    lv_obj_set_size(btn_press, 76, 22);
    lv_obj_clear_flag(btn_press, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(btn_press, kAmber, 0);
    lv_obj_set_style_bg_opa(btn_press, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_side(btn_press, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(btn_press, kAmberBright, 0);
    lv_obj_set_style_border_width(btn_press, 2, 0);
    lv_obj_set_style_border_opa(btn_press, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(btn_press, 2, 0);

    lv_obj_set_style_border_opa(btn_press, LV_OPA_COVER, LV_STATE_FOCUSED);
    lv_obj_set_style_bg_opa(btn_press, LV_OPA_10, LV_STATE_FOCUSED);

    lv_obj_set_style_width(btn_press, 64, LV_STATE_PRESSED);
    lv_obj_set_style_translate_y(btn_press, 1, LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(btn_press, kAmberBright, LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(btn_press, LV_OPA_20, LV_STATE_PRESSED);
    lv_obj_set_style_border_color(btn_press, kAmberBright, LV_STATE_PRESSED);
    lv_obj_set_style_border_width(btn_press, 3, LV_STATE_PRESSED);
    lv_obj_set_style_border_opa(btn_press, LV_OPA_COVER, LV_STATE_PRESSED);

    lv_obj_align(btn_press, LV_ALIGN_BOTTOM_MID, 0, -4);
    ui.btn_press = btn_press;

    lv_obj_t* label_btn = lv_label_create(btn_press);
    lv_obj_remove_style_all(label_btn);
    lv_obj_set_style_text_font(label_btn, font_small, 0);
    lv_obj_set_style_text_color(label_btn, kTextWarm, 0);
    lv_obj_set_style_text_opa(label_btn, LV_OPA_80, 0);
    lv_obj_set_style_text_letter_space(label_btn, 1, 0);
    lv_obj_center(label_btn);
    ui.btn_label = label_btn;

    static lv_style_transition_dsc_t focus_transition;
    static constexpr lv_style_prop_t focus_props[] = {
        LV_STYLE_BG_OPA, LV_STYLE_BORDER_OPA,  LV_STYLE_BORDER_WIDTH,
        LV_STYLE_WIDTH,  LV_STYLE_TRANSLATE_Y, LV_STYLE_PROP_INV,
    };
    lv_style_transition_dsc_init(&focus_transition, focus_props, lv_anim_path_ease_out, 120, 0, nullptr);
    lv_obj_set_style_transition(btn_press, &focus_transition, LV_STATE_FOCUSED);
    lv_obj_set_style_transition(btn_press, &focus_transition, LV_STATE_PRESSED);

    apply_language();
}

void StartupView::destroy() const {}

void StartupView::update() {}

void StartupView::apply_language() const {
    lv_label_set_text(ui.btn_label, i18n::text(i18n::TextId::Press));
}
