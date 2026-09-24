#ifndef FORWARD_OFFLINE_SCENES_CAPTURE_TYPES_H
#define FORWARD_OFFLINE_SCENES_CAPTURE_TYPES_H

#include "scenes/scene3d_shared.h"

namespace forward_offline {

struct CaptureCamera {
    Scene3dVec3 position, target, forward, right, up;
    float focal_length, half_width, half_height;
};

struct CapturePoint {
    Scene3dVec3 position;
    int surface_id;
};

}  // namespace forward_offline
#endif
