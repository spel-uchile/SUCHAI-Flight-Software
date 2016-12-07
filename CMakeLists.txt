cmake_minimum_required(VERSION 3.5)
project(SUCHAI_Flight_Software)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        OS/Linux/osDelay.c
        OS/Linux/osQueue.c
        OS/Linux/osScheduler.c
        OS/Linux/osSemphr.c
        OS/Linux/osThread.c
        OS/Linux/pthread_queue.c
        System/cmdDRP.c
        System/cmdOBC.c
        System/repoCommand.c
        System/repoData.c
        System/taskDispatcher.c
        System/taskExecuter.c
        System/taskHousekeeping.c
        System/taskConsole.c
        main.c
        SUCHAI_config.h)

include_directories(
        System/include/
        OS/include)

link_libraries(-lpthread)

add_executable(SUCHAI_Flight_Software ${SOURCE_FILES})