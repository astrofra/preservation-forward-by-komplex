cmake_minimum_required(VERSION 3.20)
project(forward_native_harness LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 QUIET CONFIG)
if(NOT SDL2_FOUND)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
  add_library(SDL2::SDL2 ALIAS PkgConfig::SDL2)
endif()

add_executable(forward_native
  src/main.cpp
)

if(TARGET SDL2::SDL2main)
  target_link_libraries(forward_native PRIVATE SDL2::SDL2 SDL2::SDL2main)
else()
  target_link_libraries(forward_native PRIVATE SDL2::SDL2)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(forward_native PRIVATE -Wall -Wextra -Wpedantic)
endif()
