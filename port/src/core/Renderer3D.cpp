#include "Renderer3D.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

#include "Image32.h"

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

uint32_t SampleTexture(const Image32& image, float u, float v, bool wrap) {
  if (image.Empty()) {
    return 0xFFFFFFFFu;
  }

  float su = u;
  float sv = v;
  if (wrap) {
    su = su - std::floor(su);
    sv = sv - std::floor(sv);
  } else {
    su = std::clamp(su, 0.0f, 1.0f);
    sv = std::clamp(sv, 0.0f, 1.0f);
  }

  const int x =
      std::clamp(static_cast<int>(su * static_cast<float>(image.width - 1)), 0, image.width - 1);
  const int y = std::clamp(static_cast<int>(sv * static_cast<float>(image.height - 1)),
                           0,
                           image.height - 1);
  return image.pixels[static_cast<size_t>(y) * static_cast<size_t>(image.width) +
                      static_cast<size_t>(x)];
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
  const float focal_length = (0.5f * static_cast<float>(target_width_)) / std::tan(half_fov);
  const float center_x = (static_cast<float>(target_width_) - 1.0f) * 0.5f;
  const float center_y = (static_cast<float>(target_height_) - 1.0f) * 0.5f;

  std::vector<ProjectedVertex> transformed(mesh.positions.size());
  for (size_t i = 0; i < mesh.positions.size(); ++i) {
    Vec3 v = mesh.positions[i] * instance.uniform_scale;
    if (instance.use_basis_rotation) {
      v = instance.basis_x * v.x + instance.basis_y * v.y + instance.basis_z * v.z;
    } else {
      v = RotateXYZ(v, instance.rotation_radians);
    }
    v = v + instance.translation;
    const Vec3 rel = v - camera.position;
    const Vec3 view(rel.Dot(camera.right), rel.Dot(camera.up), rel.Dot(camera.forward));
    transformed[i].view_pos = view;
    transformed[i].z = view.z;

    Vec3 normal = (mesh.normals.size() == mesh.positions.size()) ? mesh.normals[i]
                                                                  : mesh.positions[i].Normalized();
    if (instance.use_basis_rotation) {
      normal =
          (instance.basis_x * normal.x + instance.basis_y * normal.y + instance.basis_z * normal.z)
              .Normalized();
    } else {
      normal = RotateXYZ(normal, instance.rotation_radians).Normalized();
    }
    transformed[i].view_normal = Vec3(normal.Dot(camera.right),
                                      normal.Dot(camera.up),
                                      normal.Dot(camera.forward))
                                     .Normalized();

    if (instance.texture) {
      if (instance.use_mesh_uv && mesh.texcoords.size() == mesh.positions.size()) {
        transformed[i].u = mesh.texcoords[i].x;
        transformed[i].v = mesh.texcoords[i].y;
      } else {
        // Use smoothed normals for fake phong/env map UVs.
        const Vec3 n = transformed[i].view_normal;
        transformed[i].u = 0.5f + 0.5f * n.x;
        transformed[i].v = 0.5f - 0.5f * n.y;
      }
    }
  }

  const float winding_sign = ComputeMeshWindingSign(mesh);

  for (const Triangle& tri : mesh.triangles) {
    const ProjectedVertex& a = transformed[static_cast<size_t>(tri.a)];
    const ProjectedVertex& b = transformed[static_cast<size_t>(tri.b)];
    const ProjectedVertex& c = transformed[static_cast<size_t>(tri.c)];

    std::vector<ProjectedVertex> clipped =
        ClipTriangleAgainstNearPlane(a, b, c, camera.near_plane);
    if (clipped.size() < 3) {
      continue;
    }

    if (instance.enable_backface_culling &&
        !IsFrontFacing(clipped[0], clipped[1], clipped[2], winding_sign)) {
      continue;
    }

    for (ProjectedVertex& v : clipped) {
      const float inv_z = 1.0f / v.view_pos.z;
      v.fx = center_x + v.view_pos.x * focal_length * inv_z;
      v.fy = center_y - v.view_pos.y * focal_length * inv_z;
      v.x = static_cast<int>(std::lround(v.fx));
      v.y = static_cast<int>(std::lround(v.fy));
      v.z = v.view_pos.z;
      v.visible = true;
    }

    if (instance.draw_fill) {
      for (size_t i = 1; i + 1 < clipped.size(); ++i) {
        DrawFilledTriangle(target, clipped[0], clipped[i], clipped[i + 1], instance);
      }
    }

    if (instance.draw_wire) {
      for (size_t i = 0; i < clipped.size(); ++i) {
        const ProjectedVertex& p0 = clipped[i];
        const ProjectedVertex& p1 = clipped[(i + 1) % clipped.size()];
        DrawLine(target, p0.x, p0.y, p1.x, p1.y, instance.wire_color);
      }
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

float Renderer3D::ComputeMeshWindingSign(const Mesh& mesh) const {
  float accum = 0.0f;
  for (const Triangle& tri : mesh.triangles) {
    const Vec3& a = mesh.positions[static_cast<size_t>(tri.a)];
    const Vec3& b = mesh.positions[static_cast<size_t>(tri.b)];
    const Vec3& c = mesh.positions[static_cast<size_t>(tri.c)];
    const Vec3 n = (b - a).Cross(c - a);
    const Vec3 centroid = (a + b + c) * (1.0f / 3.0f);
    accum += n.Dot(centroid);
  }
  return (accum >= 0.0f) ? 1.0f : -1.0f;
}

std::vector<Renderer3D::ProjectedVertex> Renderer3D::ClipTriangleAgainstNearPlane(
    const ProjectedVertex& a,
    const ProjectedVertex& b,
    const ProjectedVertex& c,
    float near_plane) const {
  auto inside = [near_plane](const ProjectedVertex& v) { return v.view_pos.z >= near_plane; };
  auto intersect = [near_plane](const ProjectedVertex& s, const ProjectedVertex& e) {
    ProjectedVertex out;
    const float dz = e.view_pos.z - s.view_pos.z;
    const float t = (std::abs(dz) <= 1e-6f)
                        ? 0.0f
                        : std::clamp((near_plane - s.view_pos.z) / dz, 0.0f, 1.0f);
    out.view_pos = s.view_pos + (e.view_pos - s.view_pos) * t;
    out.view_pos.z = near_plane;
    out.z = out.view_pos.z;
    out.view_normal = (s.view_normal + (e.view_normal - s.view_normal) * t).Normalized();
    out.u = s.u + (e.u - s.u) * t;
    out.v = s.v + (e.v - s.v) * t;
    return out;
  };

  std::vector<ProjectedVertex> input = {a, b, c};
  std::vector<ProjectedVertex> output;
  output.reserve(4);

  for (size_t i = 0; i < input.size(); ++i) {
    const ProjectedVertex& s = input[i];
    const ProjectedVertex& e = input[(i + 1) % input.size()];
    const bool s_inside = inside(s);
    const bool e_inside = inside(e);

    if (s_inside && e_inside) {
      output.push_back(e);
    } else if (s_inside && !e_inside) {
      output.push_back(intersect(s, e));
    } else if (!s_inside && e_inside) {
      output.push_back(intersect(s, e));
      output.push_back(e);
    }
  }
  return output;
}

bool Renderer3D::IsFrontFacing(const ProjectedVertex& a,
                               const ProjectedVertex& b,
                               const ProjectedVertex& c,
                               float winding_sign) const {
  const Vec3 n = (b.view_pos - a.view_pos).Cross(c.view_pos - a.view_pos);
  const Vec3 centroid = (a.view_pos + b.view_pos + c.view_pos) * (1.0f / 3.0f);
  const float signed_facing = n.Dot(centroid) * winding_sign;
  return signed_facing < 0.0f;
}

void Renderer3D::DrawFilledTriangle(Surface32& target,
                                    const ProjectedVertex& a,
                                    const ProjectedVertex& b,
                                    const ProjectedVertex& c,
                                    const RenderInstance& instance) {
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

      uint32_t base_color = instance.fill_color;
      if (instance.texture) {
        const float u = w0 * a.u + w1 * b.u + w2 * c.u;
        const float v = w0 * a.v + w1 * b.v + w2 * c.v;
        base_color = SampleTexture(*instance.texture, u, v, instance.texture_wrap);
      }
      const Vec3 interp_normal =
          (a.view_normal * w0 + b.view_normal * w1 + c.view_normal * w2).Normalized();
      const float ndotv = std::abs(interp_normal.z);
      const float light_intensity = instance.texture_unlit
                                        ? 1.0f
                                        : (instance.texture ? (0.78f + 0.22f * ndotv)
                                                            : (0.22f + 0.78f * ndotv));
      const uint32_t shaded_color = ModulateColor(base_color, light_intensity);
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
