cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)
project(rom-writer)

pico_sdk_init()
get_filename_component(PARENT_DIR ../../ ABSOLUTE)
add_executable(
    rom-writer src/main.cpp
    ${PARENT_DIR}/common/src/Rom.cpp
)
include_directories(
    ${PARENT_DIR}/common/include
)

target_link_libraries(
    rom-writer pico_stdlib hardware_spi
)

pico_enable_stdio_usb(rom-writer 1)
pico_add_extra_outputs(rom-writer)
