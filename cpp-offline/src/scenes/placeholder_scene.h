#ifndef FORWARD_OFFLINE_SCENES_PLACEHOLDER_SCENE_H
#define FORWARD_OFFLINE_SCENES_PLACEHOLDER_SCENE_H

#include "scenes/scene.h"

namespace forward_offline {

class PlaceholderScene : public Scene {
public:
    PlaceholderScene();

    virtual const char* script_name() const;
    virtual void on_show();
    virtual void render(RgbSurface& surface, float scene_time_seconds, float delta_seconds);

private:
    float phase_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_SCENES_PLACEHOLDER_SCENE_H
