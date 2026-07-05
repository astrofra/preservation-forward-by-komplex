cmake_minimum_required(VERSION 3.15)

project(forward_offline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(forward-export
    src/main.cpp
    src/app/export_config.cpp
    src/app/forward_app.cpp
    src/app/intro_script.cpp
    src/app/manifest_writer.cpp
    src/app/song_position_transport.cpp
    src/assets/normalized_asset_loader.cpp
    src/assets/original_asset_loader.cpp
    src/assets/stb_image_impl.cpp
    src/audio/module_player.cpp
    src/audio/wav_writer.cpp
    src/core/java_random.cpp
    src/core/offline_timeline.cpp
    src/platform/file_utils.cpp
    src/render/indexed_surface.cpp
    src/render/rgb_surface.cpp
    src/render/tga_writer.cpp
    src/scenes/domina_routine.cpp
    src/scenes/feta_scene.cpp
    src/scenes/kukot_scene.cpp
    src/scenes/maku_scene.cpp
    src/scenes/mute95_scene.cpp
    src/scenes/placeholder_scene.cpp
    src/scenes/saari_scene.cpp
    src/scenes/scene3d_shared.cpp
    src/scenes/uppol_routine.cpp
    src/scenes/watercube_scene.cpp
)

target_include_directories(forward-export PRIVATE src third_party)

if(MSVC)
    target_compile_options(forward-export PRIVATE /W4 /EHsc)
else()
    target_compile_options(forward-export PRIVATE -Wall -Wextra -pedantic)
endif()
