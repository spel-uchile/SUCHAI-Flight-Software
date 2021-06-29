if(NOT DEFINED SCH_OS)
    message(FATAL_ERROR "Variable SCH_OS is not set!")
endif ()

string(TOLOWER ${SCH_OS} SCH_OS_PATH)
message("SCH_OS_PATH ${SCH_OS_PATH}")

set(SOURCE_FILES
        ${SCH_OS_PATH}/osDelay.c
        ${SCH_OS_PATH}/osQueue.c
        ${SCH_OS_PATH}/osScheduler.c
        ${SCH_OS_PATH}/osSemphr.c
        ${SCH_OS_PATH}/osThread.c
)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${SCH_OS_PATH}/pthread_queue.c)
    list(APPEND SOURCE_FILES ${SCH_OS_PATH}/pthread_queue.c)
endif()

add_library(suchai-fs-os ${SOURCE_FILES})
# Use pthread_setname_np included in <features.h>
target_compile_definitions(suchai-fs-os PUBLIC _GNU_SOURCE)
target_include_directories(suchai-fs-os PUBLIC include)
target_link_libraries(suchai-fs-os PRIVATE -lpthread)

if(${SCH_OS} STREQUAL FREERTOS)
    message("Including FreeRTOS")
    add_subdirectory(freertos/FreeRTOS)
    target_link_libraries(suchai-fs-os PUBLIC freertos)
endif()
