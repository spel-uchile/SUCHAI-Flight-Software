include(../cmake/variables.cmake)

# Generate core config.h
get_filename_component(CONFIG_H_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../include" ABSOLUTE)
configure_file (
        "${CONFIG_H_PATH}/suchai/config.h.in"
        "${CONFIG_H_PATH}/suchai/config.h"
)
include_directories(${CONFIG_H_PATH})
message("Configuration file written to ${CONFIG_H_PATH}/suchai/config.h")
####
## INCLUDE TARGETS AND LINK
####
add_subdirectory(drivers)
add_subdirectory(lib)
add_subdirectory(os)
add_subdirectory(system)

target_link_libraries(suchai-fs-core PUBLIC suchai-fs-os)
target_link_libraries(suchai-fs-core PUBLIC suchai-fs-drivers)
target_link_libraries(suchai-fs-core PUBLIC suchai-fs-lib)
