#include "app/forward_app.h"

#include <chrono>
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

std::string format_duration_seconds(double seconds) {
    if (seconds < 0.0) {
        seconds = 0.0;
    }

    const std::uint64_t total_milliseconds =
        static_cast<std::uint64_t>((seconds * 1000.0) + 0.5);
    const std::uint64_t hours = total_milliseconds / 3600000ULL;
    const std::uint64_t minutes = (total_milliseconds / 60000ULL) % 60ULL;
    const std::uint64_t secs = (total_milliseconds / 1000ULL) % 60ULL;
    const std::uint64_t milliseconds = total_milliseconds % 1000ULL;

    std::ostringstream stream;
    stream << std::setfill('0')
           << std::setw(2) << hours << ':'
           << std::setw(2) << minutes << ':'
           << std::setw(2) << secs << '.'
           << std::setw(3) << milliseconds;
    return stream.str();
}

}  // namespace

ForwardApp::ForwardApp(const ExportConfig& config)
    : config_(config),
      timeline_(config.fps, config.sample_rate),
      intro_script_(),
      kukot_script_(),
      maku_script_(),
      watercube_script_(),
      feta_script_(),
      sequence_audio_render_(),
      frame_buffer_(config.width, config.height),
      mute95_scene_(),
      domina_routine_(),
      saari_scene_(),
      kukot_scene_(),
      maku_scene_(),
      watercube_scene_(),
      feta_scene_(),
      uppol_routine_(),
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
    if (!resolve_export_span(&error_message)) {
        std::cerr << error_message << '\n';
        return 1;
    }

    const std::chrono::steady_clock::time_point render_start_time =
        std::chrono::steady_clock::now();

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
        } else if (is_kukot_sequence()) {
            process_kukot_script(frame_sample_index);
        } else if (is_maku_sequence()) {
            process_maku_script(frame_sample_index);
        } else if (is_watercube_sequence()) {
            process_watercube_script(frame_sample_index);
        } else if (is_feta_sequence()) {
            process_feta_script(frame_sample_index);
        } else if (is_uppol_sequence()) {
            process_uppol_script(frame_sample_index);
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
        } else if (is_kukot_sequence()) {
            kukot_scene_.render(frame_buffer_, static_cast<float>(demo_time_seconds), delta_seconds);
        } else if (is_maku_sequence()) {
            maku_scene_.render(frame_buffer_, static_cast<float>(demo_time_seconds), delta_seconds);
        } else if (is_watercube_sequence()) {
            watercube_scene_.render(frame_buffer_, static_cast<float>(demo_time_seconds), delta_seconds);
        } else if (is_feta_sequence()) {
            if (active_renderable_ == ActiveRenderable::scene) {
                feta_scene_.render(frame_buffer_,
                                   static_cast<float>(demo_time_seconds - active_start_seconds_),
                                   delta_seconds);
            } else if (active_renderable_ == ActiveRenderable::routine) {
                uppol_routine_.render(frame_buffer_,
                                      static_cast<float>(demo_time_seconds - active_start_seconds_),
                                      delta_seconds);
            }
        } else if (is_uppol_sequence()) {
            uppol_routine_.render(frame_buffer_,
                                  static_cast<float>(demo_time_seconds - active_start_seconds_),
                                  delta_seconds);
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
    } else if (is_kukot_sequence()) {
        kukot_scene_.dispose();
    } else if (is_maku_sequence()) {
        maku_scene_.dispose();
    } else if (is_watercube_sequence()) {
        watercube_scene_.dispose();
    } else if (is_feta_sequence()) {
        feta_scene_.dispose();
        uppol_routine_.dispose();
    } else if (is_uppol_sequence()) {
        uppol_routine_.dispose();
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

    const std::chrono::steady_clock::time_point render_end_time =
        std::chrono::steady_clock::now();
    const double render_time_seconds =
        std::chrono::duration_cast<std::chrono::duration<double> >(
            render_end_time - render_start_time).count();
    const double animation_duration_seconds =
        static_cast<double>(timeline_.total_samples_for_frames(total_frames)) /
        static_cast<double>(timeline_.sample_rate());
    const double export_fps = render_time_seconds > 0.0
        ? static_cast<double>(total_frames) / render_time_seconds
        : 0.0;

    std::cout << "wrote " << config_.frame_count << " frames and one wav under "
              << config_.output_dir << '\n';
    std::cout << "render time: " << format_duration_seconds(render_time_seconds) << '\n';
    std::cout << "animation duration: " << format_duration_seconds(animation_duration_seconds) << '\n';
    std::cout << "export throughput: " << std::fixed << std::setprecision(2) << export_fps
              << " fps\n";
    return 0;
}

