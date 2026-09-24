#ifndef __SCREEN3_H__
#define __SCREEN3_H__

#include "main.h"

extern lv_obj_t * scr3 ;   ///main
extern lv_style_t style_screen3;


extern lv_obj_t *label_coordinate_big , *label_coordinate_lon_t , *label_coordinate_lat_t , *label_coordinate_alt_t;
extern lv_style_t  style_coordinate_big , style_coordinate_lon_t , style_coordinate_lat_t , style_coordinate_alt_t;


extern unsigned char coordinate_status; //定位状态
extern unsigned char designation_ew;    //东西经标识
extern unsigned char designation_sn;    //南北纬标识
extern unsigned char reserved_set3;    
extern double coordinate_lon;            //经度
extern double coordinate_lat;            //纬度
extern double coordinate_alt;            //高程





void lv_screen3_init(void);
void lv_screen3_display(void);
void lv_screen3_update();

	
void lv_singularxyz_coordinate(lv_obj_t * obj, unsigned char type, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_coordinate();

void lv_singularxyz_coordinate_lon_text(lv_obj_t * obj, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_coordinate_lon_text();

void lv_singularxyz_coordinate_lat_text(lv_obj_t * obj, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_coordinate_lat_text();

void lv_singularxyz_coordinate_alt_text(lv_obj_t * obj,  lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_coordinate_alt_text();


#endif
