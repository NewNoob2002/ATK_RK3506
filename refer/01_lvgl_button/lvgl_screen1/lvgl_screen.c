#include "main.h"


int screen_destition=10;


void LVGL_screen_init()
{
	  lv_screen1_init();
		lv_screen2_init();
		lv_screen3_init();
//		lv_screen4_init();
		lv_screen5_init();
		lv_screen6_init();
//		lv_screen7_init();
		lv_screen8_init();
	
	  lv_scr_load(scr1);
}
















//lv_obj_t *scr1 = NULL;              //����Ļ
//lv_style_t style_main;
///*****************************************************************/
///************  ������screen���壬��ϵͳ����һ��screen   ************/
///*****************************************************************/
//void lv_sigularxyz_scr_main(void)
//{
//	  scr1 = lv_obj_create(NULL);
//    lv_style_init( &style_main );
//    lv_style_set_bg_opa( &style_main, LV_OPA_COVER );
//	  lv_style_set_bg_color( &style_main, lv_color_hex(0x0000) );
//	  lv_obj_add_style(scr1, &style_main, 0);	
//}



//void lv_sigularxyz_scr_poweroff_display(void)
//{
//		 lv_obj_add_flag(obj_bottom_main, LV_OBJ_FLAG_HIDDEN);
//	   lv_obj_add_flag(obj_bottom_status, LV_OBJ_FLAG_HIDDEN);
//	   lv_obj_add_flag(obj_bottom_coordinate, LV_OBJ_FLAG_HIDDEN);
//		 lv_obj_add_flag(obj_bottom_trans, LV_OBJ_FLAG_HIDDEN);
//		 lv_obj_add_flag(obj_bottom_radio, LV_OBJ_FLAG_HIDDEN);
//		 lv_obj_add_flag(obj_bottom_mobile, LV_OBJ_FLAG_HIDDEN);	 
//		 lv_obj_add_flag(obj_bottom_record, LV_OBJ_FLAG_HIDDEN);
//		 lv_obj_add_flag(obj_bottom_wifi, LV_OBJ_FLAG_HIDDEN);
//		 lv_obj_add_flag(obj_bottom_version, LV_OBJ_FLAG_HIDDEN);
//	   lv_obj_clear_flag(label_poweroff, LV_OBJ_FLAG_HIDDEN);
//}
