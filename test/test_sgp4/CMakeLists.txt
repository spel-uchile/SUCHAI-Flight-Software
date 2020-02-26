cmake_minimum_required(VERSION 3.5)
project(SUCHAI_Flight_Software_Test)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        ../../src/drivers/x86/sgp4/src/c/TLE.c
        ../../src/drivers/x86/sgp4/src/c/SGP4.c
        ../../src/drivers/x86/linenoise/linenoise.c
        ../../src/drivers/x86/data_storage.c
        ../../src/drivers/x86/init.c
        ../../src/os/Linux/osDelay.c
        ../../src/os/Linux/osQueue.c
        ../../src/os/Linux/osScheduler.c
        ../../src/os/Linux/osSemphr.c
        ../../src/os/Linux/osThread.c
        ../../src/os/Linux/pthread_queue.c
        ../../src/system/cmdTM.c
        ../../src/system/cmdCOM.c
        ../../src/system/cmdOBC.c
        ../../src/system/cmdDRP.c
        ../../src/system/cmdSensors.c
        ../../src/system/cmdConsole.c
        ../../src/system/repoCommand.c
        ../../src/system/repoData.c
        ../../src/system/repoDataSchema.c
        ../../src/system/taskDispatcher.c
        ../../src/system/taskExecuter.c
#        ../../src/system/taskInit.c
#        ../../src/system/taskConsole.c
#        ../../src/system/taskCommunications.c
#        ../../src/system/taskWatchdog.c
        ../../src/system/utils.c
        ../../src/system/globals.c
        src/system/taskTest.c
        src/system/main.c
        )

include_directories(
        ../../src/system/include
        ../../src/os/include
        ../../src/drivers/x86/include
        ../../src/drivers/x86/libcsp/include
        ../../src/drivers/x86/linenoise
        ../../src/drivers/x86/sgp4/src/c
        src/system/include
        /usr/include/postgresql
)

set(GCC_COVERAGE_COMPILE_FLAGS "-D_GNU_SOURCE")

add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

link_directories(../../src/drivers/x86/libcsp/lib)

link_libraries(-lm -lcsp -lzmq -lsqlite3 -lpq -lpthread)

add_executable(SUCHAI_Flight_Software_Test ${SOURCE_FILES})