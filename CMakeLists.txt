cmake_minimum_required(VERSION 3.16...3.19)

# Project name and a few useful settings. Other commands can pick up the results
project(
  SUCHAI-Flight-Software
  VERSION 3.0
  DESCRIPTION "SUCHAI Nanosatellite Flight Software Framework"
  LANGUAGES C)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(CMAKE_C_STANDARD 99)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# The compiled library code is here
add_subdirectory(apps/simple)
add_subdirectory(src)

# Framework entry point
add_executable(suchai-fs src/system/main.c)
target_link_libraries(suchai-fs suchai-fs-core)
target_link_libraries(suchai-fs suchai-app)