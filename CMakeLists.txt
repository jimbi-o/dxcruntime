cmake_minimum_required(VERSION 3.15)
project(dxcruntime
  VERSION 1.0
  DESCRIPTION "runtime dxc"
  LANGUAGES CXX
)

option(LIB_TEST_MODE "library test mode (does not launch app)" OFF)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

add_subdirectory(src)

# for Visual Studio
if(LIB_TEST_MODE)
  set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT dxcruntime)
endif()
