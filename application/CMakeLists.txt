cmake_minimum_required(VERSION 3.16)
project(rk3506_display LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Private LVGL 8.3.11: do not link the SDK's LVGL package.
set(LVGL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lvgl")
file(GLOB_RECURSE LVGL_SOURCES CONFIGURE_DEPENDS "${LVGL_DIR}/src/*.c")
add_library(lvgl8311 STATIC ${LVGL_SOURCES})
target_include_directories(lvgl8311 PUBLIC "${LVGL_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/config")
target_compile_definitions(lvgl8311 PUBLIC LV_CONF_INCLUDE_SIMPLE)
target_link_libraries(lvgl8311 PUBLIC m)

option(RK3506_LVGL_DEMO "Build standalone LVGL benchmark with on-screen FPS/CPU monitor" OFF)
if(RK3506_LVGL_DEMO)
    set(RK3506_LVGL_DEMO_REFRESH_MS 8 CACHE STRING "LVGL demo refresh period in ms")
    set(RK3506_LVGL_DEMO_DRAW_DIVISOR 4 CACHE STRING "LVGL demo draw buffer fraction denominator")
    target_compile_definitions(lvgl8311 PUBLIC
        RK3506_LVGL_DEMO=1
        RK3506_LVGL_DEMO_REFRESH_MS=${RK3506_LVGL_DEMO_REFRESH_MS}
        RK3506_LVGL_DEMO_DRAW_DIVISOR=${RK3506_LVGL_DEMO_DRAW_DIVISOR}
        _POSIX_C_SOURCE=200809L)
endif()

add_library(rm690a0 STATIC display/rm690a0.c platform/linux_display.c)
target_include_directories(rm690a0 PUBLIC display platform)
target_compile_options(rm690a0 PRIVATE -Wall -Wextra -Werror)
add_library(lv_display STATIC display/lv_display.c)
target_link_libraries(lv_display PUBLIC rm690a0 lvgl8311)
target_compile_options(lv_display PRIVATE -Wall -Wextra -Werror)
enable_testing()
if(RK3506_LVGL_DEMO)
    file(GLOB LVGL_BENCHMARK_ASSETS CONFIGURE_DEPENDS
        "${LVGL_DIR}/demos/benchmark/assets/*.c*")
    add_executable(lvgl_demo
        demo/lvgl_demo.c
        demo/demo_perf.c
        "${LVGL_DIR}/demos/benchmark/lv_demo_benchmark.c"
        ${LVGL_BENCHMARK_ASSETS})
    target_include_directories(lvgl_demo PRIVATE "${LVGL_DIR}/demos")
    target_link_libraries(lvgl_demo PRIVATE lv_display)
    target_compile_options(lvgl_demo PRIVATE -Wall -Wextra -Werror)
    add_executable(lvgl_demo_smoke
        ../tests/lvgl_demo_smoke.c
        demo/demo_perf.c
        "${LVGL_DIR}/demos/benchmark/lv_demo_benchmark.c"
        ${LVGL_BENCHMARK_ASSETS})
    target_include_directories(lvgl_demo_smoke PRIVATE "${LVGL_DIR}/demos" demo)
    target_link_libraries(lvgl_demo_smoke PRIVATE lvgl8311)
    target_compile_options(lvgl_demo_smoke PRIVATE -Wall -Wextra -Werror -UNDEBUG)
    add_test(NAME lvgl_demo_smoke COMMAND lvgl_demo_smoke)
else()
    # The benchmark uses a POSIX LVGL tick; normal applications use lv_tick_inc().
    add_executable(lvgl_baseline baseline.c)
    target_link_libraries(lvgl_baseline PRIVATE lvgl8311)
    add_test(NAME lvgl_baseline COMMAND lvgl_baseline)
    add_executable(display_test display_test.c)
    target_link_libraries(display_test PRIVATE lv_display)
    target_compile_options(display_test PRIVATE -Wall -Wextra -Werror)
    add_executable(display_protocol_test ../tests/display_protocol_test.c)
    target_link_libraries(display_protocol_test PRIVATE lv_display)
    target_compile_options(display_protocol_test PRIVATE -Wall -Wextra -Werror -UNDEBUG)
    add_test(NAME display_protocol COMMAND display_protocol_test)
    add_executable(linux_display_test ../tests/linux_display_test.c)
    target_link_libraries(linux_display_test PRIVATE rm690a0)
    target_compile_options(linux_display_test PRIVATE -Wall -Wextra -Werror -UNDEBUG)
    target_link_options(linux_display_test PRIVATE
        -Wl,--wrap=open,--wrap=open64,--wrap=close,--wrap=ioctl,--wrap=flock,--wrap=nanosleep
        -Wl,--wrap=__ioctl_time64,--wrap=__nanosleep64)
    add_test(NAME linux_display COMMAND linux_display_test)

    # P4 offline slice: original dialplate view and resources, without MCU HAL or business services.
    file(GLOB P4_RESOURCES CONFIGURE_DEPENDS
        "App/Resource/font/*.c"
        "App/Resource/img/*.c"
        "App/Resource/symbol/*.c")
    add_library(p4_dialplate STATIC
        App/Pages/Dialplate/DialplateView.cpp
        App/Resource/ResourcePool.cpp
        App/Utils/ResourceManager/ResourceManager.cpp
        App/Utils/lv_anim_label/numberFlow.cpp
        App/Utils/lv_anim_label/numberFlow_clock.cpp
        App/Utils/lv_ext/lv_anim_timeline_wrapper.c
        ${P4_RESOURCES})
    target_include_directories(p4_dialplate
        PUBLIC App
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../third_party")
    target_link_libraries(p4_dialplate PUBLIC lvgl8311)
    target_compile_options(p4_dialplate PRIVATE -Wall -Wextra)

    add_library(p4_page_manager STATIC
        App/Utils/PageManager/PageBase.cpp
        App/Utils/PageManager/PageManager_Base.cpp
        App/Utils/PageManager/PageManager_Router.cpp
        App/Utils/PageManager/PageManager_State.cpp
        App/Utils/PageManager/PageManager_Anim.cpp
        App/Utils/PageManager/PageManager_Drag.cpp)
    target_include_directories(p4_page_manager PUBLIC App)
    target_link_libraries(p4_page_manager PUBLIC lvgl8311)
    target_compile_options(p4_page_manager PRIVATE -Wall -Wextra)

    add_library(p4_app STATIC
        App/ButtonGesture.cpp
        App/P4App.cpp
        App/Pages/StatusBar/StatusBar.cpp
        App/Pages/WorkSettings/WorkSettingsView.cpp
        App/Pages/RecordConfig/RecordConfigView.cpp
        App/Pages/SystemInfos/SystemInfosView.cpp
        App/Pages/Shutdown/ShutdownView.cpp
        App/Pages/StarMap/StarMapView.cpp
        App/Pages/Startup/StartupView.cpp
        App/Pages/HardwareCheckView/HardwareCheckView.cpp
        App/Pages/SaveConfig/SaveConfigView.cpp
        App/Utils/I18n/I18n.cpp
        App/Utils/I18n/I18n_en.cpp
        App/Utils/I18n/I18n_ru.cpp)
    target_include_directories(p4_app PUBLIC App)
    target_link_libraries(p4_app PUBLIC p4_page_manager p4_dialplate)
    target_compile_options(p4_app PRIVATE -Wall -Wextra -Werror)

    add_executable(p4_navigation_preview ../tests/p4_navigation_preview.cpp)
    target_link_libraries(p4_navigation_preview PRIVATE p4_app)
    target_compile_options(p4_navigation_preview PRIVATE -Wall -Wextra -Werror -UNDEBUG)
    add_test(NAME p4_navigation_preview COMMAND p4_navigation_preview)

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        add_executable(p4_screen p4_screen.cpp platform/linux_button.c)
        target_link_libraries(p4_screen PRIVATE p4_app lv_display)
        target_compile_options(p4_screen PRIVATE -Wall -Wextra -Werror)
        add_executable(button_probe button_probe.c platform/linux_button.c)
        target_include_directories(button_probe PRIVATE platform)
        target_compile_options(button_probe PRIVATE -Wall -Wextra -Werror)
    endif()

    add_executable(p4_dialplate_preview ../tests/p4_dialplate_preview.cpp)
    target_link_libraries(p4_dialplate_preview PRIVATE p4_dialplate)
    target_compile_options(p4_dialplate_preview PRIVATE -Wall -Wextra -Werror -UNDEBUG)
    add_test(NAME p4_dialplate_preview COMMAND p4_dialplate_preview)
endif()
