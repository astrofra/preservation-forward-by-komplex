#include <SDL.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "core/Camera.h"
#include "core/Image32.h"
#include "core/Mesh.h"
#include "core/MeshLoaderIgu.h"
#include "core/Renderer3D.h"
#include "core/Surface32.h"
#include "core/Vec3.h"

namespace {

using forward::core::Camera;
using forward::core::Image32;
using forward::core::Mesh;
using forward::core::RenderInstance;
using forward::core::Renderer3D;
using forward::core::Surface32;
using forward::core::Vec3;

constexpr int kLogicalWidth = 512;
constexpr int kLogicalHeight = 256;
constexpr int kWindowScale = 1;  // 1x1 mode only
constexpr double kTickHz = 50.0;
constexpr double kTickDtSeconds = 1.0 / kTickHz;
constexpr float kPi = 3.14159265358979323846f;

struct RuntimeStats {
  uint64_t rendered_frames = 0;
  uint64_t simulated_ticks = 0;
};

enum class SceneMode {
  kMute95,
  kDomina,
  kMute95DominaSequence,
  kSaari,
  kFeta,
};

struct DemoState {
  double timeline_seconds = 0.0;
  double scene_start_seconds = 0.0;
  bool paused = false;
  bool fullscreen = false;
  bool show_post = false;
  float feta_fov_degrees = 84.0f;  // horizontal FOV
  SceneMode scene_mode = SceneMode::kFeta;
  std::string scene_label;
  std::string mesh_label;
  std::string post_label;
};

struct QuickWinPostLayer {
  Image32 primary;
  Image32 secondary;
  bool enabled = false;
};

struct FetaSceneAssets {
  Image32 babyenv;
  Image32 flare;
  bool enabled = false;
};

struct KaaakmaBackgroundPass {
  Mesh mesh;
  Image32 texture;
  bool enabled = false;
};

struct Particle {
  Vec3 position;
  float size = 1.0f;
  float energy = 1.0f;
};

struct MmaamkaParticlePass {
  Image32 flare;
  std::vector<Particle> particles;
  double last_timeline_seconds = 0.0;
  uint32_t rng_state = 0x1998u;
  bool initialized = false;
  bool enabled = false;
};

struct Mute95CreditPair {
  Image32 first;
  Image32 second;
};

struct Mute95SceneAssets {
  std::array<Mute95CreditPair, 5> credits;
  std::array<uint32_t, 256> palette{};
  bool enabled = false;
};

struct Mute95Runtime {
  int cell_w = 8;
  int cell_h = 8;
  int cols = 0;
  int rows = 0;
  std::vector<float> flow_x;
  std::vector<float> flow_y;
  std::vector<uint8_t> buffer_a;
  std::vector<uint8_t> buffer_b;
  bool current_is_a = true;
  int frame_counter = 0;
  int active_credit = -1;
  int cue_step = -1;
  double credit_start_seconds = -1.0;
  double prev_scene_seconds = 0.0;
  uint64_t java_random_state = 0u;
  bool initialized = false;
};

struct DominaSceneAssets {
  Image32 phorward;
  Image32 komplex;
  bool use_komplex = false;
  bool enabled = false;
};

struct DominaRuntime {
  int frame_counter = 0;
  bool fade_to_black = false;
  double fade_start_seconds = 0.0;
  bool initialized = false;
};

struct Quat {
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
  float w = 1.0f;
};

struct SaariSceneAssets {
  struct TrackKey {
    double time_ms = 0.0;
    Vec3 value;
  };
  struct RotTrackKey {
    double time_ms = 0.0;
    Quat value;
  };
  struct AnimatedObject {
    std::string name;
    Mesh mesh;
    Vec3 base_position;
    Quat base_rotation;
    std::vector<TrackKey> position_track;
    std::vector<RotTrackKey> rotation_track;
  };

