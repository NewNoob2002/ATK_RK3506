add_executable(display_protocol_test display_protocol_test.c)
target_link_libraries(display_protocol_test PRIVATE lv_display)
target_compile_options(display_protocol_test PRIVATE -Wall -Wextra -Werror -UNDEBUG)
add_test(NAME display_protocol COMMAND display_protocol_test)

add_executable(linux_display_test linux_display_test.c)
target_link_libraries(linux_display_test PRIVATE rm690a0)
target_compile_options(linux_display_test PRIVATE -Wall -Wextra -Werror -UNDEBUG)
target_link_options(linux_display_test PRIVATE
    -Wl,--wrap=open,--wrap=open64,--wrap=close,--wrap=ioctl,--wrap=flock,--wrap=nanosleep
    -Wl,--wrap=__ioctl_time64,--wrap=__nanosleep64
)
add_test(NAME linux_display COMMAND linux_display_test)

add_executable(p4_framework_test p4_framework_test.cpp)
target_link_libraries(p4_framework_test PRIVATE p4_data_center p4_log)
target_compile_options(p4_framework_test PRIVATE -Wall -Wextra -Werror -UNDEBUG)
add_test(NAME p4_framework COMMAND p4_framework_test)

add_executable(p4_navigation_preview p4_navigation_preview.cpp)
target_link_libraries(p4_navigation_preview PRIVATE p4_app)
target_compile_options(p4_navigation_preview PRIVATE -Wall -Wextra -Werror -UNDEBUG)
add_test(NAME p4_navigation_preview COMMAND p4_navigation_preview)

add_executable(p4_status_test p4_status_test.cpp)
target_link_libraries(p4_status_test PRIVATE p4_app)
target_compile_options(p4_status_test PRIVATE -Wall -Wextra -Werror -UNDEBUG)
add_test(NAME p4_status COMMAND p4_status_test)

add_executable(p4_system_dash_test p4_system_dash_test.cpp)
target_link_libraries(p4_system_dash_test PRIVATE p4_app)
target_compile_options(p4_system_dash_test PRIVATE -Wall -Wextra -Werror -UNDEBUG)
add_test(NAME p4_system_dash COMMAND p4_system_dash_test "${CMAKE_CURRENT_BINARY_DIR}/system-dash-pages")

add_executable(p4_dialplate_preview p4_dialplate_preview.cpp)
target_link_libraries(p4_dialplate_preview PRIVATE p4_dialplate)
target_compile_options(p4_dialplate_preview PRIVATE -Wall -Wextra -Werror -UNDEBUG)
add_test(NAME p4_dialplate_preview COMMAND p4_dialplate_preview)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_executable(linux_button_test linux_button_test.c "${APPLICATION_DIR}/platform/linux_button.c")
    target_compile_options(linux_button_test PRIVATE -Wall -Wextra -Werror -UNDEBUG)
    add_test(NAME linux_button COMMAND linux_button_test)
endif()
