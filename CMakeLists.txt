cmake_minimum_required(VERSION 3.5)
project(SUCHAI_Flight_Software)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES
        src/drivers/x86/data_storage.c
        src/drivers/x86/init.c
        src/os/x86/osDelay.c
        src/os/x86/osQueue.c
        src/os/x86/osScheduler.c
        src/os/x86/osSemphr.c
        src/os/x86/osThread.c
        src/os/x86/pthread_queue.c
        src/system/cmdDRP.c
        src/system/cmdOBC.c
        src/system/cmdCOM.c
        src/system/cmdFP.c
        src/system/cmdTM.c
        src/system/cmdEPS.c
        src/system/cmdConsole.c
        src/system/repoCommand.c
        src/system/repoData.c
        src/system/taskDispatcher.c
        src/system/taskExecuter.c
        src/system/taskHousekeeping.c
        src/system/taskCommunications.c
        src/system/taskConsole.c
        src/system/taskFlightPlan.c
        src/system/cmdFP.c
        src/system/taskInit.c
        src/system/taskWatchdog.c
        src/system/main.c
        )

include_directories(
        src/system/include/
        src/os/include
        src/drivers/x86/include
        src/drivers/x86/libcsp/include
        /usr/include/postgresql
)

link_directories(
        src/drivers/x86/libcsp/lib
)

link_libraries(-lcsp -lzmq -lsqlite3 -lpq -lpthread)

# Use pthread_setname_np included in <features.h>
add_definitions(-D_GNU_SOURCE)

add_executable(SUCHAI_Flight_Software ${SOURCE_FILES})

