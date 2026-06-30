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
      write_log(true) {
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
        << "  --no-log              Skip output/log.txt\n"
        << "  --help                Show this message\n";
}

}  // namespace forward_offline