  Mesh terrain;
  Image32 terrain_texture;
  Mesh backdrop_mesh;
  Image32 backdrop_texture;
  float backdrop_scale = 1.0f;
  float camera_fov_degrees = 80.0f;
  std::vector<TrackKey> camera_track;
  std::vector<TrackKey> target_track;
  std::vector<AnimatedObject> animated_objects;
  bool enabled = false;
};

struct SaariRuntime {
  std::vector<uint32_t> noise_lut;
  std::vector<int> scanline_order;
  uint32_t rng_state = 0x53414152u;  // "SAAR"
  float shock_percent = 0.0f;
  float shock_decay = 0.0f;
  double prev_scene_seconds = 0.0;
  bool initial_suh0_sent = false;
  bool first_suh_sent = false;
  bool initialized = false;
};

uint32_t PackArgb(uint8_t r, uint8_t g, uint8_t b) {
  return (0xFFu << 24u) | (static_cast<uint32_t>(r) << 16u) |
         (static_cast<uint32_t>(g) << 8u) | static_cast<uint32_t>(b);
}

uint8_t UnpackR(uint32_t argb) { return static_cast<uint8_t>((argb >> 16u) & 0xFFu); }
uint8_t UnpackG(uint32_t argb) { return static_cast<uint8_t>((argb >> 8u) & 0xFFu); }
uint8_t UnpackB(uint32_t argb) { return static_cast<uint8_t>(argb & 0xFFu); }

SDL_Rect ComputePresentationRect(SDL_Renderer* renderer) {
  int output_w = 0;
  int output_h = 0;
  SDL_GetRendererOutputSize(renderer, &output_w, &output_h);

  const int scale_x = std::max(1, output_w / kLogicalWidth);
  const int scale_y = std::max(1, output_h / kLogicalHeight);
  const int scale = std::max(1, std::min(scale_x, scale_y));

  const int out_w = kLogicalWidth * scale;
  const int out_h = kLogicalHeight * scale;
  const int out_x = (output_w - out_w) / 2;
  const int out_y = (output_h - out_h) / 2;

  return SDL_Rect{out_x, out_y, out_w, out_h};
}

std::string ResolveMeshPath() {
  const std::array<std::string, 3> mesh_names = {
      "meshes/fetus.igu", "meshes/half8.igu", "meshes/octa8.igu"};
  std::error_code ec;
  std::filesystem::path cursor = std::filesystem::current_path(ec);
  if (ec) {
    return {};
  }

  while (true) {
    for (const std::string& mesh_name : mesh_names) {
      const std::filesystem::path candidate = cursor / "original" / "forward" / mesh_name;
      if (std::filesystem::exists(candidate, ec) && !ec) {
        return candidate.string();
      }
    }

    const std::filesystem::path parent = cursor.parent_path();
    if (parent == cursor) {
      break;
    }
    cursor = parent;
  }

  for (const std::string& mesh_name : mesh_names) {
    const std::filesystem::path candidate =
        std::filesystem::path("original") / "forward" / mesh_name;
    std::error_code ec;
    if (std::filesystem::exists(candidate, ec) && !ec) {
      return candidate.string();
    }
  }
  return {};
}

std::string ResolveForwardAssetPath(const std::string& relative_path) {
  std::error_code ec;
  std::filesystem::path cursor = std::filesystem::current_path(ec);
  if (ec) {
    return {};
  }

  while (true) {
    const std::filesystem::path candidate =
        cursor / "original" / "forward" / relative_path;
    if (std::filesystem::exists(candidate, ec) && !ec) {
      return candidate.string();
    }

    const std::filesystem::path parent = cursor.parent_path();
    if (parent == cursor) {
      break;
    }
    cursor = parent;
  }

  const std::filesystem::path candidate =
      std::filesystem::path("original") / "forward" / relative_path;
  std::error_code ec2;
  if (std::filesystem::exists(candidate, ec2) && !ec2) {
    return candidate.string();
  }
  return {};
}

template <size_t N>
std::string ResolveFirstExistingForwardPath(const std::array<std::string, N>& relative_paths) {
  for (const std::string& path : relative_paths) {
    const std::string resolved = ResolveForwardAssetPath(path);
    if (!resolved.empty()) {
      return resolved;
    }
  }
  return {};
}

bool LoadForwardImage(const std::string& relative_path, Image32* out_image, std::string* out_error) {
  const std::string path = ResolveForwardAssetPath(relative_path);
  if (path.empty()) {
    if (out_error) {
      *out_error = "asset not found: " + relative_path;
    }
    return false;
  }
  return forward::core::LoadImage32(path, *out_image, out_error);
}

Image32 ExtractTopHalf(const Image32& src) {
  if (src.Empty() || src.height < 2) {
    return {};
  }
  const int out_h = src.height / 2;
  Image32 out;
  out.width = src.width;
  out.height = out_h;
  out.pixels.resize(static_cast<size_t>(out.width) * static_cast<size_t>(out.height));
  for (int y = 0; y < out_h; ++y) {
    const size_t src_row = static_cast<size_t>(y) * static_cast<size_t>(src.width);
    const size_t dst_row = static_cast<size_t>(y) * static_cast<size_t>(out.width);
    for (int x = 0; x < out.width; ++x) {
      out.pixels[dst_row + static_cast<size_t>(x)] = src.pixels[src_row + static_cast<size_t>(x)];
    }
  }
  return out;
}

Image32 ExtractRect(const Image32& src, int x, int y, int w, int h) {
  if (src.Empty() || w <= 0 || h <= 0) {
    return {};
  }
  const int sx = std::clamp(x, 0, src.width - 1);
  const int sy = std::clamp(y, 0, src.height - 1);
  const int ex = std::clamp(x + w, 0, src.width);
  const int ey = std::clamp(y + h, 0, src.height);
  const int out_w = std::max(0, ex - sx);
  const int out_h = std::max(0, ey - sy);
  if (out_w <= 0 || out_h <= 0) {
    return {};
  }

  Image32 out;
  out.width = out_w;
  out.height = out_h;
  out.pixels.resize(static_cast<size_t>(out_w) * static_cast<size_t>(out_h));
  for (int row = 0; row < out_h; ++row) {
    const size_t src_row = static_cast<size_t>(sy + row) * static_cast<size_t>(src.width);
    const size_t dst_row = static_cast<size_t>(row) * static_cast<size_t>(out_w);
    for (int col = 0; col < out_w; ++col) {
      out.pixels[dst_row + static_cast<size_t>(col)] =
          src.pixels[src_row + static_cast<size_t>(sx + col)];
    }
  }
  return out;
}

bool BuildSaariTerrainMeshFromHeightmap(const Image32& heightmap, Mesh* out_mesh) {
  if (!out_mesh || heightmap.Empty() || heightmap.width < 2 || heightmap.height < 2) {
    return false;
  }

  const int w = heightmap.width;
  const int h = heightmap.height;
  const float step = 200.0f / static_cast<float>(w);   // maajmka.KAmAjAK(..., 200.0f / n3, 0.16f, ...)
  const float height_scale = 0.16f;

  out_mesh->Clear();
  out_mesh->positions.reserve(static_cast<size_t>(w) * static_cast<size_t>(h));
  out_mesh->texcoords.reserve(static_cast<size_t>(w) * static_cast<size_t>(h));
  out_mesh->triangles.reserve(static_cast<size_t>(w - 1) * static_cast<size_t>(h - 1) * 2u);

  for (int gy = 0; gy < h; ++gy) {
    for (int gx = 0; gx < w; ++gx) {
      const size_t idx = static_cast<size_t>(gy) * static_cast<size_t>(w) + static_cast<size_t>(gx);
      const uint8_t r = UnpackR(heightmap.pixels[idx]);
      const float hgt = static_cast<float>(std::max(0, static_cast<int>(r) - 16)) * height_scale;

      const float px = -static_cast<float>(gx - w / 2) * step;
      const float pz = static_cast<float>(gy - h / 2) * step;
      out_mesh->positions.emplace_back(px, hgt, pz);

      // kmjakmk uses repeating UVs derived from grid indices.
      const float u = static_cast<float>(gx) * (1.0f / static_cast<float>(w));
      const float v = -static_cast<float>(gy) * (1.0f / static_cast<float>(h));
      out_mesh->texcoords.emplace_back(u, v);
    }
  }

  for (int gy = 0; gy < h - 1; ++gy) {
    for (int gx = 0; gx < w - 1; ++gx) {
      const int a = gy * w + gx;
      const int b = gy * w + gx + 1;
      const int c = (gy + 1) * w + gx;
      const int d = (gy + 1) * w + gx + 1;
      out_mesh->triangles.push_back({a, d, b});
      out_mesh->triangles.push_back({d, a, c});
    }
  }
  out_mesh->RebuildVertexNormals();
  return !out_mesh->Empty();
}

std::vector<std::string> SplitWhitespace(std::string line) {
  for (char& c : line) {
    if (c == '\t') {
      c = ' ';
    }
  }
  std::istringstream iss(line);
  std::vector<std::string> out;
  std::string token;
  while (iss >> token) {
    out.push_back(token);
  }
  return out;
}

int CountChar(const std::string& s, char ch) {
  return static_cast<int>(std::count(s.begin(), s.end(), ch));
}

std::string ExtractQuoted(const std::string& line) {
  const size_t first_quote = line.find('"');
  const size_t last_quote = line.rfind('"');
  if (first_quote == std::string::npos || last_quote == std::string::npos ||
      last_quote <= first_quote) {
    return {};
  }
  return line.substr(first_quote + 1, last_quote - first_quote - 1);
}

Quat QuatNormalize(const Quat& q) {
  const float len_sq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
  if (len_sq <= 1e-12f) {
    return {};
  }
  const float inv_len = 1.0f / std::sqrt(len_sq);
  return Quat{q.x * inv_len, q.y * inv_len, q.z * inv_len, q.w * inv_len};
}

Quat QuatConjugate(const Quat& q) { return Quat{-q.x, -q.y, -q.z, q.w}; }

Quat QuatMul(const Quat& a, const Quat& b) {
  return Quat{
      a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
      a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
      a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
      a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
  };
}

Quat QuatFromAxisAngle(const Vec3& axis, float angle_radians) {
  Vec3 n = axis.Normalized();
  if (n.LengthSq() <= 1e-12f) {
    n = Vec3(0.0f, 0.0f, 1.0f);
  }
  const float half = angle_radians * 0.5f;
  const float s = std::sin(half);
  return QuatNormalize(Quat{n.x * s, n.y * s, n.z * s, std::cos(half)});
}

Quat BuildSaariKlunssiScriptedRotation(float t_seconds) {
  // Java maajmka: identity, then X/Y/Z matrix rotations with f/3, 2f/3, 3f/3.
  const Quat qx = QuatFromAxisAngle(Vec3(1.0f, 0.0f, 0.0f), t_seconds / 3.0f);
  const Quat qy = QuatFromAxisAngle(Vec3(0.0f, 1.0f, 0.0f), t_seconds * (2.0f / 3.0f));
  const Quat qz = QuatFromAxisAngle(Vec3(0.0f, 0.0f, 1.0f), t_seconds);
  return QuatNormalize(QuatMul(qz, QuatMul(qy, qx)));
}

Vec3 RotateByQuat(const Vec3& v, const Quat& q) {
  const Quat p{v.x, v.y, v.z, 0.0f};
  const Quat out = QuatMul(QuatMul(q, p), QuatConjugate(q));
  return Vec3(out.x, out.y, out.z);
}

Quat QuatSlerp(const Quat& a_in, const Quat& b_in, float t) {
  Quat a = QuatNormalize(a_in);
  Quat b = QuatNormalize(b_in);
  float cos_theta = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
  if (cos_theta < 0.0f) {
    b.x = -b.x;
    b.y = -b.y;
    b.z = -b.z;
    b.w = -b.w;
    cos_theta = -cos_theta;
  }

  if (cos_theta > 0.9995f) {
    const Quat lerped{
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        a.w + (b.w - a.w) * t,
    };
    return QuatNormalize(lerped);
  }

  const float theta = std::acos(std::clamp(cos_theta, -1.0f, 1.0f));
  const float sin_theta = std::sin(theta);
  if (std::abs(sin_theta) <= 1e-6f) {
    return a;
  }
  const float w0 = std::sin((1.0f - t) * theta) / sin_theta;
  const float w1 = std::sin(t * theta) / sin_theta;
  return Quat{
      a.x * w0 + b.x * w1,
      a.y * w0 + b.y * w1,
      a.z * w0 + b.z * w1,
      a.w * w0 + b.w * w1,
  };
}

void SetRenderInstanceBasisFromQuat(RenderInstance& instance, const Quat& q_in) {
  const Quat q = QuatNormalize(q_in);
  const float xx = q.x * q.x;
  const float yy = q.y * q.y;
  const float zz = q.z * q.z;
  const float xy = q.x * q.y;
  const float xz = q.x * q.z;
  const float yz = q.y * q.z;
  const float wx = q.w * q.x;
  const float wy = q.w * q.y;
  const float wz = q.w * q.z;

  const float m00 = 1.0f - 2.0f * (yy + zz);
  const float m01 = 2.0f * (xy - wz);
  const float m02 = 2.0f * (xz + wy);
  const float m10 = 2.0f * (xy + wz);
  const float m11 = 1.0f - 2.0f * (xx + zz);
  const float m12 = 2.0f * (yz - wx);
  const float m20 = 2.0f * (xz - wy);
  const float m21 = 2.0f * (yz + wx);
  const float m22 = 1.0f - 2.0f * (xx + yy);

  // Columns are local axes in world space.
  instance.basis_x = Vec3(m00, m10, m20);
  instance.basis_y = Vec3(m01, m11, m21);
  instance.basis_z = Vec3(m02, m12, m22);
  instance.use_basis_rotation = true;
}

Quat SampleSaariRotationTrackAtMs(const std::vector<SaariSceneAssets::RotTrackKey>& track,
                                  double t_ms,
                                  const Quat& fallback) {
  if (track.empty()) {
    return fallback;
  }
  if (t_ms <= track.front().time_ms) {
    return track.front().value;
  }
  if (t_ms >= track.back().time_ms) {
    return track.back().value;
  }

  auto it = std::upper_bound(
      track.begin(), track.end(), t_ms, [](double t, const SaariSceneAssets::RotTrackKey& key) {
        return t < key.time_ms;
      });
  if (it == track.begin()) {
    return it->value;
  }
  const SaariSceneAssets::RotTrackKey& b = *it;
  const SaariSceneAssets::RotTrackKey& a = *(it - 1);
  const double dt = std::max(1e-6, b.time_ms - a.time_ms);
  const float f = static_cast<float>((t_ms - a.time_ms) / dt);
  return QuatSlerp(a.value, b.value, std::clamp(f, 0.0f, 1.0f));
}

bool ParseSaariAseCameraTracks(const std::string& path,
                               std::vector<SaariSceneAssets::TrackKey>* out_camera,
                               std::vector<SaariSceneAssets::TrackKey>* out_target,
                               float* out_fov_degrees) {
  if (!out_camera || !out_target) {
    return false;
  }
  std::ifstream input(path);
  if (!input.is_open()) {
    return false;
  }

  out_camera->clear();
  out_target->clear();
  bool in_tm_animation = false;
  int tm_depth = 0;
  std::string active_node;
  std::string line;

  while (std::getline(input, line)) {
    const std::vector<std::string> tokens = SplitWhitespace(line);
    if (tokens.empty()) {
      if (in_tm_animation) {
        tm_depth += CountChar(line, '{');
        tm_depth -= CountChar(line, '}');
        if (tm_depth <= 0) {
          in_tm_animation = false;
          active_node.clear();
        }
      }
      continue;
    }

    if (tokens[0] == "*CAMERA_FOV" && tokens.size() >= 2 && out_fov_degrees) {
      const float fov_rad = std::stof(tokens[1]);
      *out_fov_degrees = fov_rad * (180.0f / kPi);
    }

    if (tokens[0] == "*TM_ANIMATION") {
      in_tm_animation = true;
      tm_depth = 0;
      active_node.clear();
    }

    if (in_tm_animation && tokens[0] == "*NODE_NAME") {
      const size_t first_quote = line.find('"');
      const size_t last_quote = line.rfind('"');
      if (first_quote != std::string::npos && last_quote != std::string::npos &&
          last_quote > first_quote) {
        active_node = line.substr(first_quote + 1, last_quote - first_quote - 1);
      }
    }

    if (in_tm_animation && tokens[0] == "*CONTROL_POS_SAMPLE" && tokens.size() >= 5) {
      const double time_ms = std::stod(tokens[1]);
      const float x = std::stof(tokens[2]);
      const float y = std::stof(tokens[3]);
      const float z = std::stof(tokens[4]);
      if (active_node == "Camera01") {
        out_camera->push_back({time_ms, Vec3(x, y, z)});
      } else if (active_node == "Camera01.Target") {
        out_target->push_back({time_ms, Vec3(x, y, z)});
      }
    }

    if (in_tm_animation) {
      tm_depth += CountChar(line, '{');
      tm_depth -= CountChar(line, '}');
      if (tm_depth <= 0) {
        in_tm_animation = false;
        active_node.clear();
      }
    }
  }

  return !out_camera->empty() && !out_target->empty();
}

bool ParseSaariAseObjects(const std::string& path,
                          std::vector<SaariSceneAssets::AnimatedObject>* out_objects) {
  if (!out_objects) {
    return false;
  }
  std::ifstream input(path);
  if (!input.is_open()) {
    return false;
  }

  struct Face {
    int a = 0;
    int b = 0;
    int c = 0;
  };
  struct RotDeltaKey {
    double time_ms = 0.0;
    Vec3 axis;
    float angle = 0.0f;
  };
  struct RawObject {
    std::string name;
    Vec3 tm_pos;
    Vec3 tm_rot_axis{0.0f, 0.0f, 1.0f};
    float tm_rot_angle = 0.0f;
    std::vector<Vec3> vertices_world;
    std::vector<Face> faces;
    std::vector<SaariSceneAssets::TrackKey> pos_track;
    std::vector<RotDeltaKey> rot_track_delta;
  };

  auto ensure_vec3_size = [](std::vector<Vec3>* v, int idx) {
    if (idx < 0 || !v) {
      return;
    }
    const size_t need = static_cast<size_t>(idx + 1);
    if (v->size() < need) {
      v->resize(need);
    }
  };
  auto ensure_face_size = [](std::vector<Face>* v, int idx) {
    if (idx < 0 || !v) {
      return;
    }
    const size_t need = static_cast<size_t>(idx + 1);
    if (v->size() < need) {
      v->resize(need);
    }
  };

  auto parse_face = [](const std::vector<std::string>& tokens, Face* out_face) -> bool {
    if (!out_face || tokens.empty()) {
      return false;
    }
    bool has_a = false;
    bool has_b = false;
    bool has_c = false;
    for (size_t i = 0; i + 1 < tokens.size(); ++i) {
      if (tokens[i] == "A") {
        out_face->a = std::stoi(tokens[i + 1]);
        has_a = true;
      } else if (tokens[i] == "B") {
        out_face->b = std::stoi(tokens[i + 1]);
        has_b = true;
      } else if (tokens[i] == "C") {
        out_face->c = std::stoi(tokens[i + 1]);
        has_c = true;
      }
    }
    return has_a && has_b && has_c;
  };

  auto finalize_object = [&](RawObject&& raw) {
    if (raw.name.empty() || raw.vertices_world.empty() || raw.faces.empty()) {
      return;
    }
    if (raw.name != "meditate" && raw.name != "klunssi") {
      return;
    }

    SaariSceneAssets::AnimatedObject out;
    out.name = raw.name;
    out.base_position = raw.tm_pos;
    out.base_rotation = QuatFromAxisAngle(raw.tm_rot_axis, raw.tm_rot_angle);

    const Quat inv_base_rot = QuatConjugate(out.base_rotation);
    out.mesh.positions.reserve(raw.vertices_world.size());
    for (const Vec3& p_world : raw.vertices_world) {
      const Vec3 local = RotateByQuat(p_world - raw.tm_pos, inv_base_rot);
      out.mesh.positions.push_back(local);
    }
    out.mesh.triangles.reserve(raw.faces.size());
    for (const Face& f : raw.faces) {
      if (f.a < 0 || f.b < 0 || f.c < 0) {
        continue;
      }
      if (f.a >= static_cast<int>(out.mesh.positions.size()) ||
          f.b >= static_cast<int>(out.mesh.positions.size()) ||
          f.c >= static_cast<int>(out.mesh.positions.size())) {
        continue;
      }
      out.mesh.triangles.push_back({f.a, f.b, f.c});
    }
    out.mesh.RebuildVertexNormals();
    if (out.mesh.Empty()) {
      return;
    }

    std::sort(raw.pos_track.begin(), raw.pos_track.end(), [](const auto& a, const auto& b) {
      return a.time_ms < b.time_ms;
    });
    out.position_track = std::move(raw.pos_track);

    std::sort(raw.rot_track_delta.begin(),
              raw.rot_track_delta.end(),
              [](const auto& a, const auto& b) { return a.time_ms < b.time_ms; });
    if (!raw.rot_track_delta.empty()) {
      Quat accum{0.0f, 0.0f, 0.0f, 1.0f};
      out.rotation_track.reserve(raw.rot_track_delta.size());
      for (const RotDeltaKey& key : raw.rot_track_delta) {
        const Quat delta = QuatFromAxisAngle(key.axis, key.angle);
        // Java path accumulates rot samples by multiplying current sample with previous absolute.
        accum = QuatNormalize(QuatMul(delta, accum));
        out.rotation_track.push_back({key.time_ms, accum});
      }
    }

    out_objects->push_back(std::move(out));
  };

  out_objects->clear();
  RawObject current;

  bool in_geom = false;
  int geom_depth = 0;

  bool in_node_tm = false;
  int node_tm_depth = 0;

  bool in_mesh = false;
  int mesh_depth = 0;
  bool in_vertex_list = false;
  int vertex_list_depth = 0;
  bool in_face_list = false;
  int face_list_depth = 0;

  bool in_tm_animation = false;
  int tm_animation_depth = 0;
  std::string active_track_node;

  std::string line;
  while (std::getline(input, line)) {
    const int brace_delta = CountChar(line, '{') - CountChar(line, '}');
    const std::vector<std::string> tokens = SplitWhitespace(line);
    std::string line_colon = line;
    std::replace(line_colon.begin(), line_colon.end(), ':', ' ');
    const std::vector<std::string> tokens_colon = SplitWhitespace(line_colon);

    if (!in_geom) {
      if (!tokens.empty() && tokens[0] == "*GEOMOBJECT") {
        in_geom = true;
        geom_depth = 0;
        current = RawObject{};
      }
    }

    if (in_geom) {
      if (!tokens.empty() && tokens[0] == "*NODE_NAME" && current.name.empty() && !in_node_tm &&
          !in_tm_animation) {
        current.name = ExtractQuoted(line);
      }

      if (!tokens.empty() && tokens[0] == "*NODE_TM") {
        in_node_tm = true;
        node_tm_depth = 0;
      }
      if (in_node_tm && !tokens.empty()) {
        if (tokens[0] == "*TM_POS" && tokens.size() >= 4) {
          current.tm_pos.Set(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
        } else if (tokens[0] == "*TM_ROTAXIS" && tokens.size() >= 4) {
          current.tm_rot_axis.Set(
              std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
        } else if (tokens[0] == "*TM_ROTANGLE" && tokens.size() >= 2) {
          current.tm_rot_angle = std::stof(tokens[1]);
        }
      }

      if (!tokens.empty() && tokens[0] == "*MESH") {
        in_mesh = true;
        mesh_depth = 0;
      }
      if (in_mesh) {
        if (!tokens.empty() && tokens[0] == "*MESH_VERTEX_LIST") {
          in_vertex_list = true;
          vertex_list_depth = 0;
        }
        if (!tokens.empty() && tokens[0] == "*MESH_FACE_LIST") {
          in_face_list = true;
          face_list_depth = 0;
        }
        if (in_vertex_list && !tokens.empty() && tokens[0] == "*MESH_VERTEX" && tokens.size() >= 5) {
          const int idx = std::stoi(tokens[1]);
          ensure_vec3_size(&current.vertices_world, idx);
          current.vertices_world[static_cast<size_t>(idx)] =
              Vec3(std::stof(tokens[2]), std::stof(tokens[3]), std::stof(tokens[4]));
        }
        if (in_face_list && !tokens_colon.empty() && tokens_colon[0] == "*MESH_FACE") {
          int idx = -1;
          if (tokens_colon.size() >= 2) {
            idx = std::stoi(tokens_colon[1]);
          }
          Face face{};
          if (idx >= 0 && parse_face(tokens_colon, &face)) {
            ensure_face_size(&current.faces, idx);
            current.faces[static_cast<size_t>(idx)] = face;
          }
        }
      }

      if (!tokens.empty() && tokens[0] == "*TM_ANIMATION") {
        in_tm_animation = true;
        tm_animation_depth = 0;
        active_track_node.clear();
      }
      if (in_tm_animation && !tokens.empty()) {
        if (tokens[0] == "*NODE_NAME") {
          active_track_node = ExtractQuoted(line);
        } else if (active_track_node == current.name && tokens[0] == "*CONTROL_POS_SAMPLE" &&
                   tokens.size() >= 5) {
          current.pos_track.push_back(
              {std::stod(tokens[1]), Vec3(std::stof(tokens[2]), std::stof(tokens[3]), std::stof(tokens[4]))});
        } else if (active_track_node == current.name && tokens[0] == "*CONTROL_ROT_SAMPLE" &&
                   tokens.size() >= 6) {
          current.rot_track_delta.push_back({std::stod(tokens[1]),
                                             Vec3(std::stof(tokens[2]),
                                                  std::stof(tokens[3]),
                                                  std::stof(tokens[4])),
                                             std::stof(tokens[5])});
        }
      }
    }

    if (in_node_tm) {
      node_tm_depth += brace_delta;
      if (node_tm_depth <= 0) {
        in_node_tm = false;
      }
    }
    if (in_vertex_list) {
      vertex_list_depth += brace_delta;
      if (vertex_list_depth <= 0) {
        in_vertex_list = false;
      }
    }
    if (in_face_list) {
      face_list_depth += brace_delta;
      if (face_list_depth <= 0) {
        in_face_list = false;
      }
    }
    if (in_mesh) {
      mesh_depth += brace_delta;
      if (mesh_depth <= 0) {
        in_mesh = false;
      }
    }
    if (in_tm_animation) {
      tm_animation_depth += brace_delta;
      if (tm_animation_depth <= 0) {
        in_tm_animation = false;
        active_track_node.clear();
      }
    }
    if (in_geom) {
      geom_depth += brace_delta;
      if (geom_depth <= 0) {
        in_geom = false;
        finalize_object(std::move(current));
      }
    }
  }

  if (in_geom) {
    finalize_object(std::move(current));
  }
  return !out_objects->empty();
}

Vec3 SampleSaariTrackAtMs(const std::vector<SaariSceneAssets::TrackKey>& track, double t_ms) {
  if (track.empty()) {
    return Vec3();
  }
  if (t_ms <= track.front().time_ms) {
    return track.front().value;
  }
  if (t_ms >= track.back().time_ms) {
    return track.back().value;
  }

  auto it = std::upper_bound(
      track.begin(), track.end(), t_ms, [](double t, const SaariSceneAssets::TrackKey& key) {
        return t < key.time_ms;
      });
  if (it == track.begin()) {
    return it->value;
  }
  const SaariSceneAssets::TrackKey& b = *it;
  const SaariSceneAssets::TrackKey& a = *(it - 1);
  const double dt = std::max(1e-6, b.time_ms - a.time_ms);
  const float f = static_cast<float>((t_ms - a.time_ms) / dt);
  return a.value + (b.value - a.value) * f;
}

void SetCameraLookAt(Camera& camera, const Vec3& position, const Vec3& target, const Vec3& world_up) {
  camera.position = position;
  Vec3 forward = (target - position).Normalized();
  if (forward.LengthSq() < 1e-6f) {
    forward = Vec3(0.0f, 0.0f, 1.0f);
  }
  Vec3 right = world_up.Cross(forward).Normalized();
  if (right.LengthSq() < 1e-6f) {
    right = Vec3(1.0f, 0.0f, 0.0f);
  }
  const Vec3 up = forward.Cross(right).Normalized();

  camera.forward = forward;
  camera.right = right;
  camera.up = up;
}

void UpdateWindowTitle(SDL_Window* window,
                       const DemoState& state,
                       const RuntimeStats& stats,
                       double elapsed_since_last_title) {
  const double fps = static_cast<double>(stats.rendered_frames) /
                     std::max(elapsed_since_last_title, 0.0001);
  const double ups = static_cast<double>(stats.simulated_ticks) /
                     std::max(elapsed_since_last_title, 0.0001);

  std::ostringstream title;
  title << "forward native harness | "
        << (state.paused ? "paused" : "running")
        << " | fps " << std::fixed << std::setprecision(1) << fps
        << " | ups " << std::fixed << std::setprecision(1) << ups
        << " | fov " << std::fixed << std::setprecision(1) << state.feta_fov_degrees
        << " | scene " << state.scene_label << " | mesh " << state.mesh_label
        << " | logical " << kLogicalWidth << "x"
        << kLogicalHeight << " | post " << state.post_label << " | audio pending";

  SDL_SetWindowTitle(window, title.str().c_str());
}

void DrawScrollingLayer(Surface32& surface,
                        const Image32& image,
                        int scroll_offset,
                        uint8_t global_alpha) {
  if (image.Empty() || global_alpha == 0) {
    return;
  }

  const int copy_w = std::min(kLogicalWidth, image.width);
  const int wrapped =
      ((scroll_offset % image.height) + image.height) % image.height;
  const int first_h = std::min(kLogicalHeight, image.height - wrapped);

  surface.AlphaBlitToBack(image.pixels.data(),
                          image.width,
                          image.height,
                          0,
                          wrapped,
                          0,
                          0,
                          copy_w,
                          first_h,
                          global_alpha);

  if (first_h < kLogicalHeight) {
    surface.AlphaBlitToBack(image.pixels.data(),
                            image.width,
                            image.height,
                            0,
                            0,
                            0,
                            first_h,
                            copy_w,
                            kLogicalHeight - first_h,
                            global_alpha);
  }
}

void DrawQuickWinPostLayer(Surface32& surface,
                           const DemoState& state,
                           const QuickWinPostLayer& post) {
  if (!state.show_post || !post.enabled || post.primary.Empty()) {
    return;
  }

  const float t = static_cast<float>(state.timeline_seconds);
  const float blend = 0.5f + 0.5f * std::sin(t * 0.23f);
  const uint8_t alpha_primary = static_cast<uint8_t>(40 + 45 * blend);
  const uint8_t alpha_secondary = static_cast<uint8_t>(10 + 25 * (1.0f - blend));

  const int scroll_primary = static_cast<int>(t * 44.0f);
  DrawScrollingLayer(surface, post.primary, scroll_primary, alpha_primary);

  if (!post.secondary.Empty()) {
    const int scroll_secondary =
        static_cast<int>(t * 31.0f + static_cast<float>(post.secondary.height) * 0.3f);
    DrawScrollingLayer(surface, post.secondary, scroll_secondary, alpha_secondary);
  }
}

Vec3 FetaTranslationAtTime(float timeline_seconds) {
  return Vec3(0.0f,
              0.12f * std::sin(timeline_seconds * 0.37f),
              2.55f + 0.35f * std::sin(timeline_seconds * 0.21f));
}

void ConfigureFetaInstance(RenderInstance& instance, const FetaSceneAssets& feta, float t) {
  instance.rotation_radians.Set(0.28f * std::sin(t * 0.14f), -t * 0.52f, t * 0.11f);
  instance.translation = FetaTranslationAtTime(t);
  instance.fill_color = PackArgb(220, 220, 220);
  instance.wire_color = PackArgb(110, 255, 220);
  instance.draw_fill = true;
  instance.draw_wire = false;
  instance.texture = feta.enabled ? &feta.babyenv : nullptr;
  instance.use_mesh_uv = true;
  instance.texture_wrap = true;
  instance.enable_backface_culling = true;
}

void ConfigureFetaHaloInstance(RenderInstance& instance,
                               const FetaSceneAssets& feta,
                               float t,
                               float scale_multiplier,
                               uint32_t tint) {
  instance.rotation_radians.Set(0.28f * std::sin(t * 0.14f), -t * 0.52f, t * 0.11f);
  instance.translation = FetaTranslationAtTime(t);
  instance.fill_color = tint;
  instance.wire_color = 0;
  instance.draw_fill = true;
  instance.draw_wire = false;
  instance.texture = feta.enabled ? &feta.babyenv : nullptr;
  instance.use_mesh_uv = true;
  instance.texture_wrap = true;
  instance.enable_backface_culling = true;
  instance.uniform_scale *= scale_multiplier;
}

void ConfigureKaaakmaBackgroundInstance(RenderInstance& instance,
                                        const KaaakmaBackgroundPass& background,
                                        const Camera& camera,
                                        float t) {
  instance.rotation_radians.Set(std::sin(t * 0.1f) * 0.25f, t * 0.25f, 0.0f);
  instance.translation = camera.position;
  instance.fill_color = PackArgb(255, 255, 255);
  instance.wire_color = 0;
  instance.draw_fill = true;
  instance.draw_wire = false;
  instance.texture = &background.texture;
  instance.use_mesh_uv = false;
  instance.texture_wrap = true;
  instance.enable_backface_culling = false;
}

uint32_t NextRandomU32(uint32_t* state) {
  uint32_t x = *state;
  if (x == 0u) {
    x = 0x6D2B79F5u;
  }
  x ^= x << 13u;
  x ^= x >> 17u;
  x ^= x << 5u;
  *state = x;
  return x;
}

float RandomRange(uint32_t* state, float min_value, float max_value) {
  const uint32_t r = NextRandomU32(state);
  const float unit = static_cast<float>(r & 0x00FFFFFFu) / static_cast<float>(0x01000000u);
  return min_value + (max_value - min_value) * unit;
}

void InitJavaRandomState(Mute95Runtime& runtime, uint64_t seed) {
  constexpr uint64_t kMask = (1ull << 48ull) - 1ull;
  runtime.java_random_state = (seed ^ 0x5DEECE66Dull) & kMask;
}

uint32_t JavaRandomNextBits(Mute95Runtime& runtime, int bits) {
  constexpr uint64_t kMask = (1ull << 48ull) - 1ull;
  runtime.java_random_state =
      (runtime.java_random_state * 0x5DEECE66Dull + 0xBull) & kMask;
  return static_cast<uint32_t>(runtime.java_random_state >> (48 - bits));
}

float JavaRandomNextFloat(Mute95Runtime& runtime) {
  return static_cast<float>(JavaRandomNextBits(runtime, 24)) /
         static_cast<float>(1u << 24u);
}

std::vector<uint8_t>& Mute95CurrentBuffer(Mute95Runtime& runtime) {
  return runtime.current_is_a ? runtime.buffer_a : runtime.buffer_b;
}

const std::vector<uint8_t>& Mute95PrevBuffer(const Mute95Runtime& runtime) {
  return runtime.current_is_a ? runtime.buffer_b : runtime.buffer_a;
}

void Mute95SwapBuffers(Mute95Runtime& runtime) {
  runtime.current_is_a = !runtime.current_is_a;
}

bool LoadGifGlobalPalette(const std::string& path, std::array<uint32_t, 256>* out_palette) {
  std::ifstream input(path, std::ios::binary);
  if (!input.is_open()) {
    return false;
  }

  uint8_t header[13] = {};
  input.read(reinterpret_cast<char*>(header), sizeof(header));
  if (input.gcount() != static_cast<std::streamsize>(sizeof(header))) {
    return false;
  }
  if (!(header[0] == 'G' && header[1] == 'I' && header[2] == 'F')) {
    return false;
  }

  const bool has_global_table = (header[10] & 0x80u) != 0u;
  if (!has_global_table) {
    return false;
  }
  const int global_size = 1 << ((header[10] & 0x07u) + 1);
  if (global_size <= 0 || global_size > 256) {
    return false;
  }

  std::array<uint8_t, 256 * 3> raw{};
  const size_t bytes = static_cast<size_t>(global_size) * 3u;
  input.read(reinterpret_cast<char*>(raw.data()), static_cast<std::streamsize>(bytes));
  if (input.gcount() != static_cast<std::streamsize>(bytes)) {
    return false;
  }

  for (int i = 0; i < 256; ++i) {
    const size_t base = static_cast<size_t>(std::min(i, global_size - 1)) * 3u;
    (*out_palette)[static_cast<size_t>(i)] = PackArgb(raw[base + 0], raw[base + 1], raw[base + 2]);
  }
  return true;
}

void InitializeMute95Runtime(Mute95Runtime& runtime) {
  runtime.cols = kLogicalWidth / runtime.cell_w;
  runtime.rows = kLogicalHeight / runtime.cell_h;
  const size_t cell_count =
      static_cast<size_t>(runtime.cols) * static_cast<size_t>(runtime.rows);
  runtime.flow_x.assign(cell_count, 0.0f);
  runtime.flow_y.assign(cell_count, 0.0f);

  const size_t pixel_count =
      static_cast<size_t>(kLogicalWidth) * static_cast<size_t>(kLogicalHeight);
  runtime.buffer_a.assign(pixel_count, 0u);
  runtime.buffer_b.assign(pixel_count, 0u);
  runtime.current_is_a = true;
  for (size_t i = 0; i < pixel_count; ++i) {
    Mute95CurrentBuffer(runtime)[i] = static_cast<uint8_t>(i & 0xFFu);
  }
  InitJavaRandomState(runtime, 999ull);

  runtime.frame_counter = 0;
  runtime.active_credit = -1;
  runtime.cue_step = -1;
  runtime.credit_start_seconds = -1.0;
  runtime.prev_scene_seconds = 0.0;
  runtime.initialized = true;
}

void DrawMute95Credits(Surface32& surface,
                       const Mute95SceneAssets& assets,
                       Mute95Runtime& runtime,
                       double scene_seconds) {
  if (runtime.active_credit < 0 || runtime.active_credit >= static_cast<int>(assets.credits.size()) ||
      runtime.credit_start_seconds < 0.0) {
    return;
  }

  const double dt = scene_seconds - runtime.credit_start_seconds;
  if (dt < 0.0 || dt > 9.0) {
    return;
  }

  float alpha_first = 0.0f;
  float alpha_second = 0.0f;
  if (dt < 1.5) {
    alpha_first = static_cast<float>(dt / 1.5);
  } else if (dt < 4.0) {
    alpha_first = 1.0f;
    alpha_second = static_cast<float>((dt - 1.5) / (4.0 - 1.5));
  } else if (dt < 6.0) {
    alpha_first = 1.0f - static_cast<float>((dt - 4.0) / (6.0 - 4.0));
    alpha_second = 1.0f;
  } else {
    alpha_second = 1.0f - static_cast<float>((dt - 6.0) / (9.0 - 6.0));
  }

  const Mute95CreditPair& pair = assets.credits[static_cast<size_t>(runtime.active_credit)];
  const int dst_x = (kLogicalWidth - 256) / 2;
  const int dst_y = (kLogicalHeight - 50) / 2;
  const int src_x = 8;
  const int src_y = 40;
  const int copy_w = 256;
  const int copy_h = 50;

  if (!pair.first.Empty() && alpha_first > 0.0f) {
    surface.AdditiveBlitToBack(
        pair.first.pixels.data(),
        pair.first.width,
        pair.first.height,
        src_x,
        src_y,
        dst_x,
        dst_y,
        copy_w,
        copy_h,
        static_cast<uint8_t>(std::clamp(alpha_first * 255.0f, 0.0f, 255.0f)));
  }
  if (!pair.second.Empty() && alpha_second > 0.0f) {
    surface.AdditiveBlitToBack(
        pair.second.pixels.data(),
        pair.second.width,
        pair.second.height,
        src_x,
        src_y,
        dst_x,
        dst_y,
        copy_w,
        copy_h,
        static_cast<uint8_t>(std::clamp(alpha_second * 255.0f, 0.0f, 255.0f)));
  }
}

void DrawMute95FrameAtTime(Surface32& surface,
                           const Mute95SceneAssets& assets,
                           Mute95Runtime& runtime,
                           double scene_seconds) {
  if (!assets.enabled) {
    surface.ClearBack(PackArgb(0, 0, 0));
    surface.SwapBuffers();
    return;
  }
  if (!runtime.initialized) {
    InitializeMute95Runtime(runtime);
  }

  static constexpr std::array<double, 5> kCueSeconds = {3.0, 5.0, 7.0, 9.0, 11.0};
  if (runtime.cue_step + 1 < static_cast<int>(kCueSeconds.size())) {
    const int next_cue = runtime.cue_step + 1;
    if (scene_seconds >= kCueSeconds[static_cast<size_t>(next_cue)]) {
      runtime.cue_step = next_cue;
      runtime.active_credit = next_cue;
      runtime.credit_start_seconds = scene_seconds;
    }
  }

  float dt = static_cast<float>(scene_seconds - runtime.prev_scene_seconds);
  runtime.prev_scene_seconds = scene_seconds;
  if (dt <= 0.0f || dt > 0.2f) {
    dt = 1.0f / static_cast<float>(kTickHz);
  }
  const float strength = std::max(0.05f, dt * 10.0f);

  const int cx = runtime.cols / 2;
  const int cy = runtime.rows / 2;
  const float phase_x = static_cast<float>(runtime.frame_counter % 4) * 0.2f;
  const float phase_y = static_cast<float>(runtime.frame_counter % 5) * 0.2f;

  std::vector<uint8_t>& current = Mute95CurrentBuffer(runtime);
  const std::vector<uint8_t>& previous = Mute95PrevBuffer(runtime);

  for (int gy = 0; gy < runtime.rows; ++gy) {
    for (int gx = 0; gx < runtime.cols; ++gx) {
      const size_t cell = static_cast<size_t>(gy) * static_cast<size_t>(runtime.cols) +
                          static_cast<size_t>(gx);
      const float prev_fx = runtime.flow_x[cell];
      const float prev_fy = runtime.flow_y[cell];
      runtime.flow_x[cell] += (static_cast<float>(gx - cx) * strength + phase_x);
      runtime.flow_y[cell] += (static_cast<float>(gy - cy) * strength + phase_y);

      const int shift_x = static_cast<int>(runtime.flow_x[cell]) - static_cast<int>(prev_fx);
      const int shift_y = static_cast<int>(runtime.flow_y[cell]) - static_cast<int>(prev_fy);

      const int dst_x0 = gx * runtime.cell_w;
      const int dst_y0 = gy * runtime.cell_h;
      const int src_x0 = dst_x0 - shift_x;
      const int src_y0 = dst_y0 - shift_y;
      if (src_x0 < 0 || src_y0 < 0 || src_x0 + runtime.cell_w > kLogicalWidth ||
          src_y0 + runtime.cell_h > kLogicalHeight) {
        continue;
      }

      for (int y = 0; y < runtime.cell_h; ++y) {
        const size_t src_row =
            static_cast<size_t>(src_y0 + y) * static_cast<size_t>(kLogicalWidth) +
            static_cast<size_t>(src_x0);
        const size_t dst_row =
            static_cast<size_t>(dst_y0 + y) * static_cast<size_t>(kLogicalWidth) +
            static_cast<size_t>(dst_x0);
        for (int x = 0; x < runtime.cell_w; ++x) {
          current[dst_row + static_cast<size_t>(x)] =
              previous[src_row + static_cast<size_t>(x)];
        }
      }
    }
  }

  const int sparkle_cap = std::min(static_cast<int>(scene_seconds * 1.8 + 22.0), 255);
  for (int i = 0; i < 220; ++i) {
    const size_t idx = static_cast<size_t>(
        JavaRandomNextFloat(runtime) * static_cast<float>(current.size() - 1u));
    const int boosted = std::min(sparkle_cap, static_cast<int>(current[idx]) + 45);
    current[idx] = static_cast<uint8_t>(boosted);
  }

  for (size_t i = 0; i < current.size(); ++i) {
    current[i] = static_cast<uint8_t>((static_cast<int>(current[i]) +
                                       static_cast<int>(previous[i])) >>
                                      1);
  }

  Mute95SwapBuffers(runtime);
  ++runtime.frame_counter;

  const std::vector<uint8_t>& display = Mute95CurrentBuffer(runtime);
  for (int y = 0; y < kLogicalHeight; ++y) {
    for (int x = 0; x < kLogicalWidth; ++x) {
      const uint8_t idx = display[static_cast<size_t>(y) * static_cast<size_t>(kLogicalWidth) +
                                  static_cast<size_t>(x)];
      surface.SetBackPixel(x, y, assets.palette[idx]);
    }
  }

  DrawMute95Credits(surface, assets, runtime, scene_seconds);
  surface.SwapBuffers();
}

void DrawMute95Frame(Surface32& surface,
                     const DemoState& state,
                     const Mute95SceneAssets& assets,
                     Mute95Runtime& runtime) {
  const double scene_seconds = std::max(0.0, state.timeline_seconds - state.scene_start_seconds);
  DrawMute95FrameAtTime(surface, assets, runtime, scene_seconds);
}

void InitializeDominaRuntime(DominaRuntime& runtime) {
  runtime.frame_counter = 0;
  runtime.fade_to_black = false;
  runtime.fade_start_seconds = 0.0;
  runtime.initialized = true;
}

void StartDominaFadeToBlack(DominaRuntime& runtime, double scene_seconds) {
  if (runtime.fade_to_black) {
    return;
  }
  runtime.fade_to_black = true;
  runtime.fade_start_seconds = scene_seconds;
}

void DrawDominaFrameAtTime(Surface32& surface,
                           const DominaSceneAssets& assets,
                           DominaRuntime& runtime,
                           double scene_seconds,
                           bool trigger_script_fade_event) {
  if (!assets.enabled) {
    surface.ClearBack(PackArgb(0, 0, 0));
    surface.SwapBuffers();
    return;
  }
  if (!runtime.initialized) {
    InitializeDominaRuntime(runtime);
  }
  if (trigger_script_fade_event && scene_seconds >= 2.0) {
    StartDominaFadeToBlack(runtime, scene_seconds);
  }

  const Image32& source =
      (assets.use_komplex && !assets.komplex.Empty()) ? assets.komplex : assets.phorward;
  if (source.Empty()) {
    surface.ClearBack(PackArgb(0, 0, 0));
    surface.SwapBuffers();
    return;
  }

  float fade_to_source = static_cast<float>((scene_seconds - 0.2) / 8.0);
  if (runtime.fade_to_black) {
    fade_to_source =
        1.0f - static_cast<float>((scene_seconds - runtime.fade_start_seconds) * 0.1);
  }
  fade_to_source = std::clamp(fade_to_source, 0.0f, 1.0f);
  const float source_weight = fade_to_source;
  const float base_weight = 1.0f - source_weight;

  surface.ClearBack(PackArgb(0, 0, 0));
  const int copy_w = std::min(kLogicalWidth, source.width);
  if (copy_w > 0 && source.height > 0) {
    const int frame_counter = std::max(0, static_cast<int>(scene_seconds * kTickHz));
    const int scroll_y =
        -((frame_counter * kLogicalHeight) % source.height);
    const int wrapped_y = ((scroll_y % source.height) + source.height) % source.height;

    for (int y = 0; y < kLogicalHeight; ++y) {
      const int src_y = (wrapped_y + y) % source.height;
      const size_t src_row =
          static_cast<size_t>(src_y) * static_cast<size_t>(source.width);
      for (int x = 0; x < copy_w; ++x) {
        const uint32_t src = source.pixels[src_row + static_cast<size_t>(x)];
        const float base_r = runtime.fade_to_black ? 0.0f : 255.0f;
        const float base_g = runtime.fade_to_black ? 0.0f : 255.0f;
        const float base_b = runtime.fade_to_black ? 0.0f : 255.0f;
        const uint8_t r = static_cast<uint8_t>(
            std::clamp(base_r * base_weight + static_cast<float>(UnpackR(src)) * source_weight,
                       0.0f,
                       255.0f));
        const uint8_t g = static_cast<uint8_t>(
            std::clamp(base_g * base_weight + static_cast<float>(UnpackG(src)) * source_weight,
                       0.0f,
                       255.0f));
        const uint8_t b = static_cast<uint8_t>(
            std::clamp(base_b * base_weight + static_cast<float>(UnpackB(src)) * source_weight,
                       0.0f,
                       255.0f));
        surface.SetBackPixel(x, y, PackArgb(r, g, b));
      }
    }
  }

  surface.SwapBuffers();
  runtime.frame_counter = std::max(0, static_cast<int>(scene_seconds * kTickHz));
}

void DrawDominaFrame(Surface32& surface,
                     const DemoState& state,
                     const DominaSceneAssets& assets,
                     DominaRuntime& runtime) {
  const double scene_seconds = std::max(0.0, state.timeline_seconds - state.scene_start_seconds);
  DrawDominaFrameAtTime(surface, assets, runtime, scene_seconds, true);
}

void InitializeSaariRuntime(SaariRuntime& runtime) {
  runtime.noise_lut.resize(1000);
  for (uint32_t& value : runtime.noise_lut) {
    const uint32_t grey = NextRandomU32(&runtime.rng_state) % 195u;
    value = PackArgb(static_cast<uint8_t>(grey),
                     static_cast<uint8_t>(grey),
                     static_cast<uint8_t>(grey));
  }

  runtime.scanline_order.resize(static_cast<size_t>(kLogicalHeight));
  for (int i = 0; i < kLogicalHeight; ++i) {
    runtime.scanline_order[static_cast<size_t>(i)] = i;
  }
  for (int i = 0; i < 3000; ++i) {
    const int a = i % kLogicalHeight;
    const int b = static_cast<int>(NextRandomU32(&runtime.rng_state) %
                                   static_cast<uint32_t>(kLogicalHeight - 1));
    std::swap(runtime.scanline_order[static_cast<size_t>(a)],
              runtime.scanline_order[static_cast<size_t>(b)]);
  }

  runtime.shock_percent = 0.0f;
  runtime.shock_decay = 0.0f;
  runtime.prev_scene_seconds = 0.0;
  runtime.initial_suh0_sent = false;
  runtime.first_suh_sent = false;
  runtime.initialized = true;
}

void TriggerSaariMessage(SaariRuntime& runtime, bool suh_full) {
  if (suh_full) {
    runtime.shock_percent = 100.0f;
    runtime.shock_decay = 200.0f;
  } else {
    runtime.shock_percent = 68.0f;
    runtime.shock_decay = 0.0f;
  }
}

void ApplySaariShockOverlay(Surface32& surface, SaariRuntime& runtime, int line_count) {
  if (line_count <= 0 || runtime.noise_lut.empty() || runtime.scanline_order.empty()) {
    return;
  }
  line_count = std::clamp(line_count, 0, kLogicalHeight);
  const int random_offset =
      static_cast<int>(NextRandomU32(&runtime.rng_state) %
                       static_cast<uint32_t>(runtime.scanline_order.size()));
  uint32_t* back = surface.BackPixelsMutable();
  if (!back) {
    return;
  }

  for (int i = 0; i < line_count; ++i) {
    const int y =
        runtime.scanline_order[static_cast<size_t>((i + random_offset) % kLogicalHeight)];
    const int lut_start = static_cast<int>(
        NextRandomU32(&runtime.rng_state) %
        static_cast<uint32_t>(std::max(1, static_cast<int>(runtime.noise_lut.size()) - kLogicalWidth)));
    uint32_t* row = back + static_cast<size_t>(y) * static_cast<size_t>(kLogicalWidth);
    for (int x = 0; x < kLogicalWidth; ++x) {
      const uint32_t dec = runtime.noise_lut[static_cast<size_t>(lut_start + x)];
      const uint32_t src = row[static_cast<size_t>(x)];
      const int r = std::max(0, static_cast<int>(UnpackR(src)) - static_cast<int>(UnpackR(dec)));
      const int g = std::max(0, static_cast<int>(UnpackG(src)) - static_cast<int>(UnpackG(dec)));
      const int b = std::max(0, static_cast<int>(UnpackB(src)) - static_cast<int>(UnpackB(dec)));
      row[static_cast<size_t>(x)] = PackArgb(static_cast<uint8_t>(r),
                                             static_cast<uint8_t>(g),
                                             static_cast<uint8_t>(b));
    }
  }
}

void DrawSaariFrameAtTime(Surface32& surface,
                          const SaariSceneAssets& saari,
                          SaariRuntime& runtime,
                          Camera& camera,
                          Renderer3D& renderer,
                          RenderInstance& backdrop_instance,
                          RenderInstance& terrain_instance,
                          RenderInstance& object_instance,
                          double scene_seconds,
                          bool trigger_script_messages) {
  if (!saari.enabled || saari.terrain.Empty()) {
    surface.ClearBack(PackArgb(0, 0, 0));
    surface.SwapBuffers();
    return;
  }
  if (!runtime.initialized) {
    InitializeSaariRuntime(runtime);
  }

  if (trigger_script_messages) {
    if (!runtime.initial_suh0_sent) {
      TriggerSaariMessage(runtime, false);  // "suh0"
      runtime.initial_suh0_sent = true;
    }
    if (scene_seconds >= 5.12 && !runtime.first_suh_sent) {
      TriggerSaariMessage(runtime, true);  // "_100 msg saari suh"
      runtime.first_suh_sent = true;
    }
  }

  float dt = static_cast<float>(scene_seconds - runtime.prev_scene_seconds);
  runtime.prev_scene_seconds = scene_seconds;
  if (dt <= 0.0f || dt > 0.2f) {
    dt = 1.0f / static_cast<float>(kTickHz);
  }
  if (runtime.shock_percent > 0.0f) {
    runtime.shock_percent = std::max(0.0f, runtime.shock_percent - runtime.shock_decay * dt);
  }

  const double t_ms = scene_seconds * 1000.0;
  Vec3 cam_pos(0.0f, 0.0f, 0.0f);
  Vec3 cam_target(0.0f, 0.0f, 1.0f);
  if (!saari.camera_track.empty() && !saari.target_track.empty()) {
    cam_pos = SampleSaariTrackAtMs(saari.camera_track, t_ms);
    cam_target = SampleSaariTrackAtMs(saari.target_track, t_ms);
  }
  SetCameraLookAt(camera, cam_pos, cam_target, Vec3(0.0f, 1.0f, 0.0f));
  camera.fov_degrees = saari.camera_fov_degrees;

  surface.ClearBack(PackArgb(220, 230, 245));
  if (!saari.backdrop_mesh.Empty() && !saari.backdrop_texture.Empty()) {
    backdrop_instance.rotation_radians.Set(0.0f, 0.0f, 0.0f);
    backdrop_instance.translation = camera.position;
    backdrop_instance.uniform_scale = saari.backdrop_scale;
    backdrop_instance.fill_color = PackArgb(255, 255, 255);
    backdrop_instance.draw_fill = true;
    backdrop_instance.draw_wire = false;
    backdrop_instance.use_basis_rotation = false;
    backdrop_instance.texture = &saari.backdrop_texture;
    backdrop_instance.use_mesh_uv = true;
    backdrop_instance.texture_wrap = true;
    backdrop_instance.enable_backface_culling = false;
    renderer.DrawMesh(surface, saari.backdrop_mesh, camera, backdrop_instance);
  }

  terrain_instance.rotation_radians.Set(-0.04f, 0.0f, 0.0f);
  terrain_instance.translation = Vec3(-504.0f, -75.0f, 6.0f);
  terrain_instance.uniform_scale = 1.0f;
  terrain_instance.fill_color = PackArgb(255, 255, 255);
  terrain_instance.wire_color = PackArgb(28, 32, 24);
  terrain_instance.draw_fill = true;
  terrain_instance.draw_wire = false;
  terrain_instance.use_basis_rotation = false;
  terrain_instance.texture = &saari.terrain_texture;
  terrain_instance.use_mesh_uv = true;
  terrain_instance.texture_wrap = true;
  terrain_instance.enable_backface_culling = true;
  renderer.DrawMesh(surface, saari.terrain, camera, terrain_instance);

  if (!saari.animated_objects.empty()) {
    const Quat meditate_pi = QuatFromAxisAngle(Vec3(0.0f, 0.0f, 1.0f), kPi);
    const float t_scene = static_cast<float>(scene_seconds);
    const Quat klunssi_scripted = BuildSaariKlunssiScriptedRotation(t_scene);
    object_instance.uniform_scale = 1.0f;
    object_instance.fill_color = PackArgb(255, 255, 255);
    object_instance.wire_color = 0;
    object_instance.draw_fill = true;
    object_instance.draw_wire = false;
    object_instance.texture = &saari.backdrop_texture;
    object_instance.use_mesh_uv = false;
    object_instance.texture_wrap = true;
    object_instance.enable_backface_culling = true;

    for (const SaariSceneAssets::AnimatedObject& obj : saari.animated_objects) {
      if (obj.mesh.Empty()) {
        continue;
      }
      Vec3 obj_pos = obj.base_position;
      if (!obj.position_track.empty()) {
        obj_pos = SampleSaariTrackAtMs(obj.position_track, t_ms);
      }
      Quat obj_rot = obj.base_rotation;
      if (!obj.rotation_track.empty()) {
        obj_rot = SampleSaariRotationTrackAtMs(obj.rotation_track, t_ms, obj.base_rotation);
      }
      if (obj.name == "klunssi") {
        // Java override: clear matrix and apply scripted per-frame rotations.
        obj_rot = klunssi_scripted;
      } else if (obj.name == "meditate") {
        // Java override: add constant Z rotation on top of track orientation.
        obj_rot = QuatNormalize(QuatMul(meditate_pi, obj_rot));
      }

      object_instance.translation = obj_pos;
      SetRenderInstanceBasisFromQuat(object_instance, obj_rot);
      renderer.DrawMesh(surface, obj.mesh, camera, object_instance);
    }
  }

  const int lines = static_cast<int>(runtime.shock_percent * static_cast<float>(kLogicalHeight) / 100.0f);
  ApplySaariShockOverlay(surface, runtime, lines);
  surface.SwapBuffers();
}

void DrawSaariFrame(Surface32& surface,
                    const DemoState& state,
                    const SaariSceneAssets& saari,
                    SaariRuntime& runtime,
                    Camera& camera,
                    Renderer3D& renderer,
                    RenderInstance& backdrop_instance,
                    RenderInstance& terrain_instance,
                    RenderInstance& object_instance) {
  const double scene_seconds = std::max(0.0, state.timeline_seconds - state.scene_start_seconds);
  DrawSaariFrameAtTime(surface,
                       saari,
                       runtime,
                       camera,
                       renderer,
                       backdrop_instance,
                       terrain_instance,
                       object_instance,
                       scene_seconds,
                       true);
}

Vec3 RotateXSimple(const Vec3& v, float angle) {
  const float s = std::sin(angle);
  const float c = std::cos(angle);
  return Vec3(v.x, v.y * c - v.z * s, v.y * s + v.z * c);
}

Vec3 RotateYSimple(const Vec3& v, float angle) {
  const float s = std::sin(angle);
  const float c = std::cos(angle);
  return Vec3(v.x * c + v.z * s, v.y, -v.x * s + v.z * c);
}

void InitializeMmaamkaParticles(MmaamkaParticlePass& pass, int count, double timeline_seconds) {
  pass.particles.assign(static_cast<size_t>(count), Particle{});
  pass.last_timeline_seconds = timeline_seconds;
  pass.rng_state = 0x1998u;
  pass.initialized = true;

  for (Particle& p : pass.particles) {
    // feta uses mmaamka mode 0: random cloud around origin, not an emitter stream.
    p.position.Set(RandomRange(&pass.rng_state, -5.0f, 5.0f),
                   RandomRange(&pass.rng_state, -5.0f, 5.0f),
                   RandomRange(&pass.rng_state, -5.0f, 5.0f));
    p.size = RandomRange(&pass.rng_state, 0.35f, 1.15f);
    p.energy = RandomRange(&pass.rng_state, 0.45f, 1.0f);
  }
}

void StepMmaamkaParticles(MmaamkaParticlePass& pass, double timeline_seconds) {
  if (!pass.enabled || pass.flare.Empty()) {
    return;
  }

  if (!pass.initialized) {
    InitializeMmaamkaParticles(pass, 300, timeline_seconds);
    return;
  }
  pass.last_timeline_seconds = timeline_seconds;
}

bool ProjectPointToScreen(const Camera& camera,
                          const Vec3& world_pos,
                          int* out_x,
                          int* out_y,
                          float* out_depth) {
  const Vec3 rel = world_pos - camera.position;
  const Vec3 view(rel.Dot(camera.right), rel.Dot(camera.up), rel.Dot(camera.forward));
  if (view.z <= camera.near_plane) {
    return false;
  }

  const float half_fov =
      (camera.fov_degrees * (kPi / 180.0f)) * 0.5f;
  const float focal_length =
      (0.5f * static_cast<float>(kLogicalWidth)) / std::tan(half_fov);
  const float center_x = (static_cast<float>(kLogicalWidth) - 1.0f) * 0.5f;
  const float center_y = (static_cast<float>(kLogicalHeight) - 1.0f) * 0.5f;
  const float inv_z = 1.0f / view.z;
  const float sx = center_x + view.x * focal_length * inv_z;
  const float sy = center_y - view.y * focal_length * inv_z;

  *out_x = static_cast<int>(std::lround(sx));
  *out_y = static_cast<int>(std::lround(sy));
  *out_depth = view.z;
  return true;
}

void DrawMmaamkaParticles(Surface32& surface,
                          const Camera& camera,
                          const MmaamkaParticlePass& pass,
                          double timeline_seconds) {
  if (!pass.enabled || pass.flare.Empty()) {
    return;
  }

  const float t = static_cast<float>(timeline_seconds);
  const float rot_y = -t * 0.5f;
  const float rot_x = 0.08f * std::sin(t * 0.33f);
  const Vec3 cloud_center(0.0f, 0.0f, 3.2f);

  for (const Particle& p : pass.particles) {
    Vec3 world = RotateYSimple(p.position, rot_y);
    world = RotateXSimple(world, rot_x);
    world = world + cloud_center;

    int sx = 0;
    int sy = 0;
    float depth = 1.0f;
    if (!ProjectPointToScreen(camera, world, &sx, &sy, &depth)) {
      continue;
    }

    const float projected = (24.0f / std::max(depth, 0.2f)) * p.size;
    const int sprite_size = std::clamp(static_cast<int>(std::lround(projected)), 2, 54);
    const float intensity_f = (20.0f / std::max(depth, 0.3f)) * p.energy;
    const uint8_t intensity = static_cast<uint8_t>(
        std::clamp(static_cast<int>(std::lround(intensity_f * 16.0f)), 12, 255));

    const int dst_x = sx - sprite_size / 2;
    const int dst_y = sy - sprite_size / 2;
    surface.AdditiveBlitScaledToBack(pass.flare.pixels.data(),
                                     pass.flare.width,
                                     pass.flare.height,
                                     dst_x,
                                     dst_y,
                                     sprite_size,
                                     sprite_size,
                                     intensity);
  }
}

void DrawFetaFrame(Surface32& surface,
                   const DemoState& state,
                   const Mesh& mesh,
                   const KaaakmaBackgroundPass& background,
                   MmaamkaParticlePass& particles,
                   Camera& camera,
                   Renderer3D& renderer,
                   RenderInstance& mesh_instance,
                   RenderInstance& halo_instance,
                   RenderInstance& background_instance,
                   Surface32& halo_surface,
                   const FetaSceneAssets& feta,
                   const QuickWinPostLayer& post) {
  surface.ClearBack(PackArgb(2, 3, 8));

  const float t = static_cast<float>(state.timeline_seconds);
  camera.position = Vec3(0.0f, 0.0f, 0.0f);
  camera.right = Vec3(1.0f, 0.0f, 0.0f);
  camera.up = Vec3(0.0f, 1.0f, 0.0f);
  camera.forward = Vec3(0.0f, 0.0f, 1.0f);
  camera.fov_degrees = state.feta_fov_degrees;

  if (background.enabled) {
    ConfigureKaaakmaBackgroundInstance(background_instance, background, camera, t);
    renderer.DrawMesh(surface, background.mesh, camera, background_instance);
  }

  if (feta.enabled) {
    struct HaloPass {
      float scale;
      uint8_t intensity;
      uint32_t tint;
    };
    static const std::array<HaloPass, 3> kHaloPasses = {
        HaloPass{1.025f, 150, PackArgb(90, 255, 120)},
        HaloPass{1.055f, 100, PackArgb(120, 255, 145)},
        HaloPass{1.090f, 50, PackArgb(165, 255, 185)},
    };

    for (const HaloPass& pass : kHaloPasses) {
      halo_surface.ClearBack(PackArgb(0, 0, 0));
      halo_instance.uniform_scale = mesh_instance.uniform_scale;
      ConfigureFetaHaloInstance(halo_instance, feta, t, pass.scale, pass.tint);
      renderer.DrawMesh(halo_surface, mesh, camera, halo_instance);
      halo_surface.SwapBuffers();
      surface.AdditiveBlitToBack(halo_surface.FrontPixels(),
                                 kLogicalWidth,
                                 kLogicalHeight,
                                 0,
                                 0,
                                 0,
                                 0,
                                 kLogicalWidth,
                                 kLogicalHeight,
                                 pass.intensity);
    }
  }

  ConfigureFetaInstance(mesh_instance, feta, t);
  renderer.DrawMesh(surface, mesh, camera, mesh_instance);

  StepMmaamkaParticles(particles, state.timeline_seconds);
  DrawMmaamkaParticles(surface, camera, particles, state.timeline_seconds);

  DrawQuickWinPostLayer(surface, state, post);
  surface.SwapBuffers();
}

void DrawMute95DominaSequenceFrame(Surface32& surface,
                                   const DemoState& state,
                                   const Mute95SceneAssets& mute95_assets,
                                   Mute95Runtime& mute95_runtime,
                                   const DominaSceneAssets& domina_assets,
                                   DominaRuntime& domina_runtime,
                                   const SaariSceneAssets& saari_assets,
                                   SaariRuntime& saari_runtime,
                                   Camera& camera,
                                   Renderer3D& renderer,
                                   RenderInstance& saari_backdrop_instance,
                                   RenderInstance& saari_terrain_instance,
                                   RenderInstance& saari_object_instance) {
  const double script_seconds = std::max(0.0, state.timeline_seconds - state.scene_start_seconds);

  // Script order in forward.java: show mute95 -> show domina -> show saari.
  if (script_seconds < 13.0) {
    DrawMute95FrameAtTime(surface, mute95_assets, mute95_runtime, script_seconds);
    return;
  }

  if (script_seconds < 29.0 || !saari_assets.enabled) {
    const double domina_seconds = script_seconds - 13.0;
    DrawDominaFrameAtTime(surface, domina_assets, domina_runtime, domina_seconds, true);
    return;
  }

  const double saari_seconds = script_seconds - 29.0;
  DrawSaariFrameAtTime(surface,
                       saari_assets,
                       saari_runtime,
                       camera,
                       renderer,
                       saari_backdrop_instance,
                       saari_terrain_instance,
                       saari_object_instance,
                       saari_seconds,
                       true);
}

void DrawFrame(Surface32& surface,
               const DemoState& state,
               const Mute95SceneAssets& mute95_assets,
               Mute95Runtime& mute95_runtime,
               const DominaSceneAssets& domina_assets,
               DominaRuntime& domina_runtime,
               const SaariSceneAssets& saari_assets,
               SaariRuntime& saari_runtime,
               const Mesh& mesh,
               const KaaakmaBackgroundPass& background,
               MmaamkaParticlePass& particles,
               Camera& camera,
               Renderer3D& renderer,
               RenderInstance& mesh_instance,
               RenderInstance& halo_instance,
               RenderInstance& background_instance,
               RenderInstance& saari_backdrop_instance,
               RenderInstance& saari_terrain_instance,
               RenderInstance& saari_object_instance,
               Surface32& halo_surface,
               const FetaSceneAssets& feta,
               const QuickWinPostLayer& post) {
  if (state.scene_mode == SceneMode::kMute95) {
    DrawMute95Frame(surface, state, mute95_assets, mute95_runtime);
    return;
  }
  if (state.scene_mode == SceneMode::kDomina) {
    DrawDominaFrame(surface, state, domina_assets, domina_runtime);
    return;
  }
  if (state.scene_mode == SceneMode::kSaari) {
    DrawSaariFrame(surface,
                   state,
                   saari_assets,
                   saari_runtime,
                   camera,
                   renderer,
                   saari_backdrop_instance,
                   saari_terrain_instance,
                   saari_object_instance);
    return;
  }
  if (state.scene_mode == SceneMode::kMute95DominaSequence) {
    DrawMute95DominaSequenceFrame(surface,
                                  state,
                                  mute95_assets,
                                  mute95_runtime,
                                  domina_assets,
                                  domina_runtime,
                                  saari_assets,
                                  saari_runtime,
                                  camera,
                                  renderer,
                                  saari_backdrop_instance,
                                  saari_terrain_instance,
                                  saari_object_instance);
    return;
  }

  DrawFetaFrame(surface,
                state,
                mesh,
                background,
                particles,
                camera,
                renderer,
                mesh_instance,
                halo_instance,
                background_instance,
                halo_surface,
                feta,
                post);
}

}  // namespace

int main(int argc, char** argv) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
    return 1;
  }

