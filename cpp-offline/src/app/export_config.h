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
    int intro_frames_per_row;
    int intro_rows_per_order;
    bool write_log;
    std::string sequence_name;
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
