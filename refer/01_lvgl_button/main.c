#include "main.h"


int screen_flag=1;
int screen_flag_change=1;
int Recv_message = 0;
int workstatus_change=0;


#define SPI_DEV "/dev/spidev0.0"


#define KEY_PIN   43   // P1-B3 = GPIO1_B3
int key_status , key_status_last;





static uint32_t cnt = 0;
int main(void) 
{
    gpio_init(KEY_PIN, GPIO_MODE_INPUT);
    SPI_init();
    LCD_init();


    lv_init();
    lv_port_disp_init(294, 126, 0);
    
    // LVGL_screen_init();


    lv_obj_t * spinner = lv_spinner_create(lv_scr_act(), 1000, 60);
    lv_obj_set_size(spinner, 100, 100);
    lv_obj_center(spinner);
 

    // lv_obj_t * btn = lv_btn_create(lv_scr_act());     /*Add a button the current screen*/
    // lv_obj_set_pos(btn, 0, 0);                            /*Set its position*/
    // lv_obj_set_size(btn, 50, 20);                          /*Set its size*/
    // lv_obj_center(btn);


    //  lv_obj_t * label2 = lv_label_create(lv_scr_act());
    // lv_obj_set_width(label2, 150);
    // lv_label_set_text(label2, "SingularXYZ");
    // lv_obj_align(label2, LV_ALIGN_CENTER, 0, 40);


     showpic_a();

    // LCD_All_Color_16();
    // while(1)usleep(1000*1000*3);;

    usleep(1000*1000*3);

    while (true) 
    {
        // key_status =  key_get(KEY_PIN);
        // if(key_status != key_status_last && key_status == 0 )
        // {
        //     printf("key trigger\r\n");
        //     screnn2_time=0;
        //     screen_flag_change=1;
        //     screen_flag++;
        //     if(screen_flag>=6)    screen_flag=1;   
        //     printf("screen_flag :%d\r\n",screen_flag);
        // }
        // key_status_last = key_status;


        // switch(screen_flag)
        // {
        //     case 1:
        //                 lv_screen1_update();
        //         break;
        //     case 2:
        //                 lv_screen2_update();
        //         break;
        //     case 3:
        //                 lv_screen3_update();
        //         break;
        //     case 4:
        //                 lv_screen5_update();
        //         break;
        //     case 5:
        //                 lv_screen6_update();
        //         break;
        //     default:
        //         break;
        // }
       
        if (cnt++ % 10*5 == 0) {
            //printf("tick: %lu\n", lv_tick_get());
            cnt=0;
        }
        
        usleep(1000*3);
        lv_task_handler();

    }
     
    return 0;
}
