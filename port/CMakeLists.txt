cmake_minimum_required(VERSION 3.20)
project(forward_native_harness LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(FORWARD_ENABLE_XM_AUDIO "Enable libxmp-backed XM playback" ON)
set(FORWARD_USE_PKGCONFIG_DEFAULT ON)
if(WIN32)
  set(FORWARD_USE_PKGCONFIG_DEFAULT OFF)
endif()
option(FORWARD_USE_PKGCONFIG
       "Allow pkg-config fallback for dependency lookup"
       ${FORWARD_USE_PKGCONFIG_DEFAULT})
set(FORWARD_FETCH_SDL2_DEFAULT OFF)
if(WIN32)
  set(FORWARD_FETCH_SDL2_DEFAULT ON)
endif()
option(FORWARD_FETCH_SDL2
       "Fetch SDL2 from source if no system package is found"
       ${FORWARD_FETCH_SDL2_DEFAULT})

find_package(SDL2 QUIET CONFIG)
if(NOT TARGET SDL2::SDL2 AND NOT SDL2_FOUND)
  find_package(SDL2 QUIET MODULE)
endif()

if(NOT TARGET SDL2::SDL2)
  if(TARGET SDL2::SDL2-static)
    add_library(SDL2::SDL2 ALIAS SDL2::SDL2-static)
  elseif(TARGET SDL2)
    add_library(SDL2::SDL2 ALIAS SDL2)
  elseif(TARGET SDL2-static)
    add_library(SDL2::SDL2 ALIAS SDL2-static)
  endif()
endif()

if(NOT TARGET SDL2::SDL2)
  if(FORWARD_USE_PKGCONFIG)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
    add_library(SDL2::SDL2 ALIAS PkgConfig::SDL2)
  endif()
endif()

if(NOT TARGET SDL2::SDL2 AND FORWARD_FETCH_SDL2)
  include(FetchContent)
  set(SDL_SHARED OFF CACHE BOOL "" FORCE)
  set(SDL_STATIC ON CACHE BOOL "" FORCE)
  set(SDL_TEST OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(
    sdl2_external
    URL https://github.com/libsdl-org/SDL/releases/download/release-2.32.10/SDL2-2.32.10.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  FetchContent_MakeAvailable(sdl2_external)

  if(NOT TARGET SDL2::SDL2)
    if(TARGET SDL2::SDL2-static)
      add_library(SDL2::SDL2 ALIAS SDL2::SDL2-static)
    elseif(TARGET SDL2)
      add_library(SDL2::SDL2 ALIAS SDL2)
    elseif(TARGET SDL2-static)
      add_library(SDL2::SDL2 ALIAS SDL2-static)
    endif()
  endif()
endif()

if(NOT TARGET SDL2::SDL2)
  message(FATAL_ERROR
    "SDL2 was not found. Install SDL2 (for example via vcpkg) or configure with "
    "-DFORWARD_FETCH_SDL2=ON.")
endif()

set(XMP_LINK_TARGET "")
set(FORWARD_HAS_LIBXMP FALSE)

if(FORWARD_ENABLE_XM_AUDIO)
  find_package(libxmp QUIET CONFIG)
  find_package(xmp QUIET CONFIG)

  foreach(candidate_target IN ITEMS xmp::xmp libxmp::xmp XMP::xmp unofficial::libxmp::xmp)
    if(TARGET ${candidate_target})
      set(XMP_LINK_TARGET ${candidate_target})
      set(FORWARD_HAS_LIBXMP TRUE)
      break()
    endif()
  endforeach()

  if(NOT FORWARD_HAS_LIBXMP)
    find_path(XMP_INCLUDE_DIR NAMES xmp.h)
    find_library(XMP_LIBRARY NAMES xmp libxmp)
    if(XMP_INCLUDE_DIR AND XMP_LIBRARY)
      add_library(XMP::xmp UNKNOWN IMPORTED)
      set_target_properties(XMP::xmp PROPERTIES
        IMPORTED_LOCATION "${XMP_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${XMP_INCLUDE_DIR}"
      )
      set(XMP_LINK_TARGET XMP::xmp)
      set(FORWARD_HAS_LIBXMP TRUE)
    endif()
  endif()

  if(NOT FORWARD_HAS_LIBXMP AND FORWARD_USE_PKGCONFIG)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBXMP QUIET IMPORTED_TARGET libxmp)
    if(TARGET PkgConfig::LIBXMP)
      set(XMP_LINK_TARGET PkgConfig::LIBXMP)
      set(FORWARD_HAS_LIBXMP TRUE)
    endif()
  endif()
endif()

if(NOT FORWARD_HAS_LIBXMP)
  message(STATUS "libxmp not found: XM music playback will be disabled at runtime.")
endif()

add_executable(forward_native
  src/main.cpp
  src/core/Image32.cpp
  src/core/GifIndexed.cpp
  src/core/IndexedSurface8.cpp
  src/core/LegacyPacked10.cpp
  src/core/Mesh.cpp
  src/core/MeshLoaderIgu.cpp
  src/core/Renderer3D.cpp
  src/core/Surface32.cpp
  src/core/Timeline.cpp
  src/core/XmPlayer.cpp
)

target_include_directories(forward_native PRIVATE src)

target_link_libraries(forward_native PRIVATE SDL2::SDL2)
if(FORWARD_HAS_LIBXMP)
  target_link_libraries(forward_native PRIVATE ${XMP_LINK_TARGET})
endif()

target_compile_definitions(forward_native PRIVATE SDL_MAIN_HANDLED)
if(FORWARD_HAS_LIBXMP)
  target_compile_definitions(forward_native PRIVATE FORWARD_HAS_LIBXMP=1)
else()
  target_compile_definitions(forward_native PRIVATE FORWARD_HAS_LIBXMP=0)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(forward_native PRIVATE -Wall -Wextra -Wpedantic)
endif()
