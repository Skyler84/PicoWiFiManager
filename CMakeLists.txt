cmake_minimum_required(VERSION 3.15)

project(PicoWiFiManager CXX C ASM)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
# set(CMAKE_SYSTEM_NAME Generic)

include(${CMAKE_CURRENT_LIST_DIR}/pico_sdk_import.cmake)
add_compile_definitions(MBEDTLS_CONFIG_FILE="mbedtls_config.h")

pico_sdk_init()

message(STATUS "CC ${CMAKE_C_COMPILER}")
message(STATUS "CXX ${CMAKE_CXX_COMPILER}")

FetchContent_Declare(
    picogui
    GIT_REPOSITORY https://github.com/Skyler84/PicoGUI.git
    GIT_TAG        master
)

FetchContent_Declare(
    littlefs
    GIT_REPOSITORY https://github.com/lurk101/littlefs-lib.git
    GIT_TAG        master
)

FetchContent_MakeAvailable(picogui)
FetchContent_MakeAvailable(littlefs)


# add_compile_options(-H)

add_library(pico_wifi_manager STATIC
    src/pico_wifi_manager.cpp
)

add_library(pico_wifi_manager_gui STATIC
    src/gui/manager_view.cpp
    src/gui/scan_view.cpp
    src/gui/network_info_view.cpp
)

add_executable(pico_wifi_manager_app
    app/main.cpp
)

target_include_directories(pico_wifi_manager PUBLIC
    include/
)

target_link_libraries(pico_wifi_manager
    pico_util
    pico_cyw43_arch_lwip_poll
    pico_stdio_usb 
    hardware_watchdog 
    pico_binary_info 
    hardware_gpio 
    pico_platform
    pico_stdlib
    pico_mbedtls 
    pico_lwip_mbedtls
    pico_display
    littlefs-lib
)

target_link_libraries(pico_wifi_manager_gui
    pico_wifi_manager
    gui
)

target_link_libraries(pico_wifi_manager_app
    pico_wifi_manager_gui
)

pico_add_extra_outputs(pico_wifi_manager_app)