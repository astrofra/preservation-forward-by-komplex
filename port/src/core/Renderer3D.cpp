#include "Renderer3D.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace forward::core {
namespace {

constexpr float kPi = 3.14159265358979323846f;

Vec3 RotateX(const Vec3& v, float angle) {
  const float s = std::sin(angle);
  const float c = std::cos(angle);
  return Vec3(v.x, v.y * c - v.z * s, v.y * s + v.z * c);
}

Vec3 RotateY(const Vec3& v, float angle) {
  const float s = std::sin(angle);
  const float c = std::cos(angle);
  return Vec3(v.x * c + v.z * s, v.y, -v.x * s + v.z * c);
}

Vec3 RotateZ(const Vec3& v, float angle) {
  const float s = std::sin(angle);
  const float c = std::cos(angle);
  return Vec3(v.x * c - v.y * s, v.x * s + v.y * c, v.z);
}

Vec3 RotateXYZ(const Vec3& v, const Vec3& rotation_radians) {
  return RotateZ(RotateY(RotateX(v, rotation_radians.x), rotation_radians.y),
                 rotation_radians.z);
}

float EdgeFunction(float ax, float ay, float bx, float by, float px, float py) {
  return (px - ax) * (by - ay) - (py - ay) * (bx - ax);
}

uint8_t ChannelR(uint32_t argb) { return static_cast<uint8_t>((argb >> 16u) & 0xFFu); }
uint8_t ChannelG(uint32_t argb) { return static_cast<uint8_t>((argb >> 8u) & 0xFFu); }
uint8_t ChannelB(uint32_t argb) { return static_cast<uint8_t>(argb & 0xFFu); }

uint32_t PackArgb(uint8_t r, uint8_t g, uint8_t b) {
  return (0xFFu << 24u) | (static_cast<uint32_t>(r) << 16u) |
         (static_cast<uint32_t>(g) << 8u) | static_cast<uint32_t>(b);
}

uint32_t ModulateColor(uint32_t base, float intensity) {
  const float clamped = std::clamp(intensity, 0.0f, 1.0f);
  const uint8_t r = static_cast<uint8_t>(std::clamp(ChannelR(base) * clamped, 0.0f, 255.0f));
  const uint8_t g = static_cast<uint8_t>(std::clamp(ChannelG(base) * clamped, 0.0f, 255.0f));
  const uint8_t b = static_cast<uint8_t>(std::clamp(ChannelB(base) * clamped, 0.0f, 255.0f));
  return PackArgb(r, g, b);
}

}  // namespace

Renderer3D::Renderer3D(int target_width, int target_height)
    : target_width_(target_width), target_height_(target_height) {}

void Renderer3D::DrawMesh(Surface32& target,
                          const Mesh& mesh,
                          const Camera& camera,
                          const RenderInstance& instance) {
  if (mesh.Empty()) {
    return;
  }
  EnsureDepthBuffer();
  ClearDepthBuffer();

  const float half_fov = (camera.fov_degrees * (kPi / 180.0f)) * 0.5f;
  const float focal_length = (0.5f * static_cast<float>(target_height_)) / std::tan(half_fov);
  const float center_x = (static_cast<float>(target_width_) - 1.0f) * 0.5f;
  const float center_y = (static_cast<float>(target_height_) - 1.0f) * 0.5f;

  std::vector<ProjectedVertex> projected(mesh.positions.size());
  for (size_t i = 0; i < mesh.positions.size(); ++i) {
    Vec3 v = mesh.positions[i] * instance.uniform_scale;
    v = RotateXYZ(v, instance.rotation_radians);
    v = v + instance.translation;
    v = v - camera.position;
    projected[i].view_pos = v;

    if (v.z <= camera.near_plane) {
      projected[i].visible = false;
      continue;
    }

    const float inv_z = 1.0f / v.z;
    projected[i].fx = center_x + v.x * focal_length * inv_z;
    projected[i].fy = center_y - v.y * focal_length * inv_z;
    projected[i].x = static_cast<int>(std::lround(projected[i].fx));
    projected[i].y = static_cast<int>(std::lround(projected[i].fy));
    projected[i].z = v.z;
    projected[i].visible = true;
  }

  for (const Triangle& tri : mesh.triangles) {
    const ProjectedVertex& a = projected[static_cast<size_t>(tri.a)];
    const ProjectedVertex& b = projected[static_cast<size_t>(tri.b)];
    const ProjectedVertex& c = projected[static_cast<size_t>(tri.c)];
    if (!a.visible || !b.visible || !c.visible) {
      continue;
    }

    if (instance.draw_fill) {
      DrawFilledTriangle(target, a, b, c, instance.fill_color);
    }
    if (instance.draw_wire) {
      DrawLine(target, a.x, a.y, b.x, b.y, instance.wire_color);
      DrawLine(target, b.x, b.y, c.x, c.y, instance.wire_color);
      DrawLine(target, c.x, c.y, a.x, a.y, instance.wire_color);
    }
  }
}

