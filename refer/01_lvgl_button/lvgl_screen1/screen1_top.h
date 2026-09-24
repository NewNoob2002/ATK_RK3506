#ifndef __SCREEN1_TOP_H__
#define __SCREEN1_TOP_H__

#include "main.h"



extern lv_obj_t * screen1_top ;            //¶¥²¿×´Ì¬À¸
extern lv_style_t screen1_style_top;

/**********************¶¥²¿×´Ì¬À¸×é¼þ*****************************/
extern lv_obj_t *screen1_battery ,*screen1_wifi ,*screen1_bt ,*screen1_vol , *screen1_gps ,*screen1_sd ,*screen1_radio ,*screen1_4g; //¶¥²¿×´Ì¬À¸±êÇ©£¬´ÓÓÒÏò×ó
extern lv_style_t screen1_screen1_style_battery ,screen1_screen1_style_wifi ,screen1_screen1_style_bt ,screen1_screen1_style_vol ,screen1_screen1_style_gps ,screen1_style_sd ,screen1_style_radio , screen1_style_4g; //¶¥²¿×´Ì¬À¸±êÇ©ÑùÊ½£¬´ÓÓÒÏò×ó




/**********************/
extern unsigned char gprs_status;      //4G×´Ì¬
extern unsigned char sdcard_status;    //ÄÚ²¿´æ´¢×´Ì¬
//extern unsigned char gnss_status;      //GNSS×´Ì¬
extern unsigned char voice_status;     //ÓïÒôÄ£¿é×´Ì¬
extern unsigned char bluetooth_status; //À¶ÑÀ×´Ì¬





void lv_screen1_top_init(void);

void lv_singularxyz_4g(lv_obj_t * obj, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_4g();

void lv_singularxyz_radio(lv_obj_t * obj,  lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_radio();

void lv_singularxyz_sd(lv_obj_t * obj, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_sd();

void lv_singularxyz_gps(lv_obj_t * obj, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_gps();


void lv_singularxyz_wifi(lv_obj_t * obj, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_wifi();

void lv_singularxyz_battery(lv_obj_t * obj,lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_battery(unsigned int value);

void lv_singularxyz_update_top_status(void);











#endif
