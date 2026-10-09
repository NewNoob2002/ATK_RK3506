cmake_minimum_required(VERSION 3.16)
project(rk3506_display LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(RK3506_DEBUG_MONITOR "Enable LVGL FPS/CPU and memory monitors" OFF)
option(RK3506_LVGL_DEMO "Build standalone LVGL benchmark with on-screen FPS/CPU monitor" OFF)
option(RK3506_SDL_PREVIEW "Build host SDL preview for P4 pages" OFF)

# Modular sub-components
include(cmake/lvgl.cmake)
add_subdirectory(display)

enable_testing()

if(RK3506_LVGL_DEMO)
    add_subdirectory(demo)
else()
    # Application layer
    add_subdirectory(App)

    # Standalone utilities and application entrypoints
    add_executable(lvgl_baseline baseline.c)
    target_link_libraries(lvgl_baseline PRIVATE lvgl8311)
    add_test(NAME lvgl_baseline COMMAND lvgl_baseline)

    add_executable(display_test display_test.c)
    target_link_libraries(display_test PRIVATE lv_display)
    target_compile_options(display_test PRIVATE -Wall -Wextra -Werror)

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        add_executable(p4_screen p4_screen.cpp platform/linux_button.c)
        target_link_libraries(p4_screen PRIVATE p4_app lv_display)
        target_compile_options(p4_screen PRIVATE -Wall -Wextra -Werror)

        add_executable(button_probe button_probe.c platform/linux_button.c)
        target_include_directories(button_probe PRIVATE platform)
        target_compile_definitions(button_probe PRIVATE _POSIX_C_SOURCE=200809L)
        target_compile_options(button_probe PRIVATE -Wall -Wextra -Werror)

        if(RK3506_SDL_PREVIEW)
            find_package(SDL2 REQUIRED CONFIG)
            add_executable(p4_sdl p4_sdl.cpp)
            target_link_libraries(p4_sdl PRIVATE p4_app SDL2::SDL2)
            target_compile_options(p4_sdl PRIVATE -Wall -Wextra -Werror)
        endif()
    endif()

    # Test suite
    set(APPLICATION_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../tests" "${CMAKE_CURRENT_BINARY_DIR}/tests")
endif()