  const std::string mesh_path = ResolveMeshPath();
  if (mesh_path.empty()) {
    std::cerr << "Unable to locate mesh file. Tried from project and port directories.\n";
    SDL_Quit();
    return 1;
  }

  Mesh mesh;
  std::string mesh_error;
  if (!forward::core::LoadIguMesh(mesh_path, mesh, &mesh_error)) {
    std::cerr << "LoadIguMesh failed: " << mesh_error << "\n";
    SDL_Quit();
    return 1;
  }

  Camera camera;
  camera.position = Vec3(0.0f, 0.0f, 0.0f);
  camera.fov_degrees = 84.0f;
  camera.near_plane = 0.1f;

  RenderInstance mesh_instance;
  const float radius = mesh.BoundingRadius();
  mesh_instance.uniform_scale = (radius > 0.001f) ? (1.0f / radius) : 1.0f;
  mesh_instance.translation = Vec3(0.0f, 0.0f, 2.6f);
  mesh_instance.draw_fill = true;
  mesh_instance.draw_wire = true;
  mesh_instance.enable_backface_culling = true;

  RenderInstance halo_instance;
  halo_instance.uniform_scale = mesh_instance.uniform_scale * 1.075f;
  halo_instance.draw_fill = true;
  halo_instance.draw_wire = false;
  halo_instance.enable_backface_culling = true;

