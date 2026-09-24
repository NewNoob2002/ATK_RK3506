#include "main.h"

lv_obj_t * scr2 = NULL;   ///main
lv_style_t style_screen2;


lv_obj_t   *screen2_ws , *label_radio_mode_t , *label_radio_protocol_t , *label_radio_channel_t;
lv_style_t  screen2_style_ws  ,  style_radio_mode_t ,  style_radio_protocol_t ,  style_radio_channel_t;


lv_obj_t  *screen2_ntripText, *screen2_ntrip_ipText, *screen2_ntrip_pointText ;
lv_style_t  screen2_style_ntripText,screen2_style_ntrip_ipText,screen2_style_ntrip_pointText;



unsigned char radio_status;
unsigned char radio_mode;
unsigned char radio_protocol;
unsigned char radio_channel;

long int screnn2_time=0;

void lv_screen2_init(void)
{
		scr2 = lv_obj_create(NULL);
		lv_style_init( &style_screen2 );
		lv_style_set_bg_opa( &style_screen2, LV_OPA_COVER );
		lv_style_set_bg_color( &style_screen2, lv_color_hex(0x0000) );
		lv_obj_add_style(scr2, &style_screen2, 0);	
    
		lv_singularxyz_workstatus_text(scr2 , 0+screen_destition, 25);           lv_update_workstatus_text();
	
	  lv_singularxyz_radio_mode_text(scr2, 0, 85+screen_destition, 22);       lv_update_radio_mode_text(0);
		lv_singularxyz_radio_protocol_text(scr2, 0, 85+screen_destition, 47);   lv_update_radio_protocol_text(0);
		lv_singularxyz_radio_chanel_text(scr2, 0, 85+screen_destition, 72);     lv_update_radio_chanel_text(0);
	  
	  lv_singularxyz_ntrip_text(scr2,1,85+screen_destition, 22);          lv_update_ntrip_text(0);
	  lv_singularxyz_ntrip_ip_text(scr2,1,85+screen_destition, 47);       lv_update_ntrip_ip_text(0);
	  lv_singularxyz_ntrip_point_text(scr2,1,85+screen_destition, 72);    lv_update_ntrip_point_text(0);
}


