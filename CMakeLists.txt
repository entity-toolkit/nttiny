cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME nttiny)
set(EXECUTABLE ${PROJECT_NAME})

add_executable(${EXECUTABLE})

# main source file
set(SRC_FILES "examples.cpp")

project(${PROJECT_NAME} LANGUAGES CXX)
option(USE_CXX17_STD "Nttiny should be build with C++17 compiler option enabled" ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -Ofast -DNDEBUG -fPIE")

set(EXTERN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern)

# -------------------------------------------------------------------------- #
#                                 main source                                #
# -------------------------------------------------------------------------- #
add_subdirectory(src)
set(INC_DIRS ${INC_DIRS} ${PROJECT_SOURCE_DIR}/src)
set(INC_DIRS ${INC_DIRS} ${PROJECT_SOURCE_DIR}/src/plotters)
set(INC_DIRS ${INC_DIRS} ${PROJECT_SOURCE_DIR}/src/assets)
set(INC_DIRS ${INC_DIRS} ${PROJECT_SOURCE_DIR}/src/ui)
set(INC_DIRS ${INC_DIRS} ${PROJECT_SOURCE_DIR}/src/renderers)

message(STATUS "src files = ${SRC_FILES}")

target_compile_features(${EXECUTABLE} PRIVATE cxx_std_17)
target_sources(${EXECUTABLE} PRIVATE ${SRC_FILES})

target_include_directories(${EXECUTABLE} PRIVATE ${INC_DIRS})
target_link_libraries(${EXECUTABLE} PRIVATE ${LIBS})