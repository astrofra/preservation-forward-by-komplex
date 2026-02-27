#pragma once

#include "Vec3.h"

namespace forward::core {

// Mirrors the Java shape of mmjakka: position + transformed/projected fields.
struct Vertex : public Vec3 {
  float tx = 0.0f;
  float ty = 0.0f;
  float tz = 0.0f;

  float sx = 0.0f;
  float sy = 0.0f;
  float sz = 0.0f;

  int clip_flags = 0;
  float intensity = 0.0f;

  Vertex() = default;
  Vertex(float x_in, float y_in, float z_in) : Vec3(x_in, y_in, z_in) {}

  void ResetDerived() {
    tx = 0.0f;
    ty = 0.0f;
    tz = 0.0f;
    sx = 0.0f;
    sy = 0.0f;
    sz = 0.0f;
    clip_flags = 0;
    intensity = 0.0f;
  }
};

}  // namespace forward::core
