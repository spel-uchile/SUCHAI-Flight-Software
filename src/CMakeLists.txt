# Define libcsp path and build command
set(libcsp ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcsp/libcsp/lib/libcsp.a)
set(buildcsp python2 ./waf configure --with-os=posix --enable-if-zmqhub --enable-if-kiss --enable-crc32 --with-rtable cidr --with-driver-usart=linux --install-csp --prefix=libcsp build install)

# Update lib submodules and build csp
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # Init submodules
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    else()
        # Build libcsp if not found
        if(NOT EXISTS "${libcsp}")
            execute_process(COMMAND ${buildcsp} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcsp)
        endif()
    endif()
endif()

set(SOURCE_FILES
        drivers/x86/init.c
        os/Linux/osDelay.c
        os/Linux/osQueue.c
        os/Linux/osScheduler.c
        os/Linux/osSemphr.c
        os/Linux/osThread.c
        os/Linux/pthread_queue.c
        lib/storage/storage_ram.c
        lib/utils/math_utils.c
        lib/utils/log_utils.c
        lib/igrf/igrf13.c
        lib/sgp4/src/c/SGP4.c
        lib/sgp4/src/c/TLE.c
        lib/linenoise/linenoise.c
        system/globals.c
        system/cmdDRP.c
        system/cmdOBC.c
        system/cmdCOM.c
        system/cmdFP.c
        system/cmdTM.c
        system/cmdConsole.c
        system/repoCommand.c
        system/repoData.c
        system/repoDataSchema.c
        system/taskDispatcher.c
        system/taskExecuter.c
        system/taskCommunications.c
        system/taskConsole.c
        system/taskFlightPlan.c
        system/taskInit.c
        system/taskWatchdog.c
)

add_library(suchai-fs-core ${SOURCE_FILES})

target_include_directories(suchai-fs-core PUBLIC
        system/include
        lib/storage/include
        lib/utils/include/suchai
        lib/igrf/include
        lib/linenoise
        lib/sgp4/src/c
        lib/libcsp/libcsp/include
        os/include
        drivers/x86/include
        /usr/include/postgresql
)

# Use pthread_setname_np included in <features.h>
target_compile_definitions(suchai-fs-core PUBLIC _GNU_SOURCE)

target_link_libraries(suchai-fs-core PUBLIC ${libcsp})
target_link_libraries(suchai-fs-core PRIVATE -lm -lzmq -lsqlite3 -lpq -lpthread)
