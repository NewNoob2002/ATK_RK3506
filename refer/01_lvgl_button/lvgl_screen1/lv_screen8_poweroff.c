#include "main.h"

lv_obj_t * scr8= NULL;   ///main
lv_style_t style_screen8;


void lv_screen8_init(void)
{
	scr8 = lv_obj_create(NULL);
	lv_style_init( &style_screen8 );
	lv_style_set_bg_opa( &style_screen8, LV_OPA_COVER );
	lv_style_set_bg_color( &style_screen8, lv_color_hex(0x0000) );
	lv_obj_add_style(scr8, &style_screen8, 0);	
  lv_sigularxyz_scr_poweroff();
}



void lv_screen8_display(void)
{
	 lv_obj_add_flag(scr1, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr2, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr3, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr4, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr5, LV_OBJ_FLAG_HIDDEN);	 
	 lv_obj_add_flag(scr6, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr7, LV_OBJ_FLAG_HIDDEN);
   lv_obj_clear_flag(scr8, LV_OBJ_FLAG_HIDDEN);
	 lv_scr_load(scr8);
}


lv_obj_t *label_poweroff = NULL; 
void lv_sigularxyz_scr_poweroff(void)
{
	  label_poweroff = lv_label_create(scr8);	
	  
    lv_label_set_text(label_poweroff, "Power off ....");
		
    lv_obj_align(label_poweroff, LV_ALIGN_LEFT_MID, 65+screen_destition, 10);
	
		static lv_style_t style_poweroff_t;
	  lv_style_init( &style_poweroff_t );
		lv_style_set_text_font(&style_poweroff_t, &lv_font_montserrat_28);  
		
		lv_style_set_text_color(&style_poweroff_t, lv_palette_main(LV_PALETTE_GREY));
				  
	  lv_obj_add_style(label_poweroff, &style_poweroff_t, 0);
}