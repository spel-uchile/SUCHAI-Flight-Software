cmake_minimum_required(VERSION 3.16...3.19)
project(SUCHAI_Flight_Software_Test)

set(CMAKE_C_STANDARD 99)

include(../../cmake/variables.cmake)

# Generate core config.h
get_filename_component(CONFIG_H_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../include" ABSOLUTE)
configure_file (
        "${CONFIG_H_PATH}/suchai/config.h.in"
        "${CONFIG_H_PATH}/suchai/config.h"
)
include_directories(${CONFIG_H_PATH})
message("Configuration file written to ${CONFIG_H_PATH}/suchai/config.h")


####
## INCLUDE TARGETS AND LINK
####

include_directories(
        ../../src/system/include
        ../../src/os/include
        ../../src/drivers/x86/include
        ../../src/lib/storage/include
        ../../src/lib/libcsp/libcsp/include
        ../../src/lib/sgp4/src/c
        ../../src/lib/utils/include
        ../../src/lib/linenoise
        ../../include
        include
)

add_subdirectory(../../src/lib build_lib)
add_subdirectory(../../src/os build_os)
add_subdirectory(../../src/system build_core)

add_executable(suchai-unitest src/system/main.c)
target_link_libraries(suchai-unitest PUBLIC suchai-fs-core)
target_link_libraries(suchai-unitest PUBLIC suchai-fs-os)
target_link_libraries(suchai-unitest PUBLIC suchai-fs-lib)
target_link_libraries(suchai-unitest PUBLIC cunit)