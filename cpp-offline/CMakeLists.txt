cmake_minimum_required(VERSION 3.15)

project(forward_offline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(forward-export
    src/main.cpp
    src/app/export_config.cpp
    src/app/forward_app.cpp
    src/app/manifest_writer.cpp
    src/audio/wav_writer.cpp
    src/core/offline_timeline.cpp
    src/platform/file_utils.cpp
    src/render/rgb_surface.cpp
    src/render/tga_writer.cpp
    src/scenes/placeholder_scene.cpp
)

target_include_directories(forward-export PRIVATE src)

if(MSVC)
    target_compile_options(forward-export PRIVATE /W4 /EHsc)
else()
    target_compile_options(forward-export PRIVATE -Wall -Wextra -pedantic)
endif()
