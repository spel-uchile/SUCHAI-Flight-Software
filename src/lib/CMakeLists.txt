# Define libcsp path and build command
set(LIBCSP_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libcsp)
set(LIBCSP_PPREFIX ${LIBCSP_PATH}/libcsp)
set(LIBCSP_BUILD_CMD python2 ./waf configure --with-os=posix --enable-if-zmqhub --enable-if-kiss --enable-crc32 --with-rtable cidr --with-driver-usart=linux --with-max-connections=100 --with-conn-queue-length=1000 --with-router-queue-length=1000 --install-csp --prefix=${LIBCSP_PPREFIX} build install)

# Build libcsp if not found
if(NOT EXISTS "${LIBCSP_PPREFIX}/lib/libcsp.a")
    message("Building libcsp...")
    execute_process(COMMAND ${LIBCSP_BUILD_CMD} WORKING_DIRECTORY "${LIBCSP_PATH}" RESULT_VARIABLE CSP_RESULTS)
    message("${CSP_RESULTS}")
endif()

set(SOURCE_FILES
        utils/math_utils.c
        utils/log_utils.c
        igrf/igrf13.c
        sgp4/src/c/SGP4.c
        sgp4/src/c/TLE.c
        linenoise/linenoise.c
)

if( SCH_STORAGE_MODE STREQUAL "SCH_ST_RAM")
    message("Using storage/storage_ram.c")
    list(APPEND SOURCE_FILES storage/storage_ram.c)
elseif(${SCH_STORAGE_MODE} STREQUAL SCH_ST_SQLITE)
    message("Using storage/storage_sqlite.c")
    list(APPEND SOURCE_FILES storage/storage_sqlite.c)
elseif(${SCH_STORAGE_MODE} STREQUAL SCH_ST_FLASH)
    message("Using storage/storage_flash.c")
    list(APPEND SOURCE_FILES storage/storage_flash.c)
else()
    message(FATAL_ERROR "Invalid storage mode ${SCH_STORAGE_MODE}!")
endif()

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
target_link_libraries(suchai-fs-lib PUBLIC ${LIBCSP_PPREFIX}/lib/libcsp.a)
target_link_libraries(suchai-fs-lib PRIVATE -lm -lzmq)
target_link_libraries(suchai-fs-lib PUBLIC ${LIBCSP_PPREFIX}/lib/libcsp.a)
# Link proper storage libraries
if(SCH_STORAGE_MODE STREQUAL "SCH_ST_SQLITE")
    target_link_libraries(suchai-fs-lib PRIVATE -lsqlite3)
elseif(SCH_STORAGE_MODE STREQUAL "SCH_ST_POSTGRES")
    target_link_libraries(suchai-fs-lib PRIVATE -lpq)
endif()

