#@author: Deng Zhimao
#@email: dengzhimao@alientek.com
#B站作品: https://space.bilibili.com/474103963?spm_id_from=333.788.0.0

#开源电子网: http://www.openedv.com/forum.php
#正点原子官网: https://www.alientek.com
#店铺: https://zhengdianyuanzi.tmall.com

#LICENCE GPLV3 
cmake_minimum_required(VERSION 3.8)

project(01_lvgl_button)
set(TOOLCHAIN_DIR /opt/atk-dlrk3506-toolchain)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/arm-buildroot-linux-gnueabihf-g++)
set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/arm-buildroot-linux-gnueabihf-gcc)
set(SYSROOT ${TOOLCHAIN_DIR}/arm-buildroot-linux-gnueabihf/sysroot/usr/include)
set(CMAKE_SYSROOT ${TOOLCHAIN_DIR}/arm-buildroot-linux-gnueabihf/sysroot)


include_directories(
        /home/alientek/Desktop/lvgl8/lvgl
        /home/alientek/Desktop/lvgl8/lv_drivers
        ${CMAKE_SYSROOT}/usr/include/c++/12.4.0
        )

add_definitions(-DUSE_RK3506=1)
add_definitions(-DUSE_DRM=1 -DUSE_EVDEV=1)



include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/lvgl8)
include_directories(${PROJECT_SOURCE_DIR}/common)
include_directories(${PROJECT_SOURCE_DIR}/sys)
include_directories(${PROJECT_SOURCE_DIR}/USER)
include_directories(${PROJECT_SOURCE_DIR}/lvgl_font)
include_directories(${PROJECT_SOURCE_DIR}/lvgl_screen)


aux_source_directory(. SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/lvgl8 SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/sys SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/common SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/USER SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/lvgl_font SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/lvgl_screen SRCS)


add_executable(${PROJECT_NAME} ${SRCS})


target_link_libraries(${PROJECT_NAME}
        /home/alientek/Desktop/lvgl8/lvgl/build/lib/liblvgl.a
        /home/alientek/Desktop/lvgl8/lv_drivers/build/lib/liblv_drivers.a
        freetype drm evdev
)


add_custom_target(distclean
    COMMAND ${CMAKE_COMMAND} -E remove CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove Makefile
    COMMAND ${CMAKE_COMMAND} -E remove_directory CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${PROJECT_NAME}
    COMMENT "Clean all CMake generated files and ${PROJECT_NAME}"
)
