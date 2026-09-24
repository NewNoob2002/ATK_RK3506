//#include "main.h"

//lv_obj_t * scr4 = NULL;   ///main
//lv_style_t style_screen4;


//lv_obj_t *screen4_ntripIcon ,*screen4_ntripText, *screen4_ntrip_ipText, *screen4_ntrip_pointText ;
//lv_style_t  screen4_style_ntripIcon, screen4_style_ntripText,screen4_style_ntrip_ipText,screen4_style_ntrip_pointText;


unsigned char NtripClient_status=1;
unsigned char NtripClient_IP[4]={192,168,1,1};
unsigned char NtripClient_Mountpoint[32]="OFFICE";

unsigned char NtripServer_status;
unsigned char NtripServer_IP[4];
unsigned char NtripServer_Mountpoint[32];



//void lv_screen4_init(void)
//{
//	scr4 = lv_obj_create(NULL);

//	lv_style_init( &style_screen4);
//	lv_style_set_bg_opa( &style_screen4, LV_OPA_COVER );
//	lv_style_set_bg_color( &style_screen4, lv_color_hex(0x0000) );
//	lv_obj_add_style(scr4, &style_screen4, 0);	

//	
//	lv_singularxyz_trans(scr4, 2, 20, 30);                    lv_update_trans(0);
//	lv_singularxyz_trans_ntrip_text(scr4,1,105, 25);          lv_update_trans_ntrip_text();
//	lv_singularxyz_trans_ntrip_ip_text(scr4,1,105, 50);          lv_update_trans_ntrip_ip_text();
//	lv_singularxyz_trans_ntrip_point_text(scr4,1,105, 75);          lv_update_trans_ntrip_point_text();
//}



//void lv_screen4_display(void)
//{
//	 lv_obj_add_flag(scr1, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr2, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr3, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_clear_flag(scr4, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr5, LV_OBJ_FLAG_HIDDEN);	 
//	 lv_obj_add_flag(scr6, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr7, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr8, LV_OBJ_FLAG_HIDDEN);
//	 lv_scr_load(scr4);
//}



//void lv_screen4_update()
//{
//	if(screen_flag_change|Recv_message)
//	{
//		  lv_update_trans();
//			lv_update_trans_ntrip_text();
//			lv_update_trans_ntrip_ip_text();
//			lv_update_trans_ntrip_point_text();
//		  lv_screen4_display();
//		  screen_flag_change=0;
//		  Recv_message=0;
//	}
//}


//void lv_singularxyz_trans(lv_obj_t * obj,unsigned char type, lv_coord_t x_ofs, lv_coord_t y_ofs)
//{
//	  lv_style_init( &screen4_style_ntripIcon );
//	
//	  screen4_ntripIcon = lv_label_create(obj);	
//		lv_label_set_text(screen4_ntripIcon, LV_SYMBOL_TRANS);	
//	
//    lv_obj_align(screen4_ntripIcon, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
//		
//	  if (0 == type)
//		  lv_style_set_text_font(&screen4_style_ntripIcon, &lv_font_singularxyz_28);
//		else if (1 == type)
//			lv_style_set_text_font(&screen4_style_ntripIcon, &lv_font_singularxyz_32); 
//    else		
//      lv_style_set_text_font(&screen4_style_ntripIcon, &lv_font_singularxyz_53);
//		
//	  lv_obj_add_style(screen4_ntripIcon, &screen4_style_ntripIcon, 0);
//}






//void lv_update_trans()
//{
//	if (NtripClient_status|NtripServer_status) 
//			lv_style_set_text_color(&screen4_style_ntripIcon, lv_palette_main(LV_PALETTE_GREEN));
//	else
//		  lv_style_set_text_color(&screen4_style_ntripIcon, lv_palette_main(LV_PALETTE_YELLOW));
//}




//void lv_singularxyz_trans_ntrip_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs)
//{
//	  lv_style_init( &screen4_style_ntripText );
//	
//	  screen4_ntripText = lv_label_create(obj);	
//    lv_obj_align(screen4_ntripText, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
//	
//		lv_style_set_text_font(&screen4_style_ntripText, &lv_font_montserrat_20);  
//		
//		if (1 == state)
//			lv_style_set_text_color(&screen4_style_ntripText, lv_palette_main(LV_PALETTE_GREEN));
//		else
//			lv_style_set_text_color(&screen4_style_ntripText, lv_palette_main(LV_PALETTE_GREY));
//				  
//	  lv_obj_add_style(screen4_ntripText, &screen4_style_ntripText, 0);
//}





