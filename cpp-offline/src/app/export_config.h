#ifndef FORWARD_OFFLINE_APP_EXPORT_CONFIG_H
#define FORWARD_OFFLINE_APP_EXPORT_CONFIG_H

#include <iosfwd>
#include <string>

namespace forward_offline {

struct ExportConfig {
    ExportConfig();

    std::string output_dir;
    int width;
    int height;
    int fps;
    int sample_rate;
    int frame_count;
    bool write_log;
};

enum class ParseStatus {
    ok,
    help,
    error
};

ParseStatus parse_export_config(int argc, char** argv, ExportConfig& config, std::ostream& stream);
void print_usage(std::ostream& stream);

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_APP_EXPORT_CONFIG_H
