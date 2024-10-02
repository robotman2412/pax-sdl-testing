
# SPDX-License-Identidier: MIT

cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

project(pax-sdl)
set(target pax-sdl)

find_package(SDL2 REQUIRED)
set(PAX_STATIC true)
add_compile_options(-ggdb -O2)
add_subdirectory(pax-graphics)

add_executable(${target}
    src/main.c
)

include_directories(${target} ${SDL2_INCLUDE_DIRS})
target_link_libraries(${target} PUBLIC ${SDL2_LIBRARIES})
if(DEFINED PAX_VERSION_MAJOR)
    target_link_libraries(${target} PUBLIC pax_gfx pax_gui)
else()
    target_link_libraries(${target} PUBLIC pax_graphics)
endif()
