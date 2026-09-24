
//#include "main.h"

//lv_obj_t * scr7 = NULL;   ///main
//lv_style_t style_screen7;

//lv_obj_t   *label_version_big , *label_version_mcu_t ,*label_version_main_t;
//lv_style_t  style_version_big ,  style_version_mcu_t , style_version_main_t;


//void lv_screen7_init(void)
//{
//	scr7 = lv_obj_create(NULL);
//	lv_style_init( &style_screen7 );
//	lv_style_set_bg_opa( &style_screen7, LV_OPA_COVER );
//	lv_style_set_bg_color( &style_screen7, lv_color_hex(0x0000) );
//	lv_obj_add_style(scr7, &style_screen7, 0);	

//	lv_singularxyz_version(scr7, 2, 20, 50);             lv_update_version(1);
//	lv_singularxyz_version_mcu_text(scr7, 2, 130, 45);   lv_update_version_mcu_text(HW_VERSION);
//	lv_singularxyz_version_main_text(scr7, 2, 130, 80);  lv_update_version_main_text(SW_VERSION);
//	
//}



//void lv_screen7_display(void)
//{
//	 lv_obj_add_flag(scr1, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr2, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr3, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr4, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr5, LV_OBJ_FLAG_HIDDEN);	 
//	 lv_obj_add_flag(scr6, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_clear_flag(scr7, LV_OBJ_FLAG_HIDDEN);
//   lv_obj_add_flag(scr8, LV_OBJ_FLAG_HIDDEN);
//	 lv_scr_load(scr7);
//}



//void lv_screen7_update()
//{
//	if(screen_flag_change|Recv_message)
//	{
//		
//		
//		  lv_screen7_display();
//		  screen_flag_change=0;
//		  Recv_message=0;
//	}
//}



//void lv_singularxyz_version(lv_obj_t * obj, unsigned char type, lv_coord_t x_ofs, lv_coord_t y_ofs)
//{
//	  lv_style_init( &style_version_big );
//	   
//	  label_version_big = lv_label_create(obj);	
//		lv_label_set_text(label_version_big, LV_SYMBOL_VERSION);	
//	
//    lv_obj_align(label_version_big, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
//		
//	  if (0 == type)
//		  lv_style_set_text_font(&style_version_big, &lv_font_singularxyz_28);
//		else if (1 == type)
//			lv_style_set_text_font(&style_version_big, &lv_font_singularxyz_32); 
//    else		
//      lv_style_set_text_font(&style_version_big, &lv_font_singularxyz_53);
//		
//	  lv_obj_add_style(label_version_big, &style_version_big, 0);
//}





//void lv_update_version(unsigned char state)
//{
//	  if (1 == state)		
//			lv_style_set_text_color(&style_version_big, lv_palette_main(LV_PALETTE_YELLOW));
//		else if (2 == state) 
//			lv_style_set_text_color(&style_version_big, lv_palette_main(LV_PALETTE_GREEN));
//		else
//			lv_style_set_text_color(&style_version_big, lv_palette_main(LV_PALETTE_RED));
//		
//}





//void lv_singularxyz_version_mcu_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs)
//{
//	  lv_style_init( &style_version_mcu_t );
//	
//	  label_version_mcu_t = lv_label_create(obj);	
//	  
//    lv_obj_align(label_version_mcu_t, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
//	
//		lv_style_set_text_font(&style_version_mcu_t, &lv_font_montserrat_20);  
//		
//		if (1 == state)
//			lv_style_set_text_color(&style_version_mcu_t, lv_palette_main(LV_PALETTE_GREEN));
//		else
//			lv_style_set_text_color(&style_version_mcu_t, lv_palette_main(LV_PALETTE_GREY));
//				  
//	  lv_obj_add_style(label_version_mcu_t, &style_version_mcu_t, 0);
//}






//void lv_update_version_mcu_text(char *version)
//{
//	 lv_label_set_text_fmt(label_version_mcu_t, "DVer: %s", version);
//}





//void lv_singularxyz_version_main_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs)
//{
//	  lv_style_init( &style_version_main_t );
//	
//	  label_version_main_t = lv_label_create(obj);	
//	  
//		
//    lv_obj_align(label_version_main_t, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
//	
//		lv_style_set_text_font(&style_version_main_t, &lv_font_montserrat_20);  
//		
//		if (1 == state)
//			lv_style_set_text_color(&style_version_main_t, lv_palette_main(LV_PALETTE_GREEN));
//		else
//			lv_style_set_text_color(&style_version_main_t, lv_palette_main(LV_PALETTE_GREY));
//				  
//	  lv_obj_add_style(label_version_main_t, &style_version_main_t, 0);
//}





//void lv_update_version_main_text(char *version)
//{
//	lv_label_set_text_fmt(label_version_main_t, "MVer: %s", version);
//}