void lv_screen2_display(void)
{
	 lv_obj_add_flag(scr1, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_clear_flag(scr2, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr3, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr4, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr5, LV_OBJ_FLAG_HIDDEN);	 
	 lv_obj_add_flag(scr6, LV_OBJ_FLAG_HIDDEN);
//	 lv_obj_add_flag(scr7, LV_OBJ_FLAG_HIDDEN);
	 lv_obj_add_flag(scr8, LV_OBJ_FLAG_HIDDEN);
	 lv_scr_load(scr2);
}


void lv_screen2_update()
{
	if(screen_flag_change|Recv_message|workstatus_change|screnn2_time>=500)
	{
		  if(screen_flag_change==1||workstatus_change==0||screnn2_time>=500)  query_workstatus();
		 // if(Recv_message==1&&workstatus_change==0) query_workstatus();
		//  if(workstatus_change==0) query_workstatus();
		
     if(screnn2_time>=500)  { screnn2_time=0; workstatus_change=0;} 
		 lv_update_workstatus_text();
		 lv_screen2_display();
		 screen_flag_change=0;
		 Recv_message=0;
		// workstatus_change=0;
	}
  
}





int workstatus=0;   //1：RTK桥接  2：电台移动站   3：网络移动站    4：电台基站    5：网络基站
int work_status_enable=0;



void query_workstatus()
{
	if(Work_mode==0)//移动站
	{
		 if(radio_status==0)//电台关闭
		 {    
			  if(gprs_status==1&&NtripClient_status==1&&NtripServer_status==0) //网络移动站
				{
					workstatus=1;
					work_status_enable=1;
				}
				else   //无效情况，默认显示网络移动站
				{
					workstatus=1;
					work_status_enable=0;
				}
		 }
		 else if(radio_status==1)//电台开启
		 {
			  if(radio_mode==0)  //无效情况，默认显示网络移动站
				{
					workstatus=1;
					work_status_enable=0;
				}
				else if(radio_mode==1)  
				{
					 if(gprs_status==0&&NtripClient_status==0&&NtripServer_status==0) //电台移动站
					 {
					 	 workstatus=2;
						 work_status_enable=1;
					 }
					 else  //无效情况，默认显示网络移动站
					 {
						 workstatus=1;
					   work_status_enable=0;
					 }
				}
			  else if(radio_mode==2)  
				{
					 if(gprs_status==1&&NtripClient_status==1&&NtripServer_status==0)//RTK桥接
					 {
						  workstatus=5;
					    work_status_enable=1;
					 }
					 else //无效情况，默认显示网络移动站
					 {
						  workstatus=1;
					    work_status_enable=0;
					 }
				}
		 }
	}
	else if(Work_mode==1)//基准站
	{
		 if(radio_status==0)//电台关闭
		 {
			   if(gprs_status==1&&NtripClient_status==0&&NtripServer_status==1)//网络基站
				 {  
					  workstatus=3;
					  work_status_enable=1;
				 }
				 else //无效情况，默认显示网络基站
				 { 
					  workstatus=3;
					  work_status_enable=0;
				 }
		 }
		 else if(radio_status==1)//电台开启
		 {
			  if(radio_mode==0)
				{
					 if(gprs_status==0&&NtripClient_status==0&&NtripServer_status==0)//电台基站
					 {
						 workstatus=4;
					   work_status_enable=1;
					 }
				}
				else//无效情况，默认显示网络基站
				{
					 workstatus=3;
					 work_status_enable=0;
				}
		 }
	}
	else  if(Work_mode==2)  //单点定位
	{
		 workstatus=1;
		 work_status_enable=0;
	}
	else  if(Work_mode==3)  //自动基准站
	{
		 workstatus=1;
		 work_status_enable=0;
	}
}


void chang_workstatus()
{
	if(workstatus==1)  //网络移动站
	{
		Work_mode=0; radio_status=0;   gprs_status=1;
		NtripClient_status=1; NtripServer_status=0;
	}
	
	else if(workstatus==2) //电台移动站
	{
		Work_mode=0; radio_status=1; radio_mode=1; gprs_status=0;
		NtripClient_status=0; NtripServer_status=0;
	}
	
	else if(workstatus==3)  //网络基站
	{
		Work_mode=1; radio_status=0;   gprs_status=1;
		NtripClient_status=0; NtripServer_status=1;
	}
	
	else if(workstatus==4)  //电台基站
	{
		Work_mode=1; radio_status=1;  radio_mode=0; gprs_status=0;
		NtripClient_status=0; NtripServer_status=0;
	}
	
	else if(workstatus==5) //RTK桥接
	{
		Work_mode=0; radio_status=1; radio_mode=2; gprs_status=1;
		NtripClient_status=1; NtripServer_status=0;
	}
}





void lv_singularxyz_workstatus_text(lv_obj_t * obj, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &screen2_style_ws );
	  screen2_ws = lv_label_create(obj);	
	  lv_obj_align(screen2_ws, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	  lv_style_set_text_color(&screen2_style_ws, lv_palette_main(LV_PALETTE_BLUE));
		lv_style_set_text_font(&screen2_style_ws, &lv_workstatus_icon);
	  lv_obj_add_style(screen2_ws, &screen2_style_ws, 0);
}



void lv_update_workstatus_text()
{
	//query_workstatus();
	if(workstatus==1) //网络移动站
	{
		  lv_style_set_text_color(&screen2_style_ws, lv_palette_main(LV_PALETTE_LIGHT_GREEN));
		  lv_label_set_text(screen2_ws, LV_SYMBOL_workstatus1);//网络移动站
		  update_screen2_radio(0);
			update_screen2_ntrip(1);
	}
	
	else if(workstatus==2) //电台移动站
	{
		  lv_style_set_text_color(&screen2_style_ws, lv_palette_main(LV_PALETTE_LIGHT_GREEN));
		  lv_label_set_text(screen2_ws, LV_SYMBOL_workstatus2);	//电台移动站
			update_screen2_radio(1);
		  update_screen2_ntrip(0);
	}
	
	else if(workstatus==3) //网络基站
	{
		  lv_style_set_text_color(&screen2_style_ws, lv_palette_main(LV_PALETTE_BLUE));
		  lv_label_set_text(screen2_ws, LV_SYMBOL_workstatus4);	//网络基站
			update_screen2_radio(0);
			update_screen2_ntrip(1);
	}
	
	else if(workstatus==4) //电台基站
	{
		  lv_style_set_text_color(&screen2_style_ws, lv_palette_main(LV_PALETTE_BLUE));
		  lv_label_set_text(screen2_ws, LV_SYMBOL_workstatus5);	//电台基站
			update_screen2_radio(1);
		  update_screen2_ntrip(0);
	}
	
	
	else if(workstatus==5)  //RTK桥接
	{
		  lv_style_set_text_color(&screen2_style_ws, lv_palette_main(LV_PALETTE_BLUE));
		  lv_label_set_text(screen2_ws, LV_SYMBOL_workstatus3);	//RTK桥接
			update_screen2_radio(1);
		  update_screen2_ntrip(0);
	}
	
}








void update_screen2_radio(int show_flag)
{
	lv_update_radio_mode_text(show_flag);
	lv_update_radio_protocol_text(show_flag);
	lv_update_radio_chanel_text(show_flag);
}





void lv_singularxyz_radio_mode_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &style_radio_mode_t );
	
	  label_radio_mode_t = lv_label_create(obj);	
	
    lv_obj_align(label_radio_mode_t, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	
		lv_style_set_text_font(&style_radio_mode_t, &lv_font_montserrat_20);  
		
		if (1 == state)
			lv_style_set_text_color(&style_radio_mode_t, lv_palette_main(LV_PALETTE_GREEN));
		else
			lv_style_set_text_color(&style_radio_mode_t, lv_palette_main(LV_PALETTE_GREY));
				  
	  lv_obj_add_style(label_radio_mode_t, &style_radio_mode_t, 0);
}





