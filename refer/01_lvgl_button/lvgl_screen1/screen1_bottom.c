#include "main.h"


lv_obj_t *screen1_bottom = NULL;    
lv_style_t screen1_bottom_style;

lv_obj_t  *label_satellite_big , *label_sat_num , *label_sat_num_real , *label_wm_big , *label_wm_t , *label_position_big , *label_position_t;
lv_style_t  style_satellite_big , style_sat_num ,style_sat_num_real , style_wm_big , style_wm_t , style_position_big , style_position_t;


/**********************/
unsigned char satellite_num;  //卫星颗数
unsigned char satellite_num_real;  //卫星颗数
unsigned char Work_mode=0;   //工作模式




void lv_screen1_bottom_init(void)
{
	  screen1_bottom = lv_label_create(scr1);
    lv_style_init( &screen1_bottom_style );
		lv_label_set_text(screen1_bottom, "");
	
    lv_style_set_bg_color( &screen1_bottom_style, lv_color_hex(0x0000) );
    lv_obj_add_style(screen1_bottom, &screen1_bottom_style, 0);	
		lv_obj_set_pos(screen1_bottom, 0, 45); //x,y
	
  	lv_obj_align(screen1_bottom,  LV_ALIGN_TOP_MID,  0,  0);//注意
		lv_obj_set_width(screen1_bottom, lv_pct(100));
    lv_obj_set_height(screen1_bottom, 120);
		 
		lv_singularxyz_satellite(screen1_bottom, 5+screen_destition, 10);               lv_update_satellite(0);	
	
	  lv_singularxyz_satellite_num_real(screen1_bottom, 1, 130+screen_destition, 20);       lv_update_satellite_num_real(0);
	
		lv_singularxyz_satellite_num(screen1_bottom, 1, -165+screen_destition, 11);       lv_update_satellite_num(0);
	 
	  lv_singularxyz_workmode(screen1_bottom, 1, 175+screen_destition, 2);           lv_update_workmode(1); 
		lv_singularxyz_workmode_text(screen1_bottom, 1, 42+screen_destition, 30);	    lv_update_workmode_text(1);
		lv_singularxyz_position(screen1_bottom,  1, 238+screen_destition, 2);          lv_update_position(0);
    lv_singularxyz_position_text(screen1_bottom, 107+screen_destition, 30);		      lv_update_position_text(0);
}







void lv_singularxyz_satellite(lv_obj_t * obj , lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &style_satellite_big );
	
	  label_satellite_big = lv_label_create(obj);	
		lv_label_set_text(label_satellite_big, LV_SYMBOL_SATELLITE);
    lv_obj_align(label_satellite_big, LV_ALIGN_LEFT_MID, x_ofs, y_ofs);
	  lv_style_set_text_color(&style_satellite_big, lv_palette_main(LV_PALETTE_INDIGO));
	  
	  lv_obj_add_style(label_satellite_big, &style_satellite_big, 0);
}







void lv_update_satellite(unsigned char type)
{
	if (0 == type)
			lv_style_set_text_font(&style_satellite_big, &lv_font_singularxyz_48);//lv_font_singularxyz_32 
	  else	
			lv_style_set_text_font(&style_satellite_big, &lv_font_singularxyz_53);  
}








void lv_singularxyz_satellite_num(lv_obj_t * obj,  unsigned char type,  lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &style_sat_num );
	
	  label_sat_num = lv_label_create(obj);	
		
    lv_obj_align(label_sat_num, LV_ALIGN_RIGHT_MID, x_ofs, y_ofs);
		
	  if (0 == type)
			lv_style_set_text_font(&style_sat_num, &lv_font_montserrat_18);  
		else
			lv_style_set_text_font(&style_sat_num, &lv_font_montserrat_48);   
		
		
	  lv_style_set_text_color(&style_sat_num, lv_palette_main(LV_PALETTE_GREY));
	  lv_obj_add_style(label_sat_num, &style_sat_num, 0);
		
}




void lv_update_satellite_num(unsigned char value)
{
		lv_label_set_text_fmt(label_sat_num, "%d", value);	
}









void lv_singularxyz_satellite_num_real(lv_obj_t * obj,  unsigned char type,  lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &style_sat_num );
	
	  label_sat_num_real = lv_label_create(obj);	
		
    lv_obj_align(label_sat_num_real, LV_ALIGN_LEFT_MID, x_ofs, y_ofs);
		
		lv_style_set_text_font(&style_sat_num_real, &lv_font_montserrat_20);  
		
		
	  lv_style_set_text_color(&style_sat_num_real, lv_palette_main(LV_PALETTE_GREY));
	  lv_obj_add_style(label_sat_num_real, &style_sat_num_real, 0);
		
}







void lv_update_satellite_num_real(unsigned char value)
{
	  lv_label_set_text_fmt(label_sat_num_real, "/%d", value);	
}







