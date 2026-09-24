#define _POSIX_C_SOURCE 200809L

#include "linux_button.h"
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

/* 单独测按键电平，避免屏幕同步 SPI flush 阻塞采样。由外部 timeout 限时。 */
int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s GPIO1_CHIP\n", argv[0]);
        return 2;
    }
    int fd = linux_button_open(argv[1]);
    if (fd < 0) {
        perror("button open");
        return 1;
    }
    int last = -1;
    for (;;) {
        int pressed = linux_button_pressed(fd);
        if (pressed < 0) {
            perror("button read");
            close(fd);
            return 1;
        }
        if (pressed != last) {
            puts(pressed ? "LOW pressed" : "HIGH released");
            fflush(stdout);
            last = pressed;
        }
        const struct timespec interval = {.tv_sec = 0, .tv_nsec = 20000000};
        if (nanosleep(&interval, NULL) < 0 && errno != EINTR) {
            perror("nanosleep");
            close(fd);
            return 1;
        }
    }
}
