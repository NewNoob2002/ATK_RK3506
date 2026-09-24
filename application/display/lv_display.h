#ifndef LV_DISPLAY_H
#define LV_DISPLAY_H
#include "lvgl.h"
#include "rm690a0.h"

struct lv_display {
    struct rm690a0* screen;
    lv_disp_t* display;
    lv_disp_drv_t driver;
    lv_disp_draw_buf_t draw;
    lv_color_t buffers[2][RM_WIDTH * RM_HEIGHT / 4];
    int error;
};
int lv_display_register(struct lv_display* port, struct rm690a0* screen);
void lv_display_pattern(void);
#endif