void lv_update_radio_mode_text(int mode_flag)
{
	  if(work_status_enable) 
			lv_style_set_text_color(&style_radio_mode_t, lv_palette_main(LV_PALETTE_GREEN));
		else
			lv_style_set_text_color(&style_radio_mode_t, lv_palette_main(LV_PALETTE_GREY));
		
		
	  if(mode_flag)
		{
			 if(workstatus == 2)	 lv_label_set_text(label_radio_mode_t, "Mode: Receive");	//电台移动站
			 else if(workstatus == 4)		   lv_label_set_text(label_radio_mode_t, "Mode: Transmit");//电台基站
			 else if(workstatus == 5)	 lv_label_set_text(label_radio_mode_t, "Mode: Bridge");	//桥接
		}
	  else                   
			 lv_label_set_text(label_radio_mode_t, "");	
}





void lv_singularxyz_radio_protocol_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &style_radio_protocol_t );
	
	  label_radio_protocol_t = lv_label_create(obj);	
	
    lv_obj_align(label_radio_protocol_t, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	
		lv_style_set_text_font(&style_radio_protocol_t, &lv_font_montserrat_20);  
		
		if (1 == state)
			lv_style_set_text_color(&style_radio_protocol_t, lv_palette_main(LV_PALETTE_GREEN));
		else
			lv_style_set_text_color(&style_radio_protocol_t, lv_palette_main(LV_PALETTE_GREY));
				  
	  lv_obj_add_style(label_radio_protocol_t, &style_radio_protocol_t, 0);
}





