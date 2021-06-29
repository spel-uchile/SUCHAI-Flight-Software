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
        ${PROJECT_BINARY_DIR}/include
        storage/include
        utils/include
        igrf/include
        sgp4/src/c
        linenoise
        libcsp/libcsp/include)
target_link_libraries(suchai-fs-lib PRIVATE suchai-fs-os)
target_link_libraries(suchai-fs-lib PRIVATE -lm -lzmq)
