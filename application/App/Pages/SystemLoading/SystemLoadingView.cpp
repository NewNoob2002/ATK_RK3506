#include "SystemLoadingView.h"
#include <initializer_list>
#include "Utils/PageStyle.h"

using namespace page;

namespace {
constexpr lv_coord_t kTrackWidth = 246;
constexpr lv_color_t kOrange = LV_COLOR_MAKE(0xFF, 0x93, 0x1E);
constexpr lv_color_t kGreen = LV_COLOR_MAKE(0x28, 0xC7, 0x6F);
constexpr lv_color_t kGrey = LV_COLOR_MAKE(0x99, 0x99, 0x99);
constexpr lv_color_t kTrack = LV_COLOR_MAKE(0x33, 0x33, 0x33);

void set_opa(void* obj, int32_t opa) {
    lv_obj_set_style_opa(static_cast<lv_obj_t*>(obj), static_cast<lv_opa_t>(opa), 0);
}
void hide_after_fade(lv_anim_t* anim) {
    lv_obj_add_flag(static_cast<lv_obj_t*>(anim->var), LV_OBJ_FLAG_HIDDEN);
}
void draw_check(lv_event_t* event) {
    auto* obj = lv_event_get_target(event);
    lv_area_t area;
    lv_obj_get_coords(obj, &area);
    const lv_point_t points[] = {{static_cast<lv_coord_t>(area.x1 + 1), static_cast<lv_coord_t>(area.y1 + 11)},
                                 {static_cast<lv_coord_t>(area.x1 + 8), static_cast<lv_coord_t>(area.y1 + 18)},
                                 {static_cast<lv_coord_t>(area.x1 + 24), static_cast<lv_coord_t>(area.y1 + 2)}};
    lv_draw_line_dsc_t line;
    lv_draw_line_dsc_init(&line);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &line);
    auto* ctx = lv_event_get_draw_ctx(event);
    lv_draw_line(ctx, &line, &points[0], &points[1]);
    lv_draw_line(ctx, &line, &points[1], &points[2]);
}
void rotate_gear(void* obj, int32_t angle) {
    lv_img_set_angle(static_cast<lv_obj_t*>(obj), static_cast<int16_t>(angle));
}
void animate(lv_obj_t* obj, lv_anim_exec_xcb_t exec, int32_t from, int32_t to, uint32_t time_ms,
             lv_anim_ready_cb_t finished = nullptr) {
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, obj);
    lv_anim_set_values(&anim, from, to);
    lv_anim_set_time(&anim, time_ms);
    lv_anim_set_exec_cb(&anim, exec);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);
    lv_anim_set_ready_cb(&anim, finished);
    if (!lv_anim_start(&anim)) {
        exec(obj, to);
        if (finished)
            finished(&anim);
        LV_LOG_ERROR("SystemLoading: animation allocation failed; applied endpoint");
    }
}
} // namespace