  RenderInstance background_instance;
  RenderInstance saari_backdrop_instance;
  RenderInstance saari_terrain_instance;
  RenderInstance saari_object_instance;
  saari_backdrop_instance.enable_backface_culling = false;
  saari_backdrop_instance.draw_fill = true;
  saari_backdrop_instance.draw_wire = false;
  saari_terrain_instance.enable_backface_culling = true;
  saari_terrain_instance.draw_fill = true;
  saari_terrain_instance.draw_wire = false;
  saari_object_instance.enable_backface_culling = true;
  saari_object_instance.draw_fill = true;
  saari_object_instance.draw_wire = false;

  FetaSceneAssets feta;
  Mute95SceneAssets mute95;
  Mute95Runtime mute95_runtime;
  DominaSceneAssets domina;
  DominaRuntime domina_runtime;
  SaariSceneAssets saari;
  SaariRuntime saari_runtime;
  MmaamkaParticlePass particles;
  KaaakmaBackgroundPass background;
  {
    std::string image_error;
    const std::array<std::pair<std::string, std::string>, 5> credit_files = {
        std::pair<std::string, std::string>{"images/kosmos/sav1.jpg", "images/kosmos/sav2.jpg"},
        std::pair<std::string, std::string>{"images/kosmos/jmag1.jpg", "images/kosmos/jmag2.jpg"},
        std::pair<std::string, std::string>{"images/kosmos/jugi1.jpg", "images/kosmos/jugi2.jpg"},
        std::pair<std::string, std::string>{"images/kosmos/anis1.jpg", "images/kosmos/anis2.jpg"},
        std::pair<std::string, std::string>{"images/kosmos/car1.jpg", "images/kosmos/car2.jpg"},
    };

    bool all_credits_loaded = true;
    for (size_t i = 0; i < credit_files.size(); ++i) {
      const auto& pair = credit_files[i];
      bool ok = LoadForwardImage(pair.first, &mute95.credits[i].first, &image_error);
      if (!ok) {
        std::cerr << "mute95 credit load failed: " << image_error << "\n";
        all_credits_loaded = false;
      }
      ok = LoadForwardImage(pair.second, &mute95.credits[i].second, &image_error);
      if (!ok) {
        std::cerr << "mute95 credit load failed: " << image_error << "\n";
        all_credits_loaded = false;
      }
    }

    const std::string palette_path = ResolveForwardAssetPath("images/kosmos/krad3.gif");
    const bool has_palette = !palette_path.empty() && LoadGifGlobalPalette(palette_path, &mute95.palette);
    if (!has_palette) {
      std::cerr << "mute95 palette load failed: unable to parse GIF global palette\n";
    }
    mute95.enabled = all_credits_loaded && has_palette;
  }

