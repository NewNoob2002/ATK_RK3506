#ifndef __XYZ_GPIO_H__
#define __XYZ_GPIO_H__

#include <stdint.h>

/* GPIO 模式定义（兼容 ESP32 语义） */
typedef enum {
    GPIO_MODE_INPUT  = 0,
    GPIO_MODE_OUTPUT = 1,
    GPIO_MODE_INPUT_OUTPUT = 2,   // RK3506 sysfs 不支持真正双向，仅作兼容
} gpio_mode_t;

/* 引脚类型：直接使用 sysfs GPIO 编号 */
typedef int gpio_num_t;

/* API */
void gpio_init(int pin, gpio_mode_t gpio_mode);
void gpio_set(int pin, int dat);
int  gpio_get(int pin);
int  key_get(int pin);
void gpio_toggle(int pin);

#endif /* __XYZ_GPIO_H__ */