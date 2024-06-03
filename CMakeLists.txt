
# SPDX-License-Identidier: MIT

cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

project(pax-sdl)
set(target pax-sdl)
add_compile_options(-ggdb -O2)

find_package(SDL2 REQUIRED)
add_subdirectory(pax-graphics)

add_executable(${target}
    src/main.c
)

include_directories(${target} ${SDL2_INCLUDE_DIRS})
target_link_libraries(${target} ${SDL2_LIBRARIES})
target_link_libraries(${target} pax_graphics)
