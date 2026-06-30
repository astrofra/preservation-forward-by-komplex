#ifndef FORWARD_OFFLINE_APP_MANIFEST_WRITER_H
#define FORWARD_OFFLINE_APP_MANIFEST_WRITER_H

#include <cstdint>
#include <fstream>
#include <string>

namespace forward_offline {

class ManifestWriter {
public:
    ManifestWriter();

    bool open(const std::string& path, std::string* error_message);
    void write_frame(std::uint32_t capture_index,
                     std::uint32_t render_frame,
                     std::uint64_t demo_time_ms,
                     std::uint64_t scene_time_ms,
                     const std::string& scene,
                     const std::string& next_script_time_hex,
                     const std::string& frame_path);
    void close();

private:
    static std::string csv_escape(const std::string& value);
    static std::string seconds_string(std::uint64_t time_ms);

    std::ofstream stream_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_APP_MANIFEST_WRITER_H
