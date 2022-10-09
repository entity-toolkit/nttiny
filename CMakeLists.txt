cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME nttiny)

project(${PROJECT_NAME} 
        VERSION 0.6.4
        DESCRIPTION "OpenGL-based runtime scientific visualization library"
        LANGUAGES CXX
)

add_compile_options(-W -Wall)
add_compile_options(-Ofast -DNDEBUG -fPIE)
option(USE_CXX17_STD "Nttiny should be built with C++17 compiler option enabled" ON)
set(CMAKE_CXX_STANDARD 17)

# set default install prefix
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/bin CACHE PATH "install directory" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

message(STATUS "Install path: ${CMAKE_INSTALL_PREFIX}")

add_subdirectory(${PROJECT_NAME})
add_subdirectory(examples)

