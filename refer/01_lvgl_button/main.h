#ifndef __MAIN_H__
#define __MAIN_H__


typedef unsigned char u8; 
typedef unsigned short u16;       // 对应 16 位无符号类型  
typedef volatile unsigned int vu32; // 对应 32 位易变无符号类型  
typedef unsigned int u32;       // 对应 32位无符号类型  

#define USE_DRM 1
#define USE_EVDEV 1

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <malloc.h>
#include <math.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <pthread.h>
#include <semaphore.h>


#include <lvgl/lvgl.h>

#include "timestamp.h"

#include <lvgl/lv_conf.h>


#include "lv_port_disp.h"
#include "xyz_gpio.h"
#include "LCD.h"
#include "lv_font_xyz.h"
// #include "lvgl_screen.h"



// #include "lv_screen1_main.h"
// #include "lv_screen2_workstatus.h"
// #include "lv_screen3_coordinate.h"
// #include "lv_screen4_ntrip.h"
// #include "lv_screen5_record.h"
// #include "lv_screen6_wifi.h"
// #include "lv_screen7_version.h"
// #include "lv_screen8_poweroff.h"
// #include "lvgl_screen.h"
// #include "screen1_bottom.h"
// #include "screen1_top.h"

extern sem_t sem_tx;
extern pthread_mutex_t buf_lock;



extern lv_font_t lv_font_singularxyz_22;
extern lv_font_t lv_font_singularxyz_28;
extern lv_font_t lv_font_singularxyz_32;
extern lv_font_t lv_font_singularxyz_48;
extern lv_font_t lv_font_singularxyz_53;

// extern const lv_font_t lv_font_montserrat_28;
// extern const lv_font_t lv_font_montserrat_48;
extern const lv_font_t lv_font_montserrat_28;
extern const lv_font_t lv_font_montserrat_48;

extern lv_font_t lv_workstatus_icon;



#define ALIGN(x, a)     (((x) + (a - 1)) & ~(a - 1))

extern int screen_flag;
extern int screen_flag_change;
extern int Recv_message;
extern int workstatus_change;







#endif

