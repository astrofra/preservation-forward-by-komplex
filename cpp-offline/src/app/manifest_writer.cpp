#include "app/manifest_writer.h"

#include <iomanip>
#include <sstream>

namespace forward_offline {

ManifestWriter::ManifestWriter() : stream_() {
}

bool ManifestWriter::open(const std::string& path, std::string* error_message) {
    stream_.open(path.c_str(), std::ios::out | std::ios::trunc);
    if (!stream_) {
        if (error_message != NULL) {
            *error_message = "unable to write manifest: " + path;
        }
        return false;
    }

    stream_ << "capture_index,render_frame,demo_time_ms,demo_time_seconds,"
               "scene_time_ms,scene_time_seconds,scene,next_script_time_hex,frame_path\n";
    return true;
}

void ManifestWriter::write_frame(std::uint32_t capture_index,
                                 std::uint32_t render_frame,
                                 std::uint64_t demo_time_ms,
                                 std::uint64_t scene_time_ms,
                                 const std::string& scene,
                                 const std::string& next_script_time_hex,
                                 const std::string& frame_path) {
    stream_ << capture_index << ','
            << render_frame << ','
            << demo_time_ms << ','
            << seconds_string(demo_time_ms) << ','
            << scene_time_ms << ','
            << seconds_string(scene_time_ms) << ','
            << csv_escape(scene) << ','
            << csv_escape(next_script_time_hex) << ','
            << csv_escape(frame_path) << '\n';
}

void ManifestWriter::close() {
    if (!stream_) {
        return;
    }

    stream_.flush();
    stream_.close();
}

std::string ManifestWriter::csv_escape(const std::string& value) {
    if (value.find_first_of(",\"") == std::string::npos) {
        return value;
    }

    std::string escaped;
    escaped.reserve(value.size() + 2);
    escaped.push_back('"');
    for (std::size_t index = 0; index < value.size(); ++index) {
        if (value[index] == '"') {
            escaped.push_back('"');
        }
        escaped.push_back(value[index]);
    }
    escaped.push_back('"');
    return escaped;
}

std::string ManifestWriter::seconds_string(std::uint64_t time_ms) {
    std::ostringstream builder;
    builder << std::fixed << std::setprecision(3)
            << (static_cast<double>(time_ms) / 1000.0);
    return builder.str();
}

}  // namespace forward_offline
