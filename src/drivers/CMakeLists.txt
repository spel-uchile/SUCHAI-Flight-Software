if(NOT DEFINED SCH_ARCH)
    message(FATAL_ERROR "Variable SCH_ARCH is not set!")
endif ()

string(TOLOWER ${SCH_ARCH} SCH_ARCH_PATH)
message("SCH_ARCH_PATH ${SCH_ARCH_PATH}")

set(SOURCE_FILES
        ${SCH_ARCH_PATH}/init.c
)

# Use pthread_setname_np included in <features.h>
add_library(suchai-fs-drivers ${SOURCE_FILES})
target_include_directories(suchai-fs-drivers PUBLIC ${SCH_ARCH_PATH}/include)
target_link_libraries(suchai-fs-drivers PRIVATE suchai-fs-core)