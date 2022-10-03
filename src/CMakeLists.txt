# ------------------------------------------ #
# external libraries                         #
# ------------------------------------------ #
set(INC_DIRS ${INC_DIRS} ${EXTERN_DIR})

# # glad
set(SRC_FILES ${SRC_FILES} ${EXTERN_DIR}/glad/glad.cpp)
set(INC_DIRS ${INC_DIRS} ${EXTERN_DIR}/glad)

# # KHR
set(INC_DIRS ${INC_DIRS} ${EXTERN_DIR}/KHR)

# # GLFW
find_package(glfw3 REQUIRED)
set(INC_DIRS ${INC_DIRS} ${GLFW_INCLUDE_DIR})
set(LIBS ${LIBS} ${GLFW_LIBRARIES})

# # ImGui
file(GLOB SRC_FILES ${SRC_FILES} ${EXTERN_DIR}/imgui/*.cpp)
set(SRC_FILES ${SRC_FILES} ${EXTERN_DIR}/imgui/backends/imgui_impl_opengl3.cpp)
set(SRC_FILES ${SRC_FILES} ${EXTERN_DIR}/imgui/backends/imgui_impl_glfw.cpp)
set(INC_DIRS ${INC_DIRS} ${EXTERN_DIR}/imgui)
set(INC_DIRS ${INC_DIRS} ${EXTERN_DIR}/imgui/backends)

# # ImPlot
file(GLOB SRC_FILES ${SRC_FILES} ${EXTERN_DIR}/implot/*.cpp)
set(INC_DIRS ${INC_DIRS} ${EXTERN_DIR}/implot)

# # plog
set(INC_DIRS ${INC_DIRS} ${EXTERN_DIR}/plog/include)

# # Filesystem
set(INC_DIRS ${INC_DIRS} ${EXTERN_DIR}/filesystem/include)
set(INC_DIRS ${INC_DIRS} ${EXTERN_DIR}/filesystem/include/ghc)

# ------------------------------------------ #
# main source                                #
# ------------------------------------------ #
set(INC_DIRS ${INC_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(INC_DIRS ${INC_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/plotters)
set(INC_DIRS ${INC_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/renderers)
set(INC_DIRS ${INC_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/ui)
file(GLOB_RECURSE SRC_FILES ${SRC_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

target_sources(${EXECUTABLE} PRIVATE ${SRC_FILES})
target_include_directories(${EXECUTABLE} PRIVATE ${INC_DIRS})
target_link_libraries(${EXECUTABLE} PRIVATE ${LIBS} ${CMAKE_DL_LIBS})

# add_subdirectory(assets)
# add_subdirectory(plotters)
# add_subdirectory(renderers)
# add_subdirectory(ui)

# if(glfw3_FOUND)
# find_path(GLFW3_INCLUDE_DIR "GLFW/glfw3.h")
# find_library(GLFW3_LIBRARY "libglfw3.a")

# if(GLFW3_INCLUDE_DIR)
# message(STATUS "Looking for GLFW3 headers: ${GLFW3_INCLUDE_DIR} - found")
# else()
# message(STATUS "Looking for GLFW3 headers - not found")
# set(BUILD_GLFW3 ON)
# endif()

# if(GLFW3_LIBRARY)
# message(STATUS "Found GLFW3: ${GLFW3_LIBRARY}")
# else()
# message(STATUS "GLFW3 static library - not found")
# set(BUILD_GLFW3 ON)
# endif()
# else(glfw3_FOUND)
# message(STATUS "GLFW3 package - not found")
# set(BUILD_GLFW3 ON)
# endif(glfw3_FOUND)
# endif(NOT BUILD_GLFW3)

# Build GLFW3 locally into the binary directory
# if(BUILD_GLFW3)
# message(STATUS "GLFW3 will be downloaded and built")
# set(GLFW3_PREFIX ${CMAKE_BINARY_DIR}/glfw3)
# set(GLFW3_INCLUDE_DIR ${GLFW3_PREFIX}/include)
# set(GLFW3_LIBRARY ${GLFW3_PREFIX}/lib/libglfw3.a)

# # Build settings
# set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
# set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
# set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")

# # Download and build
# ExternalProject_Add(glfw3
# URL "https://github.com/glfw/glfw/releases/download/3.2.1/glfw-3.2.1.zip"
# PREFIX ${GLFW3_PREFIX}
# CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${GLFW3_PREFIX}"
# UPDATE_COMMAND "")
# endif(BUILD_GLFW3)

# set(GLFW3_INCLUDE_DIRS ${GLFW3_INCLUDE_DIR} CACHE STRING "GLFW3 Include directories")
# set(GLFW3_LIBRARIES ${GLFW3_LIBRARY}
# X11
# Xrandr
# Xinerama
# Xxf86vm
# Xcursor
# CACHE STRING "GLFW3 Libraries"
# )
