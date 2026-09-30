#include "ShutdownView.h"

using namespace page;

void ShutdownView::create(lv_obj_t* root) {
    const lv_font_t* font = resource_pool::get_font("oswaldBold_18");
    const lv_font_t* font_small = resource_pool::get_font("oswaldBold_12");

    lv_obj_t* main_cont = lv_obj_create(root);
    lv_obj_remove_style_all(main_cont);
    lv_obj_set_size(main_cont, LV_HOR_RES, LV_VER_RES);
    lv_obj_center(main_cont);
    ui.shutdown.cont = main_cont;

    lv_obj_t* label = lv_label_create(main_cont);
    lv_obj_remove_style_all(label);
    lv_obj_set_style_text_font(label, font_small, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 30);
    ui.shutdown.hint_label = label;

    lv_obj_t* cont = lv_obj_create(main_cont);
    lv_obj_remove_style_all(cont);
    lv_obj_set_style_border_color(cont, lv_color_white(), 0);
    lv_obj_set_style_border_width(cont, 1, 0);
    lv_obj_set_size(cont, 102, 12);
    lv_obj_align(cont, LV_ALIGN_CENTER, 0, 10);
    ui.shutdown.bar.cont = cont;

    lv_obj_t* bar = lv_obj_create(cont);
    lv_obj_remove_style_all(bar);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0xf9181c), 0);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
    lv_obj_set_style_opa(bar, LV_OPA_COVER, 0);
    lv_obj_set_width(bar, 0);
    lv_obj_set_height(bar, 10);
    lv_obj_align(bar, LV_ALIGN_LEFT_MID, 0, 0);
    ui.shutdown.bar.obj = bar;

    lv_anim_init(&ui.shutdown.bar.anim);
    lv_anim_set_var(&ui.shutdown.bar.anim, bar);
    lv_anim_set_playback_time(&ui.shutdown.bar.anim, 0);
    lv_anim_set_repeat_count(&ui.shutdown.bar.anim, 0);

    lv_obj_t* bar_percent = lv_label_create(main_cont);
    lv_obj_remove_style_all(bar_percent);
    lv_obj_set_style_text_font(bar_percent, font_small, 0);
    lv_label_set_text(bar_percent, "0%");
    lv_obj_align(bar_percent, LV_ALIGN_CENTER, 70, 10);
    ui.shutdown.bar.label = bar_percent;

    lv_obj_t* btn_press = lv_obj_create(main_cont);
    lv_obj_remove_style_all(btn_press);
    lv_obj_set_size(btn_press, 65, 30);
    lv_obj_clear_flag(btn_press, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_align(btn_press, LV_ALIGN_BOTTOM_MID, 0, -10);

    lv_obj_set_style_bg_opa(btn_press, LV_OPA_COVER, 0);
    lv_obj_set_style_width(btn_press, 70, LV_STATE_PRESSED);
    lv_obj_set_style_height(btn_press, 25, LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(btn_press, lv_color_hex(0x666666), 0);
    lv_obj_set_style_bg_color(btn_press, lv_color_hex(0xdd3c3b), LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(btn_press, lv_color_hex(0xff931e), LV_STATE_FOCUSED);
    lv_obj_set_style_radius(btn_press, 9, 0);

    static lv_style_transition_dsc_t tran;
    static constexpr lv_style_prop_t prop[] = {LV_STYLE_WIDTH, LV_STYLE_HEIGHT, LV_STYLE_PROP_INV};
    lv_style_transition_dsc_init(&tran, prop, lv_anim_path_ease_out, 200, 0, nullptr);
    lv_obj_set_style_transition(btn_press, &tran, LV_STATE_PRESSED);
    lv_obj_set_style_transition(btn_press, &tran, LV_STATE_FOCUSED);
    lv_obj_update_layout(btn_press);
    ui.shutdown.btn_press = btn_press;

    lv_obj_t* label_btn = lv_label_create(btn_press);
    lv_obj_remove_style_all(label_btn);
    lv_obj_set_style_text_font(label_btn, font, 0);
    lv_obj_center(label_btn);
    ui.shutdown.btn_label = label_btn;

    lv_obj_t* btn_wifi = lv_obj_create(main_cont);
    lv_obj_remove_style_all(btn_wifi);
    lv_obj_set_size(btn_wifi, 36, 28);
    lv_obj_clear_flag(btn_wifi, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(btn_wifi, LV_ALIGN_TOP_LEFT, 10, 34);
    lv_obj_set_style_bg_opa(btn_wifi, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn_wifi, lv_color_hex(0x666666), 0);
    lv_obj_set_style_bg_color(btn_wifi, lv_color_hex(0xff931e), LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(btn_wifi, lv_color_hex(0xbbbbbb), LV_STATE_PRESSED);
    lv_obj_set_style_width(btn_wifi, 40, LV_STATE_PRESSED);
    lv_obj_set_style_height(btn_wifi, 24, LV_STATE_PRESSED);
    lv_obj_set_style_radius(btn_wifi, 6, 0);

    lv_style_transition_dsc_init(&tran, prop, lv_anim_path_ease_out, 200, 0, nullptr);
    lv_obj_set_style_transition(btn_wifi, &tran, LV_STATE_PRESSED);
    lv_obj_set_style_transition(btn_wifi, &tran, LV_STATE_FOCUSED);
    lv_obj_update_layout(btn_wifi);
    ui.shutdown.btn_wifi = btn_wifi;

    lv_obj_t* label_wifi = lv_label_create(btn_wifi);
    lv_obj_remove_style_all(label_wifi);
    lv_obj_set_style_text_font(label_wifi, resource_pool::get_font("statusbar"), 0);
    lv_label_set_text(label_wifi, CUSTOM_SYMBOL_WIFI);
    lv_obj_center(label_wifi);
    ui.shutdown.btn_wifi_label = label_wifi;

    lv_obj_t* wifi_loading_label = lv_label_create(main_cont);
    lv_obj_remove_style_all(wifi_loading_label);
    lv_obj_set_width(wifi_loading_label, 24);
    lv_obj_set_style_text_font(wifi_loading_label, font, 0);
    lv_obj_set_style_text_color(wifi_loading_label, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_text_align(wifi_loading_label, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_text(wifi_loading_label, "...");
    lv_obj_align_to(wifi_loading_label, btn_wifi, LV_ALIGN_OUT_RIGHT_MID, 6, -3);
    lv_obj_add_flag(wifi_loading_label, LV_OBJ_FLAG_HIDDEN);
    ui.shutdown.wifi_loading_label = wifi_loading_label;

    lv_obj_t* btn_language = lv_obj_create(main_cont);
    lv_obj_remove_style_all(btn_language);
    lv_obj_set_size(btn_language, 36, 28);
    lv_obj_clear_flag(btn_language, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(btn_language, LV_ALIGN_TOP_RIGHT, -10, 34);
    lv_obj_set_style_bg_opa(btn_language, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btn_language, lv_color_hex(0x666666), 0);
    lv_obj_set_style_bg_color(btn_language, lv_color_hex(0xff931e), LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(btn_language, lv_color_hex(0xbbbbbb), LV_STATE_PRESSED);
    lv_obj_set_style_radius(btn_language, 6, 0);
    ui.shutdown.btn_language = btn_language;

    lv_obj_t* img_language = lv_img_create(btn_language);
    lv_obj_remove_style_all(img_language);
    lv_obj_center(img_language);
    ui.shutdown.btn_language_img = img_language;

    apply_language();
}

void ShutdownView::destroy() {}

void ShutdownView::set_wifi_status(const bool enabled) const {
    const lv_color_t color = enabled ? lv_palette_main(LV_PALETTE_BLUE) : lv_color_white();
    lv_obj_set_style_text_color(ui.shutdown.btn_wifi_label, color, LV_STATE_DEFAULT);
}

void ShutdownView::set_wifi_loading(const bool loading, const uint8_t step) const {
    if (loading) {
        static const char* const dots[] = {".", "..", "..."};
        lv_label_set_text(ui.shutdown.wifi_loading_label, dots[step % 3]);
        lv_obj_clear_flag(ui.shutdown.wifi_loading_label, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(ui.shutdown.wifi_loading_label, LV_OBJ_FLAG_HIDDEN);
    }
}

void ShutdownView::apply_language() const {
    lv_label_set_text(ui.shutdown.hint_label, i18n::text(i18n::TextId::ShutdownHint));
    lv_label_set_text(ui.shutdown.btn_label, i18n::text(i18n::TextId::Press));
    lv_img_set_src(ui.shutdown.btn_language_img,
                   resource_pool::get_image(i18n::get_language() == i18n::Language::Russian ? "NationalFlag_RU"
                                                                                            : "NationalFlag_EN"));
    lv_obj_center(ui.shutdown.btn_language_img);
}
