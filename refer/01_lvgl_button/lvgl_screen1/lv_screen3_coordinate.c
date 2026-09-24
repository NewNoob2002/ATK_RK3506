#include "main.h"

lv_obj_t * scr3 = NULL;   ///main
lv_style_t style_screen3;


lv_obj_t *label_coordinate_big , *label_coordinate_lon_t , *label_coordinate_lat_t , *label_coordinate_alt_t;
lv_style_t  style_coordinate_big , style_coordinate_lon_t , style_coordinate_lat_t , style_coordinate_alt_t;


unsigned char coordinate_status; //定位状态
unsigned char designation_ew;    //东西经标识
unsigned char designation_sn;    //南北纬标识
unsigned char reserved_set3;    
double coordinate_lon;            //经度
double coordinate_lat;            //纬度
double coordinate_alt;            //高程


void lv_screen3_init(void)
{
	scr3 = lv_obj_create(NULL);
	lv_style_init( &style_screen3 );
	lv_style_set_bg_opa( &style_screen3, LV_OPA_COVER );
	lv_style_set_bg_color( &style_screen3, lv_color_hex(0x0000) );
	lv_obj_add_style(scr3, &style_screen3, 0);	

	
	lv_singularxyz_coordinate(scr3, 2, 0+screen_destition, 30);        lv_update_coordinate(0);
	lv_singularxyz_coordinate_lon_text(scr3, 90+screen_destition, 25);    lv_update_coordinate_lon_text(0);//西经
	lv_singularxyz_coordinate_lat_text(scr3, 90+screen_destition, 50);   lv_update_coordinate_lat_text(0);//北纬
	lv_singularxyz_coordinate_alt_text(scr3, 90+screen_destition, 75);   lv_update_coordinate_alt_text(0);//高程
}


void lv_screen3_display(void)
{
	 lv_obj_add_flag(scr1, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr2, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_clear_flag(scr3, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr4, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr5, LV_OBJ_FLAG_HIDDEN);	 
	 lv_obj_add_flag(scr6, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr7, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr8, LV_OBJ_FLAG_HIDDEN);
	 lv_scr_load(scr3);
}



void lv_screen3_update()
{
	if(screen_flag_change|Recv_message)
	{
		  lv_update_coordinate();
      lv_update_coordinate_lon_text();
      lv_update_coordinate_lat_text();
      lv_update_coordinate_alt_text();
		
		  lv_screen3_display();
		  screen_flag_change=0;
		  Recv_message=0;
	}
}




void lv_singularxyz_coordinate(lv_obj_t * obj, unsigned char type, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
		label_coordinate_big = lv_label_create(obj);	
		lv_label_set_text(label_coordinate_big, LV_SYMBOL_COORDINATE);	
	  lv_style_init( &style_coordinate_big );
	  lv_obj_align(label_coordinate_big, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	  if (0 == type)
		  lv_style_set_text_font(&style_coordinate_big, &lv_font_singularxyz_28);
		else if (1 == type)
			lv_style_set_text_font(&style_coordinate_big, &lv_font_singularxyz_32); 
    else		
      lv_style_set_text_font(&style_coordinate_big, &lv_font_singularxyz_53);
		
	  lv_obj_add_style(label_coordinate_big, &style_coordinate_big, 0);
}






void lv_update_coordinate()
{
	  if(coordinate_status == 0 )  //none
			lv_style_set_text_color(&style_coordinate_big, lv_palette_main(LV_PALETTE_RED));
	  else if (coordinate_status == 1 )	///single	
			lv_style_set_text_color(&style_coordinate_big, lv_palette_main(LV_PALETTE_YELLOW));
		else if (coordinate_status == 4 ) ///fix
			lv_style_set_text_color(&style_coordinate_big, lv_palette_main(LV_PALETTE_GREEN));
		else if (coordinate_status == 5 ) ///float
			lv_style_set_text_color(&style_coordinate_big, lv_palette_main(LV_PALETTE_YELLOW));
		else if (coordinate_status == 7 ) ///FIXEDPOS
			lv_style_set_text_color(&style_coordinate_big, lv_palette_main(LV_PALETTE_BLUE));
		
}






void lv_singularxyz_coordinate_lon_text(lv_obj_t * obj, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &style_coordinate_lon_t);
	  label_coordinate_lon_t = lv_label_create(obj);	
	  lv_style_set_text_color(&style_coordinate_lon_t, lv_palette_main(LV_PALETTE_GREY));
    lv_obj_align(label_coordinate_lon_t, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
		lv_style_set_text_font(&style_coordinate_lon_t, &lv_font_montserrat_20);  
	  lv_obj_add_style(label_coordinate_lon_t, &style_coordinate_lon_t, 0);
}






void lv_update_coordinate_lon_text()
{
//	if(designation_ew==0)
//	  lv_label_set_text_fmt(label_coordinate_lon_t, " E:%0.6f", coordinate_lon);
//	else
//		lv_label_set_text_fmt(label_coordinate_lon_t, "W:%0.6f", coordinate_lon);
	lv_label_set_text_fmt(label_coordinate_lon_t, "B:%0.6f", coordinate_lon);
}






void lv_singularxyz_coordinate_lat_text(lv_obj_t * obj, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &style_coordinate_lat_t );
	  label_coordinate_lat_t = lv_label_create(obj);	
    lv_obj_align(label_coordinate_lat_t, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
		lv_style_set_text_font(&style_coordinate_lat_t, &lv_font_montserrat_20);  
	  lv_style_set_text_color(&style_coordinate_lat_t, lv_palette_main(LV_PALETTE_GREY));
	  lv_obj_add_style(label_coordinate_lat_t, &style_coordinate_lat_t, 0);
}






void lv_update_coordinate_lat_text()
{
//	if(designation_sn==0)
//	  lv_label_set_text_fmt(label_coordinate_lat_t, "S:%0.6f", coordinate_lat);		
//	else
//		lv_label_set_text_fmt(label_coordinate_lat_t, "N:%0.6f", coordinate_lat);	
	lv_label_set_text_fmt(label_coordinate_lat_t, "L:%0.6f", coordinate_lat);	
}





void lv_singularxyz_coordinate_alt_text(lv_obj_t * obj,  lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &style_coordinate_alt_t );
	  label_coordinate_alt_t = lv_label_create(obj);	
    lv_obj_align(label_coordinate_alt_t, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
		lv_style_set_text_font(&style_coordinate_alt_t, &lv_font_montserrat_20);  
	  lv_style_set_text_color(&style_coordinate_alt_t, lv_palette_main(LV_PALETTE_GREY));
	  lv_obj_add_style(label_coordinate_alt_t, &style_coordinate_alt_t, 0);
}






void lv_update_coordinate_alt_text()
{
	lv_label_set_text_fmt(label_coordinate_alt_t, "H:%0.6f", coordinate_alt);	
}

