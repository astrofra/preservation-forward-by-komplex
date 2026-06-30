#ifndef FORWARD_OFFLINE_SCENES_ROUTINE_H
#define FORWARD_OFFLINE_SCENES_ROUTINE_H

#include <string>

#include "render/rgb_surface.h"

namespace forward_offline {

class Routine {
public:
    virtual ~Routine() {}

    virtual const char* script_name() const = 0;

    virtual void init() {}
    virtual void on_show() {}
    virtual void dispose() {}
    virtual void render(RgbSurface& surface, float scene_time_seconds, float delta_seconds) = 0;
    virtual void handle_message(const std::string& message, float scene_time_seconds) {
        (void)message;
        (void)scene_time_seconds;
    }
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_SCENES_ROUTINE_H
