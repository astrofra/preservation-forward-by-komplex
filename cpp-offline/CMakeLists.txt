cmake_minimum_required(VERSION 3.15)

if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

project(forward_offline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_executable(forward-export
    src/main.cpp
    src/app/export_config.cpp
    src/app/capture_model.cpp
    src/app/maku_capture.cpp
    src/app/forward_app.cpp
    src/app/intro_script.cpp
    src/app/manifest_writer.cpp
    src/app/saari_capture.cpp
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
    src/render/png_writer.cpp
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

find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(COMMAND "${GIT_EXECUTABLE}" describe --always --dirty
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE FORWARD_SOURCE_REVISION OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)
    if(FORWARD_SOURCE_REVISION)
        target_compile_definitions(forward-export PRIVATE FORWARD_SOURCE_REVISION="${FORWARD_SOURCE_REVISION}")
    endif()
endif()

if(MSVC)
    target_compile_options(forward-export PRIVATE /W4 /EHsc)
else()
    target_compile_options(forward-export PRIVATE -Wall -Wextra -pedantic)
endif()

include(CTest)
if(BUILD_TESTING)
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_Interpreter_FOUND)
        add_test(NAME saari-gsplat-export
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_saari_capture.py"
                    "$<TARGET_FILE:forward-export>" "${CMAKE_CURRENT_SOURCE_DIR}/..")
        add_test(NAME maku-gsplat-export
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_maku_capture.py"
                    "$<TARGET_FILE:forward-export>" "${CMAKE_CURRENT_SOURCE_DIR}/..")
    endif()
endif()
