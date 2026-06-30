#ifndef FORWARD_OFFLINE_APP_FORWARD_APP_H
#define FORWARD_OFFLINE_APP_FORWARD_APP_H

#include <string>

#include "app/export_config.h"
#include "core/offline_timeline.h"
#include "render/rgb_surface.h"
#include "scenes/placeholder_scene.h"

namespace forward_offline {

class ForwardApp {
public:
    explicit ForwardApp(const ExportConfig& config);

    int run();

private:
    bool prepare_output(std::string* error_message) const;
    bool write_log(std::string* error_message) const;
    std::string frame_file_name(unsigned int frame_index) const;

    ExportConfig config_;
    OfflineTimeline timeline_;
    RgbSurface frame_buffer_;
    PlaceholderScene scene_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_APP_FORWARD_APP_H
