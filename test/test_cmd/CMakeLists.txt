cmake_minimum_required(VERSION 3.5)
project(SUCHAI_Flight_Software_Test)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        ../../src/drivers/Linux/data_storage.c
        ../../src/os/Linux/osDelay.c
        ../../src/os/Linux/osQueue.c
        ../../src/os/Linux/osScheduler.c
        ../../src/os/Linux/osSemphr.c
        ../../src/os/Linux/osThread.c
        ../../src/os/Linux/pthread_queue.c
        ../../src/system/cmdOBC.c
        ../../src/system/cmdDRP.c
        ../../src/system/cmdConsole.c
        src/system/cmdTestCommand.c
        ../../src/system/repoCommand.c
        ../../src/system/repoData.c
        ../../src/system/taskDispatcher.c
        ../../src/system/taskExecuter.c
        ../../src/drivers/Linux/init.c
        src/system/taskTest.c
        src/system/main.c
        )

include_directories(
        ../../src/system/include
        ../../src/os/include
        ../../src/drivers/Linux/include
        ../../src/drivers/Linux/libcsp/include
        src/system/include
)

link_directories(../../src/drivers/Linux/libcsp/lib)

link_libraries(-lpthread -lsqlite3 -lcsp -lzmq)

add_executable(SUCHAI_Flight_Software_Test ${SOURCE_FILES})