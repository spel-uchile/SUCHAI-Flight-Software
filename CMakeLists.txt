cmake_minimum_required(VERSION 3.5)
project(SUCHAI_Flight_Software)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        drivers/Linux/data_storage.c
        os/Linux/osDelay.c
        os/Linux/osQueue.c
        os/Linux/osScheduler.c
        os/Linux/osSemphr.c
        os/Linux/osThread.c
        os/Linux/pthread_queue.c
        system/cmdDRP.c
        system/cmdOBC.c
        system/cmdConsole.c
        system/repoCommand.c
        system/repoData.c
        system/taskDispatcher.c
        system/taskExecuter.c
        system/taskHousekeeping.c
        system/taskConsole.c
        system/cmdTestCommand.c
        system/taskTest.c
        system/main.c)

include_directories(
        .
        system/include/
        os/include
        drivers/Linux/include
        drivers/Linux/libcsp/include
)

link_directories(drivers/Linux/libcsp/lib)

link_libraries(-lpthread -lsqlite3 -lcsp -lzmq)

add_executable(SUCHAI_Flight_Software ${SOURCE_FILES})