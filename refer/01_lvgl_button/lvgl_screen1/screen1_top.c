#include "main.h"

lv_obj_t * screen1_top = NULL;            //¶¥²¿×´Ì¬À¸
lv_style_t screen1_style_top;

/**********************¶¥²¿×´Ì¬À¸×é¼þ*****************************/
lv_obj_t *screen1_battery ,*screen1_wifi ,*screen1_bt ,*screen1_vol , *screen1_gps ,*screen1_sd ,*screen1_radio ,*screen1_4g; //¶¥²¿×´Ì¬À¸±êÇ©£¬´ÓÓÒÏò×ó
lv_style_t screen1_style_battery ,screen1_style_wifi ,screen1_style_bt ,screen1_style_vol ,screen1_style_gps ,screen1_style_sd ,screen1_style_radio , screen1_style_4g; //¶¥²¿×´Ì¬À¸±êÇ©ÑùÊ½£¬´ÓÓÒÏò×ó




/**********************/
unsigned char gprs_status;      //4G×´Ì¬

//unsigned char gnss_status;      //GNSS×´Ì¬





/*****************************************************************/
/************  ¶¥²¿×´Ì¬À¸¶¨Òå   ************/
/*****************************************************************/
void lv_screen1_top_init(void)
{
	  screen1_top = lv_label_create(scr1);  //lv_label_create
    lv_style_init( &screen1_style_top );
    lv_label_set_text(screen1_top, "");
	
    lv_style_set_bg_color( &screen1_style_top, lv_color_hex(0x0000) );
    lv_obj_add_style(screen1_top, &screen1_style_top, 0);	
		lv_obj_set_pos(screen1_top, 0, 0);
  	lv_obj_align(screen1_top,  LV_ALIGN_TOP_MID,  0,  0);
		lv_obj_set_width(screen1_top, lv_pct(100));
    lv_obj_set_height(screen1_top, 35);

	  lv_singularxyz_4g(screen1_top, 10+screen_destition, 10);            lv_update_4g();
	  lv_singularxyz_radio(screen1_top, 58+screen_destition, 10);        lv_update_radio();
	  lv_singularxyz_sd(screen1_top, 106+screen_destition, 10);           lv_update_sd();
	  lv_singularxyz_gps(screen1_top,154+screen_destition, 10);           lv_update_gps();
		lv_singularxyz_wifi(screen1_top,202+screen_destition, 10);          lv_update_wifi();
		lv_singularxyz_battery(screen1_top, 255+screen_destition , 10);      lv_update_battery(0);
		
}




