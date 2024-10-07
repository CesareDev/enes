project(enes)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cpu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/op_code.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bus.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cartridge.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ppu/ppu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ppu/addr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ppu/control.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ppu/mask.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ppu/scroll.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ppu/status.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/render/frame.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/render/palette.c
)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dep/include)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dep/lib)
set(LIB_NAME raylib m)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PRIVATE ${LIB_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB_NAME})
