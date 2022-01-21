###
# TEST MAKE FILE
##
set(SOURCE_FILES
        src/system/cmdDummy.c
        src/system/taskTest.c
        src/system/main.c
        )
add_executable(suchai-test ${SOURCE_FILES})
target_include_directories(suchai-test PUBLIC include)
target_link_libraries(suchai-test PUBLIC suchai-fs-core)