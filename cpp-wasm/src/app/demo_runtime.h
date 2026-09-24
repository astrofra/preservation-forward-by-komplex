#ifndef FORWARD_PLAYER_DEMO_RUNTIME_H
#define FORWARD_PLAYER_DEMO_RUNTIME_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "app/intro_script.h"
#include "audio/module_player.h"
#include "render/rgb_surface.h"

namespace forward_player {

enum { kWidth = 512, kHeight = 256, kSampleRate = 22050, kTickSamples = 441 };

struct Action {
    std::uint64_t sample;
    forward_offline::ScriptCommand command;
};

// Owns only one PCM block. Hosts copy each block into their playback storage.
class ScoreAudio {
public:
    ScoreAudio();
    bool initialize(std::string* error);
    bool prepare_block(std::size_t frames, std::string* error);
    bool ready() const { return prepared_ == total_; }
    std::uint64_t total_samples() const { return total_; }
    std::uint64_t prepared_samples() const { return prepared_; }
    std::uint64_t module_switch_sample() const { return intro_end_; }
    std::uint64_t credits_sample() const { return credits_start_; }
    std::uint64_t block_start() const { return block_start_; }
    const forward_offline::SequenceAudioRender& block() const { return block_; }
    const std::vector<Action>& actions() const { return actions_; }
private:
    bool build_actions(std::string* error);
    bool append_commands(const std::vector<forward_offline::ScriptCommand>& commands,
                         unsigned phase, std::string* error);
    forward_offline::ModuleAudioStream stream_;
    forward_offline::SequenceAudioRender block_;
    std::vector<forward_offline::SongPositionEvent> rows_[2];
    std::vector<Action> actions_;
    std::uint64_t intro_end_, credits_start_, total_, prepared_, block_start_;
    unsigned phase_;
};

class DemoRuntime {
public:
    explicit DemoRuntime(const ScoreAudio& audio);
    ~DemoRuntime();
    // Call indexes 0..7 once, yielding between assets when hosted in a browser.
    bool initialize_scene(unsigned index, std::string* error);
    bool reset(std::string* error);
    // Never skips a stateful visual tick. Hosts bound catch-up work per callback.
    unsigned advance_to_sample(std::uint64_t sample, unsigned max_ticks = 8);
    bool caught_up(std::uint64_t sample) const;
    const forward_offline::RgbSurface& framebuffer() const { return frame_; }
    const std::string& scene_name() const { return active_; }
    std::uint64_t next_tick() const { return next_tick_; }
    std::uint64_t rendered_sample() const { return next_tick_ ? (next_tick_ - 1) * kTickSamples : 0; }
    std::size_t executed_actions() const { return next_action_; }
private:
    void execute(const Action& action);
    void render_tick(std::uint64_t sample);
    struct Scenes;
    std::unique_ptr<Scenes> scenes_;
    const ScoreAudio& audio_;
    forward_offline::RgbSurface frame_;
    std::string active_;
    std::uint64_t active_start_, next_tick_;
    std::size_t next_action_;
    unsigned initialized_;
};
}
#endif
