#pragma once

#include <cmath>

namespace forward::core {

struct Vec2 {
  float x = 0.0f;
  float y = 0.0f;

  Vec2() = default;
  Vec2(float x_in, float y_in) : x(x_in), y(y_in) {}

  void Set(float x_in, float y_in) {
    x = x_in;
    y = y_in;
  }

  float LengthSq() const { return x * x + y * y; }

  float Length() const { return std::sqrt(LengthSq()); }

  Vec2 Normalized() const {
    const float len = Length();
    if (len <= 0.0f) {
      return Vec2{};
    }
    return Vec2{x / len, y / len};
  }

  Vec2 operator+(const Vec2& rhs) const { return Vec2{x + rhs.x, y + rhs.y}; }
  Vec2 operator-(const Vec2& rhs) const { return Vec2{x - rhs.x, y - rhs.y}; }
  Vec2 operator*(float scalar) const { return Vec2{x * scalar, y * scalar}; }
};

}  // namespace forward::core
