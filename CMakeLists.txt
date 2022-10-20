cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME nttiny)

project(${PROJECT_NAME}
  VERSION 0.7.0.2
  DESCRIPTION "OpenGL-based runtime scientific visualization library"
  LANGUAGES CXX
)

set(DEBUG OFF CACHE BOOL "Debug mode")

if(${DEBUG} STREQUAL "OFF")
  set(CMAKE_BUILD_TYPE Release)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -DNDEBUG")
  message(STATUS "Release mode")
else()
  set(CMAKE_BUILD_TYPE Debug)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -DDEBUG")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
  message(STATUS "Debug mode")
endif()

set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set default install prefix
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/bin CACHE PATH "install directory" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

message(STATUS "Install path: ${CMAKE_INSTALL_PREFIX}")

add_subdirectory(${PROJECT_NAME})

if(${BUILD_EXAMPLES} STREQUAL "ON")
  add_subdirectory(examples)
endif()
