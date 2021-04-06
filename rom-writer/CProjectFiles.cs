namespace GameCard {
    public static class CProjectFiles {
        public static string CMakeLists =
@"cmake_minimum_required(VERSION 3.12)
get_filename_component(PARENT_DIR ../ ABSOLUTE)
include(${PARENT_DIR}/common/pico_sdk_import.cmake)
project(game-writer)

pico_sdk_init()

add_executable(
    game-writer main.cpp GameFile.cpp
    ${PARENT_DIR}/common/src/Rom.cpp
)
include_directories(
    ${PARENT_DIR}/common/include
)

target_link_libraries(
    game-writer pico_stdlib hardware_spi
)

pico_enable_stdio_usb(game-writer 1)
pico_enable_stdio_uart(game-writer 0)
pico_add_extra_outputs(game-writer)
";

        public static string MainFile =
@"#include <stdio.h>
#include <pico/stdlib.h>
#include <Rom.hpp>
#include " + "\"" + @"GameFile.hpp" + "\"" + @"

using namespace gamecard;

int main() {
    stdio_init_all();
    sleep_ms(1000);
    
    printf(" + "\"Welcome to game writer!\\n\"" + @");
    
    const M25lc512 rom;
    
    printf(" + "\"Writing game code to chip...\\n\"" + @");
    for(int i = 0; i < GAME_FILE_LEN; i++) {
        rom.write(i, gameFile_g + i, 1);
        printf(" + "\"%d \"" + @", gameFile_g[i]);
    }
    printf(" + "\"\\n\"" + @");
    
    printf(" + "\"Reading back code from chip...\\n\"" + @");
    uint8_t datum;
    for(int i = 0; i < GAME_FILE_LEN; i++) {
        rom.read(i, &datum, 1);
        printf(" + "\"%d \"" + @", datum);
    }
    
    printf(" + "\"\\nDone.\\n\"" + @");
    
    return 0;
}
";
        public static string PicoSdkImport =
@"# This is a copy of <PICO_SDK_PATH>/external/pico_sdk_import.cmake

# This can be dropped into an external project to help locate this SDK
# It should be include()ed prior to project()

if (DEFINED ENV{PICO_SDK_PATH} AND (NOT PICO_SDK_PATH))
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
    message(" + "\"" + @"Using PICO_SDK_PATH from environment ('${PICO_SDK_PATH}')" + "\"" + @")
endif ()

if (DEFINED ENV{PICO_SDK_FETCH_FROM_GIT} AND (NOT PICO_SDK_FETCH_FROM_GIT))
    set(PICO_SDK_FETCH_FROM_GIT $ENV{PICO_SDK_FETCH_FROM_GIT})
    message(" + "\"" + @"Using PICO_SDK_FETCH_FROM_GIT from environment ('${PICO_SDK_FETCH_FROM_GIT}')" + "\"" + @")
endif ()

if (DEFINED ENV{PICO_SDK_FETCH_FROM_GIT_PATH} AND (NOT PICO_SDK_FETCH_FROM_GIT_PATH))
    set(PICO_SDK_FETCH_FROM_GIT_PATH $ENV{PICO_SDK_FETCH_FROM_GIT_PATH})
    message(" + "\"" + @"Using PICO_SDK_FETCH_FROM_GIT_PATH from environment ('${PICO_SDK_FETCH_FROM_GIT_PATH}')" + "\"" + @")
endif ()

set(PICO_SDK_PATH " + "\"" + @"${PICO_SDK_PATH}" + "\"" + @" CACHE PATH " + "\"" + @"Path to the PICO SDK" + "\"" + @")
set(PICO_SDK_FETCH_FROM_GIT " + "\"" + @"${PICO_SDK_FETCH_FROM_GIT}" + "\"" + @" CACHE BOOL " + "\"" + @"Set to ON to fetch copy of PICO SDK from git if not otherwise locatable" + "\"" + @")
set(PICO_SDK_FETCH_FROM_GIT_PATH " + "\"" + @"${PICO_SDK_FETCH_FROM_GIT_PATH}" + "\"" + @" CACHE FILEPATH " + "\"" + @"location to download SDK" + "\"" + @")

if (NOT PICO_SDK_PATH)
    if (PICO_SDK_FETCH_FROM_GIT)
        include(FetchContent)
        set(FETCHCONTENT_BASE_DIR_SAVE ${FETCHCONTENT_BASE_DIR})
        if (PICO_SDK_FETCH_FROM_GIT_PATH)
            get_filename_component(FETCHCONTENT_BASE_DIR " + "\"" + @"${PICO_SDK_FETCH_FROM_GIT_PATH}" + "\"" + @" REALPATH BASE_DIR " + "\"" + @"${CMAKE_SOURCE_DIR}" + "\"" + @")
        endif ()
        FetchContent_Declare(
                pico_sdk
                GIT_REPOSITORY https://github.com/raspberrypi/pico-sdk
                GIT_TAG master
        )
        if (NOT pico_sdk)
            message(" + "\"" + @"Downloading PICO SDK" + "\"" + @")
            FetchContent_Populate(pico_sdk)
            set(PICO_SDK_PATH ${pico_sdk_SOURCE_DIR})
        endif ()
        set(FETCHCONTENT_BASE_DIR ${FETCHCONTENT_BASE_DIR_SAVE})
    else ()
        message(FATAL_ERROR
                " + "\"" + @"PICO SDK location was not specified. Please set PICO_SDK_PATH or set PICO_SDK_FETCH_FROM_GIT to on to fetch from git." + "\"" + @"
                )
    endif ()
endif ()

get_filename_component(PICO_SDK_PATH " + "\"" + @"${PICO_SDK_PATH}" + "\"" + @" REALPATH BASE_DIR " + "\"" + @"${CMAKE_BINARY_DIR}" + "\"" + @")
if (NOT EXISTS ${PICO_SDK_PATH})
    message(FATAL_ERROR " + "\"" + @"Directory '${PICO_SDK_PATH}' not found" + "\"" + @")
endif ()

set(PICO_SDK_INIT_CMAKE_FILE ${PICO_SDK_PATH}/pico_sdk_init.cmake)
if (NOT EXISTS ${PICO_SDK_INIT_CMAKE_FILE})
    message(FATAL_ERROR " + "\"" + @"Directory '${PICO_SDK_PATH}' does not appear to contain the PICO SDK" + "\"" + @")
endif ()

set(PICO_SDK_PATH ${PICO_SDK_PATH} CACHE PATH " + "\"" + @"Path to the PICO SDK" + "\"" + @" FORCE)

include(${PICO_SDK_INIT_CMAKE_FILE})
";
    }
}