  if (std::filesystem::path(mesh_path).filename().string() == "fetus.igu") {
    std::string image_error;
    const std::string babyenv_path = ResolveForwardAssetPath("images/babyenv.jpg");
    const std::string flare_path = ResolveForwardAssetPath("images/flare1.jpg");
    const std::string kosmusp_path = ResolveForwardAssetPath("images/verax/kosmusp.jpg");
    const std::string background_mesh_path = ResolveFirstExistingForwardPath(
        std::array<std::string, 2>{"meshes/octa8.igu", "meshes/half8.igu"});

    const bool has_babyenv =
        !babyenv_path.empty() && forward::core::LoadImage32(babyenv_path, feta.babyenv, &image_error);
    if (!has_babyenv && !babyenv_path.empty()) {
      std::cerr << "feta babyenv load failed: " << image_error << "\n";
    }

    const bool has_flare =
        !flare_path.empty() && forward::core::LoadImage32(flare_path, feta.flare, &image_error);
    if (!has_flare && !flare_path.empty()) {
      std::cerr << "feta flare load failed: " << image_error << "\n";
    }

    if (!kosmusp_path.empty() &&
        !forward::core::LoadImage32(kosmusp_path, background.texture, &image_error)) {
      std::cerr << "kaaakma background texture load failed: " << image_error << "\n";
    }
    if (!background_mesh_path.empty() &&
        !forward::core::LoadIguMesh(background_mesh_path, background.mesh, &mesh_error)) {
      std::cerr << "kaaakma background mesh load failed: " << mesh_error << "\n";
      background.mesh.Clear();
    }
    if (!background.mesh.Empty() && !background.texture.Empty()) {
      const float background_radius = background.mesh.BoundingRadius();
      background_instance.uniform_scale =
          (background_radius > 0.001f) ? (10000.0f / background_radius) : 10000.0f;
      background.enabled = true;
    }

    feta.enabled = has_babyenv;
    particles.flare = feta.flare;
    particles.enabled = has_flare;
  }

