###
# SIMPLE APP CMAKE FILE
##
set(SOURCE_FILES
        src/system/main.c
        src/system/taskHousekeeping.c
        src/system/cmdAPP.c
)

add_executable(suchai-app ${SOURCE_FILES})
target_include_directories(suchai-app PUBLIC include)
target_link_libraries(suchai-app PUBLIC suchai-fs-core)
