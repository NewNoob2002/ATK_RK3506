#include "linux_button.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>

int linux_button_open(const char* event_device) {
    return open(event_device, O_RDONLY | O_NONBLOCK | O_CLOEXEC);
}

int linux_button_read(int event_fd) {
    struct input_event event;
    const ssize_t size = read(event_fd, &event, sizeof(event));
    if (size < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return 0;
        return -1;
    }
    if (size != (ssize_t)sizeof(event)) {
        errno = EIO;
        return -1;
    }
    if (event.type != EV_KEY)
        return 0;
    if (event.code == KEY_MENU && event.value == 0)
        return 5; // 长按确认需要知道松开时刻
    if (event.value != 1)
        return 0;
    switch (event.code) {
        case KEY_VOLUMEUP:
            return 1;
        case KEY_VOLUMEDOWN:
            return 2;
        case KEY_MENU:
            return 3;
        case KEY_ESC:
            return 4;
        default:
            return 0;
    }
}
