project(enes)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cpu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/op_code.c
)

# set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dep/include)
# set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dep/lib)
# set(LIB_NAME )

add_executable(${PROJECT_NAME} ${SOURCES})

# target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
# target_link_directories(${PROJECT_NAME} PRIVATE ${LIB_DIR})
# target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB_NAME})
