#ifndef __SCREEN6_H__
#define __SCREEN6_H__

#include "main.h"

extern lv_obj_t * scr5 ;   ///main
extern lv_style_t style_screen5;

extern lv_obj_t   *label_record_big , *label_record_rsc_t , *label_record_name_t ,*label_record_type_t,*label_record_time_t  ;
extern lv_style_t  style_record_big ,  style_record_rsc_t ,  style_record_name_t , style_record_type_t ,style_record_time_t ;




extern unsigned char record_status;
extern float record_RSC;
extern char record_name[16];
extern char record_type;
extern char record_spacetime;


void lv_screen5_init(void);

void lv_screen5_display(void);
void lv_screen5_update();


void lv_singularxyz_record(lv_obj_t * obj,  unsigned char type, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_record();

void lv_singularxyz_record_rsc_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_record_rsc_text();

void lv_singularxyz_record_name_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_record_name_text();

void lv_singularxyz_record_type_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_record_type_text();

void lv_singularxyz_record_time_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_record_time_text();

#endif

