#pragma once

#include "Vec3.h"

namespace forward::core {

struct Camera {
  Vec3 position{0.0f, 0.0f, 0.0f};
  Vec3 right{1.0f, 0.0f, 0.0f};
  Vec3 up{0.0f, 1.0f, 0.0f};
  Vec3 forward{0.0f, 0.0f, 1.0f};
  float fov_degrees = 70.0f;
  float near_plane = 0.1f;
};

}  // namespace forward::core
