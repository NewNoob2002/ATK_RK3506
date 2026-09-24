/**
 * @file lv_port_disp.h
 *
 */

#ifndef LV_PORT_DISP_H
#define LV_PORT_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

void lv_port_disp_init(lv_coord_t hor_res, lv_coord_t ver_res, int rot);





static void lcd_write_cmd(uint8_t cmd);
static void lcd_set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
static void flush_chunk_complete(void *userdata);
static void disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p);
// void lv_port_disp_init(void);




extern uint16_t lvgl_num;
extern lv_disp_drv_t *g_disp_drv;
extern int lvgl_x1 , lvgl_x2 , lvgl_y1, lvgl_y2;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PORT_DISP_H*/

