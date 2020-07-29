cmake_minimum_required(VERSION 3.5)
project(SUCHAI_Flight_Software)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES
        src/drivers/simulator/sgp4/src/c/SGP4.c
        src/drivers/simulator/sgp4/src/c/TLE.c
        src/drivers/simulator/linenoise/linenoise.c
        src/drivers/simulator/data_storage.c
        src/drivers/simulator/init.c
        src/os/sim/osDelay.c
        src/os/sim/osQueue.c
        src/os/sim/osScheduler.c
        src/os/sim/osSemphr.c
        src/os/sim/osThread.c
        src/os/sim/pthread_queue.c
        src/lib/math_utils.c
        src/lib/log_utils.c
        src/system/globals.c
        src/system/cmdDRP.c
        src/system/cmdOBC.c
        src/system/cmdCOM.c
        src/system/cmdFP.c
        src/system/cmdTM.c
        src/system/cmdEPS.c
        src/system/cmdConsole.c
        src/system/cmdSensors.c
        src/system/repoCommand.c
        src/system/repoData.c
        src/system/repoDataSchema.c
        src/system/taskDispatcher.c
        src/system/taskExecuter.c
        src/system/taskHousekeeping.c
        src/system/taskCommunications.c
        src/system/taskConsole.c
        src/system/taskFlightPlan.c
        src/system/taskSensors.c
        src/system/taskInit.c
        src/system/taskWatchdog.c
        src/system/main.c
        )

include_directories(
        src/system/include
        src/lib/include
        src/os/include
        src/drivers/simulator/sgp4/src/c
        src/drivers/simulator/linenoise
        src/drivers/simulator/include
        src/drivers/simulator/libcsp/include
        /usr/include/postgresql
)

link_directories(
        src/drivers/simulator/libcsp/lib
)

link_libraries(-lm -lcsp -lzmq -lsqlite3 -lpq -lpthread)

# Use pthread_setname_np included in <features.h>
add_definitions(-D_GNU_SOURCE)

add_executable(SUCHAI_Flight_Software ${SOURCE_FILES})

