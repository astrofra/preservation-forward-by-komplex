#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "app/demo_runtime.h"
#include "host/sdl_view.h"

using namespace forward_player;

int play(int argc, char** argv) {
    double smoke_seconds = 0;
    for (int i = 1; i < argc; ++i) {
        const std::string arg(argv[i]);
        if (arg == "--help") {
            std::cout << "Run from the repository root. Space: pause/resume, R: restart, F: fullscreen, Esc: exit.\n"
                         "--smoke-seconds N exits after N seconds of playback.\n";
            return 0;
        }
        if (arg != "--smoke-seconds" || i + 1 == argc) { std::cerr << "Unknown argument: " << arg << '\n'; return 1; }
        char* end = NULL;
        smoke_seconds = std::strtod(argv[++i], &end);
        if (!end || *end || !(smoke_seconds > 0)) return 1;
    }
    std::string error;
    SdlView view;
    if (!view.open(&error)) { std::cerr << error << '\n'; return 1; }
    ScoreAudio score;
    if (!score.initialize(&error)) { std::cerr << error << '\n'; return 1; }
    DemoRuntime demo(score);
    std::vector<std::int16_t> pcm;
    pcm.reserve(static_cast<std::size_t>(score.total_samples()) * 2);
    unsigned scene = 0;
    while (scene < 8 || !score.ready()) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) if (event.type == SDL_QUIT) return 0;
        if (scene < 8) {
            if (!demo.initialize_scene(scene++, &error)) { std::cerr << error << '\n'; return 1; }
        } else {
            if (!score.prepare_block(16384, &error)) { std::cerr << error << '\n'; return 1; }
            const std::vector<std::int16_t>& block = score.block().interleaved_samples;
            pcm.insert(pcm.end(), block.begin(), block.end());
        }
        const std::string title = "Forward - preparing " + std::to_string(100 * score.prepared_samples() / score.total_samples()) + "%";
        SDL_SetWindowTitle(view.window(), title.c_str());
    }
    SDL_AudioSpec desired = {}, obtained = {};
    desired.freq = kSampleRate; desired.channels = 2; desired.format = AUDIO_S16SYS; desired.samples = 512;
    const SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);
    if (!device) { std::cerr << SDL_GetError() << '\n'; return 1; }
    const Uint32 bytes = static_cast<Uint32>(pcm.size() * sizeof(pcm[0]));
    if (SDL_QueueAudio(device, pcm.data(), bytes) < 0) {
        std::cerr << SDL_GetError() << '\n'; SDL_CloseAudioDevice(device); return 1;
    }
    bool running = true, paused = false, fullscreen = false;
    int result = 0;
    demo.advance_to_sample(0);
    view.present(demo.framebuffer(), &error);
    SDL_PauseAudioDevice(device, 0);
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                paused = true; SDL_PauseAudioDevice(device, 1);
            }
            if (event.type != SDL_KEYDOWN || event.key.repeat) continue;
            if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
            if (event.key.keysym.sym == SDLK_SPACE) { paused = !paused; SDL_PauseAudioDevice(device, paused ? 1 : 0); }
            if (event.key.keysym.sym == SDLK_f) {
                fullscreen = !fullscreen;
                SDL_SetWindowFullscreen(view.window(), fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
            }
            if (event.key.keysym.sym == SDLK_r) {
                SDL_PauseAudioDevice(device, 1); SDL_ClearQueuedAudio(device);
                if (!demo.reset(&error) || SDL_QueueAudio(device, pcm.data(), bytes) < 0) {
                    if (error.empty()) error = SDL_GetError();
                    result = 1; running = false; break;
                }
                demo.advance_to_sample(0);
                paused = false; SDL_PauseAudioDevice(device, 0);
            }
        }
        const std::uint64_t queued = SDL_GetQueuedAudioSize(device) / 4;
        const std::uint64_t submitted = score.total_samples() - std::min(queued, score.total_samples());
        // SDL cannot expose the exact audible cursor. Compensate one backend period.
        const std::uint64_t sample = submitted > obtained.samples ? submitted - obtained.samples : 0;
        if (!paused) {
            demo.advance_to_sample(sample, 8);
            if (!demo.caught_up(sample)) { paused = true; SDL_PauseAudioDevice(device, 1); }
        } else if (!demo.caught_up(sample)) demo.advance_to_sample(sample, 8);
        if (!view.present(demo.framebuffer(), &error)) { result = 1; break; }
        const std::string title = "Forward / " + demo.scene_name() + (paused ? " - paused (Space resumes)" : " - 512x256");
        SDL_SetWindowTitle(view.window(), title.c_str());
        if (!paused && (queued == 0 || (smoke_seconds > 0 && sample >= smoke_seconds * kSampleRate))) running = false;
        SDL_Delay(2);
    }
    SDL_CloseAudioDevice(device);
    if (result) std::cerr << error << '\n';
    return result;
}

int main(int argc, char** argv) {
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) { std::cerr << SDL_GetError() << '\n'; return 1; }
    const int result = play(argc, argv);
    SDL_Quit();
    return result;
}
