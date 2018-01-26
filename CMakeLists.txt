cmake_minimum_required(VERSION 3.5)
project(SUCHAI_Flight_Software)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/drivers/Linux/data_storage.c
        src/os/Linux/osDelay.c
        src/os/Linux/osQueue.c
        src/os/Linux/osScheduler.c
        src/os/Linux/osSemphr.c
        src/os/Linux/osThread.c
        src/os/Linux/pthread_queue.c
        src/system/cmdDRP.c
        src/system/cmdOBC.c
        src/system/cmdCOM.c
        src/system/cmdConsole.c
        src/system/repoCommand.c
        src/system/repoData.c
        src/system/taskDispatcher.c
        src/system/taskExecuter.c
        src/system/taskHousekeeping.c
        src/system/taskCommunications.c
        src/system/taskConsole.c
        src/system/main.c
        src/system/taskFlightPlan.c
        src/system/cmdFP.c
)

include_directories(
        src/system/include/
        src/os/include
        src/drivers/Linux/include
        src/drivers/Linux/libcsp/include
)

link_directories(src/drivers/Linux/libcsp/lib)

link_libraries(-lpthread -lsqlite3 -lcsp -lzmq)

add_executable(SUCHAI_Flight_Software ${SOURCE_FILES})