void lv_update_radio_protocol_text(int protocol_flag)
{
	if(work_status_enable)
			lv_style_set_text_color(&style_radio_protocol_t, lv_palette_main(LV_PALETTE_GREEN));
		else
			lv_style_set_text_color(&style_radio_protocol_t, lv_palette_main(LV_PALETTE_GREY));
		
	if(protocol_flag)
	{
			if(radio_protocol==1)       lv_label_set_text_fmt(label_radio_protocol_t, "Protocol:TRIMTALK");
			else if(radio_protocol==2)  lv_label_set_text_fmt(label_radio_protocol_t, "Protocol:TRIMMK3");
			else if(radio_protocol==4)  lv_label_set_text_fmt(label_radio_protocol_t, "Protocol:TT450S");
			else if(radio_protocol==5)  lv_label_set_text_fmt(label_radio_protocol_t, "Protocol:TRANSEOT");
			else if(radio_protocol==9)  lv_label_set_text_fmt(label_radio_protocol_t, "Protocol:SOUTH");
			else if(radio_protocol==10)  lv_label_set_text_fmt(label_radio_protocol_t, "Protocol:HUACE");
			else if(radio_protocol==13)  lv_label_set_text_fmt(label_radio_protocol_t, "Protocol:SATEL");
			else if(radio_protocol==16)  lv_label_set_text_fmt(label_radio_protocol_t, "Protocol:CCS");
		  else                          lv_label_set_text_fmt(label_radio_protocol_t, "Protocol:");
	}
	else                          
		  lv_label_set_text_fmt(label_radio_protocol_t, "");
	
}





void lv_singularxyz_radio_chanel_text(lv_obj_t * obj , unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &style_radio_channel_t );
	
	  label_radio_channel_t = lv_label_create(obj);	
	
    lv_obj_align(label_radio_channel_t, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	
		lv_style_set_text_font(&style_radio_channel_t, &lv_font_montserrat_20);  
		
		if (1 == state)
			lv_style_set_text_color(&style_radio_channel_t, lv_palette_main(LV_PALETTE_GREEN));
		else
			lv_style_set_text_color(&style_radio_channel_t, lv_palette_main(LV_PALETTE_GREY));
				  
	  lv_obj_add_style(label_radio_channel_t, &style_radio_channel_t, 0);
}





void lv_update_radio_chanel_text(int chanel_flag)
{
	 if(work_status_enable)
			lv_style_set_text_color(&style_radio_channel_t, lv_palette_main(LV_PALETTE_GREEN));
	 else
			lv_style_set_text_color(&style_radio_channel_t, lv_palette_main(LV_PALETTE_GREY));
			
		
	 if(chanel_flag)    
		 lv_label_set_text_fmt(label_radio_channel_t, "Channel: %d", radio_channel);
	 else             
		 lv_label_set_text_fmt(label_radio_channel_t, "");
}





























void update_screen2_ntrip(int show_flag)
{
	lv_update_ntrip_text(show_flag);
	lv_update_ntrip_ip_text(show_flag);
	lv_update_ntrip_point_text(show_flag);
}






void lv_singularxyz_ntrip_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &screen2_style_ntripText );
	
	  screen2_ntripText = lv_label_create(obj);	
    lv_obj_align(screen2_ntripText, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	
		lv_style_set_text_font(&screen2_style_ntripText, &lv_font_montserrat_20);  
		
		if (1 == state)
			lv_style_set_text_color(&screen2_style_ntripText, lv_palette_main(LV_PALETTE_GREEN));
		else
			lv_style_set_text_color(&screen2_style_ntripText, lv_palette_main(LV_PALETTE_GREY));
				  
	  lv_obj_add_style(screen2_ntripText, &screen2_style_ntripText, 0);
}





void lv_update_ntrip_text(int show_flag)
{
	if(show_flag)
	{
			if (work_status_enable)
				lv_style_set_text_color(&screen2_style_ntripText, lv_palette_main(LV_PALETTE_GREEN));
			else
				lv_style_set_text_color(&screen2_style_ntripText, lv_palette_main(LV_PALETTE_GREY));
			
			if(workstatus==1)   lv_label_set_text_fmt(screen2_ntripText, "Ntrip Client");
			else if(workstatus==3)   lv_label_set_text_fmt(screen2_ntripText, "Ntrip Server");
//			if(NtripClient_status)        lv_label_set_text_fmt(screen2_ntripText, "Ntrip Client");
//			else if(NtripServer_status)   lv_label_set_text_fmt(screen2_ntripText, "Ntrip Server");
//			else                          lv_label_set_text_fmt(screen2_ntripText, "NONE");
	}
	else
	{
		lv_label_set_text_fmt(screen2_ntripText, "");
	}
}



