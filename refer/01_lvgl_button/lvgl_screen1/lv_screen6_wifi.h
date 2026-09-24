#ifndef __screen6_H__
#define __screen6_H__

#include "main.h"

extern lv_obj_t * scr6 ;   ///main
extern lv_style_t style_screen6;

extern lv_obj_t   *label_router_big , *label_router_mode_t , *label_router_ssid_t , *label_router_ip_t;
extern lv_style_t  style_router_big ,  style_router_mode_t ,  style_router_ssid_t ,  style_router_ip_t;



extern unsigned char wifi_status;
extern unsigned char wifi_mode;
extern unsigned char wifi_IP[4];
extern unsigned char wifi_SSID[16];




void lv_screen6_init(void);

void lv_screen6_display(void);
void lv_screen6_update();


void lv_singularxyz_router(lv_obj_t * obj,  unsigned char type, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_router(unsigned char state);

void lv_singularxyz_router_mode_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_router_mode_text(unsigned char mode);

void lv_singularxyz_router_ssid_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_router_ssid_text(char *ssid);

void lv_singularxyz_router_ip_text(lv_obj_t * obj, unsigned char state, lv_coord_t x_ofs, lv_coord_t y_ofs);
void lv_update_router_ip_text(char *ip);

#endif
