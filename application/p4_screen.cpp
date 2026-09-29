#define _POSIX_C_SOURCE 200809L

#include "App/P4App.h"
#include "App/Status/DemoStatus.h"
extern "C" {
#include "display/lv_display.h"
#include "platform/linux_button.h"
#include "platform/linux_display.h"
}

#include <cerrno>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unistd.h>

static volatile sig_atomic_t stop;
static void on_signal(int) {
    stop = 1;
}

static bool number(const char* text, unsigned long max, unsigned long& value) {
    char* end;
    errno = 0;
    value = std::strtoul(text, &end, 10);
    return !errno && text[0] >= '0' && text[0] <= '9' && !*end && value && value <= max;
}

static std::uint64_t milliseconds() {
    timespec now{};
    if (clock_gettime(CLOCK_MONOTONIC, &now) < 0)
        return 0;
    return std::uint64_t(now.tv_sec) * 1000 + std::uint64_t(now.tv_nsec) / 1000000;
}

static P4App::InputAction input_action(int action) {
    switch (action) {
        case 1:
            return P4App::InputAction::NextFocus;
        case 2:
            return P4App::InputAction::PreviousFocus;
        case 3:
            return P4App::InputAction::Press;
        case 4:
            return P4App::InputAction::Back;
        case 5:
            return P4App::InputAction::Release;
        default:
            return P4App::InputAction::None;
    }
}

static const char* input_action_name(P4App::InputAction action) {
    switch (action) {
        case P4App::InputAction::NextFocus:
            return "next-focus";
        case P4App::InputAction::PreviousFocus:
            return "previous-focus";
        case P4App::InputAction::Confirm:
            return "confirm";
        case P4App::InputAction::Back:
            return "back";
        case P4App::InputAction::Press:
            return "press";
        case P4App::InputAction::Release:
            return "release";
        default:
            return "none";
    }
}

int main(int argc, char** argv) {
    const bool display_only = argc > 1 && std::strcmp(argv[1], "--display-only") == 0;
    const char* page = display_only && argc > 3 && std::strcmp(argv[2], "--page") == 0 ? argv[3] : nullptr;
    const int first = page ? 4 : display_only ? 2 : 1;
    const char* event_device = "/dev/input/event0";
    unsigned long speed, chunk = 4096;
    if (display_only) {
        if ((argc != first + 3 && argc != first + 4) || !number(argv[first + 2], UINT32_MAX, speed)
            || (argc == first + 4 && !number(argv[first + 3], 32768, chunk))) {
            std::fprintf(stderr,
                         "Usage: %s [--display-only [--page Pages/NAME]] SPI_DEVICE GPIO1_CHIP VERIFIED_SPEED_HZ "
                         "[EVEN_CHUNK_BYTES]\n",
                         argv[0]);
            return EXIT_FAILURE;
        }
    } else {
        if (argc < first + 3 || argc > first + 5 || !number(argv[first + 2], UINT32_MAX, speed)
            || (argc == first + 5 && !number(argv[first + 4], 32768, chunk))) {
            std::fprintf(stderr,
                         "Usage: %s SPI_DEVICE GPIO1_CHIP VERIFIED_SPEED_HZ [EVDEV_DEVICE [EVEN_CHUNK_BYTES]]\n",
                         argv[0]);
            return EXIT_FAILURE;
        }
        if (argc >= first + 4)
            event_device = argv[first + 3];
    }
    if (chunk < 4 || chunk % 2) {
        std::fprintf(stderr, "EVEN_CHUNK_BYTES must be an even value from 4 to 32768\n");
        return EXIT_FAILURE;
    }
    struct sigaction action{};
    action.sa_handler = on_signal;
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGINT, &action, nullptr) < 0 || sigaction(SIGTERM, &action, nullptr) < 0) {
        std::perror("sigaction");
        return EXIT_FAILURE;
    }
    // 先打开 evdev，避免屏初始化后才发现输入设备不可用。
    int button_fd = display_only ? -1 : linux_button_open(event_device);
    if (!display_only && button_fd < 0) {
        std::perror(event_device);
        return EXIT_FAILURE;
    }
    linux_display io;
    rm690a0 screen;
    if (linux_display_open(&io, &screen, argv[first], argv[first + 1], speed, chunk) < 0) {
        std::perror("display open");
        if (button_fd >= 0)
            close(button_fd);
        return EXIT_FAILURE;
    }
    int result = EXIT_FAILURE;
    if (rm_init(&screen) < 0) {
        std::perror("RM690A0 init");
    } else if (!stop) {
        lv_init();
        lv_display port{};
        if (lv_display_register(&port, &screen) < 0) {
            std::perror("LVGL display");
        } else {
            {
                P4App app;
                if (!app.Init()) {
                    std::fprintf(stderr, "P4 app init failed\n");
                } else {
                    if (page) {
                        lv_tick_inc(50);
                        lv_timer_handler();
                    }
                    if (page && !app.ShowPage(page)) {
                        std::fprintf(stderr, "Unknown or unavailable page: %s\n", page);
                    } else {
                        std::uint64_t last = milliseconds();
                        std::uint64_t next_status_update = last;
                        while (!stop && !port.error) {
                            std::uint64_t now = milliseconds();
                            if (!now || now < last) {
                                std::fprintf(stderr, "monotonic clock failed\n");
                                break;
                            }
                            lv_tick_inc(std::uint32_t(now - last));
                            last = now;
                            if (now >= next_status_update) {
                                app.UpdateStatus(DemoStatus::Sample(now));
                                next_status_update = now + 200;
                            }
                            if (button_fd >= 0) {
                                const int event_action = linux_button_read(button_fd);
                                if (event_action < 0) {
                                    std::perror("evdev read");
                                    break;
                                }
                                const auto action = input_action(event_action);
                                if (action != P4App::InputAction::None) {
                                    app.OnInput(action);
                                    std::fprintf(stderr, "evdev %s -> %s\n", input_action_name(action),
                                                 app.CurrentPage());
                                }
                            }
                            lv_timer_handler();
                            timespec wait{0, 5000000};
                            if (nanosleep(&wait, nullptr) < 0 && errno != EINTR) {
                                std::perror("nanosleep");
                                break;
                            }
                        }
                        if (stop && !port.error)
                            result = EXIT_SUCCESS;
                    }
                }
            }
            if (port.error) {
                errno = port.error;
                std::perror("display flush");
            }
            lv_disp_remove(port.display);
        }
    }
    if (linux_display_close(&io) < 0) {
        std::perror("display close");
        result = EXIT_FAILURE;
    }
    if (button_fd >= 0 && close(button_fd) < 0) {
        std::perror("button close");
        result = EXIT_FAILURE;
    }
    return result;
}
