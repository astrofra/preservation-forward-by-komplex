#include "app/forward_app.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "app/manifest_writer.h"
#include "audio/wav_writer.h"
#include "platform/file_utils.h"
#include "render/tga_writer.h"

namespace forward_offline {

ForwardApp::ForwardApp(const ExportConfig& config)
    : config_(config),
      timeline_(config.fps, config.sample_rate),
      frame_buffer_(config.width, config.height),
      scene_() {
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

    scene_.init();
    scene_.on_show();

    for (unsigned int frame_index = 0; frame_index < static_cast<unsigned int>(config_.frame_count); ++frame_index) {
        const float scene_time_seconds = static_cast<float>(timeline_.frame_time_seconds(frame_index));
        const float delta_seconds = static_cast<float>(timeline_.frame_duration_seconds());
        scene_.render(frame_buffer_, scene_time_seconds, delta_seconds);

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
                                    timeline_.frame_time_ms(frame_index),
                                    scene_.script_name(),
                                    "0x0",
                                    std::string("frames/") + file_name);
    }

    scene_.dispose();

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
    stream << "scene=" << scene_.script_name() << '\n';
    stream << "note=placeholder scene plus silent wav until the real Java systems are ported\n";
    return true;
}

std::string ForwardApp::frame_file_name(unsigned int frame_index) const {
    std::ostringstream builder;
    builder << "frame_" << std::setfill('0') << std::setw(6) << frame_index << ".tga";
    return builder.str();
}

}  // namespace forward_offline
