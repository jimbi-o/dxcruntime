cmake_minimum_required(VERSION 3.15)
project(dxcruntime
  VERSION 1.0
  DESCRIPTION "runtime dxc"
  LANGUAGES CXX
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  set(DXCRUNTIME_STANDALONE TRUE)
endif()

add_subdirectory(src)

# for Visual Studio
if(DXCRUNTIME_STANDALONE)
  set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT dxcruntime)
endif()
