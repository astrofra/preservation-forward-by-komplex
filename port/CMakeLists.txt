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

find_path(XMP_INCLUDE_DIR NAMES xmp.h)
find_library(XMP_LIBRARY NAMES xmp libxmp)
set(XMP_LINK_TARGET "")
if(XMP_INCLUDE_DIR AND XMP_LIBRARY)
  add_library(XMP::xmp UNKNOWN IMPORTED)
  set_target_properties(XMP::xmp PROPERTIES
    IMPORTED_LOCATION "${XMP_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${XMP_INCLUDE_DIR}"
  )
  set(XMP_LINK_TARGET XMP::xmp)
else()
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(LIBXMP REQUIRED IMPORTED_TARGET libxmp)
  set(XMP_LINK_TARGET PkgConfig::LIBXMP)
endif()

add_executable(forward_native
  src/main.cpp
  src/core/Image32.cpp
  src/core/LegacyPacked10.cpp
  src/core/Mesh.cpp
  src/core/MeshLoaderIgu.cpp
  src/core/Renderer3D.cpp
  src/core/Surface32.cpp
  src/core/Timeline.cpp
  src/core/XmPlayer.cpp
)

target_include_directories(forward_native PRIVATE src)

if(TARGET SDL2::SDL2main)
  target_link_libraries(forward_native PRIVATE SDL2::SDL2 SDL2::SDL2main ${XMP_LINK_TARGET})
else()
  target_link_libraries(forward_native PRIVATE SDL2::SDL2 ${XMP_LINK_TARGET})
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(forward_native PRIVATE -Wall -Wextra -Wpedantic)
endif()
