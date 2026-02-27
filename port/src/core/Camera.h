#pragma once

#include "Vec3.h"

namespace forward::core {

struct Camera {
  Vec3 position{0.0f, 0.0f, 0.0f};
  float fov_degrees = 70.0f;
  float near_plane = 0.1f;
};

}  // namespace forward::core