//void lv_update_trans_ntrip_text()
//{
//	if (true)
//			lv_style_set_text_color(&screen4_style_ntripText, lv_palette_main(LV_PALETTE_GREY));
//		else
//			lv_style_set_text_color(&screen4_style_ntripText, lv_palette_main(LV_PALETTE_GREEN));
//		
//	if(NtripClient_status)        lv_label_set_text_fmt(screen4_ntripText, "Ntrip Client");
//	else if(NtripServer_status)   lv_label_set_text_fmt(screen4_ntripText, "Ntrip Server");
//  else                          lv_label_set_text_fmt(screen4_ntripText, "NONE");
//}



//void lv_singularxyz_trans_ntrip_ip_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs)
//{
//	  lv_style_init( &screen4_style_ntrip_ipText );
//	
//	  screen4_ntrip_ipText = lv_label_create(obj);	
//    lv_obj_align(screen4_ntrip_ipText, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
//	
//		lv_style_set_text_font(&screen4_style_ntrip_ipText, &lv_font_montserrat_20);  
//		
//		if (1 == state)
//			lv_style_set_text_color(&screen4_style_ntrip_ipText, lv_palette_main(LV_PALETTE_GREEN));
//		else
//			lv_style_set_text_color(&screen4_style_ntrip_ipText, lv_palette_main(LV_PALETTE_GREY));
//				  
//	  lv_obj_add_style(screen4_ntrip_ipText, &screen4_style_ntrip_ipText, 0);
//}





//void lv_update_trans_ntrip_ip_text()
//{
//	if (true)
//			lv_style_set_text_color(&screen4_style_ntrip_ipText, lv_palette_main(LV_PALETTE_GREY));
//		else
//			lv_style_set_text_color(&screen4_style_ntrip_ipText, lv_palette_main(LV_PALETTE_GREEN));
//		
//	 	
//		if(NtripClient_status)        lv_label_set_text_fmt(screen4_ntrip_ipText, "IP: %d.%d.%d.%d", NtripClient_IP[0],NtripClient_IP[1],NtripClient_IP[2],NtripClient_IP[3]);
//		else if(NtripServer_status)   lv_label_set_text_fmt(screen4_ntrip_ipText,  "IP: %d.%d.%d.%d", NtripServer_IP[0],NtripServer_IP[1],NtripServer_IP[2],NtripServer_IP[3]);
//}




//void lv_singularxyz_trans_ntrip_point_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs)
//{
//	  lv_style_init( &screen4_style_ntrip_pointText );
//	
//	  screen4_ntrip_pointText = lv_label_create(obj);	
//    lv_obj_align(screen4_ntrip_pointText, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
//	
//		lv_style_set_text_font(&screen4_style_ntrip_pointText, &lv_font_montserrat_20);  
//		
//		if (1 == state)
//			lv_style_set_text_color(&screen4_style_ntrip_pointText, lv_palette_main(LV_PALETTE_GREEN));
//		else
//			lv_style_set_text_color(&screen4_style_ntrip_pointText, lv_palette_main(LV_PALETTE_GREY));
//				  
//	  lv_obj_add_style(screen4_ntrip_pointText, &screen4_style_ntrip_pointText, 0);
//}





//void lv_update_trans_ntrip_point_text()
//{
//	  if(true)
//			lv_style_set_text_color(&screen4_style_ntrip_pointText, lv_palette_main(LV_PALETTE_GREY));
//		else
//			lv_style_set_text_color(&screen4_style_ntrip_pointText, lv_palette_main(LV_PALETTE_GREEN));
//		
//	 	
//		if(NtripClient_status)        lv_label_set_text_fmt(screen4_ntrip_pointText, "MP: %s",NtripClient_Mountpoint );
//		else if(NtripServer_status)   lv_label_set_text_fmt(screen4_ntrip_pointText, "MP: %s",NtripServer_Mountpoint);
//}