  {
    std::string image_error;
    Image32 saari_height;
    Image32 saari_tex_full;
    Image32 saari_backdrop_full;

    const bool has_height =
        LoadForwardImage("images/scape/saarih15.gif", &saari_height, &image_error);
    if (!has_height) {
      std::cerr << "saari heightmap load failed: " << image_error << "\n";
    }

    const bool has_terrain_tex =
        LoadForwardImage("images/scape/saari.gif", &saari_tex_full, &image_error);
    if (!has_terrain_tex) {
      std::cerr << "saari texture load failed: " << image_error << "\n";
    }

    const bool has_backdrop =
        LoadForwardImage("images/verax/tai1sp.jpg", &saari_backdrop_full, &image_error);
    if (!has_backdrop) {
      std::cerr << "saari backdrop load failed: " << image_error << "\n";
    }

    if (has_terrain_tex) {
      saari.terrain_texture = ExtractTopHalf(saari_tex_full);
      if (saari.terrain_texture.Empty()) {
        saari.terrain_texture = std::move(saari_tex_full);
      }
    }
    if (has_backdrop) {
      saari.backdrop_texture = ExtractRect(saari_backdrop_full, 0, 0, 256, 256);
      if (saari.backdrop_texture.Empty()) {
        saari.backdrop_texture = std::move(saari_backdrop_full);
      }
    }

    bool mesh_ok = false;
    if (has_height) {
      mesh_ok = BuildSaariTerrainMeshFromHeightmap(saari_height, &saari.terrain);
      if (!mesh_ok) {
        std::cerr << "saari terrain mesh build failed\n";
      }
    }

    bool backdrop_mesh_ok = false;
    const std::string half8_path = ResolveForwardAssetPath("meshes/half8.igu");
    if (!half8_path.empty() &&
        forward::core::LoadIguMesh(half8_path, saari.backdrop_mesh, &mesh_error) &&
        !saari.backdrop_mesh.Empty()) {
      const float r = saari.backdrop_mesh.BoundingRadius();
      saari.backdrop_scale = (r > 0.001f) ? (10000.0f / r) : 10000.0f;
      backdrop_mesh_ok = true;
    } else {
      std::cerr << "saari backdrop mesh load failed\n";
    }

    const std::string saari_ase_path = ResolveForwardAssetPath("asses/alku6.ase");
    bool tracks_ok = false;
    bool objects_ok = false;
    if (!saari_ase_path.empty()) {
      tracks_ok = ParseSaariAseCameraTracks(saari_ase_path,
                                            &saari.camera_track,
                                            &saari.target_track,
                                            &saari.camera_fov_degrees);
      objects_ok = ParseSaariAseObjects(saari_ase_path, &saari.animated_objects);
    }
    if (!tracks_ok) {
      std::cerr << "saari camera tracks parse failed\n";
    }
    if (!objects_ok) {
      std::cerr << "saari ASE object parse failed\n";
    } else {
      std::cerr << "saari ASE objects loaded: " << saari.animated_objects.size() << "\n";
    }

    saari.enabled = mesh_ok && !saari.terrain_texture.Empty() && !saari.backdrop_texture.Empty() &&
                    backdrop_mesh_ok;
  }