bool ForwardApp::resolve_export_span(std::string* error_message) {
    if (!config_.has_end_song_position) {
        return true;
    }
    if (!is_intro_sequence() && !is_saari_sequence() && !is_kukot_sequence() &&
        !is_maku_sequence() && !is_watercube_sequence() && !is_feta_sequence() &&
        !is_uppol_sequence()) {
        if (error_message != NULL) {
            *error_message =
                "--until-song-position is currently supported for intro, saari, kukot, maku, watercube, feta, and uppol";
        }
        return false;
    }

    int resolved_frame_count = 0;
    if (!resolve_sequence_frame_count_for_song_position(config_.sequence_name,
                                                        config_.fps,
                                                        config_.sample_rate,
                                                        config_.end_song_position_hex,
                                                        config_.post_roll_frames,
                                                        &resolved_frame_count,
                                                        error_message)) {
        return false;
    }

    config_.frame_count = resolved_frame_count;
    return true;
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

    if (!is_intro_sequence() && !is_saari_sequence() && !is_kukot_sequence() &&
        !is_maku_sequence() && !is_watercube_sequence() && !is_feta_sequence() &&
        !is_uppol_sequence()) {
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
    if (config_.has_end_song_position) {
        stream << "until_song_position=" << song_position_string(config_.end_song_position_hex) << '\n';
        stream << "post_roll_frames=" << config_.post_roll_frames << '\n';
    }
    if (is_intro_sequence()) {
        stream << "intro_frames_per_row=" << config_.intro_frames_per_row << '\n';
        stream << "intro_rows_per_order=" << config_.intro_rows_per_order << '\n';
        stream << "note=intro script player with direct original-asset loading for mute95 and domina plus native kuninga.xm replay; demo clock now follows audio sample position\n";
    } else if (is_saari_sequence()) {
        stream << "intro_frames_per_row=" << config_.intro_frames_per_row << '\n';
        stream << "intro_rows_per_order=" << config_.intro_rows_per_order << '\n';
        stream << "scene=saari\n";
        stream << "note=first autonomous saari 3D pass with direct original-asset loading, ASE camera/object parsing, terrain/reflection rendering, native jarnomix.xm replay, and audio-clocked shock messages; camera/raster parity is still pending\n";
    } else if (is_kukot_sequence()) {
        stream << "intro_frames_per_row=" << config_.intro_frames_per_row << '\n';
        stream << "intro_rows_per_order=" << config_.intro_rows_per_order << '\n';
        stream << "scene=kukot\n";
        stream << "note=first autonomous kukot 3D pass with shared ASE track parsing, env-mapped mesh rendering, static flare cloud, and native jarnomix.xm playback sliced from song position 0x0700; spline/interpolation parity is still pending\n";
    } else if (is_maku_sequence()) {
        stream << "intro_frames_per_row=" << config_.intro_frames_per_row << '\n';
        stream << "intro_rows_per_order=" << config_.intro_rows_per_order << '\n';
        stream << "scene=maku\n";
        stream << "note=first autonomous maku terrain flythrough with direct loopk40/loopa2 asset loading, ASE camera-track playback, scripted shock feedback, and native jarnomix.xm playback sliced from song position 0x0D00; terrain/raster parity is still pending\n";
    } else if (is_watercube_sequence()) {
        stream << "intro_frames_per_row=" << config_.intro_frames_per_row << '\n';
        stream << "intro_rows_per_order=" << config_.intro_rows_per_order << '\n';
        stream << "scene=watercube\n";
        stream << "note=first autonomous watercube mixed 3D and packed-surface pass with direct nosto3/reunus2/txt1/env3/rinku2/riple2 asset loading, scripted flash and strip overlays, and native jarnomix.xm playback sliced from song position 0x1000; env-mesh lighting and face-mode parity are still pending\n";
    } else if (is_feta_sequence()) {
        stream << "intro_frames_per_row=" << config_.intro_frames_per_row << '\n';
        stream << "intro_rows_per_order=" << config_.intro_rows_per_order << '\n';
        stream << "scene=feta\n";
        stream << "note=feta scene window with native jarnomix.xm playback sliced from song position 0x1230 and scripted handoff to uppol at 0x1600; feta still uses the packed-surface direct-asset pass while uppol reuses the original indexed phorward.gif path plus Java-derived credit bitmaps\n";
    } else if (is_uppol_sequence()) {
        stream << "intro_frames_per_row=" << config_.intro_frames_per_row << '\n';
        stream << "intro_rows_per_order=" << config_.intro_rows_per_order << '\n';
        stream << "scene=uppol\n";
        stream << "note=uppol direct indexed phorward.gif pass with Java-derived fixed credit bitmaps, audio sliced from jarnomix.xm song position 0x1600, and non-interactive visual-only link lines\n";
    } else {
        stream << "scene=" << scene_.script_name() << '\n';
        stream << "note=placeholder scene plus silent wav until the real Java systems are ported\n";
    }
    return true;
}

bool ForwardApp::write_sequence_audio(WavWriter* wav_writer, std::string* error_message) const {
    const std::size_t total_sample_frames =
        static_cast<std::size_t>(timeline_.total_samples_for_frames(config_.frame_count));

    if (!is_intro_sequence() && !is_saari_sequence() && !is_kukot_sequence() &&
        !is_maku_sequence() && !is_watercube_sequence() && !is_feta_sequence() &&
        !is_uppol_sequence()) {
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

bool ForwardApp::is_kukot_sequence() const {
    return config_.sequence_name == "kukot";
}

bool ForwardApp::is_maku_sequence() const {
    return config_.sequence_name == "maku";
}

bool ForwardApp::is_watercube_sequence() const {
    return config_.sequence_name == "watercube";
}

bool ForwardApp::is_feta_sequence() const {
    return config_.sequence_name == "feta";
}

bool ForwardApp::is_uppol_sequence() const {
    return config_.sequence_name == "uppol";
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

    if (is_kukot_sequence()) {
        if (config_.width != 512 || config_.height != 256) {
            if (error_message != NULL) {
                *error_message = "kukot sequence currently requires native 512x256 output";
            }
            return false;
        }
        kukot_scene_.init();
        if (!kukot_scene_.is_ready()) {
            if (error_message != NULL) {
                *error_message = kukot_scene_.error_message();
            }
            return false;
        }
        kukot_scene_.on_show();
        active_renderable_ = ActiveRenderable::scene;
        active_name_ = kukot_scene_.script_name();
        active_start_seconds_ = 0.0;
        return true;
    }

    if (is_maku_sequence()) {
        if (config_.width != 512 || config_.height != 256) {
            if (error_message != NULL) {
                *error_message = "maku sequence currently requires native 512x256 output";
            }
            return false;
        }
        maku_scene_.init();
        if (!maku_scene_.is_ready()) {
            if (error_message != NULL) {
                *error_message = maku_scene_.error_message();
            }
            return false;
        }
        maku_scene_.on_show();
        active_renderable_ = ActiveRenderable::scene;
        active_name_ = maku_scene_.script_name();
        active_start_seconds_ = 0.0;
        return true;
    }

    if (is_watercube_sequence()) {
        if (config_.width != 512 || config_.height != 256) {
            if (error_message != NULL) {
                *error_message = "watercube sequence currently requires native 512x256 output";
            }
            return false;
        }
        watercube_scene_.init();
        if (!watercube_scene_.is_ready()) {
            if (error_message != NULL) {
                *error_message = watercube_scene_.error_message();
            }
            return false;
        }
        watercube_scene_.on_show();
        active_renderable_ = ActiveRenderable::scene;
        active_name_ = watercube_scene_.script_name();
        active_start_seconds_ = 0.0;
        return true;
    }

    if (is_feta_sequence()) {
        if (config_.width != 512 || config_.height != 256) {
            if (error_message != NULL) {
                *error_message = "feta sequence currently requires native 512x256 output";
            }
            return false;
        }
        feta_scene_.init();
        if (!feta_scene_.is_ready()) {
            if (error_message != NULL) {
                *error_message = feta_scene_.error_message();
            }
            return false;
        }
        uppol_routine_.init();
        if (!uppol_routine_.is_ready()) {
            if (error_message != NULL) {
                *error_message = uppol_routine_.error_message();
            }
            return false;
        }
        active_renderable_ = ActiveRenderable::none;
        active_name_.clear();
        active_start_seconds_ = 0.0;
        return true;
    }

    if (is_uppol_sequence()) {
        if (config_.width != 512 || config_.height != 256) {
            if (error_message != NULL) {
                *error_message = "uppol sequence currently requires native 512x256 output";
            }
            return false;
        }
        uppol_routine_.init();
        if (!uppol_routine_.is_ready()) {
            if (error_message != NULL) {
                *error_message = uppol_routine_.error_message();
            }
            return false;
        }
        uppol_routine_.on_show();
        active_renderable_ = ActiveRenderable::routine;
        active_name_ = uppol_routine_.script_name();
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

void ForwardApp::process_kukot_script(std::uint64_t sample_index) {
    if (!is_kukot_sequence()) {
        return;
    }

    while (next_song_position_event_index_ < sequence_audio_render_.song_positions.size() &&
           sequence_audio_render_.song_positions[next_song_position_event_index_].sample_index <= sample_index) {
        const SongPositionEvent& event =
            sequence_audio_render_.song_positions[next_song_position_event_index_];
        current_song_position_ = event.song_position_hex;
        const double demo_time_seconds =
            static_cast<double>(event.sample_index) / static_cast<double>(config_.sample_rate);
        const std::vector<ScriptCommand>& commands = kukot_script_.commands();

        while (next_script_index_ < commands.size() &&
               commands[next_script_index_].song_position_hex <= current_song_position_) {
            execute_script_command(commands[next_script_index_], demo_time_seconds);
            ++next_script_index_;
        }
        ++next_song_position_event_index_;
    }
}

void ForwardApp::process_maku_script(std::uint64_t sample_index) {
    if (!is_maku_sequence()) {
        return;
    }

    while (next_song_position_event_index_ < sequence_audio_render_.song_positions.size() &&
           sequence_audio_render_.song_positions[next_song_position_event_index_].sample_index <= sample_index) {
        const SongPositionEvent& event =
            sequence_audio_render_.song_positions[next_song_position_event_index_];
        current_song_position_ = event.song_position_hex;
        const double demo_time_seconds =
            static_cast<double>(event.sample_index) / static_cast<double>(config_.sample_rate);
        const std::vector<ScriptCommand>& commands = maku_script_.commands();

        while (next_script_index_ < commands.size() &&
               commands[next_script_index_].song_position_hex <= current_song_position_) {
            execute_script_command(commands[next_script_index_], demo_time_seconds);
            ++next_script_index_;
        }
        ++next_song_position_event_index_;
    }
}

void ForwardApp::process_watercube_script(std::uint64_t sample_index) {
    if (!is_watercube_sequence()) {
        return;
    }

    while (next_song_position_event_index_ < sequence_audio_render_.song_positions.size() &&
           sequence_audio_render_.song_positions[next_song_position_event_index_].sample_index <= sample_index) {
        const SongPositionEvent& event =
            sequence_audio_render_.song_positions[next_song_position_event_index_];
        current_song_position_ = event.song_position_hex;
        const double demo_time_seconds =
            static_cast<double>(event.sample_index) / static_cast<double>(config_.sample_rate);
        const std::vector<ScriptCommand>& commands = watercube_script_.commands();

        while (next_script_index_ < commands.size() &&
               commands[next_script_index_].song_position_hex <= current_song_position_) {
            execute_script_command(commands[next_script_index_], demo_time_seconds);
            ++next_script_index_;
        }
        ++next_song_position_event_index_;
    }
}

void ForwardApp::process_feta_script(std::uint64_t sample_index) {
    if (!is_feta_sequence()) {
        return;
    }

    while (next_song_position_event_index_ < sequence_audio_render_.song_positions.size() &&
           sequence_audio_render_.song_positions[next_song_position_event_index_].sample_index <= sample_index) {
        const SongPositionEvent& event =
            sequence_audio_render_.song_positions[next_song_position_event_index_];
        current_song_position_ = event.song_position_hex;
        const double demo_time_seconds =
            static_cast<double>(event.sample_index) / static_cast<double>(config_.sample_rate);
        const std::vector<ScriptCommand>& commands = feta_script_.commands();

        while (next_script_index_ < commands.size() &&
               commands[next_script_index_].song_position_hex <= current_song_position_) {
            execute_script_command(commands[next_script_index_], demo_time_seconds);
            ++next_script_index_;
        }
        ++next_song_position_event_index_;
    }
}

void ForwardApp::process_uppol_script(std::uint64_t sample_index) {
    if (!is_uppol_sequence()) {
        return;
    }

    while (next_song_position_event_index_ < sequence_audio_render_.song_positions.size() &&
           sequence_audio_render_.song_positions[next_song_position_event_index_].sample_index <= sample_index) {
        const SongPositionEvent& event =
            sequence_audio_render_.song_positions[next_song_position_event_index_];
        current_song_position_ = event.song_position_hex;
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
        } else if (command.target == "kukot") {
            show_scene(command.target, demo_time_seconds);
        } else if (command.target == "maku") {
            show_scene(command.target, demo_time_seconds);
        } else if (command.target == "watercube") {
            show_scene(command.target, demo_time_seconds);
        } else if (command.target == "feta") {
            show_scene(command.target, demo_time_seconds);
        } else if (command.target == "uppol") {
            show_routine(command.target, demo_time_seconds);
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
        } else if (command.target == "kukot") {
            kukot_scene_.handle_message(command.argument,
                                        static_cast<float>(demo_time_seconds - active_start_seconds_));
        } else if (command.target == "maku") {
            maku_scene_.handle_message(command.argument,
                                       static_cast<float>(demo_time_seconds - active_start_seconds_));
        } else if (command.target == "watercube") {
            watercube_scene_.handle_message(command.argument,
                                            static_cast<float>(demo_time_seconds - active_start_seconds_));
        } else if (command.target == "feta") {
            feta_scene_.handle_message(command.argument,
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
    if (scene_name == "mute95") {
        mute95_scene_.on_show();
    } else if (scene_name == "kukot") {
        kukot_scene_.on_show();
    } else if (scene_name == "maku") {
        maku_scene_.on_show();
    } else if (scene_name == "watercube") {
        watercube_scene_.on_show();
    } else if (scene_name == "feta") {
        feta_scene_.on_show();
    } else {
        return;
    }
    active_renderable_ = ActiveRenderable::scene;
    active_name_ = scene_name;
    active_start_seconds_ = demo_time_seconds;
}

void ForwardApp::show_routine(const std::string& routine_name, double demo_time_seconds) {
    if (routine_name == "domina") {
        domina_routine_.on_show();
    } else if (routine_name == "uppol") {
        uppol_routine_.on_show();
    } else {
        return;
    }
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

    if (is_kukot_sequence()) {
        const std::vector<ScriptCommand>& commands = kukot_script_.commands();
        if (next_script_index_ >= commands.size()) {
            return std::string();
        }

        const unsigned int current_song_position = current_song_position_;
        const unsigned int next_song_position = commands[next_script_index_].song_position_hex;
        if (next_song_position < current_song_position) {
            return song_position_string(current_song_position);
        }

        return kukot_script_.next_position_hex(next_script_index_);
    }

    if (is_maku_sequence()) {
        const std::vector<ScriptCommand>& commands = maku_script_.commands();
        if (next_script_index_ >= commands.size()) {
            return std::string();
        }

        const unsigned int current_song_position = current_song_position_;
        const unsigned int next_song_position = commands[next_script_index_].song_position_hex;
        if (next_song_position < current_song_position) {
            return song_position_string(current_song_position);
        }

        return maku_script_.next_position_hex(next_script_index_);
    }

    if (is_watercube_sequence()) {
        const std::vector<ScriptCommand>& commands = watercube_script_.commands();
        if (next_script_index_ >= commands.size()) {
            return std::string();
        }

        const unsigned int current_song_position = current_song_position_;
        const unsigned int next_song_position = commands[next_script_index_].song_position_hex;
        if (next_song_position < current_song_position) {
            return song_position_string(current_song_position);
        }

        return watercube_script_.next_position_hex(next_script_index_);
    }

    if (is_feta_sequence()) {
        const std::vector<ScriptCommand>& commands = feta_script_.commands();
        if (next_script_index_ >= commands.size()) {
            return std::string();
        }

        const unsigned int current_song_position = current_song_position_;
        const unsigned int next_song_position = commands[next_script_index_].song_position_hex;
        if (next_song_position < current_song_position) {
            return song_position_string(current_song_position);
        }

        return feta_script_.next_position_hex(next_script_index_);
    }

    if (is_uppol_sequence()) {
        return std::string();
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
