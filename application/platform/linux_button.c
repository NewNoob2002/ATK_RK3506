#include "linux_button.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>

int linux_button_open(const char* event_device) {
    return open(event_device, O_RDONLY | O_NONBLOCK | O_CLOEXEC);
}
int linux_button_read_keys(int event_fd, unsigned power_code, unsigned function_code) {
    if (power_code > KEY_MAX || function_code > KEY_MAX || power_code == function_code) {
        errno = EINVAL;
        return -1;
    }
    struct input_event event;
    const ssize_t size = read(event_fd, &event, sizeof(event));
    if (size < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
            return LINUX_BUTTON_NONE;
        return -1;
    }
    if (size != (ssize_t)sizeof(event)) {
        errno = EIO;
        return -1;
    }
    if (event.type != EV_KEY || (event.value != 0 && event.value != 1))
        return LINUX_BUTTON_NONE;
    if (event.code == power_code)
        return event.value ? LINUX_POWER_PRESS : LINUX_POWER_RELEASE;
    if (event.code == function_code)
        return event.value ? LINUX_FUNCTION_PRESS : LINUX_FUNCTION_RELEASE;
    return LINUX_BUTTON_NONE;
}
int linux_button_read(int event_fd) {
    return linux_button_read_keys(event_fd, KEY_VOLUMEUP, KEY_VOLUMEDOWN);
}
