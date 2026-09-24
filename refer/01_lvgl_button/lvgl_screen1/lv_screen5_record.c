#include "main.h"

lv_obj_t * scr5 = NULL;   ///main
lv_style_t style_screen5;

lv_obj_t   *label_record_big , *label_record_rsc_t , *label_record_name_t ,*label_record_type_t,*label_record_time_t  ;
lv_style_t  style_record_big ,  style_record_rsc_t ,  style_record_name_t , style_record_type_t ,style_record_time_t ;


unsigned char record_status;
float record_RSC;
char record_name[16]="NEW";
char record_type=1;
char record_spacetime=1;


void lv_screen5_init(void)
{
	scr5 = lv_obj_create(NULL);
  lv_style_init( &style_screen5 );
	lv_style_set_bg_opa( &style_screen5, LV_OPA_COVER );
	lv_style_set_bg_color( &style_screen5, lv_color_hex(0x0000) );
	lv_obj_add_style(scr5, &style_screen5, 0);	
  
	lv_singularxyz_record(scr5, 2, 0+screen_destition, 30);             lv_update_record();
	lv_singularxyz_record_rsc_text(scr5, 0, 90+screen_destition, 20);   lv_update_record_rsc_text();
	lv_singularxyz_record_name_text(scr5, 0, 90+screen_destition, 40);  lv_update_record_name_text();
	lv_singularxyz_record_type_text(scr5, 0, 90+screen_destition, 60);  lv_update_record_type_text();
	lv_singularxyz_record_time_text(scr5, 0, 90+screen_destition, 80);  lv_update_record_time_text();
}


void lv_screen5_display(void)
{
	 lv_obj_add_flag(scr1, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr2, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr3, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr4, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_clear_flag(scr5, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr6, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr7, LV_OBJ_FLAG_HIDDEN);
   lv_obj_add_flag(scr8, LV_OBJ_FLAG_HIDDEN);
	 lv_scr_load(scr5);
}



void lv_screen5_update()
{
	if(screen_flag_change|Recv_message)
	{
		  lv_update_record();
		  lv_update_record_rsc_text();
		  lv_update_record_name_text();
		  lv_update_record_type_text();
		  lv_update_record_time_text();
		
		  lv_screen5_display();
		  screen_flag_change=0;
		  Recv_message=0;
	}
}


void lv_singularxyz_record(lv_obj_t * obj,  unsigned char type, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
		label_record_big = lv_label_create(obj);	
		lv_label_set_text(label_record_big, LV_SYMBOL_RECORD);	
	
    lv_obj_align(label_record_big, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	  lv_style_init( &style_record_big );
	   
	  if (0 == type)
		  lv_style_set_text_font(&style_record_big, &lv_font_singularxyz_28);
		else if (1 == type)
			lv_style_set_text_font(&style_record_big, &lv_font_singularxyz_32); 
    else		
      lv_style_set_text_font(&style_record_big, &lv_font_singularxyz_53);
		
	  lv_obj_add_style(label_record_big, &style_record_big, 0);
}





void lv_update_record()
{
	  if(record_status == 0)		
			lv_style_set_text_color(&style_record_big, lv_palette_main(LV_PALETTE_YELLOW));
		else if (record_status == 1) 
			lv_style_set_text_color(&style_record_big, lv_palette_main(LV_PALETTE_GREEN));
}






void lv_singularxyz_record_rsc_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
		label_record_rsc_t = lv_label_create(obj);	
	   	
    lv_obj_align(label_record_rsc_t, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	
	  lv_style_init( &style_record_rsc_t );
		lv_style_set_text_font(&style_record_rsc_t, &lv_font_montserrat_20);  
		
		if (1 == state)
			lv_style_set_text_color(&style_record_rsc_t, lv_palette_main(LV_PALETTE_GREEN));
		else
			lv_style_set_text_color(&style_record_rsc_t, lv_palette_main(LV_PALETTE_GREY));
				  
	  lv_obj_add_style(label_record_rsc_t, &style_record_rsc_t, 0);
}



void lv_update_record_rsc_text()
{
		lv_label_set_text_fmt(label_record_rsc_t, "RSC: %0.1fMB", record_RSC);	
}





void lv_singularxyz_record_name_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
		label_record_name_t = lv_label_create(obj);	
	   	
    lv_obj_align(label_record_name_t, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	
	  lv_style_init( &style_record_name_t );
		lv_style_set_text_font(&style_record_name_t, &lv_font_montserrat_20);  
		
		if (1 == state)
			lv_style_set_text_color(&style_record_name_t, lv_palette_main(LV_PALETTE_GREEN));
		else
			lv_style_set_text_color(&style_record_name_t, lv_palette_main(LV_PALETTE_GREY));
				  
	  lv_obj_add_style(label_record_name_t, &style_record_name_t, 0);
}





void lv_update_record_name_text()
{
		lv_label_set_text_fmt(label_record_name_t, "REC: %s", record_name);	
}






void lv_singularxyz_record_type_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
		label_record_type_t = lv_label_create(obj);	
	   	
    lv_obj_align(label_record_type_t, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	
	  lv_style_init( &style_record_type_t );
		lv_style_set_text_font(&style_record_type_t, &lv_font_montserrat_20);  
		
		if (1 == state)
			lv_style_set_text_color(&style_record_type_t, lv_palette_main(LV_PALETTE_GREEN));
		else
			lv_style_set_text_color(&style_record_type_t, lv_palette_main(LV_PALETTE_GREY));
				  
	  lv_obj_add_style(label_record_type_t, &style_record_type_t, 0);
}





void lv_update_record_type_text()
{
	  if(record_type==1) lv_label_set_text_fmt(label_record_type_t, "Type: XYZ");	
	  else if(record_type==2) lv_label_set_text_fmt(label_record_type_t, "Type: Rinex 3.02");	
}




void lv_singularxyz_record_time_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
		label_record_time_t = lv_label_create(obj);	
	   	
    lv_obj_align(label_record_time_t, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	
	  lv_style_init( &style_record_time_t );
		lv_style_set_text_font(&style_record_time_t, &lv_font_montserrat_20);  
		
		if (1 == state)
			lv_style_set_text_color(&style_record_time_t, lv_palette_main(LV_PALETTE_GREEN));
		else
			lv_style_set_text_color(&style_record_time_t, lv_palette_main(LV_PALETTE_GREY));
				  
	  lv_obj_add_style(label_record_time_t, &style_record_time_t, 0);
}





void lv_update_record_time_text()
{
	if(record_spacetime==0x00) lv_label_set_text_fmt(label_record_time_t, "Time: 15min");	
	else if(record_spacetime==0x01) lv_label_set_text_fmt(label_record_time_t, "Time: 1 hour");	
	else if(record_spacetime==0x02) lv_label_set_text_fmt(label_record_time_t, "Time: 2 hour");	
	else if(record_spacetime==0x04) lv_label_set_text_fmt(label_record_time_t, "Time: 4 hour");	
	else if(record_spacetime==0x18) lv_label_set_text_fmt(label_record_time_t, "Time: 24 hour");	
}
