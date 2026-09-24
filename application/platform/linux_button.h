#pragma once

/** GPIO1_B4 = bank 内 offset 12（ATK JP1-28）；低电平按下，申请内部上拉。 */
int linux_button_open(const char* gpiochip);
/** 返回 0/1，失败返回 -1 并设置 errno。 */
int linux_button_pressed(int line_fd);
