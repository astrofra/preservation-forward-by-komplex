#include "app/demo_runtime.h"
#include <algorithm>
#include "scenes/mute95_scene.h"
#include "scenes/domina_routine.h"
#include "scenes/saari_scene.h"
#include "scenes/kukot_scene.h"
#include "scenes/maku_scene.h"
#include "scenes/watercube_scene.h"
#include "scenes/feta_scene.h"
#include "scenes/uppol_routine.h"

namespace forward_player {
using namespace forward_offline;

ScoreAudio::ScoreAudio()
    : intro_end_(0), credits_start_(0), total_(0), prepared_(0), block_start_(0), phase_(0) {}

bool ScoreAudio::initialize(std::string* error) {
    total_ = prepared_ = block_start_ = 0;
    phase_ = 0;
    rows_[0].clear(); rows_[1].clear(); actions_.clear();
    std::uint64_t credits = 0;
    if (!module_position_sample("intro", kSampleRate, 0x1024, &intro_end_, error) ||
        !module_position_sample("saari", kSampleRate, 0x1600, &credits, error)) return false;
    credits_start_ = intro_end_ + credits;
    // First edition retains the offline package's explicit 36-second credits tail.
    total_ = credits_start_ + 36ULL * kSampleRate;
    return stream_.open("intro", kSampleRate, error);
}

bool ScoreAudio::prepare_block(std::size_t frames, std::string* error) {
    if (!total_ || !frames) {
        if (error) *error = "audio preparation not initialized or empty block requested";
        return false;
    }
    if (ready()) {
        block_.interleaved_samples.clear(); block_.song_positions.clear();
        return true;
    }
    if (phase_ == 0 && prepared_ == intro_end_) {
        if (!stream_.open("saari", kSampleRate, error)) return false;
        phase_ = 1;
    }
    const std::uint64_t limit = phase_ == 0 ? intro_end_ : total_;
    const std::size_t count = static_cast<std::size_t>(std::min<std::uint64_t>(frames, limit - prepared_));
    block_start_ = prepared_;
    if (!stream_.render(count, &block_, error)) return false;
    rows_[phase_].insert(rows_[phase_].end(), block_.song_positions.begin(), block_.song_positions.end());
    prepared_ += count;
    return !ready() || build_actions(error);
}

bool ScoreAudio::append_commands(const std::vector<ScriptCommand>& commands,
                                unsigned phase, std::string* error) {
    std::size_t row = 0;
    for (std::size_t i = 0; i < commands.size(); ++i) {
        const ScriptCommand& command = commands[i];
        if (command.verb != "show" && command.verb != "msg") continue;
        // Preserve sequential wait semantics, including backward script markers.
        while (row < rows_[phase].size() && rows_[phase][row].song_position_hex < command.song_position_hex)
            ++row;
        if (row == rows_[phase].size()) {
            if (error) *error = "score command lies beyond prepared module rows";
            return false;
        }
        Action action = {rows_[phase][row].sample_index + (phase ? intro_end_ : 0), command};
        actions_.push_back(action);
    }
    return true;
}

bool ScoreAudio::build_actions(std::string* error) {
    actions_.clear();
    if (!append_commands(IntroScript().commands(), 0, error)) return false;
    const ScriptCommand shows[] = {
        {0x0000, "show", "saari", ""}, {0x0700, "show", "kukot", ""},
        {0x0d00, "show", "maku", ""}, {0x1000, "show", "watercube", ""}
    };
    const ScriptCommand shocks[] = {
        {0x0000, "msg", "saari", "suh0"}, {0x0100, "msg", "saari", "suh"},
        {0x0600, "msg", "saari", "suh"}, {0x0608, "msg", "saari", "suh"},
        {0x0610, "msg", "saari", "suh"}, {0x0618, "msg", "saari", "suh"},
        {0x0620, "msg", "saari", "suh"}, {0x0628, "msg", "saari", "suh"},
        {0x0630, "msg", "saari", "suh"}
    };
    if (!append_commands(std::vector<ScriptCommand>(shows, shows + 4), 1, error) ||
        !append_commands(std::vector<ScriptCommand>(shocks, shocks + 9), 1, error) ||
        !append_commands(KukotScript().commands(), 1, error) ||
        !append_commands(MakuScript().commands(), 1, error) ||
        !append_commands(WatercubeScript().commands(), 1, error) ||
        !append_commands(FetaScript().commands(), 1, error)) return false;
    std::stable_sort(actions_.begin(), actions_.end(), [](const Action& a, const Action& b) {
        return a.sample < b.sample;
    });
    return true;
}

struct DemoRuntime::Scenes {
    Mute95Scene mute95;
    DominaRoutine domina;
    SaariScene saari;
    KukotScene kukot;
    MakuScene maku;
    WatercubeScene watercube;
    FetaScene feta;
    UppolRoutine uppol;
};

DemoRuntime::DemoRuntime(const ScoreAudio& audio)
    : scenes_(new Scenes()), audio_(audio), frame_(kWidth, kHeight),
      active_start_(0), next_tick_(0), next_action_(0), initialized_(0) {}
DemoRuntime::~DemoRuntime() {}

template<class T> bool load_scene(T& scene, std::string* error) {
    scene.init();
    if (scene.is_ready()) return true;
    if (error) *error = scene.error_message();
    return false;
}

bool DemoRuntime::initialize_scene(unsigned index, std::string* error) {
    if (index != initialized_ || index >= 8) {
        if (error) *error = "scenes must initialize in order";
        return false;
    }
    bool ok = false;
    switch (index) {
    case 0: ok = load_scene(scenes_->mute95, error); break;
    case 1: ok = load_scene(scenes_->domina, error); break;
    case 2: ok = load_scene(scenes_->saari, error); break;
    case 3: ok = load_scene(scenes_->kukot, error); break;
    case 4: ok = load_scene(scenes_->maku, error); break;
    case 5: ok = load_scene(scenes_->watercube, error); break;
    case 6: ok = load_scene(scenes_->feta, error); break;
    case 7: ok = load_scene(scenes_->uppol, error); break;
    }
    if (ok) ++initialized_;
    return ok;
}

bool DemoRuntime::reset(std::string* error) {
    scenes_.reset(new Scenes());
    initialized_ = 0; next_tick_ = 0; next_action_ = 0; active_start_ = 0;
    active_.clear(); frame_.clear(0);
    for (unsigned i = 0; i < 8; ++i) if (!initialize_scene(i, error)) return false;
    return true;
}

void DemoRuntime::execute(const Action& action) {
    const ScriptCommand& c = action.command;
    if (c.verb == "show") {
        active_ = c.target;
        active_start_ = action.sample;
        if (active_ == "mute95") scenes_->mute95.on_show();
        else if (active_ == "domina") scenes_->domina.on_show();
        else if (active_ == "saari") scenes_->saari.on_show();
        else if (active_ == "kukot") scenes_->kukot.on_show();
        else if (active_ == "maku") scenes_->maku.on_show();
        else if (active_ == "watercube") scenes_->watercube.on_show();
        else if (active_ == "feta") scenes_->feta.on_show();
        else if (active_ == "uppol") scenes_->uppol.on_show();
    } else if (c.verb == "msg") {
        const float time = c.target == active_
            ? static_cast<float>(static_cast<double>(action.sample - active_start_) / kSampleRate) : 0.0f;
        if (c.target == "mute95") scenes_->mute95.handle_message(c.argument, time);
        else if (c.target == "saari") scenes_->saari.handle_message(c.argument, time);
        else if (c.target == "domina") scenes_->domina.handle_message(c.argument, time);
        else if (c.target == "kukot") scenes_->kukot.handle_message(c.argument, time);
        else if (c.target == "maku") scenes_->maku.handle_message(c.argument, time);
        else if (c.target == "watercube") scenes_->watercube.handle_message(c.argument, time);
        else if (c.target == "feta") scenes_->feta.handle_message(c.argument, time);
    }
}

void DemoRuntime::render_tick(std::uint64_t sample) {
    const float time = static_cast<float>(static_cast<double>(sample - active_start_) / kSampleRate);
    frame_.clear(0);
    if (active_ == "mute95") scenes_->mute95.render(frame_, time, 0.02f);
    else if (active_ == "domina") scenes_->domina.render(frame_, time, 0.02f);
    else if (active_ == "saari") scenes_->saari.render(frame_, time, 0.02f);
    else if (active_ == "kukot") scenes_->kukot.render(frame_, time, 0.02f);
    else if (active_ == "maku") scenes_->maku.render(frame_, time, 0.02f);
    else if (active_ == "watercube") scenes_->watercube.render(frame_, time, 0.02f);
    else if (active_ == "feta") scenes_->feta.render(frame_, time, 0.02f);
    else if (active_ == "uppol") scenes_->uppol.render(frame_, time, 0.02f);
}

unsigned DemoRuntime::advance_to_sample(std::uint64_t sample, unsigned max_ticks) {
    if (initialized_ != 8 || !audio_.ready() || !audio_.total_samples()) return 0;
    sample = std::min(sample, audio_.total_samples() - 1);
    unsigned count = 0;
    while (next_tick_ <= sample / kTickSamples && count < max_ticks) {
        const std::uint64_t tick_sample = next_tick_ * kTickSamples;
        while (next_action_ < audio_.actions().size() && audio_.actions()[next_action_].sample <= tick_sample)
            execute(audio_.actions()[next_action_++]);
        render_tick(tick_sample);
        ++next_tick_; ++count;
    }
    return count;
}

bool DemoRuntime::caught_up(std::uint64_t sample) const {
    return audio_.total_samples() && next_tick_ > std::min(sample, audio_.total_samples() - 1) / kTickSamples;
}
}
