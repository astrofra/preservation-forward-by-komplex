#include "app/export_config.h"

#include <cstdlib>
#include <limits>
#include <ostream>
#include <string>

namespace forward_offline {

namespace {

bool parse_positive_int(const std::string& text, int* value) {
    if (text.empty()) {
        return false;
    }

    char* end = NULL;
    const long parsed = std::strtol(text.c_str(), &end, 10);
    if (end == NULL || *end != '\0') {
        return false;
    }
    if (parsed <= 0 || parsed > std::numeric_limits<int>::max()) {
        return false;
    }

    *value = static_cast<int>(parsed);
    return true;
}

bool parse_nonnegative_int(const std::string& text, int* value) {
    if (text.empty()) {
        return false;
    }

    char* end = NULL;
    const long parsed = std::strtol(text.c_str(), &end, 10);
    if (end == NULL || *end != '\0') {
        return false;
    }
    if (parsed < 0 || parsed > std::numeric_limits<int>::max()) {
        return false;
    }

    *value = static_cast<int>(parsed);
    return true;
}

bool parse_hex_u32(const std::string& text, unsigned int* value) {
    if (text.empty()) {
        return false;
    }

    char* end = NULL;
    const unsigned long parsed = std::strtoul(text.c_str(), &end, 0);
    if (end == NULL || *end != '\0' || parsed > std::numeric_limits<unsigned int>::max()) {
        return false;
    }

    *value = static_cast<unsigned int>(parsed);
    return true;
}

bool require_value(int argc, int index, const std::string& option, std::ostream& stream) {
    if (index + 1 < argc) {
        return true;
    }

    stream << "missing value for " << option << '\n';
    print_usage(stream);
    return false;
}

}  // namespace

ExportConfig::ExportConfig()
    : output_dir("output"),
      width(512),
      height(256),
      fps(50),
      sample_rate(22050),
      frame_count(250),
      intro_frames_per_row(6),
      intro_rows_per_order(64),
      has_end_song_position(false),
      end_song_position_hex(0U),
      post_roll_frames(0),
      write_log(true),
      sequence_name("intro") {
}

ParseStatus parse_export_config(int argc, char** argv, ExportConfig& config, std::ostream& stream) {
    for (int index = 1; index < argc; ++index) {
        const std::string arg(argv[index]);

        if (arg == "--help" || arg == "-h") {
            print_usage(stream);
            return ParseStatus::help;
        }

        if (arg == "--no-log") {
            config.write_log = false;
            continue;
        }

        if (!require_value(argc, index, arg, stream)) {
            return ParseStatus::error;
        }

        const std::string value(argv[++index]);
        int parsed = 0;

        if (arg == "--output") {
            config.output_dir = value;
        } else if (arg == "--frames") {
            if (!parse_positive_int(value, &parsed)) {
                stream << "invalid frame count: " << value << '\n';
                return ParseStatus::error;
            }
            config.frame_count = parsed;
        } else if (arg == "--width") {
            if (!parse_positive_int(value, &parsed)) {
                stream << "invalid width: " << value << '\n';
                return ParseStatus::error;
            }
            config.width = parsed;
        } else if (arg == "--height") {
            if (!parse_positive_int(value, &parsed)) {
                stream << "invalid height: " << value << '\n';
                return ParseStatus::error;
            }
            config.height = parsed;
        } else if (arg == "--fps") {
            if (!parse_positive_int(value, &parsed)) {
                stream << "invalid fps: " << value << '\n';
                return ParseStatus::error;
            }
            config.fps = parsed;
        } else if (arg == "--sample-rate") {
            if (!parse_positive_int(value, &parsed)) {
                stream << "invalid sample rate: " << value << '\n';
                return ParseStatus::error;
            }
            config.sample_rate = parsed;
        } else if (arg == "--sequence") {
            config.sequence_name = value;
        } else if (arg == "--until-song-position") {
            unsigned int parsed_hex = 0U;
            if (!parse_hex_u32(value, &parsed_hex)) {
                stream << "invalid song position hex: " << value << '\n';
                return ParseStatus::error;
            }
            config.has_end_song_position = true;
            config.end_song_position_hex = parsed_hex;
        } else if (arg == "--post-roll-frames") {
            if (!parse_nonnegative_int(value, &parsed)) {
                stream << "invalid post-roll frame count: " << value << '\n';
                return ParseStatus::error;
            }
            config.post_roll_frames = parsed;
        } else if (arg == "--intro-frames-per-row") {
            if (!parse_positive_int(value, &parsed)) {
                stream << "invalid intro frames per row: " << value << '\n';
                return ParseStatus::error;
            }
            config.intro_frames_per_row = parsed;
        } else if (arg == "--intro-rows-per-order") {
            if (!parse_positive_int(value, &parsed)) {
                stream << "invalid intro rows per order: " << value << '\n';
                return ParseStatus::error;
            }
            config.intro_rows_per_order = parsed;
        } else {
            stream << "unknown option: " << arg << '\n';
            print_usage(stream);
            return ParseStatus::error;
        }
    }

    if (config.sample_rate % config.fps != 0) {
        stream << "sample rate must be divisible by fps for exact sync: "
               << config.sample_rate << " / " << config.fps << '\n';
        return ParseStatus::error;
    }

    return ParseStatus::ok;
}

void print_usage(std::ostream& stream) {
    stream
        << "Usage: forward-export [options]\n"
        << "  --output <dir>        Output directory (default: output)\n"
        << "  --frames <count>      Number of frames to export (default: 250)\n"
        << "  --width <pixels>      Frame width (default: 512)\n"
        << "  --height <pixels>     Frame height (default: 256)\n"
        << "  --fps <rate>          Video frame rate (default: 50)\n"
        << "  --sample-rate <hz>    Audio sample rate (default: 22050)\n"
        << "  --sequence <name>     Export sequence: intro|saari|kukot|maku|watercube|feta|uppol|bootstrap\n"
        << "                        (default: intro)\n"
        << "  --until-song-position <hex>\n"
        << "                        Resolve frame count from the native XM timeline\n"
        << "  --post-roll-frames <n>\n"
        << "                        Extra frames after --until-song-position (default: 0)\n"
        << "  --intro-frames-per-row <n>\n"
        << "                        Legacy wrapper pacing hint kept for compatibility (default: 6)\n"
        << "  --intro-rows-per-order <n>\n"
        << "                        Legacy wrapper order length hint (default: 64)\n"
        << "  --no-log              Skip output/log.txt\n"
        << "  --help                Show this message\n";
}
}  // namespace forward_offline
