#include <SDL.h>
#include <memory>
#include "app/demo_runtime.h"
#include "host/sdl_view.h"

using namespace forward_player;
namespace {
std::string last_error;
ScoreAudio score;
std::unique_ptr<DemoRuntime> demo;
std::unique_ptr<SdlView> view;
}

extern "C" {
const char* forward_error() { return last_error.c_str(); }
int forward_total_samples() { return static_cast<int>(score.total_samples()); }
int forward_credits_sample() { return static_cast<int>(score.credits_sample()); }
int forward_scene_init(int index) { return demo && demo->initialize_scene(index, &last_error); }
int forward_prepare() {
    if (!score.prepare_block(16384, &last_error)) return -1;
    return static_cast<int>(score.block().interleaved_samples.size() / 2);
}
const std::int16_t* forward_audio_block() { return score.block().interleaved_samples.data(); }
int forward_block_start() { return static_cast<int>(score.block_start()); }
int forward_prepared_samples() { return static_cast<int>(score.prepared_samples()); }
int forward_reset() { return demo && demo->reset(&last_error); }
int forward_advance(int sample, int ticks, int present) {
    if (!demo || sample < 0 || ticks < 1 || ticks > 64) return -1;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {}
    const unsigned advanced = demo->advance_to_sample(static_cast<std::uint64_t>(sample), ticks);
    if (present && (advanced || sample == 0) && !view->present(demo->framebuffer(), &last_error)) return -1;
    return demo->caught_up(sample) ? 1 : 0;
}
int forward_rendered_sample() { return demo ? static_cast<int>(demo->rendered_sample()) : 0; }
const char* forward_scene() { return demo ? demo->scene_name().c_str() : ""; }
const std::uint32_t* forward_pixels() { return demo ? demo->framebuffer().pixels().data() : NULL; }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) { last_error = SDL_GetError(); return 1; }
    view.reset(new SdlView());
    if (!view->open(&last_error) || !score.initialize(&last_error)) return 1;
    demo.reset(new DemoRuntime(score));
    return 0;
}
