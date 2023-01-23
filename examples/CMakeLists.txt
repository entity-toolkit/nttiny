file(GLOB EXAMPLES *.cpp)

foreach(EXAMPLE ${EXAMPLES})
  get_filename_component(EXAMPLE_NAME ${EXAMPLE} NAME_WE)
  add_executable(${EXAMPLE_NAME})
  target_sources(${EXAMPLE_NAME} PRIVATE ${EXAMPLE})
  target_link_libraries(${EXAMPLE_NAME} PRIVATE ${PROJECT_NAME})
endforeach()