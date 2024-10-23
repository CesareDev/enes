cmake_minimum_required(VERSION 3.29.2)

# export compile_commmands.json for neovim lsp
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(enes)

add_subdirectory(enes)

set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O2")
