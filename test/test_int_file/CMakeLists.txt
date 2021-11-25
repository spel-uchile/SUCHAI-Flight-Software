###
# TEST MAKE FILE
##
set(SOURCE_FILES
        src/system/cmdFile.c
        src/system/taskTest.c
        src/system/main.c
        )
set(CURRENT_DIR ${CMAKE_CURRENT_BINARY_DIR})
configure_file(text.txt ${CMAKE_CURRENT_BINARY_DIR}/text.txt)
add_executable(suchai-test ${SOURCE_FILES})
target_include_directories(suchai-test PUBLIC include)
target_link_libraries(suchai-test PUBLIC suchai-fs-core)
get_filename_component(VARIABLES_H_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include" ABSOLUTE)
configure_file (
        "${VARIABLES_H_PATH}/suchai/variables.h.in"
        "${VARIABLES_H_PATH}/suchai/variables.h"
)