void Renderer3D::EnsureDepthBuffer() {
  const size_t target_size =
      static_cast<size_t>(target_width_) * static_cast<size_t>(target_height_);
  if (depth_buffer_.size() != target_size) {
    depth_buffer_.resize(target_size, std::numeric_limits<float>::infinity());
  }
}

void Renderer3D::ClearDepthBuffer() {
  std::fill(depth_buffer_.begin(), depth_buffer_.end(), std::numeric_limits<float>::infinity());
}

void Renderer3D::DrawFilledTriangle(Surface32& target,
                                    const ProjectedVertex& a,
                                    const ProjectedVertex& b,
                                    const ProjectedVertex& c,
                                    uint32_t fill_color) {
  const float area = EdgeFunction(a.fx, a.fy, b.fx, b.fy, c.fx, c.fy);
  if (std::abs(area) < 1e-6f) {
    return;
  }

  const int min_x = std::max(
      0, static_cast<int>(std::floor(std::min({a.fx, b.fx, c.fx}))));
  const int max_x = std::min(
      target_width_ - 1, static_cast<int>(std::ceil(std::max({a.fx, b.fx, c.fx}))));
  const int min_y = std::max(
      0, static_cast<int>(std::floor(std::min({a.fy, b.fy, c.fy}))));
  const int max_y = std::min(
      target_height_ - 1, static_cast<int>(std::ceil(std::max({a.fy, b.fy, c.fy}))));

  if (min_x > max_x || min_y > max_y) {
    return;
  }

  const Vec3 face_normal =
      (b.view_pos - a.view_pos).Cross(c.view_pos - a.view_pos).Normalized();
  const float light_intensity = 0.20f + 0.80f * std::abs(face_normal.z);
  const uint32_t shaded_color = ModulateColor(fill_color, light_intensity);

  for (int y = min_y; y <= max_y; ++y) {
    const float py = static_cast<float>(y) + 0.5f;
    for (int x = min_x; x <= max_x; ++x) {
      const float px = static_cast<float>(x) + 0.5f;

      const float w0 = EdgeFunction(b.fx, b.fy, c.fx, c.fy, px, py) / area;
      const float w1 = EdgeFunction(c.fx, c.fy, a.fx, a.fy, px, py) / area;
      const float w2 = EdgeFunction(a.fx, a.fy, b.fx, b.fy, px, py) / area;

      if (w0 < 0.0f || w1 < 0.0f || w2 < 0.0f) {
        continue;
      }

      const float z = w0 * a.z + w1 * b.z + w2 * c.z;
      const size_t index =
          static_cast<size_t>(y) * static_cast<size_t>(target_width_) + static_cast<size_t>(x);
      if (z >= depth_buffer_[index]) {
        continue;
      }

      depth_buffer_[index] = z;
      target.SetBackPixel(x, y, shaded_color);
    }
  }
}

void Renderer3D::DrawLine(Surface32& target,
                          int x0,
                          int y0,
                          int x1,
                          int y1,
                          uint32_t color) const {
  int dx = std::abs(x1 - x0);
  int sx = x0 < x1 ? 1 : -1;
  int dy = -std::abs(y1 - y0);
  int sy = y0 < y1 ? 1 : -1;
  int err = dx + dy;

  while (true) {
    target.SetBackPixel(x0, y0, color);
    if (x0 == x1 && y0 == y1) {
      break;
    }
    const int e2 = err * 2;
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    }
  }
}

}  // namespace forward::core
