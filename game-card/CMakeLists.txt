cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)
project(game-card)

pico_sdk_init()

add_executable(
    game-card src/main.cpp src/VirtualMachine.cpp
    src/Display.cpp src/Controls.cpp src/Rom.cpp
)
include_directories(
    include
)

target_link_libraries(
    game-card pico_stdlib hardware_i2c hardware_spi pico_multicore
)

pico_enable_stdio_usb(game-card 1)
pico_enable_stdio_uart(game-card 0)
pico_add_extra_outputs(game-card)
