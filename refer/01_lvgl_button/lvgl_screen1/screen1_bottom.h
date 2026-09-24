#ifndef __SCREEN1_BOTTOM_H__
#define __SCREEN1_BOTTOM_H__

#include "main.h"


extern lv_obj_t *screen1_bottom ;    
extern lv_style_t screen1_bottom_style;

extern lv_obj_t  *label_satellite_big , *label_sat_num , *label_sat_num_real , *label_wm_big , *label_wm_t , *label_position_big , *label_position_t;
extern lv_style_t  style_satellite_big , style_sat_num , style_sat_num_real , style_wm_big , style_wm_t , style_position_big , style_position_t;


/**********************/
extern unsigned char satellite_num;  //卫星数据
extern unsigned char satellite_num_real;
extern unsigned char Work_mode;   //工作模式
extern unsigned char location_state;  //定位状态
extern unsigned char reserved_set2;  //




void lv_screen1_bottom_init(void);

void lv_singularxyz_satellite(lv_obj_t * obj , lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_satellite(unsigned char type);

void lv_singularxyz_satellite_num(lv_obj_t * obj,  unsigned char type,  lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_satellite_num(unsigned char value);

void lv_singularxyz_satellite_num_real(lv_obj_t * obj,  unsigned char type,  lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_satellite_num_real(unsigned char value);

void lv_singularxyz_workmode(lv_obj_t * obj,unsigned char type, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_workmode(unsigned char mode);

void lv_singularxyz_workmode_text(lv_obj_t * obj, unsigned char type, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_workmode_text(unsigned char mode);

void lv_singularxyz_position(lv_obj_t * obj, unsigned char type, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_position(unsigned char state);

void lv_singularxyz_position_text(lv_obj_t * obj, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_position_text(unsigned char state);


//void lv_singularxyz_update_bottom_main(unsigned char value , unsigned char mode, unsigned char state );
void lv_singularxyz_update_bottom_main();


#endif
