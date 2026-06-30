#include "app/forward_app.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>

#include "app/manifest_writer.h"
#include "audio/wav_writer.h"
#include "platform/file_utils.h"
#include "render/tga_writer.h"

namespace forward_offline {

namespace {

struct SaariScriptEvent {
    unsigned int song_position_hex;
    const char* message_name;
};

const SaariScriptEvent kSaariScriptEvents[] = {
    {0x0000U, "suh0"},
    {0x0100U, "suh"},
    {0x0600U, "suh"},
    {0x0608U, "suh"},
    {0x0610U, "suh"},
    {0x0618U, "suh"},
    {0x0620U, "suh"},
    {0x0628U, "suh"},
    {0x0630U, "suh"},
    {0x0700U, ""}
};

std::string saari_next_position_hex(std::size_t index) {
    if (index >= sizeof(kSaariScriptEvents) / sizeof(kSaariScriptEvents[0])) {
        return std::string();
    }

    std::ostringstream builder;
    builder << "0x" << std::hex << kSaariScriptEvents[index].song_position_hex;
    return builder.str();
}

unsigned int progress_percent_for_frame(unsigned int completed_frames, unsigned int total_frames) {
    if (total_frames == 0U) {
        return 100U;
    }
    return static_cast<unsigned int>(
        (static_cast<unsigned long long>(completed_frames) * 100ULL) /
        static_cast<unsigned long long>(total_frames));
}

void print_export_progress(unsigned int completed_frames, unsigned int total_frames) {
    const unsigned int percent = progress_percent_for_frame(completed_frames, total_frames);
    const int bar_width = 24;
    const int filled = total_frames == 0U
        ? bar_width
        : static_cast<int>(
              (static_cast<unsigned long long>(completed_frames) * static_cast<unsigned long long>(bar_width)) /
              static_cast<unsigned long long>(total_frames));

    std::cout << '\r' << '[';
    for (int index = 0; index < bar_width; ++index) {
        std::cout << (index < filled ? '=' : ' ');
    }
    std::cout << "] " << std::setw(3) << percent << "% (" << completed_frames << "/" << total_frames << ')'
              << std::flush;
}

}  // namespace

ForwardApp::ForwardApp(const ExportConfig& config)
    : config_(config),
      timeline_(config.fps, config.sample_rate),
      intro_script_(),
      sequence_audio_render_(),
      frame_buffer_(config.width, config.height),
      mute95_scene_(),
      domina_routine_(),
      saari_scene_(),
      scene_(),
      active_renderable_(ActiveRenderable::none),
      active_name_(),
      active_start_seconds_(0.0),
      next_script_index_(0),
      next_song_position_event_index_(0),
      current_song_position_(0U) {
}

int ForwardApp::run() {
    if (!timeline_.is_valid()) {
        std::cerr << "timeline configuration error: " << timeline_.error_message() << '\n';
        return 1;
    }

    std::string error_message;
    if (!prepare_output(&error_message)) {
        std::cerr << error_message << '\n';
        return 1;
    }

    ManifestWriter manifest_writer;
    if (!manifest_writer.open(join_path(config_.output_dir, "manifest.csv"), &error_message)) {
        std::cerr << error_message << '\n';
        return 1;
    }

    WavWriter wav_writer;
    if (!wav_writer.open(join_path(join_path(config_.output_dir, "audio"), "forward.wav"),
                         config_.sample_rate, 2, 16, &error_message)) {
        std::cerr << error_message << '\n';
        return 1;
    }

    if (config_.write_log && !write_log(&error_message)) {
        std::cerr << error_message << '\n';
        return 1;
    }

    if (!initialize_sequence(&error_message)) {
        std::cerr << error_message << '\n';
        return 1;
    }
    if (!prepare_sequence_audio(&error_message)) {
        std::cerr << error_message << '\n';
        return 1;
    }

    const unsigned int total_frames = static_cast<unsigned int>(config_.frame_count);
    int last_progress_percent = -1;
    if (total_frames > 0U) {
        print_export_progress(0U, total_frames);
        last_progress_percent = 0;
    }

    for (unsigned int frame_index = 0; frame_index < total_frames; ++frame_index) {
        const std::uint64_t frame_sample_index = timeline_.sample_index_for_frame(frame_index);
        const double demo_time_seconds = timeline_.frame_time_seconds(frame_index);
        const float delta_seconds = static_cast<float>(timeline_.frame_duration_seconds());
        if (is_intro_sequence()) {
            process_intro_script(frame_sample_index);
        } else if (is_saari_sequence()) {
            process_saari_script(frame_sample_index);
        }

        frame_buffer_.clear(0);
        if (is_intro_sequence()) {
            const float local_time_seconds = static_cast<float>(demo_time_seconds - active_start_seconds_);
            if (active_renderable_ == ActiveRenderable::scene) {
                mute95_scene_.render(frame_buffer_, local_time_seconds, delta_seconds);
            } else if (active_renderable_ == ActiveRenderable::routine) {
                domina_routine_.render(frame_buffer_, local_time_seconds, delta_seconds);
            }
        } else if (is_saari_sequence()) {
            saari_scene_.render(frame_buffer_, static_cast<float>(demo_time_seconds), delta_seconds);
        } else {
            const float scene_time_seconds = static_cast<float>(demo_time_seconds);
            scene_.render(frame_buffer_, scene_time_seconds, delta_seconds);
        }

        const std::string file_name = frame_file_name(frame_index);
        const std::string absolute_frame_path =
            join_path(join_path(config_.output_dir, "frames"), file_name);
        if (!write_tga24(absolute_frame_path, frame_buffer_, &error_message)) {
            std::cerr << error_message << '\n';
            return 1;
        }

        manifest_writer.write_frame(frame_index,
                                    frame_index,
                                    timeline_.frame_time_ms(frame_index),
                                    active_renderable_ == ActiveRenderable::none
                                        ? timeline_.frame_time_ms(frame_index)
                                        : static_cast<std::uint64_t>(
                                              (demo_time_seconds - active_start_seconds_) * 1000.0),
                                    active_name_,
                                    next_script_time_hex(frame_index),
                                    std::string("frames/") + file_name);

        const unsigned int completed_frames = frame_index + 1U;
        const int progress_percent = static_cast<int>(progress_percent_for_frame(completed_frames, total_frames));
        if (progress_percent != last_progress_percent) {
            print_export_progress(completed_frames, total_frames);
            last_progress_percent = progress_percent;
        }
    }

    if (total_frames > 0U) {
        std::cout << '\n';
    }

    if (is_intro_sequence()) {
        mute95_scene_.dispose();
        domina_routine_.dispose();
    } else if (is_saari_sequence()) {
        saari_scene_.dispose();
    } else {
        scene_.dispose();
    }

    if (!write_sequence_audio(&wav_writer, &error_message)) {
        std::cerr << error_message << '\n';
        return 1;
    }

    if (!wav_writer.finalize(&error_message)) {
        std::cerr << error_message << '\n';
        return 1;
    }

    manifest_writer.close();

    std::cout << "wrote " << config_.frame_count << " frames and one wav under "
              << config_.output_dir << '\n';
    return 0;
}

bool ForwardApp::prepare_output(std::string* error_message) const {
    if (!create_directories(config_.output_dir, error_message)) {
        return false;
    }
    if (!create_directories(join_path(config_.output_dir, "frames"), error_message)) {
        return false;
    }
    if (!create_directories(join_path(config_.output_dir, "audio"), error_message)) {
        return false;
    }
    return true;
}

bool ForwardApp::prepare_sequence_audio(std::string* error_message) {
    sequence_audio_render_.interleaved_samples.clear();
    sequence_audio_render_.song_positions.clear();
    next_song_position_event_index_ = 0U;
    current_song_position_ = 0U;

    if (!is_intro_sequence() && !is_saari_sequence()) {
        return true;
    }

    return render_sequence_module_audio(
        config_.sequence_name,
        config_.sample_rate,
        static_cast<std::size_t>(timeline_.total_samples_for_frames(config_.frame_count)),
        &sequence_audio_render_,
        error_message);
}

bool ForwardApp::write_log(std::string* error_message) const {
    const std::string path = join_path(config_.output_dir, "log.txt");
    std::ofstream stream(path.c_str(), std::ios::out | std::ios::trunc);
    if (!stream) {
        if (error_message != NULL) {
            *error_message = "unable to write log file: " + path;
        }
        return false;
    }

    stream << "forward-export bootstrap scaffold\n";
    stream << "resolution=" << config_.width << "x" << config_.height << '\n';
    stream << "frames=" << config_.frame_count << '\n';
    stream << "fps=" << config_.fps << '\n';
    stream << "sample_rate=" << config_.sample_rate << '\n';
    stream << "samples_per_frame=" << timeline_.samples_per_frame() << '\n';
    stream << "sequence=" << config_.sequence_name << '\n';
    if (is_intro_sequence()) {
        stream << "intro_frames_per_row=" << config_.intro_frames_per_row << '\n';
        stream << "intro_rows_per_order=" << config_.intro_rows_per_order << '\n';
        stream << "note=intro script player with direct original-asset loading for mute95 and domina plus native kuninga.xm replay; demo clock now follows audio sample position\n";
    } else if (is_saari_sequence()) {
        stream << "intro_frames_per_row=" << config_.intro_frames_per_row << '\n';
        stream << "intro_rows_per_order=" << config_.intro_rows_per_order << '\n';
        stream << "scene=saari\n";
        stream << "note=first autonomous saari 3D pass with direct original-asset loading, ASE camera/object parsing, terrain/reflection rendering, native jarnomix.xm replay, and audio-clocked shock messages; camera/raster parity is still pending\n";
    } else {
        stream << "scene=" << scene_.script_name() << '\n';
        stream << "note=placeholder scene plus silent wav until the real Java systems are ported\n";
    }
    return true;
}

bool ForwardApp::write_sequence_audio(WavWriter* wav_writer, std::string* error_message) const {
    const std::size_t total_sample_frames =
        static_cast<std::size_t>(timeline_.total_samples_for_frames(config_.frame_count));

    if (!is_intro_sequence() && !is_saari_sequence()) {
        return wav_writer->write_silence(total_sample_frames, error_message);
    }

    if (sequence_audio_render_.interleaved_samples.empty() && total_sample_frames > 0U) {
        if (error_message != NULL) {
            *error_message = "sequence audio was not prepared before wav write";
        }
        return false;
    }

    return wav_writer->write_pcm_s16(sequence_audio_render_.interleaved_samples, error_message);
}

std::string ForwardApp::frame_file_name(unsigned int frame_index) const {
    std::ostringstream builder;
    builder << "frame_" << std::setfill('0') << std::setw(6) << frame_index << ".tga";
    return builder.str();
}

bool ForwardApp::is_intro_sequence() const {
    return config_.sequence_name == "intro";
}

bool ForwardApp::is_saari_sequence() const {
    return config_.sequence_name == "saari";
}

bool ForwardApp::initialize_sequence(std::string* error_message) {
    active_renderable_ = ActiveRenderable::none;
    active_name_.clear();
    active_start_seconds_ = 0.0;
    next_script_index_ = 0;
    next_song_position_event_index_ = 0U;
    current_song_position_ = 0U;

    if (is_intro_sequence()) {
        if (config_.width != 512 || config_.height != 256) {
            if (error_message != NULL) {
                *error_message = "intro sequence currently requires native 512x256 output";
            }
            return false;
        }
        mute95_scene_.init();
        if (!mute95_scene_.is_ready()) {
            if (error_message != NULL) {
                *error_message = mute95_scene_.error_message();
            }
            return false;
        }
        domina_routine_.init();
        if (!domina_routine_.is_ready()) {
            if (error_message != NULL) {
                *error_message = domina_routine_.error_message();
            }
            return false;
        }
        return true;
    }

    if (is_saari_sequence()) {
        if (config_.width != 512 || config_.height != 256) {
            if (error_message != NULL) {
                *error_message = "saari sequence currently requires native 512x256 output";
            }
            return false;
        }
        saari_scene_.init();
        if (!saari_scene_.is_ready()) {
            if (error_message != NULL) {
                *error_message = saari_scene_.error_message();
            }
            return false;
        }
        saari_scene_.on_show();
        active_renderable_ = ActiveRenderable::scene;
        active_name_ = saari_scene_.script_name();
        active_start_seconds_ = 0.0;
        return true;
    }

    if (config_.sequence_name == "bootstrap") {
        scene_.init();
        scene_.on_show();
        active_renderable_ = ActiveRenderable::scene;
        active_name_ = scene_.script_name();
        return true;
    }

    if (error_message != NULL) {
        *error_message = "unknown sequence: " + config_.sequence_name;
    }
    return false;
}

void ForwardApp::process_intro_script(std::uint64_t sample_index) {
    if (!is_intro_sequence()) {
        return;
    }

    while (next_song_position_event_index_ < sequence_audio_render_.song_positions.size() &&
           sequence_audio_render_.song_positions[next_song_position_event_index_].sample_index <= sample_index) {
        const SongPositionEvent& event =
            sequence_audio_render_.song_positions[next_song_position_event_index_];
        current_song_position_ = event.song_position_hex;
        const double demo_time_seconds =
            static_cast<double>(event.sample_index) / static_cast<double>(config_.sample_rate);
        const std::vector<ScriptCommand>& commands = intro_script_.commands();

        while (next_script_index_ < commands.size() &&
               commands[next_script_index_].song_position_hex <= current_song_position_) {
            execute_script_command(commands[next_script_index_], demo_time_seconds);
            ++next_script_index_;
        }
        ++next_song_position_event_index_;
    }
}

void ForwardApp::process_saari_script(std::uint64_t sample_index) {
    if (!is_saari_sequence()) {
        return;
    }

    const std::size_t event_count = sizeof(kSaariScriptEvents) / sizeof(kSaariScriptEvents[0]);

    while (next_song_position_event_index_ < sequence_audio_render_.song_positions.size() &&
           sequence_audio_render_.song_positions[next_song_position_event_index_].sample_index <= sample_index) {
        const SongPositionEvent& event =
            sequence_audio_render_.song_positions[next_song_position_event_index_];
        current_song_position_ = event.song_position_hex;
        const double demo_time_seconds =
            static_cast<double>(event.sample_index) / static_cast<double>(config_.sample_rate);

        while (next_script_index_ < event_count &&
               kSaariScriptEvents[next_script_index_].song_position_hex <= current_song_position_) {
            if (kSaariScriptEvents[next_script_index_].message_name[0] != '\0') {
                saari_scene_.handle_message(kSaariScriptEvents[next_script_index_].message_name,
                                            static_cast<float>(demo_time_seconds));
            }
            ++next_script_index_;
        }
        ++next_song_position_event_index_;
    }
}

void ForwardApp::execute_script_command(const ScriptCommand& command, double demo_time_seconds) {
    if (command.verb == "init") {
        return;
    }
    if (command.verb == "loaded") {
        return;
    }
    if (command.verb == "mod") {
        return;
    }
    if (command.verb == "go") {
        return;
    }
    if (command.verb == "shutdown") {
        return;
    }
    if (command.verb == "killmod") {
        return;
    }
    if (command.verb == "show") {
        if (command.target == "mute95") {
            show_scene(command.target, demo_time_seconds);
        } else if (command.target == "domina") {
            show_routine(command.target, demo_time_seconds);
        }
        return;
    }
    if (command.verb == "msg") {
        if (command.target == "mute95") {
            mute95_scene_.handle_message(command.argument,
                                         static_cast<float>(demo_time_seconds - active_start_seconds_));
        } else if (command.target == "domina") {
            domina_routine_.handle_message(command.argument,
                                           static_cast<float>(demo_time_seconds - active_start_seconds_));
        }
        return;
    }
    if (command.verb == "clear24") {
        frame_buffer_.clear(0);
        return;
    }
    if (command.verb == "filmbox") {
        active_renderable_ = ActiveRenderable::none;
        active_name_.clear();
        active_start_seconds_ = demo_time_seconds;
        return;
    }
    if (command.verb == "kill") {
        kill_renderable(command.target);
    }
}

void ForwardApp::show_scene(const std::string& scene_name, double demo_time_seconds) {
    if (scene_name != "mute95") {
        return;
    }

    mute95_scene_.on_show();
    active_renderable_ = ActiveRenderable::scene;
    active_name_ = scene_name;
    active_start_seconds_ = demo_time_seconds;
}

void ForwardApp::show_routine(const std::string& routine_name, double demo_time_seconds) {
    if (routine_name != "domina") {
        return;
    }

    domina_routine_.on_show();
    active_renderable_ = ActiveRenderable::routine;
    active_name_ = routine_name;
    active_start_seconds_ = demo_time_seconds;
}

void ForwardApp::kill_renderable(const std::string& name) {
    if (name == active_name_) {
        active_renderable_ = ActiveRenderable::none;
        active_name_.clear();
    }
}

std::string ForwardApp::next_script_time_hex(unsigned int frame_index) const {
    (void)frame_index;
    if (is_saari_sequence()) {
        const std::size_t event_count = sizeof(kSaariScriptEvents) / sizeof(kSaariScriptEvents[0]);
        if (next_script_index_ >= event_count) {
            return std::string();
        }
        return saari_next_position_hex(next_script_index_);
    }

    if (!is_intro_sequence()) {
        return "0x0";
    }

    const std::vector<ScriptCommand>& commands = intro_script_.commands();
    if (next_script_index_ >= commands.size()) {
        return std::string();
    }

    const unsigned int current_song_position = current_song_position_;
    const unsigned int next_song_position = commands[next_script_index_].song_position_hex;
    if (next_song_position < current_song_position) {
        return song_position_string(current_song_position);
    }

    return intro_script_.next_position_hex(next_script_index_);
}

std::string ForwardApp::song_position_string(unsigned int song_position) const {
    std::ostringstream builder;
    builder << "0x" << std::hex << song_position;
    return builder.str();
}

}  // namespace forward_offline
