cmake_minimum_required(VERSION 3.20)
project(forward_player LANGUAGES C CXX)
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
option(FORWARD_BUILD_NATIVE "Build the native SDL2 player" ON)
option(FORWARD_BUILD_TESTS "Build deterministic runtime checks" ON)

add_library(forward-core STATIC
    src/app/demo_runtime.cpp src/app/intro_script.cpp
    src/assets/normalized_asset_loader.cpp src/assets/original_asset_loader.cpp
    src/assets/stb_image_impl.cpp src/audio/module_player.cpp src/core/java_random.cpp
    src/platform/file_utils.cpp src/render/indexed_surface.cpp src/render/rgb_surface.cpp
    src/scenes/domina_routine.cpp src/scenes/feta_scene.cpp src/scenes/kukot_scene.cpp
    src/scenes/maku_scene.cpp src/scenes/mute95_scene.cpp src/scenes/saari_scene.cpp
    src/scenes/scene3d_shared.cpp src/scenes/uppol_routine.cpp src/scenes/watercube_scene.cpp)
target_include_directories(forward-core PUBLIC src PRIVATE third_party)
if(MSVC)
    target_compile_options(forward-core PRIVATE /W4)
    target_compile_definitions(forward-core PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(forward-core PRIVATE -Wall -Wextra)
endif()

if(EMSCRIPTEN)
    add_executable(forward-web src/host/web_main.cpp src/host/sdl_view.cpp)
    target_link_libraries(forward-web PRIVATE forward-core)
    target_compile_options(forward-web PRIVATE "-sUSE_SDL=2")
    target_link_options(forward-web PRIVATE
        "-sUSE_SDL=2" "-sALLOW_MEMORY_GROWTH=1" "-sINITIAL_MEMORY=67108864"
        "-sMAXIMUM_MEMORY=268435456" "-sSTACK_SIZE=2097152" "-sNO_EXIT_RUNTIME=1"
        "-sMODULARIZE=1" "-sEXPORT_NAME=createForward" "-sENVIRONMENT=web"
        "-sEXPORTED_RUNTIME_METHODS=['UTF8ToString','HEAP16','HEAPU32']"
        "-sEXPORTED_FUNCTIONS=['_main','_forward_error','_forward_total_samples','_forward_credits_sample','_forward_scene_init','_forward_prepare','_forward_audio_block','_forward_block_start','_forward_prepared_samples','_forward_reset','_forward_advance','_forward_rendered_sample','_forward_scene','_forward_pixels']")
    set_target_properties(forward-web PROPERTIES OUTPUT_NAME forward SUFFIX .js
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/web")
    # Stage only runtime files; keep historical executables and classes out of the bundle.
    set(FORWARD_ASSET_STAGE "${CMAKE_BINARY_DIR}/assets/original/forward")
    file(MAKE_DIRECTORY "${FORWARD_ASSET_STAGE}")
    foreach(asset_dir images asses meshes mods)
        file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../original/forward/${asset_dir}"
             DESTINATION "${FORWARD_ASSET_STAGE}" PATTERN "*.exe" EXCLUDE)
    endforeach()
    target_link_options(forward-web PRIVATE "SHELL:--preload-file \"${FORWARD_ASSET_STAGE}@/original/forward\"")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/web/art")
    foreach(shell_file index.html player.css player.js)
        configure_file("web/${shell_file}" "${CMAKE_BINARY_DIR}/web/${shell_file}" COPYONLY)
    endforeach()
    foreach(art_file back.gif aijja.gif)
        configure_file("../original/forward/images/${art_file}" "${CMAKE_BINARY_DIR}/web/art/${art_file}" COPYONLY)
    endforeach()
elseif(FORWARD_BUILD_NATIVE)
    find_package(SDL2 2.0.18 QUIET CONFIG)
    if(NOT SDL2_FOUND)
        include(FetchContent)
        set(SDL_SHARED OFF CACHE BOOL "" FORCE)
        set(SDL_STATIC ON CACHE BOOL "" FORCE)
        set(SDL_TEST OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(SDL2
            URL https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.32.10.tar.gz
            URL_HASH SHA256=03f9d7c191a837525c9cda6406af2f2e48be02b5e7eb03d949cc9f1e9ca41c8b)
        FetchContent_MakeAvailable(SDL2)
    endif()
    add_executable(forward-player src/host/native_main.cpp src/host/sdl_view.cpp)
    target_link_libraries(forward-player PRIVATE forward-core SDL2::SDL2)
endif()

if(FORWARD_BUILD_TESTS AND NOT EMSCRIPTEN)
    enable_testing()
    add_executable(forward-check tests/runtime_check.cpp src/render/tga_writer.cpp src/audio/wav_writer.cpp)
    target_link_libraries(forward-check PRIVATE forward-core)
    add_test(NAME forward-runtime COMMAND forward-check)
    set_tests_properties(forward-runtime PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/..")
endif()
