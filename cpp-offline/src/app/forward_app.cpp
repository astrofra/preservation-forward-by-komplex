#include "app/forward_app.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>

#include "app/manifest_writer.h"
#include "audio/wav_writer.h"
#include "platform/file_utils.h"
#include "render/tga_writer.h"

namespace forward_offline {

ForwardApp::ForwardApp(const ExportConfig& config)
    : config_(config),
      timeline_(config.fps, config.sample_rate),
      intro_transport_(config.intro_frames_per_row, config.intro_rows_per_order),
      intro_script_(),
      frame_buffer_(config.width, config.height),
      mute95_scene_(),
      domina_routine_(),
      scene_(),
      active_renderable_(ActiveRenderable::none),
      active_name_(),
      active_start_seconds_(0.0),
      next_script_index_(0) {
}

int ForwardApp::run() {
    if (!timeline_.is_valid()) {
        std::cerr << "timeline configuration error: " << timeline_.error_message() << '\n';
        return 1;
    }
    if (is_intro_sequence() && !intro_transport_.is_valid()) {
        std::cerr << "intro transport error: " << intro_transport_.error_message() << '\n';
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

    for (unsigned int frame_index = 0; frame_index < static_cast<unsigned int>(config_.frame_count); ++frame_index) {
        const double demo_time_seconds = timeline_.frame_time_seconds(frame_index);
        const float delta_seconds = static_cast<float>(timeline_.frame_duration_seconds());
        process_intro_script(frame_index);

        frame_buffer_.clear(0);
        if (is_intro_sequence()) {
            const float local_time_seconds = static_cast<float>(demo_time_seconds - active_start_seconds_);
            if (active_renderable_ == ActiveRenderable::scene) {
                mute95_scene_.render(frame_buffer_, local_time_seconds, delta_seconds);
            } else if (active_renderable_ == ActiveRenderable::routine) {
                domina_routine_.render(frame_buffer_, local_time_seconds, delta_seconds);
            }
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
    }

    if (is_intro_sequence()) {
        mute95_scene_.dispose();
        domina_routine_.dispose();
    } else {
        scene_.dispose();
    }

    if (!wav_writer.write_silence(
            static_cast<std::size_t>(timeline_.total_samples_for_frames(config_.frame_count)),
            &error_message)) {
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
        stream << "note=intro script player with direct original-asset loading for mute95 and domina; audio/timeline transport remains synthetic\n";
    } else {
        stream << "scene=" << scene_.script_name() << '\n';
        stream << "note=placeholder scene plus silent wav until the real Java systems are ported\n";
    }
    return true;
}

std::string ForwardApp::frame_file_name(unsigned int frame_index) const {
    std::ostringstream builder;
    builder << "frame_" << std::setfill('0') << std::setw(6) << frame_index << ".tga";
    return builder.str();
}

bool ForwardApp::is_intro_sequence() const {
    return config_.sequence_name == "intro";
}

bool ForwardApp::initialize_sequence(std::string* error_message) {
    active_renderable_ = ActiveRenderable::none;
    active_name_.clear();
    active_start_seconds_ = 0.0;
    next_script_index_ = 0;

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

void ForwardApp::process_intro_script(unsigned int frame_index) {
    if (!is_intro_sequence()) {
        return;
    }

    const unsigned int current_song_position = intro_transport_.song_position_for_frame(frame_index);
    const double demo_time_seconds = timeline_.frame_time_seconds(frame_index);
    const std::vector<ScriptCommand>& commands = intro_script_.commands();

    while (next_script_index_ < commands.size() &&
           commands[next_script_index_].song_position_hex <= current_song_position) {
        execute_script_command(commands[next_script_index_], demo_time_seconds);
        ++next_script_index_;
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
    if (!is_intro_sequence()) {
        return "0x0";
    }

    const std::vector<ScriptCommand>& commands = intro_script_.commands();
    if (next_script_index_ >= commands.size()) {
        return std::string();
    }

    const unsigned int current_song_position = intro_transport_.song_position_for_frame(frame_index);
    const unsigned int next_song_position = commands[next_script_index_].song_position_hex;
    if (next_song_position < current_song_position) {
        return intro_transport_.song_position_string(current_song_position);
    }

    return intro_script_.next_position_hex(next_script_index_);
}

}  // namespace forward_offline
