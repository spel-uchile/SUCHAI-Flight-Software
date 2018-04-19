cmake_minimum_required(VERSION 3.5)
project(SUCHAI_Flight_Software_Test)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        ../../src/drivers/Linux/data_storage.c
        ../../src/os/Linux/osSemphr.c
        ../../src/system/repoData.c
        ../../src/system/repoCommand.c
        ../../src/system/cmdOBC.c
        ../../src/system/cmdDRP.c
        ../../src/system/cmdFP.c
        ../../src/system/cmdConsole.c
        src/system/main.c
        )

include_directories(
        ../../src/system/include
        ../../src/os/include
        ../../src/drivers/Linux/include
        ../../src/drivers/Linux/libcsp/include
        ../test_cmd/src/system/include
)

link_directories(../../src/drivers/Linux/libcsp/lib)

link_libraries(-lpthread -lsqlite3 -lcsp -lzmq -lcunit)

add_executable(SUCHAI_Flight_Software_Test ${SOURCE_FILES})