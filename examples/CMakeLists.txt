set(EXAMPLES ${PROJECT_NAME}.example)

add_executable(${EXAMPLES})
target_sources(${EXAMPLES}
               PRIVATE 
               examples.cpp
)
target_link_libraries(${EXAMPLES} PRIVATE ${PROJECT_NAME})
