cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME nttiny)
set(EXECUTABLE ${PROJECT_NAME}.example)
set(STATIC_LIB ${PROJECT_NAME})

project(${PROJECT_NAME} LANGUAGES CXX)

add_executable(${EXECUTABLE})
add_library(${STATIC_LIB} STATIC)

# main source file

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/bin CACHE PATH "install directory" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

message(STATUS "Install path: ${CMAKE_INSTALL_PREFIX}")

option(USE_CXX17_STD "Nttiny should be built with C++17 compiler option enabled" ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -Ofast -DNDEBUG -fPIE")

set(SRC_FILES "examples.cpp")
list(APPEND INC_DIRS ${PROJECT_SOURCE_DIR}/src)

set(EXTERN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern)

add_subdirectory(src)

target_compile_features(${EXECUTABLE} PRIVATE cxx_std_17)
target_sources(${EXECUTABLE} PRIVATE ${SRC_FILES})

target_include_directories(${EXECUTABLE} PRIVATE ${INC_DIRS})
target_link_directories(${EXECUTABLE} PRIVATE ${LIB_DIRS})
target_link_libraries(${EXECUTABLE} PRIVATE ${STATIC_LIB})
target_link_libraries(${EXECUTABLE} PRIVATE ${LIBS} ${CMAKE_DL_LIBS})

install(TARGETS ${STATIC_LIB} ${EXECUTABLE} DESTINATION .)

message(NOTICE "\nmake nttiny -j: build static library")
message(NOTICE "make nttiny.example -j: build the examples.cpp")
message(NOTICE "make install: copy the static library...\n... and the examples executable to: ...\n... ${CMAKE_INSTALL_PREFIX}\n")

file(WRITE ${CMAKE_INSTALL_PREFIX}/INC_DIRS "${INC_DIRS}")
file(WRITE ${CMAKE_INSTALL_PREFIX}/LIB_DIRS "${LIB_DIRS}")
file(WRITE ${CMAKE_INSTALL_PREFIX}/LIBS ${LIBS} ${CMAKE_DL_LIBS})