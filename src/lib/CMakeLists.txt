# Define libcsp path and build command
message(${CMAKE_CURRENT_SOURCE_DIR})
set(LIBCSP_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libcsp)
set(LIBCSP_PPREFIX ${LIBCSP_PATH}/libcsp)
set(LIBCSP_BUILD_CMD python2 ./waf --with-os=posix --enable-if-zmqhub --enable-if-kiss --enable-crc32 --with-rtable cidr --with-driver-usart=linux --install-csp --prefix=${LIBCSP_PPREFIX} configure build install)

# Build libcsp if not found
message("${LIBCSP_PPREFIX}")
if(NOT EXISTS "${LIBCSP_PPREFIX}/lib/libcsp.a")
    message("Building libcsp...")
    message("cd ${LIBCSP_PATH}")
    execute_process(COMMAND ${LIBCSP_BUILD_CMD} configure WORKING_DIRECTORY "${LIBCSP_PATH}" RESULT_VARIABLE CSP_RESULTS)
    message("${CSP_RESULTS}")
endif()

set(SOURCE_FILES
        storage/storage_ram.c
        utils/math_utils.c
        utils/log_utils.c
        igrf/igrf13.c
        sgp4/src/c/SGP4.c
        sgp4/src/c/TLE.c
        linenoise/linenoise.c
)

# Use pthread_setname_np included in <features.h>
add_library(suchai-fs-lib ${SOURCE_FILES})
target_compile_definitions(suchai-fs-lib PUBLIC _GNU_SOURCE)
target_include_directories(suchai-fs-lib PUBLIC
        storage/include
        utils/include
        igrf/include
        sgp4/src/c
        linenoise
        libcsp/libcsp/include)

target_link_libraries(suchai-fs-lib PRIVATE suchai-fs-os)
target_link_libraries(suchai-fs-lib PRIVATE -lm -lzmq)
target_link_libraries(suchai-fs-lib PUBLIC ${LIBCSP_PPREFIX}/lib/libcsp.a)