void lv_singularxyz_workmode(lv_obj_t * obj,unsigned char type, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &style_wm_big );
		label_wm_big = lv_label_create(obj);	
	  
    lv_obj_align(label_wm_big, LV_ALIGN_LEFT_MID, x_ofs, y_ofs);
		
	  if (0 == type)
			lv_style_set_text_font(&style_wm_big, &lv_font_singularxyz_28); 
		else
		  lv_style_set_text_font(&style_wm_big, &lv_font_singularxyz_32);   
		
	  lv_style_set_text_color(&style_wm_big, lv_palette_main(LV_PALETTE_CYAN));
	  lv_obj_add_style(label_wm_big, &style_wm_big, 0);
}






void lv_update_workmode(unsigned char mode)
{
		if (0 == mode)
			lv_label_set_text(label_wm_big, LV_SYMBOL_ROVER);	
		else if (1 == mode)
			lv_label_set_text(label_wm_big, LV_SYMBOL_BASE);
		else if(2 == mode)
			lv_label_set_text(label_wm_big, LV_SYMBOL_ROVER);
		else if(3 == mode)
			lv_label_set_text(label_wm_big, LV_SYMBOL_BASE);
}






void lv_singularxyz_workmode_text(lv_obj_t * obj, unsigned char type, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &style_wm_t );
	  label_wm_t = lv_label_create(obj);	
	
    lv_obj_align(label_wm_t, LV_ALIGN_CENTER, x_ofs, y_ofs);
		
		lv_style_set_text_font(&style_wm_t, &lv_font_montserrat_18);  
	  lv_style_set_text_color(&style_wm_t, lv_palette_main(LV_PALETTE_GREY));
		
	  lv_obj_add_style(label_wm_t, &style_wm_t, 0);
}






void lv_update_workmode_text(unsigned char mode)
{
	if (0 == mode)	
	lv_label_set_text(label_wm_t, "Rover");	
  else if (1 == mode)
			lv_label_set_text(label_wm_t, "Base");
  else if(2 == mode)
			lv_label_set_text(label_wm_t, "Rover");
	else if(3 == mode)
			lv_label_set_text(label_wm_t, "Base");
}





void lv_singularxyz_position(lv_obj_t * obj, unsigned char type, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &style_position_big );
	
	  label_position_big = lv_label_create(obj);	
		lv_label_set_text(label_position_big, LV_SYMBOL_POSITION);	
//	lv_label_set_recolor(label_position_big, true);	
    lv_obj_align(label_position_big, LV_ALIGN_LEFT_MID, x_ofs, y_ofs);
		
	  if (0 == type)
		  lv_style_set_text_font(&style_position_big, &lv_font_singularxyz_28);
		else if (1 == type)
			lv_style_set_text_font(&style_position_big, &lv_font_singularxyz_32); 
    else		
      lv_style_set_text_font(&style_position_big, &lv_font_singularxyz_48);
		
		lv_obj_add_style(label_position_big, &style_position_big, 0);
}






void lv_update_position(unsigned char state)
{
	  if(0 == state)  //none
			lv_style_set_text_color(&style_position_big, lv_palette_main(LV_PALETTE_RED));
	  else if (1 == state)	///single	
			lv_style_set_text_color(&style_position_big, lv_palette_main(LV_PALETTE_YELLOW));
		else if (2 == state) ///fix
			lv_style_set_text_color(&style_position_big, lv_palette_main(LV_PALETTE_GREEN));
		else if (3 == state) ///float
			lv_style_set_text_color(&style_position_big, lv_palette_main(LV_PALETTE_YELLOW));
		else if (4 == state) ///FIXEDPOS
			lv_style_set_text_color(&style_position_big, lv_palette_main(LV_PALETTE_BLUE));
		
}






void lv_singularxyz_position_text(lv_obj_t * obj, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &style_position_t );
		label_position_t = lv_label_create(obj);	
    lv_obj_align(label_position_t, LV_ALIGN_CENTER, x_ofs, y_ofs);
	
		lv_style_set_text_font(&style_position_t, &lv_font_montserrat_18);   
	  lv_style_set_text_color(&style_position_t, lv_palette_main(LV_PALETTE_GREY));
	  lv_obj_add_style(label_position_t, &style_position_t, 0);
}






void lv_update_position_text(unsigned char state)
{
	  if (0 == state)		///None
			lv_label_set_text(label_position_t, "None");
		else if (1 == state)		///Single
			lv_label_set_text(label_position_t, "Single");		
		else if (2 == state)		///RTD
			lv_label_set_text(label_position_t, "RTD");	
		else if (4 == state) ///float
			lv_label_set_text(label_position_t, "Fix");
			else if (5 == state) ///fix
			lv_label_set_text(label_position_t, "Float");
		else if(7 == state) ///FIXEDPOS
			lv_label_set_text(label_position_t, "FixPos");
}






void lv_singularxyz_update_bottom_main()
{
		lv_update_satellite_num(satellite_num);//
	  lv_update_satellite_num_real(satellite_num_real);
		lv_update_workmode(Work_mode);
		lv_update_workmode_text(Work_mode);
		lv_update_position(coordinate_status);
		lv_update_position_text(coordinate_status);
}
