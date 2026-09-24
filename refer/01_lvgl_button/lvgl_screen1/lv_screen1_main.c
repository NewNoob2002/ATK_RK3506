#include "main.h"

lv_obj_t * scr1 = NULL;   ///main
lv_style_t style_screen1;


void lv_screen1_init(void)
{
	scr1 = lv_obj_create(NULL);
	lv_style_init( &style_screen1 );
	lv_style_set_bg_opa( &style_screen1, LV_OPA_COVER );
	lv_style_set_bg_color( &style_screen1, lv_color_hex(0x0000) );
	lv_obj_add_style(scr1, &style_screen1, 0);	

  
	lv_screen1_top_init();
	lv_screen1_bottom_init();
	
}


void lv_screen1_display(void)
{
	 lv_obj_clear_flag(scr1, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr2, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr3, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr4, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr5, LV_OBJ_FLAG_HIDDEN);	 
	 lv_obj_add_flag(scr6, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr7, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr8, LV_OBJ_FLAG_HIDDEN);
	 lv_scr_load(scr1);
}



void lv_screen1_update()
{
	if(screen_flag_change|Recv_message)
	{
		 lv_singularxyz_update_top_status();
		 lv_singularxyz_update_bottom_main();
		
		 screen_flag_change=0;
		 Recv_message=0;
	}
}
