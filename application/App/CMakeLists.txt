# App libraries: log, data center, page manager, dialplate offline slice, and full P4 application

add_library(p4_log STATIC Utils/Log/Log.cpp)
target_include_directories(p4_log PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_options(p4_log PRIVATE -Wall -Wextra -Werror)

add_library(p4_data_center STATIC
    Utils/DataCenter/Account.cpp
    Utils/DataCenter/DataCenter.cpp
    Utils/DataCenter/PingPongBuffer/PingPongBuffer.c
)
target_include_directories(p4_data_center PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(p4_data_center PUBLIC lvgl8311 PRIVATE p4_log)
target_compile_options(p4_data_center PRIVATE -Wall -Wextra -Werror)

add_library(p4_page_manager STATIC
    Utils/PageManager/PageBase.cpp
    Utils/PageManager/PageManager_Base.cpp
    Utils/PageManager/PageManager_Router.cpp
    Utils/PageManager/PageManager_State.cpp
    Utils/PageManager/PageManager_Anim.cpp
    Utils/PageManager/PageManager_Drag.cpp
)
target_include_directories(p4_page_manager PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(p4_page_manager PUBLIC lvgl8311 PRIVATE p4_log)
target_compile_options(p4_page_manager PRIVATE -Wall -Wextra)

# P4 offline slice: original dialplate view and resources, without MCU HAL or business services.
file(GLOB P4_RESOURCES CONFIGURE_DEPENDS
    "Resource/font/*.c"
    "Resource/img/*.c"
    "Resource/symbol/*.c"
)
get_filename_component(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../third_party" REALPATH)
add_library(p4_dialplate STATIC
    Pages/Dialplate/DialplateView.cpp
    Resource/ResourcePool.cpp
    Utils/ResourceManager/ResourceManager.cpp
    Utils/lv_anim_label/numberFlow.cpp
    Utils/lv_anim_label/numberFlow_clock.cpp
    Utils/lv_ext/lv_anim_timeline_wrapper.c
    ${P4_RESOURCES}
)
target_include_directories(p4_dialplate
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    PRIVATE "${THIRD_PARTY_DIR}"
)
target_link_libraries(p4_dialplate PUBLIC lvgl8311)
target_compile_options(p4_dialplate PRIVATE -Wall -Wextra)

add_library(p4_app STATIC
    ButtonGesture.cpp
    P4App.cpp
    Common/DataProc/DataProc.cpp
    Common/DataProc/DP_Status.cpp
    Common/SystemService.cpp
    Pages/Dialplate/DialplateModel.cpp
    Pages/WorkSettings/WorkSettingsModel.cpp
    Pages/RecordConfig/RecordConfigModel.cpp
    Pages/SystemInfos/SystemInfosModel.cpp
    Pages/SystemDash/SystemDashModel.cpp
    Pages/StarMap/StarMapModel.cpp
    Pages/Startup/StartupModel.cpp
    Pages/SystemLoading/SystemLoadingModel.cpp
    Pages/SaveConfig/SaveConfigModel.cpp
    Pages/SystemSettings/SystemSettingsModel.cpp
    Pages/SystemSettings/SystemSettings.cpp
    Pages/SystemSettings/SystemSettingsView.cpp
    Status/DemoStatus.cpp
    Pages/StatusBar/StatusBar.cpp
    Pages/Dialplate/Dialplate.cpp
    Pages/WorkSettings/WorkSettings.cpp
    Pages/WorkSettings/WorkSettingsView.cpp
    Pages/RecordConfig/RecordConfig.cpp
    Pages/RecordConfig/RecordConfigView.cpp
    Pages/SystemInfos/SystemInfos.cpp
    Pages/SystemInfos/SystemInfosView.cpp
    Pages/SystemDash/SystemDash.cpp
    Pages/SystemDash/SystemDashView.cpp
    Pages/StarMap/StarMap.cpp
    Pages/StarMap/StarMapView.cpp
    Pages/Startup/Startup.cpp
    Pages/Startup/StartupView.cpp
    Pages/SystemLoading/SystemLoading.cpp
    Pages/SystemLoading/SystemLoadingView.cpp
    Pages/SaveConfig/SaveConfig.cpp
    Pages/SaveConfig/SaveConfigView.cpp
    Utils/I18n/I18n.cpp
    Utils/I18n/I18n_en.cpp
    Utils/I18n/I18n_ru.cpp
)
target_include_directories(p4_app PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(p4_app PUBLIC p4_page_manager p4_dialplate p4_data_center p4_log)
target_compile_options(p4_app PRIVATE -Wall -Wextra -Werror)
