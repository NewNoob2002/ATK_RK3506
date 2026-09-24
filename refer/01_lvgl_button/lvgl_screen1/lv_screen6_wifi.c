#include "main.h"

lv_obj_t * scr6 = NULL;   ///main
lv_style_t style_screen6;

lv_obj_t   *label_router_big , *label_router_mode_t , *label_router_ssid_t , *label_router_ip_t;
lv_style_t  style_router_big ,  style_router_mode_t ,  style_router_ssid_t ,  style_router_ip_t;


unsigned char wifi_status;
unsigned char wifi_mode;
unsigned char wifi_IP[4];
unsigned char wifi_SSID[16];

char wifi_ip[4]={0,0,0,0};


void lv_screen6_init(void)
{
	scr6 = lv_obj_create(NULL);
	lv_style_init( &style_screen6 );
	lv_style_set_bg_opa( &style_screen6, LV_OPA_COVER );
	lv_style_set_bg_color( &style_screen6, lv_color_hex(0x0000) );
	lv_obj_add_style(scr6, &style_screen6, 0);	
	
	lv_singularxyz_router(scr6,  2, 15+screen_destition, 30);              lv_update_router(0);
	lv_singularxyz_router_mode_text(scr6, 2, 95+screen_destition, 25);    lv_update_router_mode_text(0);
	lv_singularxyz_router_ssid_text(scr6, 2, 95+screen_destition, 50);    lv_update_router_ssid_text(" ");
	lv_singularxyz_router_ip_text(scr6, 2, 95+screen_destition, 75);      lv_update_router_ip_text(wifi_ip);
}



void lv_screen6_display(void)
{
	 lv_obj_add_flag(scr1, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr2, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr3, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr4, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr5, LV_OBJ_FLAG_HIDDEN);	 
	 lv_obj_clear_flag(scr6, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr7, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr8, LV_OBJ_FLAG_HIDDEN);
	 lv_scr_load(scr6);
}


void lv_screen6_update()
{
	if(screen_flag_change|Recv_message)
	{
		lv_update_router(wifi_status);
    lv_update_router_mode_text(wifi_mode);
    lv_update_router_ssid_text(wifi_SSID);
    lv_update_router_ip_text(wifi_IP);
		
		lv_screen6_display();
		screen_flag_change=0;
		Recv_message=0;
	}
}



void lv_singularxyz_router(lv_obj_t * obj,  unsigned char type, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &style_router_big );
	
	  label_router_big = lv_label_create(obj);	
		lv_label_set_text(label_router_big, LV_SYMBOL_ROUTER);	
	
    lv_obj_align(label_router_big, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
		
	  if (0 == type)
		  lv_style_set_text_font(&style_router_big, &lv_font_singularxyz_28);
		else if (1 == type)
			lv_style_set_text_font(&style_router_big, &lv_font_singularxyz_32); 
    else		
      lv_style_set_text_font(&style_router_big, &lv_font_singularxyz_53);
		
	  lv_obj_add_style(label_router_big, &style_router_big, 0);
}





void lv_update_router(unsigned char state)
{
	 if (0 == state)		
			lv_style_set_text_color(&style_router_big, lv_palette_main(LV_PALETTE_YELLOW));
		else if (1 == state) 
			lv_style_set_text_color(&style_router_big, lv_palette_main(LV_PALETTE_GREEN));
}






void lv_singularxyz_router_mode_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &style_router_mode_t );
	
	  label_router_mode_t = lv_label_create(obj);	
	  
    lv_obj_align(label_router_mode_t, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	
		lv_style_set_text_font(&style_router_mode_t, &lv_font_montserrat_20);  
		
		if (1 == state)
			lv_style_set_text_color(&style_router_mode_t, lv_palette_main(LV_PALETTE_GREEN));
		else
			lv_style_set_text_color(&style_router_mode_t, lv_palette_main(LV_PALETTE_GREY));
				  
	  lv_obj_add_style(label_router_mode_t, &style_router_mode_t, 0);
}






void lv_update_router_mode_text(unsigned char mode)
{
	 if (0 == mode) 	
			lv_label_set_text(label_router_mode_t, "Mode: AP");	
		else
      lv_label_set_text(label_router_mode_t, "Mode: STA");
}






void lv_singularxyz_router_ssid_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &style_router_ssid_t );
	
	  label_router_ssid_t = lv_label_create(obj);	
	  
    lv_obj_align(label_router_ssid_t, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	
		lv_style_set_text_font(&style_router_ssid_t, &lv_font_montserrat_20);  
		
		if (1 == state)
			lv_style_set_text_color(&style_router_ssid_t, lv_palette_main(LV_PALETTE_GREEN));
		else
			lv_style_set_text_color(&style_router_ssid_t, lv_palette_main(LV_PALETTE_GREY));
				  
	  lv_obj_add_style(label_router_ssid_t, &style_router_ssid_t, 0);
}






void lv_update_router_ssid_text(char *ssid)
{
	  lv_label_set_text_fmt(label_router_ssid_t, "%s", ssid);//SSID: 
}





void lv_singularxyz_router_ip_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &style_router_ip_t );
	
	  label_router_ip_t = lv_label_create(obj);	
	  
    lv_obj_align(label_router_ip_t, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	
		
		lv_style_set_text_font(&style_router_ip_t, &lv_font_montserrat_20);  
		
		if (1 == state)
			lv_style_set_text_color(&style_router_ip_t, lv_palette_main(LV_PALETTE_GREEN));
		else
			lv_style_set_text_color(&style_router_ip_t, lv_palette_main(LV_PALETTE_GREY));
				  
	  lv_obj_add_style(label_router_ip_t, &style_router_ip_t, 0);
}






void lv_update_router_ip_text(char *ip)
{
	 lv_label_set_text_fmt(label_router_ip_t, "IP: %d.%d.%d.%d", ip[0],ip[1],ip[2],ip[3]);
}




