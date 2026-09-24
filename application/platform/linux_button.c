#include "linux_button.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <unistd.h>

int linux_button_open(const char* gpiochip) {
    int chip = open(gpiochip, O_RDONLY | O_CLOEXEC);
    if (chip < 0)
        return -1;
    struct gpiohandle_request request = {.lineoffsets = {12},
                                         .flags = GPIOHANDLE_REQUEST_INPUT | GPIOHANDLE_REQUEST_BIAS_PULL_UP,
                                         .consumer_label = "p4-button",
                                         .lines = 1};
    int rc = ioctl(chip, GPIO_GET_LINEHANDLE_IOCTL, &request);
    int error = errno;
    close(chip);
    errno = error;
    return rc < 0 ? -1 : request.fd;
}

int linux_button_pressed(int line_fd) {
    struct gpiohandle_data values = {0};
    if (ioctl(line_fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &values) < 0)
        return -1;
    return values.values[0] == 0;
}