  QuickWinPostLayer post;

  const std::string phorward_path = ResolveForwardAssetPath("images/phorward.gif");
  std::string image_error;
  if (!phorward_path.empty() &&
      forward::core::LoadImage32(phorward_path, domina.phorward, &image_error)) {
    domina.enabled = true;
    post.primary = domina.phorward;
    post.enabled = true;
  } else if (!phorward_path.empty()) {
    std::cerr << "domina image load failed: " << image_error << "\n";
    std::cerr << "quick-win image load failed: " << image_error << "\n";
  }

  std::string secondary_path = ResolveForwardAssetPath("images/komplex.gif");
  if (secondary_path.empty()) {
    secondary_path = ResolveForwardAssetPath("images/back.gif");
  }
  if (!secondary_path.empty()) {
    Image32 secondary;
    if (forward::core::LoadImage32(secondary_path, secondary, &image_error)) {
      post.secondary = std::move(secondary);
      domina.komplex = post.secondary;
    } else {
      std::cerr << "secondary post image load failed: " << image_error << "\n";
    }
  }

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

  const int initial_w = kLogicalWidth * kWindowScale;
  const int initial_h = kLogicalHeight * kWindowScale;

  SDL_Window* window = SDL_CreateWindow("forward native harness",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        initial_w,
                                        initial_h,
                                        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  if (!window) {
    std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
    SDL_Quit();
    return 1;
  }

  SDL_Renderer* renderer_sdl =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer_sdl) {
    renderer_sdl = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  }
  if (!renderer_sdl) {
    std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_Texture* texture = SDL_CreateTexture(renderer_sdl,
                                           SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           kLogicalWidth,
                                           kLogicalHeight);
  if (!texture) {
    std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << "\n";
    SDL_DestroyRenderer(renderer_sdl);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  Surface32 surface(kLogicalWidth, kLogicalHeight, true);
  Surface32 halo_surface(kLogicalWidth, kLogicalHeight, true);
  Renderer3D renderer_3d(kLogicalWidth, kLogicalHeight);

  DemoState state;
  if (mute95.enabled && domina.enabled && saari.enabled) {
    state.scene_mode = SceneMode::kMute95DominaSequence;
    state.scene_label = "mute95->domina->saari";
  } else if (mute95.enabled && domina.enabled) {
    state.scene_mode = SceneMode::kMute95DominaSequence;
    state.scene_label = "mute95->domina";
  } else if (mute95.enabled) {
    state.scene_mode = SceneMode::kMute95;
    state.scene_label = "mute95";
  } else if (domina.enabled) {
    state.scene_mode = SceneMode::kDomina;
    state.scene_label = "domina";
  } else if (saari.enabled) {
    state.scene_mode = SceneMode::kSaari;
    state.scene_label = "saari";
  } else if (feta.enabled && background.enabled && particles.enabled) {
    state.scene_mode = SceneMode::kFeta;
    state.scene_label = "feta+kaaakma+mmaamka";
  } else if (feta.enabled) {
    state.scene_mode = SceneMode::kFeta;
    state.scene_label = "feta";
  } else {
    state.scene_mode = SceneMode::kFeta;
    state.scene_label = "fallback";
  }
  state.mesh_label = std::filesystem::path(mesh_path).filename().string();
  state.post_label = state.show_post && post.enabled ? "phorward" : "off";

  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--scene=mute95" || arg == "--mute95") {
      if (mute95.enabled) {
        state.scene_mode = SceneMode::kMute95;
        state.scene_label = "mute95";
      }
    } else if (arg == "--scene=domina" || arg == "--domina") {
      if (domina.enabled) {
        state.scene_mode = SceneMode::kDomina;
        state.scene_label = "domina";
      }
    } else if (arg == "--scene=saari" || arg == "--saari") {
      if (saari.enabled) {
        state.scene_mode = SceneMode::kSaari;
        state.scene_label = "saari";
      }
    } else if (arg == "--scene=row" || arg == "--row" || arg == "--scene=script" ||
               arg == "--script") {
      if (mute95.enabled && domina.enabled) {
        state.scene_mode = SceneMode::kMute95DominaSequence;
        state.scene_label = saari.enabled ? "mute95->domina->saari" : "mute95->domina";
      }
    } else if (arg == "--scene=feta" || arg == "--feta") {
      state.scene_mode = SceneMode::kFeta;
      state.scene_label = feta.enabled ? "feta+kaaakma+mmaamka" : "feta-fallback";
    }
  }

