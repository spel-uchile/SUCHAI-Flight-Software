cmake_minimum_required(VERSION 3.5)
project(SUCHAI_Flight_Software_Test)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        ../../src/drivers/x86/data_storage.c
        ../../src/os/Linux/osSemphr.c
        ../../src/system/repoData.c
        ../../src/system/repoDataSchema.c
        ../../src/system/repoCommand.c
        ../../src/system/cmdOBC.c
        ../../src/system/cmdDRP.c
        ../../src/system/cmdFP.c
        ../../src/system/cmdConsole.c
        ../../src/system/cmdCOM.c
        ../../src/system/cmdTM.c
        src/system/main.c
        )

include_directories(
        ../../src/system/include
        ../../src/os/include
        ../../src/drivers/x86/include
        ../../src/drivers/x86/libcsp/include
        ../test_cmd/src/system/include
        /usr/include/postgresql
)

link_directories(../../src/drivers/x86/libcsp/lib)

link_libraries(-lcsp -lzmq -lsqlite3 -lpq -lpthread -lcunit)

add_executable(SUCHAI_Flight_Software_Test ${SOURCE_FILES})