void lv_singularxyz_4g(lv_obj_t * obj, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
		screen1_4g = lv_label_create(obj);              
    lv_label_set_text(screen1_4g, LV_SYMBOL_LTE);//LV_SYMBOL_LTE);
	  lv_style_init(&screen1_style_4g);
    lv_obj_align(screen1_4g, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	  lv_obj_add_style(screen1_4g, &screen1_style_4g, 0);
}




void lv_update_4g()
{
	unsigned char type=0;
	
	 if (gprs_status == 1)
			lv_style_set_text_color(&screen1_style_4g, lv_palette_main(LV_PALETTE_LIGHT_BLUE));
		else
			lv_style_set_text_color(&screen1_style_4g, lv_palette_main(LV_PALETTE_RED));
		
		if (0 == type)
			lv_style_set_text_font(&screen1_style_4g, &lv_font_singularxyz_22); 
	  else
			lv_style_set_text_font(&screen1_style_4g, &lv_font_singularxyz_53); 
}





void lv_singularxyz_radio(lv_obj_t * obj,  lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  screen1_radio = lv_label_create(obj);              
    lv_label_set_text(screen1_radio, LV_SYMBOL_RADIO);
	  lv_style_init( &screen1_style_radio );
    lv_obj_align(screen1_radio, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	  lv_obj_add_style(screen1_radio, &screen1_style_radio, 0);
}





void lv_update_radio()
{
	unsigned char type=0;
	if (radio_status == 1)
			lv_style_set_text_color(&screen1_style_radio, lv_palette_main(LV_PALETTE_ORANGE));
		else
			lv_style_set_text_color(&screen1_style_radio, lv_palette_main(LV_PALETTE_RED));
		
		if (0 == type)
      lv_style_set_text_font(&screen1_style_radio, &lv_font_singularxyz_22); 
	  else
			lv_style_set_text_font(&screen1_style_radio, &lv_font_singularxyz_48);  
}





void lv_singularxyz_sd(lv_obj_t * obj, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
		screen1_sd = lv_label_create(obj);              
    lv_label_set_text(screen1_sd, LV_SYMBOL_SD_CARD);
	  lv_style_init( &screen1_style_sd );
    lv_obj_align(screen1_sd, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	  lv_obj_add_style(screen1_sd, &screen1_style_sd, 0);
}	






void lv_update_sd()
{
	unsigned char type=0;
	  if (record_status == 1)
			lv_style_set_text_color(&screen1_style_sd, lv_palette_main(LV_PALETTE_PURPLE));
		else
			lv_style_set_text_color(&screen1_style_sd, lv_palette_main(LV_PALETTE_RED));
		
		if (0 == type)
		{;}
	  else
			lv_style_set_text_font(&screen1_style_sd, &lv_font_montserrat_48); 
}






void lv_singularxyz_gps(lv_obj_t * obj, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
		screen1_gps = lv_label_create(obj);              
    lv_label_set_text(screen1_gps, LV_SYMBOL_GPS);
	  lv_style_init( &screen1_style_gps );
    lv_obj_align(screen1_gps, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	  lv_obj_add_style(screen1_gps, &screen1_style_gps, 0);
}	





void lv_update_gps()
{
	unsigned char type=0;
	 if (coordinate_status == 1)
			lv_style_set_text_color(&screen1_style_gps, lv_palette_main(LV_PALETTE_YELLOW));
		else
			lv_style_set_text_color(&screen1_style_gps, lv_palette_main(LV_PALETTE_RED));
		
		if (0 == type)
		{;}
	  else
			lv_style_set_text_font(&screen1_style_gps, &lv_font_montserrat_48);
}




void lv_singularxyz_wifi(lv_obj_t * obj, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  screen1_wifi = lv_label_create(obj);   	
    lv_label_set_text(screen1_wifi, LV_SYMBOL_WIFI);
	  lv_style_init( &screen1_style_wifi );
	
    lv_obj_align(screen1_wifi, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	  lv_obj_add_style(screen1_wifi, &screen1_style_wifi, 0);
}






void lv_update_wifi()
{
	unsigned char type=0;
	if (wifi_status== 1)
	   lv_style_set_text_color(&screen1_style_wifi, lv_palette_main(LV_PALETTE_TEAL));
	else
			lv_style_set_text_color(&screen1_style_wifi, lv_palette_main(LV_PALETTE_RED));
		
		if (0 == type)
		{;}
	  else
		lv_style_set_text_font(&screen1_style_wifi, &lv_font_montserrat_48);  
}






void lv_singularxyz_battery(lv_obj_t * obj,lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  screen1_battery = lv_label_create(obj);
    lv_label_set_recolor(screen1_battery, true);	
	  lv_style_init( &screen1_style_battery);
	  lv_obj_align(screen1_battery, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	  lv_obj_add_style(screen1_battery, &screen1_style_battery, 0);
	
}





void lv_update_battery(unsigned int value)
{
	  if (value == 100)
		{
			lv_label_set_text(screen1_battery, LV_SYMBOL_BATTERY_FULL);
			lv_style_set_text_color(&screen1_style_battery, lv_palette_main(LV_PALETTE_GREEN));
		}
		else if ((value < 100) && (value >= 70))
		{
			lv_label_set_text(screen1_battery, LV_SYMBOL_BATTERY_3);
			lv_style_set_text_color(&screen1_style_battery, lv_palette_main(LV_PALETTE_GREEN));
		}
		else if ((value < 70) && (value >= 40))
		{
			lv_label_set_text(screen1_battery, LV_SYMBOL_BATTERY_2);
			lv_style_set_text_color(&screen1_style_battery, lv_palette_main(LV_PALETTE_YELLOW));
		}
		else if ((value < 40) && (value >= 10))
		{
			lv_label_set_text(screen1_battery, LV_SYMBOL_BATTERY_1);
			lv_style_set_text_color(&screen1_style_battery, lv_palette_main(LV_PALETTE_RED));
		}
		else
		{
			lv_label_set_text(screen1_battery, LV_SYMBOL_BATTERY_EMPTY);
			lv_style_set_text_color(&screen1_style_battery, lv_palette_main(LV_PALETTE_RED));
		}
}







void lv_singularxyz_update_top_status()
{
		 lv_update_4g();
		 lv_update_radio();
		 lv_update_sd();
		 lv_update_gps();
		 lv_update_wifi();
}
