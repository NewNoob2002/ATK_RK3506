#ifndef __SCREEN2_H__
#define __SCREEN2_H__

#include "main.h"

extern lv_obj_t * scr2 ;   ///main
extern lv_style_t style_screen2;


extern lv_obj_t   *screen2_ws , *label_radio_mode_t , *label_radio_protocol_t , *label_radio_channel_t;
extern lv_style_t  screen2_style_ws  ,  style_radio_mode_t ,  style_radio_protocol_t ,  style_radio_channel_t;

extern lv_obj_t  *screen2_ntripText, *screen2_ntrip_ipText, *screen2_ntrip_pointText ;
extern lv_style_t  screen2_style_ntripText,screen2_style_ntrip_ipText,screen2_style_ntrip_pointText;


extern unsigned char radio_status;
extern unsigned char radio_mode;
extern unsigned char radio_protocol;
extern unsigned char radio_channel;

extern long int screnn2_time;

extern int workstatus;   //1：RTK桥接  2：电台移动站   3：网络移动站    4：电台基站    5：网络基站
extern int work_status_enable;

void lv_screen2_init(void);
void lv_screen2_display(void);
void lv_screen2_update();

void lv_singularxyz_workstatus_text(lv_obj_t * obj , lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_workstatus_text();


void lv_singularxyz_radio_mode_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_radio_mode_text(int mode_flag);
void lv_singularxyz_radio_protocol_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_radio_protocol_text(int protocol_flag);
void lv_singularxyz_radio_chanel_text(lv_obj_t * obj , unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_radio_chanel_text(int channel_flag);

void update_screen2_radio(int show_flag);



void update_screen2_ntrip(int show_flag);
void lv_singularxyz_ntrip_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_ntrip_text(int show_flag);
void lv_singularxyz_ntrip_ip_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_ntrip_ip_text(int show_flag);
void lv_singularxyz_ntrip_point_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_ntrip_point_text(int show_flag);



void query_workstatus();
void chang_workstatus();


#endif

