###
# NANO-PB-SIMPLE-APP CMAKE FILE
##

set(SOURCE_FILES
        src/system/main.c
        src/system/task.c
        src/system/cmdNANO.c
)
find_package(Nanopb REQUIRED)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS}
        PROPERTIES GENERATED TRUE)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g -O0")

add_executable(suchai-app ${SOURCE_FILES})
add_executable(suchai-app ${PROTO_SRCS} ${PROTO_HDRS})

target_include_directories(suchai-app PUBLIC include)
target_include_directories(suchai-app PUBLIC ${NANOPB_INCLUDE_DIRS})
target_link_libraries(suchai-app PUBLIC suchai-fs-core)

