cmake_minimum_required(VERSION 3.16...3.19)

# Project name and a few useful settings. Other commands can pick up the results
project(
  SUCHAI-Flight-Software
  DESCRIPTION "SUCHAI Nanosatellite Flight Software Framework"
  LANGUAGES C)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(CMAKE_C_STANDARD 99)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# Select app or test subproject
set(APP simple CACHE STRING "Application name")
set(TEST off CACHE BOOL "Enable testing")
if(TEST)
  set(BASE_DIR test)
else()
  set(BASE_DIR apps)
endif()
message("APP ${BASE_DIR}/${APP}")

# The compiled library code is here
include_directories(${BASE_DIR}/${APP}/include)
add_subdirectory(${BASE_DIR}/${APP})
add_subdirectory(src)
