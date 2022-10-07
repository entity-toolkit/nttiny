set(STATIC_LIB ${PROJECT_NAME})
set(EXTERN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../extern)
add_library(${STATIC_LIB} STATIC)

# ------------------------------------------ #
# external libraries                         #
# ------------------------------------------ #
list(APPEND INC_DIRS ${EXTERN_DIR})

# # glad
list(APPEND SRC_FILES ${EXTERN_DIR}/glad/glad.cpp)
list(APPEND INC_DIRS ${EXTERN_DIR}/glad)

# # KHR
list(APPEND INC_DIRS ${EXTERN_DIR}/KHR)

# # GLFW
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
if (GLFW_FOUND)
  message(STATUS "Found installed GLFW3")
  list(APPEND INC_DIRS ${GLFW_INCLUDEDIR})
  list(APPEND LIBS ${GLFW_LIBRARIES})
  list(APPEND LIB_DIRS ${GLFW_LIBRARY_DIRS})
else()
  message(STATUS "GLFW3 not found, will attempt to build from source")
  add_subdirectory(${EXTERN_DIR}/glfw glfw_build)
  list(APPEND INC_DIRS ${EXTERN_DIR}/glfw/include)
  list(APPEND LIBS ${GLFW_LIBRARIES})
endif()

# # ImGui
file(GLOB SRC_FILES ${SRC_FILES} ${EXTERN_DIR}/imgui/*.cpp)
list(APPEND SRC_FILES ${EXTERN_DIR}/imgui/backends/imgui_impl_opengl3.cpp)
list(APPEND SRC_FILES ${EXTERN_DIR}/imgui/backends/imgui_impl_glfw.cpp)
list(APPEND INC_DIRS ${EXTERN_DIR}/imgui)
list(APPEND INC_DIRS ${EXTERN_DIR}/imgui/backends)

# # ImPlot
file(GLOB SRC_FILES ${SRC_FILES} ${EXTERN_DIR}/implot/*.cpp)
list(APPEND INC_DIRS ${EXTERN_DIR}/implot)

# # plog
list(APPEND INC_DIRS ${EXTERN_DIR}/plog/include)

# # Filesystem
list(APPEND INC_DIRS ${EXTERN_DIR}/filesystem/include)
list(APPEND INC_DIRS ${EXTERN_DIR}/filesystem/include/ghc)

# ------------------------------------------ #
# main source                                #
# ------------------------------------------ #
list(APPEND INC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/)
list(APPEND INC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
list(APPEND INC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/plotters)
list(APPEND INC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/renderers)
list(APPEND INC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/ui)
file(GLOB_RECURSE SRC_FILES ${SRC_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

target_compile_features(${STATIC_LIB} PRIVATE cxx_std_17)
target_sources(${STATIC_LIB} PRIVATE ${SRC_FILES})
target_include_directories(${STATIC_LIB} PUBLIC ${INC_DIRS})
target_link_directories(${STATIC_LIB} PUBLIC ${LIB_DIRS})
target_link_libraries(${STATIC_LIB} PRIVATE ${LIBS} ${CMAKE_DL_LIBS})

file(WRITE ${CMAKE_INSTALL_PREFIX}/NTTINY_INC_DIRS "${INC_DIRS}")
file(WRITE ${CMAKE_INSTALL_PREFIX}/NTTINY_LIB_DIRS "${LIB_DIRS}")
file(WRITE ${CMAKE_INSTALL_PREFIX}/NTTINY_LIBS ${LIBS} ${CMAKE_DL_LIBS})

file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

install (TARGETS ${STATIC_LIB}
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION bin)

install(FILES ${HEADERS} DESTINATION include)
