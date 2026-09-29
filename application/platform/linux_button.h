#pragma once

/** 打开 Linux evdev 按键设备，例如 /dev/input/event0。 */
int linux_button_open(const char* event_device);
/** 读取一个按键动作：1=下一焦点，2=上一焦点，3=确认按下，4=返回，5=确认松开；0=暂无动作，-1=失败。 */
int linux_button_read(int event_fd);
