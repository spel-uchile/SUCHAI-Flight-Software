cmake_minimum_required(VERSION 3.5)
project(SUCHAI_Flight_Software_GroundStation)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES
        ../../../src/drivers/groundstation/data_storage.c
        ../../../src/drivers/groundstation/init.c
        ../../../src/os/Linux/osDelay.c
        ../../../src/os/Linux/osQueue.c
        ../../../src/os/Linux/osScheduler.c
        ../../../src/os/Linux/osSemphr.c
        ../../../src/os/Linux/osThread.c
        ../../../src/os/Linux/pthread_queue.c
        ../../../src/system/cmdDRP.c
        ../../../src/system/cmdOBC.c
        ../../../src/system/cmdCOM.c
        ../../../src/system/cmdFP.c
        ../../../src/system/cmdTM.c
        ../../../src/system/cmdEPS.c
        ../../../src/system/cmdConsole.c
        ../../../src/system/repoCommand.c
        ../../../src/system/repoData.c
        ../../../src/system/taskDispatcher.c
        ../../../src/system/taskExecuter.c
        ../../../src/system/taskHousekeeping.c
        ../../../src/system/taskCommunications.c
        ../../../src/system/taskConsole.c
        ../../../src/system/taskFlightPlan.c
        ../../../src/system/cmdFP.c
        ../../../src/system/taskInit.c
        ../../../src/system/taskWatchdog.c
        ../../../src/system/main.c
        )

include_directories(
        ../../../src/system/include/
        ../../../src/os/include
        ../../../src/drivers/groundstation/include
        ../../../src/drivers/groundstation/libcsp/include
        /usr/include/postgresql
)

link_directories(
        ../../../src/drivers/Linux/libcsp/lib
        ../../../src/drivers/Linux/gs-drivers/libparam/lib
)

link_libraries(-lcsp -lzmq -lsqlite3 -lpq -lpthread)

# Use pthread_setname_np included in <features.h>
add_definitions(-D_GNU_SOURCE)

# Build nanopower drivers
if(USE_GSDRIVERS)
    add_definitions(-DSCH_USE_NANOPOWER)
    list(APPEND SOURCE_FILES src/drivers/Linux/gs-drivers/nanopower2.c)
    include_directories(src/drivers/Linux/gs-drivers/include)

    add_definitions(-DSCH_USE_NANOCOM)
    link_libraries(-lparam)
    include_directories(src/drivers/Linux/gs-drivers/libparam/include)
    include_directories(src/drivers/Linux/gs-drivers/libparam/src/include)
endif()

add_executable(SUCHAI_Flight_Software ${SOURCE_FILES})

