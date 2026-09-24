#ifndef __LVGL_SCREEN_H__
#define __LVGL_SCREEN_H__

#include "main.h"


void LVGL_screen_init();
extern int screen_destition;








#if !defined LV_SYMBOL_MOBILE
#define LV_SYMBOL_MOBILE           "\xEE\x98\x80" /*, 0xE600*/
#endif
#if !defined LV_SYMBOL_RECORD
#define LV_SYMBOL_RECORD           "\xEE\x98\x84" /*, 0xE604*/
#endif
#if !defined LV_SYMBOL_ROUTER
#define LV_SYMBOL_ROUTER           "\xEE\x98\x97" /*, 0xE617*/
#endif
#if !defined LV_SYMBOL_TRANS
#define LV_SYMBOL_TRANS           "\xEE\x99\x80" /*, 0xE640*/
#endif
#if !defined LV_SYMBOL_COORDINATE
#define LV_SYMBOL_COORDINATE           "\xEE\x9A\xA2" /*, 0xE6A2*/
#endif
#if !defined LV_SYMBOL_INTERCOM
#define LV_SYMBOL_INTERCOM           "\xEE\x99\xBE" /*, 0xE67E*/
#endif

#if !defined LV_SYMBOL_SATELLITE
#define LV_SYMBOL_SATELLITE           "\xEE\x98\x86" /*, 0xE606*/
#endif
#if !defined LV_SYMBOL_ROVER
#define LV_SYMBOL_ROVER          "\xEE\x99\xB5" /*, 0xE675*/
#endif
#if !defined LV_SYMBOL_VERSION
#define LV_SYMBOL_VERSION          "\xEE\x9A\x95" /*, 0xE695*/
#endif
#if !defined LV_SYMBOL_POSITION
#define LV_SYMBOL_POSITION          "\xEE\x98\x8B" /*, 0xE60B*/
#endif
#if !defined LV_SYMBOL_BASE
#define LV_SYMBOL_BASE          "\xEE\x9C\x97" /*, 0xE717*/
#endif
#if !defined LV_SYMBOL_POWER_FULL
#define LV_SYMBOL_POWER_FULL          "\xEE\x9A\xBD" /*, 0xE6BD*/
#endif
#if !defined LV_SYMBOL_BRIDGING
#define LV_SYMBOL_BRIDGING          "\xEE\x9D\x8C" /*, 0xE74C*/
#endif
#if !defined LV_SYMBOL_RADIO
#define LV_SYMBOL_RADIO          "\xEE\x9A\x88" /*, 0xE688*/
#endif
#if !defined LV_SYMBOL_IMU
#define LV_SYMBOL_IMU          "\xEE\x98\xB2" /*, 0xE632*/
#endif
#if !defined LV_SYMBOL_LTE
#define LV_SYMBOL_LTE          "\xEE\x98\xBF" /*58943, 0xE63F*/
#endif



#if !defined LV_SYMBOL_workstatus1
#define LV_SYMBOL_workstatus1          "\xEE\x98\x80" 
#endif
#if !defined LV_SYMBOL_workstatus2
#define LV_SYMBOL_workstatus2          "\xEE\x9A\xB4" 
#endif
#if !defined LV_SYMBOL_workstatus3
#define LV_SYMBOL_workstatus3          "\xEE\x9D\x8C" 
#endif
#if !defined LV_SYMBOL_workstatus4
#define LV_SYMBOL_workstatus4          "\xEE\x9B\x87" 
#endif
#if !defined LV_SYMBOL_workstatus5
#define LV_SYMBOL_workstatus5          "\xEE\x98\xBB" 
#endif







#endif
