
set(SOURCE_FILES
        src/taskHousekeeping.c
        src/initApp.c
)

add_library(suchai-app ${SOURCE_FILES})
target_include_directories(suchai-app PRIVATE include/)
target_link_libraries(suchai-app PRIVATE suchai-fs-core)
