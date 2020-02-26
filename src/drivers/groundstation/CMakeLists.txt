cmake_minimum_required(VERSION 3.5)
project(SUCHAI_Flight_Software_GroundStation)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES

        #pendiente conf_util.h que se genera al correr ./waf en suchai-drivers-bus, actualmente solo se copio al include de groundstation

        #nanocom-ax_client files
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/nanocom-ax_client/src/ax100_client.c

        #libparam_client files
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libparam_client/src/serialize.c
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libparam_client/src/string.c
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libparam_client/src/table.c
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libparam_client/src/rparam/deprecated_rparam.c
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libparam_client/src/rparam/query.c
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libparam_client/src/rparam/rparam.c


        #libutil files
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libutil/src/log/log.c
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libutil/src/error.c
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libutil/src/byteorder.c
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libutil/src/bytebuffer.c
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libutil/src/fletcher.c
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libutil/src/lock.c
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libutil/src/strtoint.c
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libutil/src/linux/mutex.c
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libutil/src/rtc.c
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libutil/src/string.c
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libutil/src/linux/clock.c
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libutil/src/log/appender/console.c

        #nanopower files
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/nanopower_client/src/nanopower2.c

        # FS files
        ../../../src/drivers/groundstation/sgp4/src/c/TLE.c
        ../../../src/drivers/groundstation/sgp4/src/c/SGP4.c
        ../../../src/drivers/groundstation/linenoise/linenoise.c
        ../../../src/drivers/groundstation/data_storage.c
        ../../../src/drivers/groundstation/init.c
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

        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libparam_client/src
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libparam_client/include/deprecated
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libparam_client/include
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libparam_client/include/gs

        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libgscsp/include


        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/nanocom-ax_client/include

        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libutil/include/deprecated/gs/gosh
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libutil/include
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libutil/include/gs
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/libutil
        ../../../src/drivers/groundstation/suchai-drivers-bus/lib/nanopower_client

        ../../../src/system/include/
        ../../../src/os/include
        ../../../src/drivers/groundstation/include
        ../../../src/drivers/groundstation/libcsp/include
        ../../../src/drivers/groundstation/linenoise
        ../../../src/drivers/groundstation/sgp4/src/c
        /usr/include/postgresql
)

link_directories(
        ../../../src/drivers/groundstation/libcsp/lib
        ../../../src/drivers/groundstation/gs-drivers/libparam/lib
)

link_libraries(-lm -lcsp -lzmq -lsqlite3 -lpq -lpthread)

# Use pthread_setname_np included in <features.h>
add_definitions(-D_GNU_SOURCE)

# Build nanopower drivers
if(USE_GSDRIVERS)
    add_definitions(-DSCH_USE_NANOPOWER)
    list(APPEND SOURCE_FILES src/drivers/Linux/gs-drivers/nanopower2.c)
    include_directories(src/drivers/Linux/gs-drivers/include)

    add_definitions(-DSCH_USE_NANOCOM)
    link_libraries(-lparam)
    include_directories(src/drivers/Linux/gs-drivers/libparam/include)
    include_directories(src/drivers/Linux/gs-drivers/libparam/src/include)
endif()

add_executable(SUCHAI_Flight_Software ${SOURCE_FILES})

