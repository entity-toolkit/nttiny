set(EXAMPLES ex_cart_flds ex_cart_prtls_dprec ex_logR_prtls ex_sph)

foreach(EXAMPLE ${EXAMPLES})
  add_executable(${EXAMPLE})
  target_sources(${EXAMPLE} PRIVATE ${EXAMPLE}.cpp)
  target_link_libraries(${EXAMPLE} PRIVATE ${PROJECT_NAME})
endforeach()