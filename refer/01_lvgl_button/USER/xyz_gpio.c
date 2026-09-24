#include "xyz_gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define GPIO_PATH       "/sys/class/gpio"
#define GPIO_BUF_LEN    64

/* 私有：写字符串到文件 */
static void gpio_write_file(const char *path, const char *val)
{
    int fd = open(path, O_WRONLY);
    if (fd < 0) return;
    write(fd, val, strlen(val));
    close(fd);
}

/* 私有：导出 GPIO */
static void gpio_export(int pin)
{
    char path[GPIO_BUF_LEN];
    snprintf(path, sizeof(path), "%s/export", GPIO_PATH);

    int fd = open(path, O_WRONLY);
    if (fd < 0) return;
    dprintf(fd, "%d", pin);
    close(fd);

    usleep(100000);  // 等待 udev 创建 gpioX 目录
}

/* 私有：取消导出 */
static void gpio_unexport(int pin)
{
    char path[GPIO_BUF_LEN];
    snprintf(path, sizeof(path), "%s/unexport", GPIO_PATH);
    gpio_write_file(path, "43");
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief(概述)：           GPIO初始化
//  @param(参数1)：          pin           选择的引脚（sysfs 编号）
//  @param(参数2)：          gpio_mode     端口模式
//  @return(返回值)：        void
//  Sample usage(实例)：     gpio_init(43,GPIO_MODE_OUTPUT);//P1-B3 初始化为输出
//-------------------------------------------------------------------------------------------------------------------
void gpio_init(int pin, gpio_mode_t gpio_mode)
{
    char path[GPIO_BUF_LEN];

    gpio_export(pin);

    snprintf(path, sizeof(path), "%s/gpio%d/direction", GPIO_PATH, pin);

    switch (gpio_mode) {
    case GPIO_MODE_INPUT:
        gpio_write_file(path, "in");
        /* sysfs 不支持 pull-up/down 配置，依赖硬件/设备树 */
        break;

    case GPIO_MODE_OUTPUT:
    case GPIO_MODE_INPUT_OUTPUT:
    default:
        gpio_write_file(path, "out");
        break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief(概述)：            GPIO输出设置
//  @param(参数1)：           pin         选择的引脚
//  @param(参数2)：           dat         0：低电平 1：高电平
//  @return(返回值)：         void
//  Sample usage(实例)：      gpio_set(43 , 1);//P1-B3 输出高电平
//-------------------------------------------------------------------------------------------------------------------
void gpio_set(int pin, int dat)
{
    char path[GPIO_BUF_LEN];
    snprintf(path, sizeof(path), "%s/gpio%d/value", GPIO_PATH, pin);

    gpio_write_file(path, dat ? "1" : "0");
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief(概述)：           GPIO输入读取
//  @param(参数1)：          pin         选择的引脚
//  @return(返回值)：        int         0：低电平 1：高电平
//  Sample usage(实例)：     int status = gpio_get(43);//获取 P1-B3 电平
//-------------------------------------------------------------------------------------------------------------------
int gpio_get(int pin)
{
    char path[GPIO_BUF_LEN];
    char val;

    snprintf(path, sizeof(path), "%s/gpio%d/value", GPIO_PATH, pin);
    int fd = open(path, O_RDONLY);
    if (fd < 0) return 0;

    read(fd, &val, 1);
    close(fd);

    return (val == '1') ? 1 : 0;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief(概述)：           GPIO作为按键(key)
//  @param(参数1)：          pin          选择的引脚
//  @return(返回值)：        int          0：按下 1：松开
//  Sample usage(实例)：     int status = key_get(43);
//-------------------------------------------------------------------------------------------------------------------
int key_get(int pin)
{
    if (gpio_get(pin) == 0) {
        usleep(30000);  // 30ms 消抖
        if (gpio_get(pin) == 0)
            return 0;
    } else {
        usleep(30000);
        if (gpio_get(pin) == 1)
            return 1;
    }
    return 1;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief(概述)：         GPIO电平状态翻转
//  @param(参数1)：        pin          选择的引脚
//  @return(返回值)：      void
//  Sample usage(实例)：   gpio_toggle(43);//P1-B3 电平翻转
//-------------------------------------------------------------------------------------------------------------------
void gpio_toggle(int pin)
{
    gpio_set(pin, !gpio_get(pin));
}