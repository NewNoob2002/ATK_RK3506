#pragma once

/** Normalized Power/Function evdev transitions for the test board. */
enum linux_button_event {
    LINUX_BUTTON_NONE,
    LINUX_POWER_PRESS,
    LINUX_POWER_RELEASE,
    LINUX_FUNCTION_PRESS,
    LINUX_FUNCTION_RELEASE
};
int linux_button_open(const char* event_device);
/** Test-board mapping: V+ is Power, V- is Function; MENU/ESC and repeats are ignored.
 * Returns a linux_button_event, or -1 with errno on I/O errors.
 * Emits press/release transitions; the application interprets holds and single/double clicks.
 */
int linux_button_read(int event_fd);
/** Configurable evdev adapter with explicitly supplied distinct key codes.
 * Returns a linux_button_event, or -1 with errno on I/O errors.
 * Does not read GPIO HIGH/LOW levels or configure product pins/PMIC.
 */
int linux_button_read_keys(int event_fd, unsigned power_code, unsigned function_code);
