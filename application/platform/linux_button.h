#pragma once

/** Optional configurable evdev test transitions; NOT the two-GPIO product interface. */
enum linux_button_event {
    LINUX_BUTTON_NONE,
    LINUX_POWER_PRESS,
    LINUX_POWER_RELEASE,
    LINUX_FUNCTION_PRESS,
    LINUX_FUNCTION_RELEASE
};
int linux_button_open(const char* event_device);
enum linux_test_button_action {
    LINUX_TEST_NONE,
    LINUX_TEST_NEXT,
    LINUX_TEST_PREVIOUS,
    LINUX_TEST_PRESS,
    LINUX_TEST_BACK,
    LINUX_TEST_RELEASE,
    LINUX_TEST_COMMIT
};
/** Current test-board controls: V+/V-/MENU/ESC. KEY_ENTER provides optional direct Commit.
 * Returns -1 with errno on I/O errors. Does not interpret single/double clicks.
 */
int linux_button_read(int event_fd);
/** Configurable evdev test helper with explicitly supplied distinct key codes; not bound by p4_screen.
 * Does not read GPIO HIGH/LOW levels or configure product pins/PMIC.
 */
int linux_button_read_keys(int event_fd, unsigned power_code, unsigned function_code);