void SystemLoadingView::create(lv_obj_t* root) {
    lv_obj_set_style_bg_color(root, lv_color_black(), 0);
    ui.cont = style::box(root, 8, 2, 278, 122);
    lv_obj_set_style_bg_color(ui.cont, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(ui.cont, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(ui.cont, 10, 0);
    lv_obj_set_user_data(ui.cont, this);

    ui.ring = lv_arc_create(ui.cont);
    lv_obj_remove_style_all(ui.ring);
    lv_obj_set_pos(ui.ring, 119, 4);
    lv_obj_set_size(ui.ring, 40, 40);
    lv_obj_clear_flag(ui.ring, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_arc_set_bg_angles(ui.ring, 0, 360);
    lv_arc_set_rotation(ui.ring, 270);
    lv_arc_set_range(ui.ring, 0, 100);
    lv_obj_set_style_arc_width(ui.ring, 4, LV_PART_MAIN);
    lv_obj_set_style_arc_width(ui.ring, 4, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(ui.ring, kTrack, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(ui.ring, true, LV_PART_INDICATOR);
    lv_obj_set_style_arc_opa(ui.ring, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(ui.ring, LV_OPA_COVER, LV_PART_INDICATOR);
    ui.gear = lv_img_create(ui.cont);
    lv_img_set_src(ui.gear, resource_pool::get_image("settings"));
    lv_obj_set_pos(ui.gear, 131, 16);
    lv_img_set_zoom(ui.gear, 384);
    lv_obj_set_style_img_recolor(ui.gear, lv_color_white(), 0);
    lv_obj_set_style_img_recolor_opa(ui.gear, LV_OPA_COVER, 0);
    ui.check = style::box(ui.cont, 126, 13, 26, 23);
    lv_obj_add_event_cb(ui.check, draw_check, LV_EVENT_DRAW_MAIN, nullptr);
    lv_obj_set_style_line_color(ui.check, kGreen, 0);
    lv_obj_set_style_line_width(ui.check, 3, 0);
    lv_obj_set_style_line_rounded(ui.check, true, 0);

    const auto* title_font = resource_pool::get_font("oswaldBold_18");
    const auto* small_font = resource_pool::get_font("oswaldBold_12");
    auto* demo = style::label(ui.cont, small_font, 220, 5, 44);
    lv_label_set_text(demo, "DEMO"); // Ready only describes this preview, never hardware readiness.
    lv_obj_set_style_text_color(demo, kGrey, 0);
    lv_obj_set_style_text_align(demo, LV_TEXT_ALIGN_RIGHT, 0);
    ui.service = style::label(ui.cont, title_font, 16, 40, kTrackWidth);
    lv_obj_set_style_text_align(ui.service, LV_TEXT_ALIGN_CENTER, 0);
    ui.subtitle = style::label(ui.cont, small_font, 16, 66, kTrackWidth);
    lv_obj_set_style_text_align(ui.subtitle, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(ui.subtitle, kGrey, 0);
    auto* track = style::box(ui.cont, 16, 83, kTrackWidth, 8);
    lv_obj_set_style_bg_color(track, kTrack, 0);
    lv_obj_set_style_bg_opa(track, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(track, 4, 0);
    ui.bar = style::box(track, 0, 0, 0, 8);
    lv_obj_set_style_bg_opa(ui.bar, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(ui.bar, 4, 0);
    lv_obj_set_user_data(ui.bar, this);
    ui.step = style::label(ui.cont, small_font, 16, 91, 128);
    ui.percent = style::label(ui.cont, title_font, 216, 91, 48);
    lv_obj_set_style_text_align(ui.percent, LV_TEXT_ALIGN_RIGHT, 0);
    for (int i = 0; i < 5; ++i) {
        ui.dots[i] = style::box(ui.cont, 139 + i * 18, 112, 8, 8);
        lv_obj_set_style_radius(ui.dots[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_border_width(ui.dots[i], 1, 0);
        lv_obj_set_style_bg_opa(ui.dots[i], LV_OPA_COVER, 0);
        if (i < 4) {
            ui.links[i] = style::box(ui.cont, 147 + i * 18, 115, 10, 1);
            lv_obj_set_style_bg_opa(ui.links[i], LV_OPA_COVER, 0);
        }
    }
    ui.footer = style::label(ui.cont, small_font, 16, 105, 116);
    lv_obj_set_style_text_color(ui.footer, kGrey, 0);
    ui.img_logo = lv_img_create(root);
#if defined(RGK_LOGO_USE)
    lv_img_set_src(ui.img_logo, resource_pool::get_image("RGKLogo"));
#elif defined(MIDDLE_LOGO_USE)
    lv_img_set_src(ui.img_logo, resource_pool::get_image("MiddleLogo"));
#else
    lv_img_set_src(ui.img_logo, resource_pool::get_image("startupLogo"));
#endif
    lv_obj_center(ui.img_logo);
    update_text();
    show_logo();
}

void SystemLoadingView::show_logo() const {
    lv_obj_add_flag(ui.cont, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui.img_logo, LV_OBJ_FLAG_HIDDEN);
    set_opa(ui.img_logo, LV_OPA_COVER);
}

void SystemLoadingView::show_initialization() {
    ready_ = false;
    step_ = 1;
    lv_obj_clear_flag(ui.cont, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui.check, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui.gear, LV_OBJ_FLAG_HIDDEN);
    set_opa(ui.gear, LV_OPA_COVER);
    set_opa(ui.service, LV_OPA_COVER);
    set_opa(ui.subtitle, LV_OPA_COVER);
    for (auto* dot : ui.dots)
        set_opa(dot, LV_OPA_COVER); // Cached re-entry must not inherit a cancelled fade.
    set_accent(kOrange);
    update_progress(0);
    update_text();
    animate(ui.img_logo, set_opa, LV_OPA_COVER, LV_OPA_TRANSP, kTransitionMs, hide_after_fade);
    animate(ui.cont, set_opa, LV_OPA_TRANSP, LV_OPA_COVER, kTransitionMs);
    animate(ui.gear, rotate_gear, 0, 3600, kInitializationMs);
    animate(ui.bar, set_progress, 0, 100, kInitializationMs);
}

void SystemLoadingView::show_ready() {
    lv_anim_del(ui.bar, set_progress);
    lv_anim_del(ui.gear, rotate_gear);
    ready_ = true;
    step_ = 5;
    lv_obj_clear_flag(ui.check, LV_OBJ_FLAG_HIDDEN);
    update_progress(100);
    update_text();
    animate(ui.cont, set_ready_mix, 0, LV_OPA_COVER, kTransitionMs);
    lv_obj_fade_in(ui.service, kTransitionMs, 0);
    lv_obj_fade_in(ui.subtitle, kTransitionMs, 0);
}

void SystemLoadingView::set_progress(void* obj, int32_t percent) {
    auto* view = static_cast<SystemLoadingView*>(lv_obj_get_user_data(static_cast<lv_obj_t*>(obj)));
    view->update_progress(percent);
}
void SystemLoadingView::set_ready_mix(void* obj, int32_t mix) {
    auto* view = static_cast<SystemLoadingView*>(lv_obj_get_user_data(static_cast<lv_obj_t*>(obj)));
    const auto color = lv_color_mix(kGreen, kOrange, static_cast<uint8_t>(mix));
    view->set_accent(color);
    set_opa(view->ui.check, mix);
    set_opa(view->ui.gear, LV_OPA_COVER - mix);
    for (int i = 0; i < 5; ++i) {
        lv_obj_set_style_bg_color(view->ui.dots[i], color, 0);
        lv_obj_set_style_border_color(view->ui.dots[i], color, 0);
        if (i < 4)
            lv_obj_set_style_bg_color(view->ui.links[i], color, 0);
    }
}
void SystemLoadingView::update_progress(int32_t percent) {
    progress_ = percent < 0 ? 0 : (percent > 100 ? 100 : percent);
    lv_obj_set_width(ui.bar, static_cast<lv_coord_t>(progress_ * kTrackWidth / 100));
    lv_arc_set_value(ui.ring, static_cast<int16_t>(progress_));
    lv_label_set_text_fmt(ui.percent, "%ld%%", static_cast<long>(progress_));
    const int next_step = progress_ < 20 ? 1 : progress_ < 40 ? 2 : progress_ < 70 ? 3 : progress_ < 90 ? 4 : 5;
    if (next_step != step_) {
        step_ = next_step;
        update_text();
        lv_obj_fade_in(ui.service, 160, 0);
        lv_obj_fade_in(ui.dots[step_ - 1], 160, 0);
    }
    if (ready_)
        lv_label_set_text(ui.step, i18n::text(i18n::TextId::StartupCompleted));
    else
        lv_label_set_text_fmt(ui.step, i18n::text(i18n::TextId::InitializationStep), step_);
    for (int i = 0; i < 5; ++i) {
        const bool complete = ready_ || i < step_ - 1;
        const bool active = !ready_ && i == step_ - 1;
        const auto color = ready_ ? kGreen : kOrange;
        lv_obj_set_style_bg_color(ui.dots[i], complete ? color : kTrack, 0);
        lv_obj_set_style_border_color(ui.dots[i], complete || active ? color : kGrey, 0);
        lv_obj_set_style_border_width(ui.dots[i], active ? 2 : 1, 0);
        if (i < 4)
            lv_obj_set_style_bg_color(ui.links[i], complete ? color : kTrack, 0);
    }
}

void SystemLoadingView::set_accent(lv_color_t color) {
    lv_obj_set_style_arc_color(ui.ring, color, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(ui.bar, color, 0);
}
void SystemLoadingView::update_text() const {
    static constexpr i18n::TextId stages[] = {i18n::TextId::LoadingConfiguration, i18n::TextId::PreparingServices,
                                              i18n::TextId::StartingGnssService, i18n::TextId::PreparingNetwork,
                                              i18n::TextId::FinalizingStartup};
    lv_label_set_text(ui.service, i18n::text(ready_ ? i18n::TextId::SystemReady : stages[step_ - 1]));
    lv_label_set_text(ui.subtitle,
                      i18n::text(ready_ ? i18n::TextId::ServicesAvailable : i18n::TextId::PreparingModules));
    lv_label_set_text(ui.footer, i18n::text(i18n::TextId::SystemLoadingTitle));
}
void SystemLoadingView::destroy() {
    // Includes fades and callbacks borrowing View/widget pointers; cancel before the root is destroyed.
    for (auto* obj : {ui.cont, ui.bar, ui.gear, ui.check, ui.img_logo, ui.service, ui.subtitle})
        lv_anim_del(obj, nullptr);
    for (auto* dot : ui.dots)
        lv_anim_del(dot, nullptr);
}
void SystemLoadingView::apply_language() const {
    update_text();
    if (ready_)
        lv_label_set_text(ui.step, i18n::text(i18n::TextId::StartupCompleted));
    else
        lv_label_set_text_fmt(ui.step, i18n::text(i18n::TextId::InitializationStep), step_);
}
