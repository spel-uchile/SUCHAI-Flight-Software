# Define libcsp path and build command
set(libcsp ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcsp/libcsp/lib/libcsp.a)
set(buildcsp python2 ./waf configure --with-os=posix --enable-if-zmqhub --enable-if-kiss --enable-crc32 --with-rtable cidr --with-driver-usart=linux --install-csp --prefix=libcsp build install)

# Update lib submodules and build csp
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # Get version
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --abbrev=4
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_REPO_VERSION)
    string(STRIP ${GIT_REPO_VERSION} GIT_REPO_VERSION)

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

# Configure the project
##
# Platform settings
##
set(SCH_OS_LIST LINUX;FREERTOS)
set(SCH_OS LINUX CACHE STRING "Select OS from ${SCH_OS_LIST}")
if(SCH_OS IN_LIST SCH_OS_LIST)
    set(${SCH_OS} ON)
else()
    message(FATAL_ERROR "Invalid SCH_OS selected, please select from ${SCH_OS_LIST}")
endif()
set(SCH_ARCH_LIST "X86;RPI;ESP32;NANOMIND")
set(SCH_ARCH "X86" CACHE STRING "Select ARCH from ${SCH_ARCH_LIST}")
if(SCH_ARCH IN_LIST SCH_ARCH_LIST)
    set(${SCH_ARCH} ON)
else()
    message(FATAL_ERROR "Invalid SCH_OS selected, please select from ${SCH_ARCH_LIST}")
endif()
##
# Software modules settings
##
set(SCH_LOG_LIST "NONE;RESULT;ERROR;WARN;INFO;DEBUG;VERBOSE")
set(SCH_LOG "INFO" CACHE STRING "Select log level from ${SCH_LOG_LIST}")
if(SCH_LOG IN_LIST SCH_LOG_LIST)
    set(SCH_LOG_LVL SCH_LOG_LVL_${SCH_LOG})
else()
    message(FATAL_ERROR "Invalid log level selected, please select from ${SCH_LOG_LIST}")
endif()
set(SCH_NAME \"SUCHAI-FS\" CACHE STRING "Device name")
set(SCH_DEVICE_ID 1 CACHE STRING "Device id")
set(SCH_SW_VERSION \"${GIT_REPO_VERSION}\" CACHE STRING "Software version")
set(SCH_CON_ENABLED 1 CACHE BOOL "Enable serial console task")
set(SCH_COMM_ENABLE OFF CACHE BOOL "Enable communications task")
set(SCH_FP_ENABLED ON CACHE BOOL "Enable flight plan task")
set(SCH_HK_ENABLED 0 CACHE BOOL "Enable housekeeping task")
set(SCH_MAX_WDT_TIMER 60 CACHE STRING "Period in seconds to send reset_wdt command")
set(SCH_MAX_GND_WDT_TIMER 48*3600 CACHE STRING "Software watchdog timer")
##
# Communication system settings
##
set(SCH_COMM_NODE 1 CACHE STRING "This CSP node address")
set(SCH_COM_MAX_PACKETS 10 CACHE STRING "Max. packets to transmit in a row")
set(SCH_COM_TX_DELAY_MS 3000 CACHE STRING "Delay (ms) between continuous transmissions")
set(SCH_CSP_BUFFERS 100 CACHE STRING "Available CSP buffers")
set(SCH_CSP_SOCK_LEN 100 CACHE STRING "Max number of packets in a connection queue")
set(SCH_COMM_ZMQ_OUT \"tcp://127.0.0.1:8001\" CACHE STRING "CSP ZMQ OUT socket URI")
set(SCH_COMM_ZMQ_IN  \"tcp://127.0.0.1:8002\" CACHE STRING "CSP ZMQ IN socket URI")
##
# Data repository system settings
##
set(SCH_ST_LIST RAM;SQLITE;POSTGRES;FLASH)
set(SCH_ST_MODE RAM CACHE STRING "Select OS from ${SCH_ST_LIST}")
if(SCH_ST_MODE IN_LIST SCH_ST_LIST)
    set(SCH_ST_${SCH_ST_MODE} ON)
    set(SCH_STORAGE_MODE SCH_ST_${SCH_ST_MODE})
else()
    message(FATAL_ERROR "Invalid SCH_STORAGE_MODE selected, please select from ${SCH_ST_LIST}")
endif()
set(SCH_STORAGE_TRIPLE_WR 0 CACHE STRING "Status variables tripled writing enabled")
set(SCH_STORAGE_FILE \"/tmp/suchai.db\" CACHE PATH "File to store the database, only if @SCH_STORAGE_MODE is SCH_ST_SQLITE")
set(SCH_STORAGE_PGUSER 0 CACHE STRING "PSQL User, only if @SCH_STORAGE_MODE is SCH_ST_POSTGRES")
set(SCH_STORAGE_PGPASS 0 CACHE STRING "PSQL password, only if @SCH_STORAGE_MODE is SCH_ST_POSTGRES")
set(SCH_STORAGE_PGHOST 0 CACHE STRING "PSQL password, only if @SCH_STORAGE_MODE is SCH_ST_POSTGRES")

# Generate core config.h
configure_file (
        "${PROJECT_SOURCE_DIR}/include/suchai/config.h.in"
        "${PROJECT_BINARY_DIR}/include/suchai/config.h"
)

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
        ${PROJECT_BINARY_DIR}/include
        system/include
        lib/storage/include
        lib/utils/include
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
