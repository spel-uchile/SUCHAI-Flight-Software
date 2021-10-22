###
# TEST MAKE FILE
##
set(SOURCE_FILES
        src/system/cmdTLE.c
        src/system/cmdTestTLE.c
        src/system/taskTest.c
        src/system/main.c
)
configure_file(data.csv ${CMAKE_CURRENT_BINARY_DIR}/data.csv COPYONLY)
add_executable(suchai-test ${SOURCE_FILES})
target_include_directories(suchai-test PUBLIC include)
target_link_libraries(suchai-test PUBLIC suchai-fs-core)