  RuntimeStats stats;

  uint64_t perf_prev = SDL_GetPerformanceCounter();
  const uint64_t perf_freq = SDL_GetPerformanceFrequency();
  double accumulator = 0.0;
  double title_elapsed = 0.0;
  bool running = true;

  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
          case SDLK_q:
            running = false;
            break;
          case SDLK_SPACE:
            state.paused = !state.paused;
            break;
          case SDLK_f:
            state.fullscreen = !state.fullscreen;
            SDL_SetWindowFullscreen(window,
                                    state.fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
            break;
          case SDLK_p:
            state.show_post = !state.show_post;
            state.post_label = state.show_post && post.enabled ? "phorward" : "off";
            break;
          case SDLK_LEFTBRACKET:
          case SDLK_MINUS:
            state.feta_fov_degrees = std::clamp(state.feta_fov_degrees - 1.0f, 40.0f, 120.0f);
            break;
          case SDLK_RIGHTBRACKET:
          case SDLK_EQUALS:
            state.feta_fov_degrees = std::clamp(state.feta_fov_degrees + 1.0f, 40.0f, 120.0f);
            break;
          case SDLK_1:
            if (mute95.enabled) {
              state.scene_mode = SceneMode::kMute95;
              state.scene_label = "mute95";
              state.scene_start_seconds = state.timeline_seconds;
              mute95_runtime.initialized = false;
            }
            break;
          case SDLK_2:
            state.scene_mode = SceneMode::kFeta;
            state.scene_label = feta.enabled ? "feta+kaaakma+mmaamka" : "feta-fallback";
            state.scene_start_seconds = state.timeline_seconds;
            particles.initialized = false;
            break;
          case SDLK_3:
            if (domina.enabled) {
              state.scene_mode = SceneMode::kDomina;
              state.scene_label = "domina";
              state.scene_start_seconds = state.timeline_seconds;
              domina_runtime.initialized = false;
            }
            break;
          case SDLK_4:
            if (mute95.enabled && domina.enabled) {
              state.scene_mode = SceneMode::kMute95DominaSequence;
              state.scene_label = saari.enabled ? "mute95->domina->saari" : "mute95->domina";
              state.scene_start_seconds = state.timeline_seconds;
              mute95_runtime.initialized = false;
              domina_runtime.initialized = false;
              saari_runtime.initialized = false;
            }
            break;
          case SDLK_5:
            if (saari.enabled) {
              state.scene_mode = SceneMode::kSaari;
              state.scene_label = "saari";
              state.scene_start_seconds = state.timeline_seconds;
              saari_runtime.initialized = false;
            }
            break;
          case SDLK_6:
            if (state.scene_mode == SceneMode::kSaari) {
              TriggerSaariMessage(saari_runtime, true);  // "suh"
            }
            break;
          case SDLK_7:
            if (state.scene_mode == SceneMode::kSaari) {
              TriggerSaariMessage(saari_runtime, false);  // "suh0"
            }
            break;
          default:
            break;
        }
      }
    }

    const uint64_t perf_now = SDL_GetPerformanceCounter();
    const double frame_dt =
        static_cast<double>(perf_now - perf_prev) / static_cast<double>(perf_freq);
    perf_prev = perf_now;

    accumulator += frame_dt;
    title_elapsed += frame_dt;

    int ticks_this_frame = 0;
    while (accumulator >= kTickDtSeconds) {
      if (!state.paused) {
        state.timeline_seconds += kTickDtSeconds;
      }
      accumulator -= kTickDtSeconds;
      ++ticks_this_frame;
    }
    stats.simulated_ticks += static_cast<uint64_t>(ticks_this_frame);

    DrawFrame(surface,
              state,
              mute95,
              mute95_runtime,
              domina,
              domina_runtime,
              saari,
              saari_runtime,
              mesh,
              background,
              particles,
              camera,
              renderer_3d,
              mesh_instance,
              halo_instance,
              background_instance,
              saari_backdrop_instance,
              saari_terrain_instance,
              saari_object_instance,
              halo_surface,
              feta,
              post);

    if (SDL_UpdateTexture(texture,
                          nullptr,
                          surface.FrontPixels(),
                          kLogicalWidth * static_cast<int>(sizeof(uint32_t))) != 0) {
      std::cerr << "SDL_UpdateTexture failed: " << SDL_GetError() << "\n";
      running = false;
    }

    SDL_SetRenderDrawColor(renderer_sdl, 0, 0, 0, 255);
    SDL_RenderClear(renderer_sdl);

    const SDL_Rect dst = ComputePresentationRect(renderer_sdl);
    SDL_RenderCopy(renderer_sdl, texture, nullptr, &dst);
    SDL_RenderPresent(renderer_sdl);

    ++stats.rendered_frames;

    if (title_elapsed >= 0.5) {
      UpdateWindowTitle(window, state, stats, title_elapsed);
      stats.rendered_frames = 0;
      stats.simulated_ticks = 0;
      title_elapsed = 0.0;
    }
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer_sdl);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
