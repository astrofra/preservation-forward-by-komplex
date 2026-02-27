#pragma once

#include <cmath>

namespace forward::core {

struct Vec3 {
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;

  Vec3() = default;
  Vec3(float x_in, float y_in, float z_in) : x(x_in), y(y_in), z(z_in) {}

  void Set(float x_in, float y_in, float z_in) {
    x = x_in;
    y = y_in;
    z = z_in;
  }

  float Dot(const Vec3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }

  Vec3 Cross(const Vec3& rhs) const {
    return Vec3{
        y * rhs.z - z * rhs.y,
        z * rhs.x - x * rhs.z,
        x * rhs.y - y * rhs.x,
    };
  }

  float LengthSq() const { return x * x + y * y + z * z; }

  float Length() const { return std::sqrt(LengthSq()); }

  Vec3 Normalized() const {
    const float len = Length();
    if (len <= 0.0f) {
      return Vec3{};
    }
    return Vec3{x / len, y / len, z / len};
  }

  Vec3 operator+(const Vec3& rhs) const {
    return Vec3{x + rhs.x, y + rhs.y, z + rhs.z};
  }

  Vec3 operator-(const Vec3& rhs) const {
    return Vec3{x - rhs.x, y - rhs.y, z - rhs.z};
  }

  Vec3 operator*(float scalar) const { return Vec3{x * scalar, y * scalar, z * scalar}; }
};

}  // namespace forward::core
