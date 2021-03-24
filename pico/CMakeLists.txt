cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)
project(game-card)

pico_sdk_init()

add_executable(
    game-card
    src/main.cpp src/oled.cpp
)
include_directories(
    include
)

target_link_libraries(
    game-card pico_stdlib hardware_i2c
)

pico_enable_stdio_usb(game-card 1)
pico_enable_stdio_uart(game-card 1)

pico_add_extra_outputs(game-card)
