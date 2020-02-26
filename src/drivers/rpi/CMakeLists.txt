cmake_minimum_required(VERSION 3.5)
project(SUCHAI_Flight_Software_RPi)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES
        ../../../src/drivers/rpi/sgp4/src/c/TLE.c
        ../../../src/drivers/rpi/sgp4/src/c/SGP4.c
        ../../../src/drivers/rpi/linenoise/linenoise.c
        ../../../src/drivers/rpi/data_storage.c
        ../../../src/drivers/rpi/init.c
        ../../../src/drivers/rpi/i2c.c
        ../../../src/os/Linux/osDelay.c
        ../../../src/os/Linux/osQueue.c
        ../../../src/os/Linux/osScheduler.c
        ../../../src/os/Linux/osSemphr.c
        ../../../src/os/Linux/osThread.c
        ../../../src/os/Linux/pthread_queue.c
        ../../../src/system/globals.c
        ../../../src/system/utils.c
        ../../../src/system/cmdDRP.c
        ../../../src/system/cmdOBC.c
        ../../../src/system/cmdCOM.c
        ../../../src/system/cmdFP.c
        ../../../src/system/cmdTM.c
        ../../../src/system/cmdEPS.c
        ../../../src/system/cmdConsole.c
        ../../../src/system/cmdSensors.c
        ../../../src/system/repoCommand.c
        ../../../src/system/repoData.c
        ../../../src/system/repoDataSchema.c
        ../../../src/system/taskDispatcher.c
        ../../../src/system/taskExecuter.c
        ../../../src/system/taskHousekeeping.c
        ../../../src/system/taskCommunications.c
        ../../../src/system/taskConsole.c
        ../../../src/system/taskFlightPlan.c
        ../../../src/system/taskSensors.c
        ../../../src/system/taskInit.c
        ../../../src/system/taskWatchdog.c
        ../../../src/system/main.c
        )

include_directories(
        ../../../src/system/include/
        ../../../src/os/include
        ../../../src/drivers/rpi/include
        ../../../src/drivers/rpi/libcsp/include
        ../../../src/drivers/rpi/linenoise
        ../../../src/drivers/x86/sgp4/src/c
        /usr/include/postgresql
)

link_directories(
        ../../../src/drivers/rpi/libcsp/lib
)

link_libraries(-lm -lcsp -lzmq -lsqlite3 -lpq -lpthread)

# Use pthread_setname_np included in <features.h>
add_definitions(-D_GNU_SOURCE)

add_executable(SUCHAI_Flight_Software ${SOURCE_FILES})