void lv_singularxyz_ntrip_ip_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &screen2_style_ntrip_ipText );
	
	  screen2_ntrip_ipText = lv_label_create(obj);	
    lv_obj_align(screen2_ntrip_ipText, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	
		lv_style_set_text_font(&screen2_style_ntrip_ipText, &lv_font_montserrat_20);  
		
		if (1 == state)
			lv_style_set_text_color(&screen2_style_ntrip_ipText, lv_palette_main(LV_PALETTE_GREEN));
		else
			lv_style_set_text_color(&screen2_style_ntrip_ipText, lv_palette_main(LV_PALETTE_GREY));
				  
	  lv_obj_add_style(screen2_ntrip_ipText, &screen2_style_ntrip_ipText, 0);
}





void lv_update_ntrip_ip_text(int show_flag)
{
	  if(show_flag)
		{
			if (work_status_enable)
			  lv_style_set_text_color(&screen2_style_ntrip_ipText, lv_palette_main(LV_PALETTE_GREEN));
		  else
			  lv_style_set_text_color(&screen2_style_ntrip_ipText, lv_palette_main(LV_PALETTE_GREY));
		
			
			if(workstatus==1) lv_label_set_text_fmt(screen2_ntrip_ipText, "IP: %d.%d.%d.%d", NtripClient_IP[0],NtripClient_IP[1],NtripClient_IP[2],NtripClient_IP[3]);
			else if(workstatus==3) lv_label_set_text_fmt(screen2_ntrip_ipText,  "IP: %d.%d.%d.%d", NtripServer_IP[0],NtripServer_IP[1],NtripServer_IP[2],NtripServer_IP[3]);
//			if(NtripClient_status)        lv_label_set_text_fmt(screen2_ntrip_ipText, "IP: %d.%d.%d.%d", NtripClient_IP[0],NtripClient_IP[1],NtripClient_IP[2],NtripClient_IP[3]);
//			else if(NtripServer_status)   lv_label_set_text_fmt(screen2_ntrip_ipText,  "IP: %d.%d.%d.%d", NtripServer_IP[0],NtripServer_IP[1],NtripServer_IP[2],NtripServer_IP[3]);
		}
		else
		{
			  lv_label_set_text_fmt(screen2_ntrip_ipText, "");
		}
}




void lv_singularxyz_ntrip_point_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	  lv_style_init( &screen2_style_ntrip_pointText );
	
	  screen2_ntrip_pointText = lv_label_create(obj);	
    lv_obj_align(screen2_ntrip_pointText, LV_ALIGN_TOP_LEFT, x_ofs, y_ofs);
	
		lv_style_set_text_font(&screen2_style_ntrip_pointText, &lv_font_montserrat_20);  
		
		if (1 == state)
			lv_style_set_text_color(&screen2_style_ntrip_pointText, lv_palette_main(LV_PALETTE_GREEN));
		else
			lv_style_set_text_color(&screen2_style_ntrip_pointText, lv_palette_main(LV_PALETTE_GREY));
				  
	  lv_obj_add_style(screen2_ntrip_pointText, &screen2_style_ntrip_pointText, 0);
}





void lv_update_ntrip_point_text(int show_flag)
{
	  if(show_flag)
		{
				if(work_status_enable)
					lv_style_set_text_color(&screen2_style_ntrip_pointText, lv_palette_main(LV_PALETTE_GREEN));
				else
					lv_style_set_text_color(&screen2_style_ntrip_pointText, lv_palette_main(LV_PALETTE_GREY));
				
				if(workstatus==1) lv_label_set_text_fmt(screen2_ntrip_pointText, "MP: %s",NtripClient_Mountpoint );
				else 	if(workstatus==3) lv_label_set_text_fmt(screen2_ntrip_pointText, "MP: %s",NtripServer_Mountpoint);
//				if(NtripClient_status)        lv_label_set_text_fmt(screen2_ntrip_pointText, "MP: %s",NtripClient_Mountpoint );
//				else if(NtripServer_status)   lv_label_set_text_fmt(screen2_ntrip_pointText, "MP: %s",NtripServer_Mountpoint);	
		}
		else
		{
			 lv_label_set_text_fmt(screen2_ntrip_pointText, "");
		}
	  
}


