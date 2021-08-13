cmake_minimum_required(VERSION 3.5)
project(SUCHAI_Flight_Software_Test)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        ../../src/lib/sgp4/src/c/TLE.c
        ../../src/lib/sgp4/src/c/SGP4.c
        #../../src/lib/storage/storage_ram.c
        ../../src/lib/storage/storage_sqlite.c
        ../../src/os/linux/osSemphr.c
        ../../src/os/linux/osDelay.c
        ../../src/system/repoData.c
        ../../src/system/repoDataSchema.c
        ../../src/system/repoCommand.c
        ../../src/system/cmdOBC.c
        ../../src/system/cmdDRP.c
        ../../src/system/cmdFP.c
        ../../src/system/cmdConsole.c
        ../../src/system/cmdCOM.c
        ../../src/system/cmdTM.c
        ../../src/lib/utils/log_utils.c
        ../../src/lib/utils/math_utils.c
        ../../src/system/globals.c
        src/system/main.c
        )

include_directories(
        ../../src/system/include
        ../../src/os/include
        ../../src/os/include
        ../../src/drivers/x86/include
        ../../src/lib/storage/include
        ../../src/lib/libcsp/include
        ../../src/lib/libcsp/libcsp/include
        ../../src/lib/sgp4/src/c
        ../../src/lib/utils/include
        ../../include
        ../../apps/simple/include
        ../test_cmd/src/system/include
        /usr/include/postgresql
)

link_directories(../../src/lib/libcsp/libcsp/lib)

link_libraries(-lm -lcsp -lzmq -lsqlite3 -lpq -lpthread -lcunit)

add_executable(SUCHAI_Flight_Software_Test ${SOURCE_FILES})