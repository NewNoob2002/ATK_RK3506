#include "main.h"
#include <stdlib.h>
#include <lvgl/lvgl.h>

#define MY_DISP_HOR_RES  126
#define	MY_DISP_VER_RES  294

uint16_t lvgl_num;
uint16_t data;

lv_disp_drv_t *g_disp_drv = NULL;


void LCD_Draw_Color_fill(int x1, int y1, int x2, int y2, lv_color_t * color_p)
{
    vu32 index_x =0;
	
	int w = x2 - x1 + 1;
	int h = y2 - y1 + 1;
	lvgl_num=w*h;

    LCD_SPI_SetDisplayWindow(x1, x2, y1, y2);
    LCD_SPI_WriteRAM_Prepare();

    for(index_x = 0; index_x <=lvgl_num; index_x++)
        u16TxBuffer[index_x] =(uint16_t *)(color_p+index_x)->full;

	tran(lvgl_num);
}


static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
   g_disp_drv = disp_drv;
   LCD_Draw_Color_fill(area->x1, area->y1, area->x2, area->y2, color_p);  //屏幕刷新函数
   lv_disp_flush_ready(disp_drv);
}

void my_rounder_cb(lv_disp_drv_t* disp_drv, lv_area_t* area) 
{
    (void)disp_drv;
    area->x1 = area->x1 & ~1;
    area->x2 = area->x2 | 1;
    area->y1 = area->y1 & ~1;
    area->y2 = area->y2 | 1;
}



void lv_port_disp_init(lv_coord_t hor_res, lv_coord_t ver_res, int rot)
{
    static lv_disp_draw_buf_t draw_buf_dsc_1;
    static lv_color_t buf_1[MY_DISP_VER_RES*40];                          /*A buffer for 10 rows*/
    static lv_color_t buf_2[MY_DISP_VER_RES*40];  
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, buf_2,MY_DISP_VER_RES*40);   /*Initialize the display buffer*/

    static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    disp_drv.rounder_cb = my_rounder_cb;

    disp_drv.draw_buf = &draw_buf_dsc_1;

    disp_drv.full_refresh = 0;

    disp_drv.sw_rotate = 1;
	disp_drv.rotated = LV_DISP_ROT_90;

    lv_disp_drv_register(&disp_drv);

}

