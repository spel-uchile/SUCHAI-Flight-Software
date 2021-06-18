
set(SOURCE_FILES
        src/initApp.c
        src/cmdADCS.c
        src/cmdSensors.c
        src/taskADCS.c
        src/taskSensors.c
)

add_library(suchai-app ${SOURCE_FILES})
target_include_directories(suchai-app PRIVATE include/)
target_link_libraries(suchai-app PRIVATE suchai-fs-core)
