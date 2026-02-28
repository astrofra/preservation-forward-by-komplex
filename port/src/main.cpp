#include <SDL.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "core/Camera.h"
#include "core/GifIndexed.h"
#include "core/Image32.h"
#include "core/IndexedSurface8.h"
#include "core/LegacyPacked10.h"
#include "core/Mesh.h"
#include "core/MeshLoaderIgu.h"
#include "core/Renderer3D.h"
#include "core/Surface32.h"
#include "core/Vec3.h"
#include "core/XmPlayer.h"

namespace {

using forward::core::Camera;
using forward::core::IndexedImage8;
using forward::core::IndexedSurface8;
using forward::core::Image32;
using forward::core::Mesh;
using forward::core::RenderInstance;
using forward::core::Renderer3D;
using forward::core::Surface32;
using forward::core::Vec3;
using forward::core::XmPlayer;
using forward::core::XmTiming;
namespace legacy10 = forward::core::legacy10;

constexpr int kLogicalWidth = 512;
constexpr int kLogicalHeight = 256;
constexpr int kWindowScale = 1;  // 1x1 mode only
constexpr double kTickHz = 50.0;
constexpr double kTickDtSeconds = 1.0 / kTickHz;
constexpr float kPi = 3.14159265358979323846f;
constexpr int kMute95ToDominaRow = 0x0D00;
constexpr int kMod1ToMod2Row = 0x1024;
constexpr int kMod2ToKukotRow = 0x0700;
constexpr int kMod2ToMakuRow = 0x0D00;
constexpr int kMod2ToWatercubeRow = 0x1000;
constexpr int kMod2ToFetaRow = 0x1300;
constexpr int kMod2ToUppolRow = 0x1600;
constexpr double kScriptFallbackToFetaSeconds = 66.0;
constexpr double kScriptFallbackToUppolSeconds = 74.0;

struct RuntimeStats {
  uint64_t rendered_frames = 0;
  uint64_t simulated_ticks = 0;
};

enum class SceneMode {
  kMute95,
  kDomina,
  kMute95DominaSequence,
  kSaari,
  kUppol,
  kFeta,
};

enum class SequenceStage {
  kMute95,
  kDomina,
  kSaari,
  kKukot,
  kMaku,
  kWatercube,
};

struct DemoState {
  double timeline_seconds = 0.0;
  double scene_start_seconds = 0.0;
  double frame_dt_seconds = 1.0 / 60.0;
  bool paused = false;
  bool fullscreen = false;
  bool show_post = false;
  float feta_fov_degrees = 84.0f;  // horizontal FOV
  SceneMode scene_mode = SceneMode::kFeta;
  SequenceStage sequence_stage = SequenceStage::kMute95;
  int music_module_slot = 0;
  int music_order_row = -1;
  bool script_driven = false;
  std::string scene_label;
  std::string mesh_label;
  std::string post_label;
};

struct WatercubeValidationHarness {
  bool enabled = false;
  bool has_reference_dir = false;
  std::filesystem::path output_dir;
  std::filesystem::path reference_dir;
  std::vector<int> checkpoints = {0x1004, 0x1100, 0x1200, 0x1210, 0x1220, 0x1230};
  std::unordered_set<int> captured_rows;
  int last_order_row = -1;
};

struct FetaValidationHarness {
  bool enabled = false;
  bool has_reference_dir = false;
  std::filesystem::path output_dir;
  std::filesystem::path reference_dir;
  std::vector<int> checkpoints = {0x1300, 0x1520, 0x1530, 0x1600};
  std::unordered_set<int> captured_rows;
  int last_order_row = -1;
};

struct MusicState {
  bool enabled = false;
  bool has_mod1 = false;
  bool has_mod2 = false;
  bool module2_started = false;
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

struct UppolSceneAssets {
  IndexedImage8 phorward;
  bool enabled = false;
};

struct UppolRuntime {
  std::unique_ptr<IndexedSurface8> working;
  int frame_counter = 0;
  bool initialized = false;
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

struct FetaRuntime {
  bool initialized = false;
  bool palette_index_255_black = true;
  bool current_indices_a = true;
  std::array<uint32_t, 256> palette_packed10{};
  std::vector<uint8_t> indices_a;
  std::vector<uint8_t> indices_b;
  std::vector<uint8_t> mesh_mask;
  std::vector<uint32_t> packed_frame;
  double blackfeta_start_seconds = 0.0;
  double blackmuna_start_seconds = 0.0;
  int last_order_row = -1;
  int next_script_event = 0;
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

struct MakuSceneAssets {
  Mesh terrain;
  Image32 terrain_texture;
  float camera_fov_degrees = 80.0f;
  std::vector<SaariSceneAssets::TrackKey> camera_track;
  std::vector<SaariSceneAssets::TrackKey> target_track;
  bool enabled = false;
};

struct WatercubeSceneAssets {
  Image32 panel_overlay;
  Image32 scroll_texture;
  Image32 box_texture;
  Image32 ring_texture;
  Image32 ripple_texture;
  Image32 env_texture;
  float camera_fov_degrees = 80.0f;
  std::vector<SaariSceneAssets::TrackKey> camera_track;
  std::vector<SaariSceneAssets::TrackKey> target_track;
  std::vector<SaariSceneAssets::AnimatedObject> animated_objects;
  Mesh kluns1;
  Mesh kluns2;
  bool has_kluns2 = false;
  bool enabled = false;
};

struct KukotSceneAssets {
  Image32 object_texture;
  Image32 random_tile;
  Image32 flare;
  float camera_fov_degrees = 80.0f;
  std::vector<SaariSceneAssets::TrackKey> camera_track;
  std::vector<SaariSceneAssets::TrackKey> target_track;
  std::vector<SaariSceneAssets::AnimatedObject> animated_objects;
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

struct MakuRuntime {
  float playback_speed = -3.0f;
  float go_base_seconds = 160.5f;
  double go_anchor_seconds = 0.0;
  float roll_angle = 0.0f;
  bool roll_enabled = false;
  bool ksor_enabled = false;
  float flash_intensity = 0.0f;
  float flash_decay = 0.0f;
  int next_script_event = 0;
  bool initialized = false;
};

struct WatercubeRuntime {
  int ripple_width = 256;
  int ripple_height = 256;
  int panel_width = 128;
  int panel_height = 128;

  std::vector<uint32_t> ripple_a;
  std::vector<uint32_t> ripple_b;
  std::vector<uint32_t> ripple_combined;
  std::vector<uint32_t> ring_texture_10;
  int ring_width = 0;
  int ring_height = 0;
  std::vector<uint32_t> ripple_texture_10;
  std::vector<uint32_t> panel_overlay_10;
  int panel_overlay_width = 0;
  int panel_overlay_height = 0;
  std::vector<uint32_t> panel_buffer_10;
  Image32 water_dynamic_argb;
  Image32 panel_dynamic_argb;
  std::vector<uint32_t> flash_lut_10;
  std::vector<int> flash_scanline_order;
  std::vector<uint32_t> frame_packed_10;
  int panel_scale = 2;
  float kluns1_rot_x = 0.7f;
  float kluns1_rot_z = 0.0f;
  float kluns2_rot_x = -0.7f;
  float kluns2_rot_z = 0.0f;

  uint32_t rng_state = 0x57415445u;  // "WATE"
  uint64_t java_random_state = 0ull;
  int frame_counter = 0;
  bool source_is_b = true;

  float flash_amount = 0.0f;
  float flash_decay = 0.0f;
  float roll_impulse = 0.0f;
  float shock_amount = 0.0f;
  float shock_decay = 0.0f;
  int tex_strip_offset = 0;

  int next_script_event = 0;
  int last_order_row = -1;
  bool initialized = false;
};

struct KukotRuntime {
  uint32_t rng_state = 0x4b554b4fu;  // "KUKO"
  std::vector<uint32_t> flash_lut;
  std::vector<int> flash_scanline_order;
  std::vector<Particle> particles;
  std::vector<Mesh> deformed_meshes;
  float flash_intensity = 0.0f;
  float flash_decay = 0.0f;
  int next_script_event = 0;
  int last_order_row = -1;
  double prev_scene_seconds = 0.0;
  bool initialized = false;
};

Vec3 RotateXSimple(const Vec3& v, float angle);
Vec3 RotateYSimple(const Vec3& v, float angle);
bool ProjectPointToScreen(const Camera& camera,
                          const Vec3& world_pos,
                          int* out_x,
                          int* out_y,
                          float* out_depth);

uint32_t PackArgb(uint8_t r, uint8_t g, uint8_t b) {
  return (0xFFu << 24u) | (static_cast<uint32_t>(r) << 16u) |
         (static_cast<uint32_t>(g) << 8u) | static_cast<uint32_t>(b);
}

uint8_t UnpackR(uint32_t argb) { return static_cast<uint8_t>((argb >> 16u) & 0xFFu); }
uint8_t UnpackG(uint32_t argb) { return static_cast<uint8_t>((argb >> 8u) & 0xFFu); }
uint8_t UnpackB(uint32_t argb) { return static_cast<uint8_t>(argb & 0xFFu); }

uint32_t LegacyPacked10ToArgb(uint32_t packed10) {
  // Original Java renderer stores RGB in a 10-10-10 style integer and displays through
  // a 28-bit DirectColorModel. This reproduces the effective 8-bit channels.
  const uint8_t r = static_cast<uint8_t>((packed10 >> 20u) & 0xFFu);
  const uint8_t g = static_cast<uint8_t>((packed10 >> 10u) & 0xFFu);
  const uint8_t b = static_cast<uint8_t>(packed10 & 0xFFu);
  return PackArgb(r, g, b);
}

uint32_t PackLegacy10(int r10, int g10, int b10) {
  const uint32_t r = static_cast<uint32_t>(r10 & 0x3FF);
  const uint32_t g = static_cast<uint32_t>(g10 & 0x3FF);
  const uint32_t b = static_cast<uint32_t>(b10 & 0x3FF);
  return (r << 20u) | (g << 10u) | b;
}

void ConvertArgbImageToPacked10(const Image32& image, std::vector<uint32_t>* out_packed10) {
  if (!out_packed10) {
    return;
  }
  if (image.Empty()) {
    out_packed10->clear();
    return;
  }
  out_packed10->resize(image.pixels.size());
  for (size_t i = 0; i < image.pixels.size(); ++i) {
    const uint32_t c = image.pixels[i];
    (*out_packed10)[i] = legacy10::PackRgb8To10(
        static_cast<uint8_t>((c >> 16u) & 0xFFu),
        static_cast<uint8_t>((c >> 8u) & 0xFFu),
        static_cast<uint8_t>(c & 0xFFu));
  }
}

void EnsureArgbImageStorage(Image32* image, int width, int height) {
  if (!image || width <= 0 || height <= 0) {
    return;
  }
  image->width = width;
  image->height = height;
  image->pixels.assign(static_cast<size_t>(width) * static_cast<size_t>(height), PackArgb(0, 0, 0));
}

int PackOrderRow(int order, int row) {
  return ((order & 0xFF) << 8) | (row & 0xFF);
}

SequenceStage DetermineSequenceStage(const XmTiming& timing,
                                     bool saari_enabled,
                                     bool kukot_enabled,
                                     bool maku_enabled,
                                     bool watercube_enabled,
                                     double fallback_script_seconds) {
  if (timing.valid) {
    const int order_row = PackOrderRow(timing.order, timing.row);
    if (timing.module_slot <= 1) {
      return (order_row < kMute95ToDominaRow) ? SequenceStage::kMute95 : SequenceStage::kDomina;
    }
    if (!saari_enabled) {
      return SequenceStage::kDomina;
    }
    if (order_row < kMod2ToKukotRow) {
      return SequenceStage::kSaari;
    }
    if (order_row < kMod2ToMakuRow) {
      return kukot_enabled ? SequenceStage::kKukot : SequenceStage::kSaari;
    }
    if (!maku_enabled) {
      return kukot_enabled ? SequenceStage::kKukot : SequenceStage::kSaari;
    }
    if (order_row < kMod2ToWatercubeRow || !watercube_enabled) {
      return SequenceStage::kMaku;
    }
    if (order_row < kMod2ToFetaRow) {
      return SequenceStage::kWatercube;
    }
    return SequenceStage::kWatercube;
  }

  if (fallback_script_seconds < 13.0) {
    return SequenceStage::kMute95;
  }
  if (fallback_script_seconds < 29.0 || !saari_enabled) {
    return SequenceStage::kDomina;
  }
  if (fallback_script_seconds < 36.0) {
    return SequenceStage::kSaari;
  }
  if (fallback_script_seconds < 46.0 || !maku_enabled) {
    if (!kukot_enabled) {
      return SequenceStage::kSaari;
    }
    return SequenceStage::kKukot;
  }
  if (fallback_script_seconds < 58.0 || !watercube_enabled) {
    return SequenceStage::kMaku;
  }
  return SequenceStage::kWatercube;
}

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

Image32 BuildKukotEnvTextureFromPalette(const std::array<uint32_t, 256>& palette,
                                        float blend_r,
                                        float blend_g,
                                        float blend_b) {
  Image32 out;
  out.width = 256;
  out.height = 256;
  out.pixels.resize(static_cast<size_t>(out.width) * static_cast<size_t>(out.height));

  for (int y = 0; y < 256; ++y) {
    const float d = 1.0f - static_cast<float>(y) / 255.0f;
    for (int x = 0; x < 256; ++x) {
      const uint32_t base = palette[static_cast<size_t>(x)];
      const float r = std::min(255.0f, static_cast<float>(UnpackR(base)) * d + (1.0f - d) * blend_r);
      const float g = std::min(255.0f, static_cast<float>(UnpackG(base)) * d + (1.0f - d) * blend_g);
      const float b = std::min(255.0f, static_cast<float>(UnpackB(base)) * d + (1.0f - d) * blend_b);
      out.pixels[static_cast<size_t>(y) * 256u + static_cast<size_t>(x)] =
          PackArgb(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b));
    }
  }
  return out;
}

Image32 BuildKukotRandomTile(uint32_t seed) {
  auto next_u32 = [&seed]() {
    uint32_t x = seed;
    if (x == 0u) {
      x = 0x6D2B79F5u;
    }
    x ^= x << 13u;
    x ^= x >> 17u;
    x ^= x << 5u;
    seed = x;
    return x;
  };
  auto next_unit = [&next_u32]() {
    return static_cast<float>(next_u32() & 0x00FFFFFFu) / static_cast<float>(0x01000000u);
  };

  Image32 out;
  out.width = 256;
  out.height = 256;
  out.pixels.resize(static_cast<size_t>(out.width) * static_cast<size_t>(out.height));
  for (int y = 0; y < out.height; ++y) {
    for (int x = 0; x < out.width; ++x) {
      const float r0 = next_unit();
      const float r1 = next_unit();
      const float r2 = next_unit();
      const int nr = static_cast<int>(20.0f + r0 * r0 * r0 * r0 * 200.0f);
      const int ng = static_cast<int>(26.0f + r1 * 50.0f);
      const int nb = static_cast<int>(22.0f + r2 * 26.0f);
      const uint32_t packed10 = PackLegacy10(nr, ng, nb);
      out.pixels[static_cast<size_t>(y) * static_cast<size_t>(out.width) + static_cast<size_t>(x)] =
          LegacyPacked10ToArgb(packed10);
    }
  }
  return out;
}

bool BuildTerrainMeshFromHeightmap(const Image32& heightmap,
                                   float world_span,
                                   float height_scale,
                                   int height_bias,
                                   Mesh* out_mesh) {
  if (!out_mesh || heightmap.Empty() || heightmap.width < 2 || heightmap.height < 2) {
    return false;
  }

  const int w = heightmap.width;
  const int h = heightmap.height;
  const float step = world_span / static_cast<float>(w);

  out_mesh->Clear();
  out_mesh->positions.reserve(static_cast<size_t>(w) * static_cast<size_t>(h));
  out_mesh->texcoords.reserve(static_cast<size_t>(w) * static_cast<size_t>(h));
  out_mesh->triangles.reserve(static_cast<size_t>(w - 1) * static_cast<size_t>(h - 1) * 2u);

  for (int gy = 0; gy < h; ++gy) {
    for (int gx = 0; gx < w; ++gx) {
      const size_t idx = static_cast<size_t>(gy) * static_cast<size_t>(w) + static_cast<size_t>(gx);
      const uint8_t r = UnpackR(heightmap.pixels[idx]);
      const float hgt =
          static_cast<float>(std::max(0, static_cast<int>(r) - height_bias)) * height_scale;

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

bool BuildSaariTerrainMeshFromHeightmap(const Image32& heightmap, Mesh* out_mesh) {
  // maajmka uses world span 200 and height scale 0.16 with a small bias.
  return BuildTerrainMeshFromHeightmap(heightmap, 200.0f, 0.16f, 16, out_mesh);
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
      } else if (active_node == "Camera01.Target" || active_node == "Camera01.target") {
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

bool ParseAseAnimatedObjects(const std::string& path,
                             const std::vector<std::string>& allowed_names,
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
  struct TVert {
    float u = 0.0f;
    float v = 0.0f;
  };
  struct TFace {
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
    std::vector<TVert> texverts;
    std::vector<TFace> tfaces;
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
  auto ensure_tvert_size = [](std::vector<TVert>* v, int idx) {
    if (idx < 0 || !v) {
      return;
    }
    const size_t need = static_cast<size_t>(idx + 1);
    if (v->size() < need) {
      v->resize(need);
    }
  };
  auto ensure_tface_size = [](std::vector<TFace>* v, int idx) {
    if (idx < 0 || !v) {
      return;
    }
    const size_t need = static_cast<size_t>(idx + 1);
    if (v->size() < need) {
      v->resize(need);
    }
  };

  std::unordered_set<std::string> allowed_name_set;
  for (const std::string& name : allowed_names) {
    if (!name.empty()) {
      allowed_name_set.insert(name);
    }
  }

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
    if (!allowed_name_set.empty() &&
        allowed_name_set.find(raw.name) == allowed_name_set.end()) {
      return;
    }

    SaariSceneAssets::AnimatedObject out;
    out.name = raw.name;
    out.base_position = raw.tm_pos;
    out.base_rotation = QuatFromAxisAngle(raw.tm_rot_axis, raw.tm_rot_angle);

    const Quat inv_base_rot = QuatConjugate(out.base_rotation);

    const bool has_tfaces = !raw.tfaces.empty() && raw.tfaces.size() == raw.faces.size();
    const bool has_tverts = !raw.texverts.empty();
    if (has_tfaces && has_tverts) {
      std::unordered_map<uint64_t, int> remap;
      out.mesh.positions.reserve(raw.faces.size() * 3u);
      out.mesh.texcoords.reserve(raw.faces.size() * 3u);
      out.mesh.triangles.reserve(raw.faces.size());
      for (size_t fi = 0; fi < raw.faces.size(); ++fi) {
        const Face& f = raw.faces[fi];
        const TFace& tf = raw.tfaces[fi];
        const int vi[3] = {f.a, f.b, f.c};
        const int ti[3] = {tf.a, tf.b, tf.c};
        int tri_idx[3] = {-1, -1, -1};
        for (int corner = 0; corner < 3; ++corner) {
          const int v_idx = vi[corner];
          const int t_idx = ti[corner];
          if (v_idx < 0 || t_idx < 0 ||
              v_idx >= static_cast<int>(raw.vertices_world.size()) ||
              t_idx >= static_cast<int>(raw.texverts.size())) {
            continue;
          }
          const uint64_t key =
              (static_cast<uint64_t>(static_cast<uint32_t>(v_idx)) << 32u) |
              static_cast<uint32_t>(t_idx);
          auto it = remap.find(key);
          if (it == remap.end()) {
            const Vec3 local = RotateByQuat(raw.vertices_world[static_cast<size_t>(v_idx)] - raw.tm_pos,
                                            inv_base_rot);
            const TVert uv = raw.texverts[static_cast<size_t>(t_idx)];
            const int new_idx = static_cast<int>(out.mesh.positions.size());
            out.mesh.positions.push_back(local);
            out.mesh.texcoords.emplace_back(uv.u, 1.0f - uv.v);
            remap.emplace(key, new_idx);
            tri_idx[corner] = new_idx;
          } else {
            tri_idx[corner] = it->second;
          }
        }
        if (tri_idx[0] >= 0 && tri_idx[1] >= 0 && tri_idx[2] >= 0) {
          out.mesh.triangles.push_back({tri_idx[0], tri_idx[1], tri_idx[2]});
        }
      }
    } else {
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
    }

    if (out.mesh.positions.empty() || out.mesh.triangles.empty()) {
      return;
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
  bool in_tvert_list = false;
  int tvert_list_depth = 0;
  bool in_tface_list = false;
  int tface_list_depth = 0;

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
        if (!tokens.empty() && tokens[0] == "*MESH_TVERTLIST") {
          in_tvert_list = true;
          tvert_list_depth = 0;
        }
        if (!tokens.empty() && tokens[0] == "*MESH_TFACELIST") {
          in_tface_list = true;
          tface_list_depth = 0;
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
        if (in_tvert_list && !tokens.empty() && tokens[0] == "*MESH_TVERT" && tokens.size() >= 4) {
          const int idx = std::stoi(tokens[1]);
          ensure_tvert_size(&current.texverts, idx);
          current.texverts[static_cast<size_t>(idx)] = TVert{std::stof(tokens[2]), std::stof(tokens[3])};
        }
        if (in_tface_list && !tokens.empty() && tokens[0] == "*MESH_TFACE" && tokens.size() >= 5) {
          const int idx = std::stoi(tokens[1]);
          ensure_tface_size(&current.tfaces, idx);
          current.tfaces[static_cast<size_t>(idx)] = TFace{
              std::stoi(tokens[2]), std::stoi(tokens[3]), std::stoi(tokens[4])};
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
    if (in_tvert_list) {
      tvert_list_depth += brace_delta;
      if (tvert_list_depth <= 0) {
        in_tvert_list = false;
      }
    }
    if (in_tface_list) {
      tface_list_depth += brace_delta;
      if (tface_list_depth <= 0) {
        in_tface_list = false;
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

bool ParseSaariAseObjects(const std::string& path,
                          std::vector<SaariSceneAssets::AnimatedObject>* out_objects) {
  static const std::vector<std::string> kSaariObjectNames = {"meditate", "klunssi"};
  return ParseAseAnimatedObjects(path, kSaariObjectNames, out_objects);
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
                       const MusicState& music,
                       const XmTiming& timing,
                       double elapsed_since_last_title) {
  const double fps = static_cast<double>(stats.rendered_frames) /
                     std::max(elapsed_since_last_title, 0.0001);
  const double ups = static_cast<double>(stats.simulated_ticks) /
                     std::max(elapsed_since_last_title, 0.0001);

  std::ostringstream title;
  std::string audio_label = "off";
  if (music.enabled) {
    if (timing.valid) {
      std::ostringstream ss;
      ss << "m" << timing.module_slot << " "
         << std::hex << std::setfill('0') << std::setw(2) << (timing.order & 0xFF)
         << ":" << std::setw(2) << (timing.row & 0xFF) << std::dec;
      audio_label = ss.str();
    } else {
      audio_label = "sync-pending";
    }
  }

  title << "forward native harness | "
        << (state.paused ? "paused" : "running")
        << " | fps " << std::fixed << std::setprecision(1) << fps
        << " | ups " << std::fixed << std::setprecision(1) << ups
        << " | fov " << std::fixed << std::setprecision(1) << state.feta_fov_degrees
        << " | scene " << state.scene_label << " | mesh " << state.mesh_label
        << " | logical " << kLogicalWidth << "x"
        << kLogicalHeight << " | post " << state.post_label << " | audio " << audio_label;

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

constexpr int kUppolLeft = 0;
constexpr int kUppolRight = kLogicalWidth - 150;
constexpr int kUppolCenter = (kUppolLeft + kUppolRight) / 2;
constexpr int kUppolLineHeight = 26;
constexpr double kUppolScrollSpeed = 25.0;
constexpr int kUppolTextScale = 2;
constexpr int kUppolGlyphWidth = 5 * kUppolTextScale;
constexpr int kUppolGlyphHeight = 7 * kUppolTextScale;
constexpr int kUppolGlyphAdvance = kUppolGlyphWidth + 1;
constexpr uint32_t kUppolTextColor = 0xFFFFFFFFu;

const std::array<std::string, 47> kUppolLines = {
    "",
    "forward",
    "komplex",
    "",
    "",
    "",
    "",
    "",
    "code",
    "",
    "saviour",
    "jmagic",
    "anis",
    "",
    "",
    "graphics",
    "",
    "jugi",
    "",
    "",
    "intro theme",
    "",
    "jugi",
    "",
    "",
    "main theme",
    "",
    "carebear/orange",
    "",
    "",
    "klunssi object",
    "",
    "reward",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "rebellion will not be televised",
    "",
    "",
    "",
    "__mailto:komplex@jyu.fi",
    "__http://www.jyu.fi/komplex",
    "",
};

enum class UppolAlign {
  kCenter = 0,
  kLeft = 1,
  kRight = 2,
  kLink = 3,
};

std::array<uint8_t, 7> UppolGlyphRows(char c) {
  switch (c) {
    case 'a': return {0x00, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F};
    case 'b': return {0x10, 0x10, 0x1E, 0x11, 0x11, 0x11, 0x1E};
    case 'c': return {0x00, 0x00, 0x0E, 0x10, 0x10, 0x11, 0x0E};
    case 'd': return {0x01, 0x01, 0x0F, 0x11, 0x11, 0x11, 0x0F};
    case 'e': return {0x00, 0x00, 0x0E, 0x11, 0x1F, 0x10, 0x0F};
    case 'f': return {0x03, 0x04, 0x0E, 0x04, 0x04, 0x04, 0x04};
    case 'g': return {0x00, 0x00, 0x0F, 0x11, 0x11, 0x0F, 0x01};
    case 'h': return {0x10, 0x10, 0x1E, 0x11, 0x11, 0x11, 0x11};
    case 'i': return {0x04, 0x00, 0x0C, 0x04, 0x04, 0x04, 0x0E};
    case 'j': return {0x02, 0x00, 0x06, 0x02, 0x02, 0x12, 0x0C};
    case 'k': return {0x10, 0x10, 0x12, 0x14, 0x18, 0x14, 0x12};
    case 'l': return {0x0C, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E};
    case 'm': return {0x00, 0x00, 0x1A, 0x15, 0x15, 0x15, 0x15};
    case 'n': return {0x00, 0x00, 0x1E, 0x11, 0x11, 0x11, 0x11};
    case 'o': return {0x00, 0x00, 0x0E, 0x11, 0x11, 0x11, 0x0E};
    case 'p': return {0x00, 0x00, 0x1E, 0x11, 0x11, 0x1E, 0x10};
    case 'q': return {0x00, 0x00, 0x0F, 0x11, 0x11, 0x0F, 0x01};
    case 'r': return {0x00, 0x00, 0x1A, 0x14, 0x10, 0x10, 0x10};
    case 's': return {0x00, 0x00, 0x0F, 0x10, 0x0E, 0x01, 0x1E};
    case 't': return {0x04, 0x04, 0x1F, 0x04, 0x04, 0x04, 0x03};
    case 'u': return {0x00, 0x00, 0x11, 0x11, 0x11, 0x13, 0x0D};
    case 'v': return {0x00, 0x00, 0x11, 0x11, 0x11, 0x0A, 0x04};
    case 'w': return {0x00, 0x00, 0x11, 0x15, 0x15, 0x15, 0x0A};
    case 'x': return {0x00, 0x00, 0x11, 0x0A, 0x04, 0x0A, 0x11};
    case 'y': return {0x00, 0x00, 0x11, 0x11, 0x0F, 0x01, 0x0E};
    case 'z': return {0x00, 0x00, 0x1F, 0x02, 0x04, 0x08, 0x1F};
    case '.': return {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C};
    case ':': return {0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x00};
    case '/': return {0x01, 0x02, 0x02, 0x04, 0x08, 0x08, 0x10};
    case '@': return {0x0E, 0x11, 0x17, 0x15, 0x17, 0x10, 0x0E};
    case '-': return {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00};
    case '0': return {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E};
    case '1': return {0x04, 0x0C, 0x14, 0x04, 0x04, 0x04, 0x1F};
    case '2': return {0x0E, 0x11, 0x01, 0x06, 0x08, 0x10, 0x1F};
    case '3': return {0x1E, 0x01, 0x01, 0x0E, 0x01, 0x01, 0x1E};
    case '4': return {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02};
    case '5': return {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E};
    case '6': return {0x07, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E};
    case '7': return {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08};
    case '8': return {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E};
    case '9': return {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x1C};
    default: return {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  }
}

void DrawUppolGlyph(Surface32& surface, int x, int y, char c, uint32_t argb) {
  const auto rows = UppolGlyphRows(c);
  for (int gy = 0; gy < 7; ++gy) {
    const uint8_t row = rows[static_cast<size_t>(gy)];
    for (int gx = 0; gx < 5; ++gx) {
      if ((row & (1u << (4 - gx))) == 0) {
        continue;
      }
      const int px = x + gx * kUppolTextScale;
      const int py = y + gy * kUppolTextScale;
      for (int sy = 0; sy < kUppolTextScale; ++sy) {
        for (int sx = 0; sx < kUppolTextScale; ++sx) {
          surface.SetBackPixel(px + sx, py + sy, argb);
        }
      }
      // Synthetic bold to mimic Java Font.BOLD appearance.
      surface.SetBackPixel(px + kUppolTextScale, py, argb);
      surface.SetBackPixel(px + kUppolTextScale, py + 1, argb);
    }
  }
}

int MeasureUppolTextWidth(const std::string& text) {
  return static_cast<int>(text.size()) * kUppolGlyphAdvance;
}

void DrawUppolText(Surface32& surface, int baseline_y, int x, const std::string& text, uint32_t argb) {
  const int top = baseline_y - kUppolGlyphHeight;
  int pen_x = x;
  for (char ch : text) {
    const char lower = (ch >= 'A' && ch <= 'Z') ? static_cast<char>(ch - 'A' + 'a') : ch;
    DrawUppolGlyph(surface, pen_x, top, lower, argb);
    pen_x += kUppolGlyphAdvance;
  }
}

void DrawUppolHLine(Surface32& surface, int x0, int x1, int y, uint32_t argb) {
  if (x1 < x0) {
    std::swap(x0, x1);
  }
  for (int x = x0; x <= x1; ++x) {
    surface.SetBackPixel(x, y, argb);
  }
}

std::string UppolLineAt(int index, UppolAlign* out_align, bool* out_finished) {
  if (out_align) {
    *out_align = UppolAlign::kCenter;
  }
  if (index < 0) {
    return "";
  }
  if (index > static_cast<int>(kUppolLines.size()) - 1) {
    if (out_finished) {
      *out_finished = true;
    }
    return "";
  }

  std::string line = kUppolLines[static_cast<size_t>(index)];
  if (line.rfind("l_", 0) == 0) {
    line = line.substr(2);
    if (out_align) {
      *out_align = UppolAlign::kLeft;
    }
  } else if (line.rfind("r_", 0) == 0) {
    line = line.substr(2);
    if (out_align) {
      *out_align = UppolAlign::kRight;
    }
  } else if (line.rfind("__", 0) == 0) {
    line = line.substr(2);
    if (out_align) {
      *out_align = UppolAlign::kLink;
    }
  }
  return line;
}

void InitializeUppolRuntime(UppolRuntime& runtime, const UppolSceneAssets& assets) {
  if (!assets.enabled || assets.phorward.Empty()) {
    runtime.initialized = false;
    runtime.working.reset();
    runtime.frame_counter = 0;
    return;
  }

  runtime.working = std::make_unique<IndexedSurface8>(kLogicalWidth, kLogicalHeight);
  std::array<uint8_t, 256> zero{};
  runtime.working->SetPalette(zero, zero, assets.phorward.palette_b);
  runtime.frame_counter = 0;
  runtime.initialized = true;
}

void DrawUppolFrame(Surface32& surface,
                    const DemoState& state,
                    const UppolSceneAssets& assets,
                    UppolRuntime& runtime) {
  if (!assets.enabled || assets.phorward.Empty()) {
    surface.ClearBack(PackArgb(0, 0, 0));
    surface.SwapBuffers();
    return;
  }

  if (!runtime.initialized || !runtime.working) {
    InitializeUppolRuntime(runtime, assets);
    if (!runtime.initialized || !runtime.working) {
      surface.ClearBack(PackArgb(0, 0, 0));
      surface.SwapBuffers();
      return;
    }
  }

  const int source_h = std::max(1, assets.phorward.height);
  const int scroll_y = -((runtime.frame_counter * 256) % source_h);
  runtime.working->BlitImageAt(assets.phorward, 0, scroll_y);
  runtime.working->PresentToBack(surface);

  const double scene_seconds = std::max(0.0, state.timeline_seconds - state.scene_start_seconds);
  const double d = scene_seconds * kUppolScrollSpeed;
  int n2 = static_cast<int>(d) - (kLogicalHeight + kUppolLineHeight);
  const int n3 = kLogicalHeight / kUppolLineHeight + 2;
  if (n2 / kUppolLineHeight + n3 >= static_cast<int>(kUppolLines.size())) {
    n2 = (static_cast<int>(kUppolLines.size()) - n3) * kUppolLineHeight;
  }
  int n4 = kUppolLineHeight - (n2 % kUppolLineHeight);
  const int n5 = n2 / kUppolLineHeight;

  for (int i = 0; i < n3; ++i) {
    bool finished = false;
    UppolAlign align = UppolAlign::kCenter;
    const std::string line = UppolLineAt(i + n5, &align, &finished);
    (void)finished;
    const int text_w = MeasureUppolTextWidth(line);
    const int centered_x = kUppolCenter - (text_w >> 1);
    const int baseline_y = n4 - 5;

    if (align == UppolAlign::kLeft) {
      DrawUppolText(surface, baseline_y, kUppolLeft, line, kUppolTextColor);
    } else if (align == UppolAlign::kRight) {
      DrawUppolText(surface, baseline_y, kUppolRight - text_w, line, kUppolTextColor);
    } else if (align == UppolAlign::kLink) {
      DrawUppolHLine(surface, centered_x, centered_x + text_w, n4 - 4, kUppolTextColor);
      DrawUppolText(surface, baseline_y, centered_x, line, kUppolTextColor);
    } else {
      DrawUppolText(surface, baseline_y, centered_x, line, kUppolTextColor);
    }
    n4 += kUppolLineHeight;
  }

  surface.SwapBuffers();
  ++runtime.frame_counter;
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

void InitJavaRandomStateRaw(uint64_t* state, uint64_t seed) {
  if (!state) {
    return;
  }
  constexpr uint64_t kMask = (1ull << 48ull) - 1ull;
  *state = (seed ^ 0x5DEECE66Dull) & kMask;
}

uint32_t JavaRandomNextBitsRaw(uint64_t* state, int bits) {
  if (!state) {
    return 0u;
  }
  constexpr uint64_t kMask = (1ull << 48ull) - 1ull;
  *state = (*state * 0x5DEECE66Dull + 0xBull) & kMask;
  return static_cast<uint32_t>(*state >> (48 - bits));
}

double JavaRandomNextDoubleRaw(uint64_t* state) {
  const uint64_t a = static_cast<uint64_t>(JavaRandomNextBitsRaw(state, 26));
  const uint64_t b = static_cast<uint64_t>(JavaRandomNextBitsRaw(state, 27));
  return static_cast<double>((a << 27u) | b) / static_cast<double>(1ull << 53u);
}

int JavaRandomNextIntBoundRaw(uint64_t* state, int bound) {
  if (bound <= 0) {
    return 0;
  }
  if ((bound & (bound - 1)) == 0) {
    return static_cast<int>((bound * static_cast<int64_t>(JavaRandomNextBitsRaw(state, 31))) >> 31);
  }
  int bits = 0;
  int value = 0;
  do {
    bits = static_cast<int>(JavaRandomNextBitsRaw(state, 31));
    value = bits % bound;
  } while (bits - value + (bound - 1) < 0);
  return value;
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
                           double scene_seconds,
                           double frame_dt_seconds,
                           int order_row) {
  if (!assets.enabled) {
    surface.ClearBack(PackArgb(0, 0, 0));
    surface.SwapBuffers();
    return;
  }
  if (!runtime.initialized) {
    InitializeMute95Runtime(runtime);
  }

  static constexpr std::array<int, 5> kCueRows = {0x0300, 0x0500, 0x0700, 0x0900, 0x0B00};
  static constexpr std::array<double, 5> kCueSeconds = {3.0, 5.0, 7.0, 9.0, 11.0};
  if (runtime.cue_step + 1 < static_cast<int>(kCueRows.size())) {
    const int next_cue = runtime.cue_step + 1;
    const bool trigger_from_rows =
        (order_row >= 0 && order_row >= kCueRows[static_cast<size_t>(next_cue)]);
    const bool trigger_from_seconds =
        (order_row < 0 && scene_seconds >= kCueSeconds[static_cast<size_t>(next_cue)]);
    if (trigger_from_rows || trigger_from_seconds) {
      runtime.cue_step = next_cue;
      runtime.active_credit = next_cue;
      runtime.credit_start_seconds = scene_seconds;
    }
  }

  float dt = static_cast<float>(frame_dt_seconds);
  if (dt <= 0.0f || dt > 0.2f) {
    dt = static_cast<float>(scene_seconds - runtime.prev_scene_seconds);
  }
  runtime.prev_scene_seconds = scene_seconds;
  if (dt <= 0.0f || dt > 0.2f) {
    dt = 1.0f / 60.0f;
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
  const int order_row = (state.music_module_slot == 1) ? state.music_order_row : -1;
  DrawMute95FrameAtTime(surface, assets, runtime, scene_seconds, state.frame_dt_seconds, order_row);
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
  SetCameraLookAt(camera, cam_pos, cam_target, Vec3(0.0f, 0.0f, 1.0f));
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

bool RowCrossed(int previous_row, int current_row, int target_row) {
  if (previous_row < 0 || current_row < 0 || previous_row == current_row) {
    return false;
  }
  if (previous_row < current_row) {
    return target_row > previous_row && target_row <= current_row;
  }
  // Wrapped or jumped backwards.
  return target_row > previous_row || target_row <= current_row;
}

std::string FormatOrderRowHex(int order_row) {
  std::ostringstream ss;
  ss << std::hex << std::setfill('0') << std::setw(4) << (order_row & 0xFFFF) << std::dec;
  return ss.str();
}

bool WritePpmImage(const std::filesystem::path& output_path,
                   const uint32_t* pixels,
                   int width,
                   int height) {
  if (!pixels || width <= 0 || height <= 0) {
    return false;
  }
  std::ofstream out(output_path, std::ios::binary);
  if (!out.is_open()) {
    return false;
  }
  out << "P6\n" << width << " " << height << "\n255\n";
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const uint32_t c =
          pixels[static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x)];
      const char rgb[3] = {
          static_cast<char>((c >> 16u) & 0xFFu),
          static_cast<char>((c >> 8u) & 0xFFu),
          static_cast<char>(c & 0xFFu),
      };
      out.write(rgb, 3);
    }
  }
  return out.good();
}

bool TryLoadWatercubeReferenceFrame(const std::filesystem::path& ref_dir, int order_row, Image32* out) {
  if (!out) {
    return false;
  }
  const std::string id = FormatOrderRowHex(order_row);
  const std::array<std::string, 4> stems = {"watercube_" + id, id, "0x" + id, "m2_" + id};
  const std::array<std::string, 6> exts = {".png", ".jpg", ".jpeg", ".gif", ".bmp", ".ppm"};
  std::string error;
  for (const std::string& stem : stems) {
    for (const std::string& ext : exts) {
      const std::filesystem::path candidate = ref_dir / (stem + ext);
      if (!std::filesystem::exists(candidate)) {
        continue;
      }
      if (forward::core::LoadImage32(candidate.string(), *out, &error) && !out->Empty()) {
        return true;
      }
    }
  }
  return false;
}

void CaptureWatercubeCheckpointFrame(const WatercubeValidationHarness& harness,
                                     int order_row,
                                     const XmTiming& timing,
                                     const Surface32& surface,
                                     const WatercubeRuntime& runtime) {
  const std::string id = FormatOrderRowHex(order_row);
  const std::filesystem::path native_path = harness.output_dir / ("watercube_" + id + "_native.ppm");
  WritePpmImage(native_path, surface.FrontPixels(), kLogicalWidth, kLogicalHeight);

  const std::filesystem::path metrics_path = harness.output_dir / ("watercube_" + id + "_metrics.txt");
  std::ofstream metrics(metrics_path);
  if (metrics.is_open()) {
    metrics << "module_slot=" << timing.module_slot << "\n";
    metrics << "order=0x" << std::hex << std::setw(2) << std::setfill('0') << (timing.order & 0xFF)
            << std::dec << "\n";
    metrics << "row=0x" << std::hex << std::setw(2) << std::setfill('0') << (timing.row & 0xFF)
            << std::dec << "\n";
    metrics << "order_row=0x" << id << "\n";
    metrics << "clock_ms=" << timing.clock_time_ms << "\n";
    metrics << "roll_impulse=" << runtime.roll_impulse << "\n";
    metrics << "flash_amount=" << runtime.flash_amount << "\n";
    metrics << "shock_amount=" << runtime.shock_amount << "\n";
    metrics << "tex_strip_offset=" << runtime.tex_strip_offset << "\n";
  }

  if (harness.has_reference_dir) {
    Image32 ref;
    if (TryLoadWatercubeReferenceFrame(harness.reference_dir, order_row, &ref) && !ref.Empty()) {
      const int out_w = ref.width + kLogicalWidth;
      const int out_h = std::max(ref.height, kLogicalHeight);
      std::vector<uint32_t> sidebyside(static_cast<size_t>(out_w) * static_cast<size_t>(out_h),
                                       PackArgb(0, 0, 0));
      for (int y = 0; y < ref.height; ++y) {
        for (int x = 0; x < ref.width; ++x) {
          sidebyside[static_cast<size_t>(y) * static_cast<size_t>(out_w) + static_cast<size_t>(x)] =
              ref.pixels[static_cast<size_t>(y) * static_cast<size_t>(ref.width) +
                         static_cast<size_t>(x)];
        }
      }
      const uint32_t* native = surface.FrontPixels();
      for (int y = 0; y < kLogicalHeight; ++y) {
        for (int x = 0; x < kLogicalWidth; ++x) {
          sidebyside[static_cast<size_t>(y) * static_cast<size_t>(out_w) +
                     static_cast<size_t>(x + ref.width)] =
              native[static_cast<size_t>(y) * static_cast<size_t>(kLogicalWidth) +
                     static_cast<size_t>(x)];
        }
      }
      const std::filesystem::path compare_path =
          harness.output_dir / ("watercube_" + id + "_compare.ppm");
      WritePpmImage(compare_path, sidebyside.data(), out_w, out_h);
    }
  }
}

void MaybeCaptureWatercubeCheckpoint(WatercubeValidationHarness* harness,
                                     const DemoState& state,
                                     const XmTiming& timing,
                                     const Surface32& surface,
                                     const WatercubeRuntime& runtime) {
  if (!harness || !harness->enabled) {
    return;
  }
  if (!timing.valid || timing.module_slot != 2) {
    return;
  }
  if (state.scene_mode != SceneMode::kMute95DominaSequence ||
      state.sequence_stage != SequenceStage::kWatercube) {
    return;
  }

  const int order_row = PackOrderRow(timing.order, timing.row);
  if (harness->last_order_row < 0) {
    harness->last_order_row = order_row;
  }

  for (int checkpoint : harness->checkpoints) {
    if (harness->captured_rows.find(checkpoint) != harness->captured_rows.end()) {
      continue;
    }
    const bool reached =
        (order_row == checkpoint) || RowCrossed(harness->last_order_row, order_row, checkpoint);
    if (!reached) {
      continue;
    }
    CaptureWatercubeCheckpointFrame(*harness, checkpoint, timing, surface, runtime);
    harness->captured_rows.insert(checkpoint);
    std::cerr << "watercube checkpoint captured: 0x" << FormatOrderRowHex(checkpoint) << "\n";
  }
  harness->last_order_row = order_row;
}

bool TryLoadFetaReferenceFrame(const std::filesystem::path& ref_dir,
                               int order_row,
                               Image32* out) {
  if (!out) {
    return false;
  }
  const std::string id = FormatOrderRowHex(order_row);
  const std::array<std::string, 4> stems = {
      "feta_" + id + "_reference",
      "feta_" + id,
      id,
      "frame_" + id,
  };
  const std::array<std::string, 3> exts = {".png", ".ppm", ".jpg"};
  std::string error;
  for (const std::string& stem : stems) {
    for (const std::string& ext : exts) {
      const std::filesystem::path candidate = ref_dir / (stem + ext);
      if (!std::filesystem::exists(candidate)) {
        continue;
      }
      if (forward::core::LoadImage32(candidate.string(), *out, &error) && !out->Empty()) {
        return true;
      }
    }
  }
  return false;
}

const char* SceneModeName(SceneMode mode) {
  switch (mode) {
    case SceneMode::kMute95:
      return "mute95";
    case SceneMode::kDomina:
      return "domina";
    case SceneMode::kMute95DominaSequence:
      return "row";
    case SceneMode::kSaari:
      return "saari";
    case SceneMode::kUppol:
      return "uppol";
    case SceneMode::kFeta:
      return "feta";
  }
  return "unknown";
}

void CaptureFetaCheckpointFrame(const FetaValidationHarness& harness,
                                int order_row,
                                const DemoState& state,
                                const XmTiming& timing,
                                const Surface32& surface,
                                const FetaRuntime& runtime) {
  const std::string id = FormatOrderRowHex(order_row);
  const std::filesystem::path native_path = harness.output_dir / ("feta_" + id + "_native.ppm");
  WritePpmImage(native_path, surface.FrontPixels(), kLogicalWidth, kLogicalHeight);

  const std::filesystem::path metrics_path = harness.output_dir / ("feta_" + id + "_metrics.txt");
  std::ofstream metrics(metrics_path);
  if (metrics.is_open()) {
    metrics << "module_slot=" << timing.module_slot << "\n";
    metrics << "order=0x" << std::hex << std::setw(2) << std::setfill('0') << (timing.order & 0xFF)
            << std::dec << "\n";
    metrics << "row=0x" << std::hex << std::setw(2) << std::setfill('0') << (timing.row & 0xFF)
            << std::dec << "\n";
    metrics << "order_row=0x" << id << "\n";
    metrics << "clock_ms=" << timing.clock_time_ms << "\n";
    metrics << "scene_mode=" << SceneModeName(state.scene_mode) << "\n";
    metrics << "scene_seconds=" << std::max(0.0, state.timeline_seconds - state.scene_start_seconds) << "\n";
    metrics << "palette_255_black=" << (runtime.palette_index_255_black ? 1 : 0) << "\n";
    metrics << "blackfeta_start_seconds=" << runtime.blackfeta_start_seconds << "\n";
    metrics << "blackmuna_start_seconds=" << runtime.blackmuna_start_seconds << "\n";
    metrics << "next_script_event=" << runtime.next_script_event << "\n";
  }

  if (harness.has_reference_dir) {
    Image32 ref;
    if (TryLoadFetaReferenceFrame(harness.reference_dir, order_row, &ref) && !ref.Empty()) {
      const int out_w = ref.width + kLogicalWidth;
      const int out_h = std::max(ref.height, kLogicalHeight);
      std::vector<uint32_t> sidebyside(static_cast<size_t>(out_w) * static_cast<size_t>(out_h),
                                       PackArgb(0, 0, 0));
      for (int y = 0; y < ref.height; ++y) {
        for (int x = 0; x < ref.width; ++x) {
          sidebyside[static_cast<size_t>(y) * static_cast<size_t>(out_w) + static_cast<size_t>(x)] =
              ref.pixels[static_cast<size_t>(y) * static_cast<size_t>(ref.width) +
                         static_cast<size_t>(x)];
        }
      }
      const uint32_t* native = surface.FrontPixels();
      for (int y = 0; y < kLogicalHeight; ++y) {
        for (int x = 0; x < kLogicalWidth; ++x) {
          sidebyside[static_cast<size_t>(y) * static_cast<size_t>(out_w) +
                     static_cast<size_t>(x + ref.width)] =
              native[static_cast<size_t>(y) * static_cast<size_t>(kLogicalWidth) +
                     static_cast<size_t>(x)];
        }
      }
      const std::filesystem::path compare_path = harness.output_dir / ("feta_" + id + "_compare.ppm");
      WritePpmImage(compare_path, sidebyside.data(), out_w, out_h);
    }
  }
}

void MaybeCaptureFetaCheckpoint(FetaValidationHarness* harness,
                                const DemoState& state,
                                const XmTiming& timing,
                                const Surface32& surface,
                                const FetaRuntime& runtime) {
  if (!harness || !harness->enabled) {
    return;
  }
  if (!timing.valid || timing.module_slot != 2 || !state.script_driven) {
    return;
  }

  const int order_row = PackOrderRow(timing.order, timing.row);
  if (harness->last_order_row < 0) {
    harness->last_order_row = order_row;
  }

  for (int checkpoint : harness->checkpoints) {
    if (harness->captured_rows.find(checkpoint) != harness->captured_rows.end()) {
      continue;
    }
    const bool reached =
        (order_row == checkpoint) || RowCrossed(harness->last_order_row, order_row, checkpoint);
    if (!reached) {
      continue;
    }
    CaptureFetaCheckpointFrame(*harness, checkpoint, state, timing, surface, runtime);
    harness->captured_rows.insert(checkpoint);
    std::cerr << "feta checkpoint captured: 0x" << FormatOrderRowHex(checkpoint) << "\n";
  }
  harness->last_order_row = order_row;
}

void InitializeKukotRuntime(KukotRuntime& runtime) {
  runtime.flash_intensity = 0.0f;
  runtime.flash_decay = 0.0f;
  runtime.next_script_event = 0;
  runtime.last_order_row = -1;
  runtime.prev_scene_seconds = 0.0;
  runtime.rng_state = 0x4b554b4fu;

  runtime.flash_lut.resize(1000);
  for (uint32_t& c : runtime.flash_lut) {
    const int r = static_cast<int>(NextRandomU32(&runtime.rng_state) % 38u);
    const int g = static_cast<int>(NextRandomU32(&runtime.rng_state) % 16u);
    const int b = static_cast<int>(NextRandomU32(&runtime.rng_state) % 87u);
    c = LegacyPacked10ToArgb(PackLegacy10(r, g, b));
  }

  runtime.flash_scanline_order.resize(static_cast<size_t>(kLogicalHeight));
  for (int i = 0; i < kLogicalHeight; ++i) {
    runtime.flash_scanline_order[static_cast<size_t>(i)] = i;
  }
  for (int i = 0; i < 3000; ++i) {
    const int a = i % kLogicalHeight;
    const int b = static_cast<int>(
        NextRandomU32(&runtime.rng_state) % static_cast<uint32_t>(kLogicalHeight - 1));
    std::swap(runtime.flash_scanline_order[static_cast<size_t>(a)],
              runtime.flash_scanline_order[static_cast<size_t>(b)]);
  }

  runtime.particles.assign(180, Particle{});
  const Vec3 center(-5.0f, 35.0f, 5.501f);
  constexpr float kSpawnSpread = 110.0f;
  for (Particle& p : runtime.particles) {
    p.position = center +
                 Vec3(RandomRange(&runtime.rng_state, -0.5f * kSpawnSpread, 0.5f * kSpawnSpread),
                      RandomRange(&runtime.rng_state, -0.5f * kSpawnSpread, 0.5f * kSpawnSpread),
                      RandomRange(&runtime.rng_state, -0.5f * kSpawnSpread, 0.5f * kSpawnSpread));
    p.size = RandomRange(&runtime.rng_state, 0.92f, 1.08f);
    p.energy = RandomRange(&runtime.rng_state, 0.90f, 1.10f);
  }

  runtime.deformed_meshes.clear();
  runtime.initialized = true;
}

void ApplyKukotMessage(KukotRuntime& runtime, const std::string& message) {
  if (message == "suh") {
    runtime.flash_intensity = 50.0f;
    runtime.flash_decay = 200.0f;
    return;
  }
  if (message == "suh0") {
    runtime.flash_intensity = 100.0f;
    runtime.flash_decay = 150.0f;
    return;
  }
  if (message == "suh1") {
    runtime.flash_intensity = 128.0f;
    runtime.flash_decay = 50.0f;
    return;
  }
  if (message == "suh2") {
    runtime.flash_intensity = 256.0f;
    runtime.flash_decay = 70.0f;
  }
}

void RunKukotScriptAtOrderRow(KukotRuntime& runtime, int order_row) {
  if (order_row < 0) {
    return;
  }
  struct KukotEvent {
    int order_row;
    const char* message;
  };
  static const std::array<KukotEvent, 29> kEvents = {{
      {0x0900, "suh"},
      {0x0910, "suh"},
      {0x0920, "suh"},
      {0x0930, "suh"},
      {0x0900, "suh2"},
      {0x0A00, "suh1"},
      {0x0B00, "suh0"},
      {0x0B04, "suh"},
      {0x0B08, "suh"},
      {0x0B0C, "suh"},
      {0x0B1C, "suh0"},
      {0x0B2C, "suh0"},
      {0x0B30, "suh"},
      {0x0B34, "suh"},
      {0x0B38, "suh"},
      {0x0B48, "suh0"},
      {0x0B4C, "suh1"},
      {0x0B50, "suh1"},
      {0x0B54, "suh1"},
      {0x0C00, "suh0"},
      {0x0C10, "suh0"},
      {0x0C20, "suh0"},
      {0x0C24, "suh"},
      {0x0C28, "suh"},
      {0x0C2C, "suh"},
      {0x0C3C, "suh1"},
      {0x0C40, "suh2"},
      {0x0C44, "suh2"},
      {0x0C48, "suh2"},
  }};

  if (runtime.last_order_row < 0) {
    runtime.last_order_row = order_row;
  }

  while (runtime.next_script_event < static_cast<int>(kEvents.size())) {
    const KukotEvent& ev = kEvents[static_cast<size_t>(runtime.next_script_event)];
    const bool reached = (order_row == ev.order_row) ||
                         RowCrossed(runtime.last_order_row, order_row, ev.order_row);
    if (!reached) {
      break;
    }
    ApplyKukotMessage(runtime, ev.message);
    ++runtime.next_script_event;
  }
  runtime.last_order_row = order_row;
}

void ApplyKukotFlashOverlay(Surface32& surface, KukotRuntime& runtime, int amount) {
  if (amount == 0 || runtime.flash_lut.empty() || runtime.flash_scanline_order.empty()) {
    return;
  }
  const int lines = std::clamp(std::abs(amount), 0, kLogicalHeight - 1);
  if (lines <= 0) {
    return;
  }

  uint32_t* back = surface.BackPixelsMutable();
  if (!back) {
    return;
  }

  const int random_offset = static_cast<int>(
      NextRandomU32(&runtime.rng_state) %
      static_cast<uint32_t>(runtime.flash_scanline_order.size()));
  const int lut_window = std::max(1, static_cast<int>(runtime.flash_lut.size()) - kLogicalWidth);
  for (int i = 0; i < lines; ++i) {
    const int y = runtime.flash_scanline_order[static_cast<size_t>((i + random_offset) % kLogicalHeight)];
    const int lut_start = static_cast<int>(NextRandomU32(&runtime.rng_state) %
                                           static_cast<uint32_t>(lut_window));
    uint32_t* row = back + static_cast<size_t>(y) * static_cast<size_t>(kLogicalWidth);
    for (int x = 0; x < kLogicalWidth; ++x) {
      const uint32_t src = row[static_cast<size_t>(x)];
      const uint32_t noise = runtime.flash_lut[static_cast<size_t>(lut_start + x)];
      int r = static_cast<int>(UnpackR(src));
      int g = static_cast<int>(UnpackG(src));
      int b = static_cast<int>(UnpackB(src));
      if (amount > 0) {
        r = std::min(255, r + static_cast<int>(UnpackR(noise)));
        g = std::min(255, g + static_cast<int>(UnpackG(noise)));
        b = std::min(255, b + static_cast<int>(UnpackB(noise)));
      } else {
        r = std::max(0, r - static_cast<int>(UnpackR(noise)));
        g = std::max(0, g - static_cast<int>(UnpackG(noise)));
        b = std::max(0, b - static_cast<int>(UnpackB(noise)));
      }
      row[static_cast<size_t>(x)] =
          PackArgb(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b));
    }
  }
}

void ApplyKukotHorizontalFeedbackBlur(Surface32& surface, float blend) {
  const int n = std::clamp(static_cast<int>(31.0f * blend), 0, 31);
  const int n2 = 32 - n;
  if (n2 <= 0) {
    return;
  }
  uint32_t* back = surface.BackPixelsMutable();
  if (!back) {
    return;
  }
  for (int y = 0; y < kLogicalHeight; ++y) {
    uint32_t* row = back + static_cast<size_t>(y) * static_cast<size_t>(kLogicalWidth);
    int prev_r = static_cast<int>(UnpackR(row[0])) >> 1;
    int prev_g = static_cast<int>(UnpackG(row[0])) >> 1;
    int prev_b = static_cast<int>(UnpackB(row[0])) >> 1;
    for (int x = 0; x < kLogicalWidth; ++x) {
      const uint32_t src = row[static_cast<size_t>(x)];
      const int sr = static_cast<int>(UnpackR(src));
      const int sg = static_cast<int>(UnpackG(src));
      const int sb = static_cast<int>(UnpackB(src));
      prev_r = (prev_r * n + sr * n2) >> 5;
      prev_g = (prev_g * n + sg * n2) >> 5;
      prev_b = (prev_b * n + sb * n2) >> 5;
      row[static_cast<size_t>(x)] = PackArgb(static_cast<uint8_t>(std::clamp(prev_r, 0, 255)),
                                             static_cast<uint8_t>(std::clamp(prev_g, 0, 255)),
                                             static_cast<uint8_t>(std::clamp(prev_b, 0, 255)));
    }
  }
}

void ApplyKukotTemporalAddHalf(Surface32& surface) {
  uint32_t* back = surface.BackPixelsMutable();
  const uint32_t* front = surface.FrontPixels();
  if (!back || !front) {
    return;
  }
  const size_t count = static_cast<size_t>(kLogicalWidth) * static_cast<size_t>(kLogicalHeight);
  for (size_t i = 0; i < count; ++i) {
    const uint32_t cur = back[i];
    const uint32_t prev = front[i];
    const int r = std::min(255, static_cast<int>(UnpackR(cur)) + (static_cast<int>(UnpackR(prev)) >> 1));
    const int g = std::min(255, static_cast<int>(UnpackG(cur)) + (static_cast<int>(UnpackG(prev)) >> 1));
    const int b = std::min(255, static_cast<int>(UnpackB(cur)) + (static_cast<int>(UnpackB(prev)) >> 1));
    back[i] = PackArgb(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b));
  }
}

void ApplyKukotProceduralDeformation(const Mesh& source, float phase, Mesh* out_mesh) {
  if (!out_mesh) {
    return;
  }
  if (source.Empty()) {
    out_mesh->Clear();
    return;
  }

  if (out_mesh->triangles.size() != source.triangles.size()) {
    out_mesh->triangles = source.triangles;
  }
  if (out_mesh->texcoords.size() != source.texcoords.size()) {
    out_mesh->texcoords = source.texcoords;
  }
  out_mesh->positions.resize(source.positions.size());
  out_mesh->normals.clear();

  // Java kukot path (mmajmmk.kKAMAJa, jAkKAma=2):
  // pivot (0,0.8,0), then per-vertex XY rotation:
  // angle = |p|^2 * 0.015 * sin(phase + p.z * 0.1)
  // with phase coming from kAMAJaK(f*1.9).
  constexpr float kPivotY = 0.8f;
  constexpr float kWaveScale = 0.015f;
  constexpr float kZPhase = 0.1f;
  for (size_t i = 0; i < source.positions.size(); ++i) {
    Vec3 p = source.positions[i];
    p.y -= kPivotY;
    const float radius_sq = p.LengthSq();
    const float angle = radius_sq * kWaveScale * std::sin(phase + p.z * kZPhase);
    const float s = std::sin(angle);
    const float c = std::cos(angle);
    const float x = p.x * c - p.y * s;
    const float y = p.y * c + p.x * s;
    p.x = x;
    p.y = y + kPivotY;
    out_mesh->positions[i] = p;
  }
  out_mesh->RebuildVertexNormals();
}

void DrawKukotParticles(Surface32& surface,
                        const Camera& camera,
                        const KukotSceneAssets& kukot,
                        KukotRuntime& runtime,
                        double scene_seconds) {
  (void)scene_seconds;
  if (kukot.flare.Empty() || runtime.particles.empty()) {
    return;
  }

  const float near_depth = 1.4f;
  const float far_depth = 150.0f;

  for (const Particle& p : runtime.particles) {
    int sx = 0;
    int sy = 0;
    float depth = 1.0f;
    if (!ProjectPointToScreen(camera, p.position, &sx, &sy, &depth)) {
      continue;
    }
    if (depth <= near_depth || depth >= far_depth) {
      continue;
    }

    const float projected = (512.0f / std::max(depth, near_depth)) * p.size;
    const int sprite_size = std::clamp(static_cast<int>(std::lround(projected)), 2, 72);
    const float intensity_f = 255.0f * p.energy;
    const uint8_t intensity = static_cast<uint8_t>(
        std::clamp(static_cast<int>(std::lround(intensity_f)), 96, 255));

    surface.AdditiveBlitScaledToBack(kukot.flare.pixels.data(),
                                     kukot.flare.width,
                                     kukot.flare.height,
                                     sx - sprite_size / 2,
                                     sy - sprite_size / 2,
                                     sprite_size,
                                     sprite_size,
                                     intensity);
  }
}

void DrawKukotFrameAtTime(Surface32& surface,
                          const DemoState& state,
                          const KukotSceneAssets& kukot,
                          KukotRuntime& runtime,
                          Camera& camera,
                          Renderer3D& renderer,
                          RenderInstance& object_instance,
                          double scene_seconds,
                          bool trigger_script_messages) {
  if (!kukot.enabled || kukot.object_texture.Empty() || kukot.random_tile.Empty() ||
      kukot.animated_objects.empty()) {
    surface.ClearBack(PackArgb(0, 0, 0));
    surface.SwapBuffers();
    return;
  }
  if (!runtime.initialized) {
    InitializeKukotRuntime(runtime);
  }

  const int order_row = (state.music_module_slot == 2) ? state.music_order_row : -1;
  if (trigger_script_messages) {
    RunKukotScriptAtOrderRow(runtime, order_row);
  }

  float dt = static_cast<float>(scene_seconds - runtime.prev_scene_seconds);
  runtime.prev_scene_seconds = scene_seconds;
  if (dt <= 0.0f || dt > 0.2f) {
    dt = 1.0f / static_cast<float>(kTickHz);
  }
  if (runtime.flash_intensity > 0.0f) {
    runtime.flash_intensity = std::max(0.0f, runtime.flash_intensity - runtime.flash_decay * dt);
  }

  const double t_ms = scene_seconds * 1900.0;
  Vec3 cam_pos(0.0f, 0.0f, 0.0f);
  Vec3 cam_target(0.0f, 0.0f, 1.0f);
  if (!kukot.camera_track.empty() && !kukot.target_track.empty()) {
    cam_pos = SampleSaariTrackAtMs(kukot.camera_track, t_ms);
    cam_target = SampleSaariTrackAtMs(kukot.target_track, t_ms);
  }
  // ASE camera tracks in FORWARD are authored in Z-up.
  SetCameraLookAt(camera, cam_pos, cam_target, Vec3(0.0f, 0.0f, 1.0f));
  camera.fov_degrees = kukot.camera_fov_degrees;

  surface.ClearBack(PackArgb(0, 0, 0));

  const int random_x = static_cast<int>(NextRandomU32(&runtime.rng_state) & 0xFFu);
  const int random_y = static_cast<int>(NextRandomU32(&runtime.rng_state) & 0x7Fu);
  const int x_offsets[4] = {-random_x, -random_x + 256, -random_x + 384, -random_x + 512};
  const int y_offsets[2] = {-random_y, -random_y + 128};
  for (int yy = 0; yy < 2; ++yy) {
    for (int xx = 0; xx < 4; ++xx) {
      surface.AlphaBlitToBack(kukot.random_tile.pixels.data(),
                              kukot.random_tile.width,
                              kukot.random_tile.height,
                              0,
                              0,
                              x_offsets[xx],
                              y_offsets[yy],
                              kukot.random_tile.width,
                              kukot.random_tile.height,
                              255);
    }
  }

  object_instance.uniform_scale = 1.0f;
  object_instance.fill_color = PackArgb(255, 255, 255);
  object_instance.wire_color = 0;
  object_instance.draw_fill = true;
  object_instance.draw_wire = false;
  object_instance.texture = &kukot.object_texture;
  object_instance.use_mesh_uv = false;
  object_instance.texture_wrap = true;
  object_instance.enable_backface_culling = true;

  if (runtime.deformed_meshes.size() != kukot.animated_objects.size()) {
    runtime.deformed_meshes.resize(kukot.animated_objects.size());
  }
  const float deform_phase = static_cast<float>(scene_seconds * 1.9);

  for (size_t i = 0; i < kukot.animated_objects.size(); ++i) {
    const SaariSceneAssets::AnimatedObject& obj = kukot.animated_objects[i];
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
    object_instance.translation = obj_pos;
    SetRenderInstanceBasisFromQuat(object_instance, obj_rot);

    Mesh& deformed_mesh = runtime.deformed_meshes[i];
    ApplyKukotProceduralDeformation(obj.mesh, deform_phase, &deformed_mesh);
    renderer.DrawMesh(surface, deformed_mesh, camera, object_instance);
  }

  DrawKukotParticles(surface, camera, kukot, runtime, scene_seconds);
  ApplyKukotHorizontalFeedbackBlur(surface, 0.875f);
  ApplyKukotFlashOverlay(surface, runtime, static_cast<int>(runtime.flash_intensity));
  ApplyKukotTemporalAddHalf(surface);
  surface.SwapBuffers();
}

void ApplyCameraRoll(Camera& camera, float roll_radians) {
  if (std::abs(roll_radians) < 1e-6f) {
    return;
  }
  const float s = std::sin(roll_radians);
  const float c = std::cos(roll_radians);
  const Vec3 right = camera.right;
  const Vec3 up = camera.up;
  camera.right = (right * c + up * s).Normalized();
  camera.up = (up * c - right * s).Normalized();
}

void InitializeMakuRuntime(MakuRuntime& runtime) {
  runtime.playback_speed = -3.0f;
  runtime.go_base_seconds = 160.5f;
  runtime.go_anchor_seconds = 0.0;
  runtime.roll_angle = 0.0f;
  runtime.roll_enabled = false;
  runtime.ksor_enabled = false;
  runtime.flash_intensity = 0.0f;
  runtime.flash_decay = 0.0f;
  runtime.next_script_event = 0;
  runtime.initialized = true;
}

void ApplyMakuMessage(MakuRuntime& runtime, const std::string& message, double scene_seconds) {
  if (message == "suh") {
    runtime.flash_intensity = 120.0f;
    runtime.flash_decay = 200.0f;
    return;
  }
  if (message == "suh0") {
    runtime.flash_intensity = 128.0f;
    runtime.flash_decay = 50.0f;
    return;
  }
  if (message == "suh1") {
    runtime.flash_intensity = 128.0f;
    runtime.flash_decay = 50.0f;
    return;
  }
  if (message == "suh2") {
    runtime.flash_intensity = 256.0f;
    runtime.flash_decay = 70.0f;
    return;
  }
  if (message == "ksor") {
    runtime.ksor_enabled = !runtime.ksor_enabled;
    return;
  }
  if (message == "roll") {
    runtime.roll_enabled = !runtime.roll_enabled;
    return;
  }
  if (message.rfind("go ", 0) == 0) {
    runtime.go_base_seconds = std::stof(message.substr(3));
    runtime.go_anchor_seconds = scene_seconds;
    return;
  }
  if (message.rfind("speed ", 0) == 0) {
    runtime.playback_speed = std::stof(message.substr(6));
  }
}

void RunMakuScriptAtOrderRow(MakuRuntime& runtime, int order_row, double scene_seconds) {
  if (order_row < 0) {
    return;
  }
  struct MakuEvent {
    int order_row;
    const char* message;
  };
  static const std::array<MakuEvent, 16> kEvents = {{
      {0x0D00, "go 160.5"},
      {0x0D00, "speed -3.0"},
      {0x0E00, "go 25.5"},
      {0x0E00, "speed 2.0"},
      {0x0E20, "go 0"},
      {0x0E20, "speed 2.5"},
      {0x0F00, "go 42.5"},
      {0x0F00, "speed -2.0"},
      {0x0F20, "ksor"},
      {0x0F20, "go 55.5"},
      {0x0F20, "speed 4.0"},
      {0x0F28, "ksor"},
      {0x0F30, "ksor"},
      {0x0F34, "ksor"},
      {0x0F38, "ksor"},
      {0x0F3C, "ksor"},
  }};

  while (runtime.next_script_event < static_cast<int>(kEvents.size()) &&
         order_row >= kEvents[static_cast<size_t>(runtime.next_script_event)].order_row) {
    ApplyMakuMessage(runtime,
                     kEvents[static_cast<size_t>(runtime.next_script_event)].message,
                     scene_seconds);
    ++runtime.next_script_event;
  }
}

void DrawMakuFrameAtTime(Surface32& surface,
                         const DemoState& state,
                         const MakuSceneAssets& maku,
                         MakuRuntime& runtime,
                         Camera& camera,
                         Renderer3D& renderer,
                         RenderInstance& terrain_instance,
                         double scene_seconds,
                         bool trigger_script_messages) {
  if (!maku.enabled || maku.terrain.Empty() || maku.terrain_texture.Empty()) {
    surface.ClearBack(PackArgb(0, 0, 0));
    surface.SwapBuffers();
    return;
  }
  if (!runtime.initialized) {
    InitializeMakuRuntime(runtime);
    runtime.go_anchor_seconds = scene_seconds;
  }

  const int order_row = (state.music_module_slot == 2) ? state.music_order_row : -1;
  if (trigger_script_messages) {
    RunMakuScriptAtOrderRow(runtime, order_row, scene_seconds);
  }

  if (runtime.roll_enabled) {
    runtime.roll_angle += static_cast<float>(state.frame_dt_seconds) * 3.0f;
  }
  runtime.flash_intensity =
      std::max(0.0f, runtime.flash_intensity - runtime.flash_decay * static_cast<float>(state.frame_dt_seconds));

  const double eval_seconds = static_cast<double>(runtime.go_base_seconds) +
                              (scene_seconds - runtime.go_anchor_seconds) *
                                  static_cast<double>(runtime.playback_speed);
  const double t_ms = eval_seconds * 1000.0;

  Vec3 cam_pos(0.0f, 40.0f, 140.0f);
  Vec3 cam_target(0.0f, 0.0f, 0.0f);
  if (!maku.camera_track.empty() && !maku.target_track.empty()) {
    cam_pos = SampleSaariTrackAtMs(maku.camera_track, t_ms);
    cam_target = SampleSaariTrackAtMs(maku.target_track, t_ms);
  }
  SetCameraLookAt(camera, cam_pos, cam_target, Vec3(0.0f, 0.0f, 1.0f));
  ApplyCameraRoll(camera, runtime.roll_angle);
  camera.fov_degrees = maku.camera_fov_degrees;

  surface.ClearBack(PackArgb(255, 255, 255));

  terrain_instance.rotation_radians.Set(0.0f, 0.0f, 0.0f);
  terrain_instance.translation = Vec3(0.0f, 0.0f, 0.0f);
  terrain_instance.uniform_scale = 1.0f;
  terrain_instance.fill_color = PackArgb(255, 255, 255);
  terrain_instance.wire_color = PackArgb(50, 60, 70);
  terrain_instance.draw_fill = true;
  terrain_instance.draw_wire = false;
  terrain_instance.use_basis_rotation = false;
  terrain_instance.texture = &maku.terrain_texture;
  terrain_instance.use_mesh_uv = true;
  terrain_instance.texture_wrap = true;
  terrain_instance.enable_backface_culling = true;
  renderer.DrawMesh(surface, maku.terrain, camera, terrain_instance);

  if (runtime.ksor_enabled) {
    uint32_t* back = surface.BackPixelsMutable();
    const uint32_t* front = surface.FrontPixels();
    if (back && front) {
      const size_t count = static_cast<size_t>(kLogicalWidth) * static_cast<size_t>(kLogicalHeight);
      for (size_t i = 0; i < count; ++i) {
        const uint32_t src = back[i];
        const int r = static_cast<int>(UnpackR(src));
        const int g = static_cast<int>(UnpackG(src));
        const int b = static_cast<int>(UnpackB(src));
        const uint8_t wr = static_cast<uint8_t>(std::clamp((r * 5 + 255 * 3) / 8, 0, 255));
        const uint8_t wg = static_cast<uint8_t>(std::clamp((g * 5 + 255 * 3) / 8, 0, 255));
        const uint8_t wb = static_cast<uint8_t>(std::clamp((b * 5 + 255 * 3) / 8, 0, 255));
        back[i] = PackArgb(wr, wg, wb);
      }
      surface.AlphaBlitToBack(front,
                              kLogicalWidth,
                              kLogicalHeight,
                              0,
                              0,
                              0,
                              0,
                              kLogicalWidth,
                              kLogicalHeight,
                              160);
    }
  }

  if (runtime.flash_intensity > 0.0f) {
    const float w = std::clamp(runtime.flash_intensity / 256.0f, 0.0f, 1.0f);
    uint32_t* back = surface.BackPixelsMutable();
    if (back) {
      const size_t count = static_cast<size_t>(kLogicalWidth) * static_cast<size_t>(kLogicalHeight);
      for (size_t i = 0; i < count; ++i) {
        const uint32_t src = back[i];
        const uint8_t r = static_cast<uint8_t>(
            std::clamp((1.0f - w) * static_cast<float>(UnpackR(src)) + w * 255.0f, 0.0f, 255.0f));
        const uint8_t g = static_cast<uint8_t>(
            std::clamp((1.0f - w) * static_cast<float>(UnpackG(src)) + w * 255.0f, 0.0f, 255.0f));
        const uint8_t b = static_cast<uint8_t>(
            std::clamp((1.0f - w) * static_cast<float>(UnpackB(src)) + w * 255.0f, 0.0f, 255.0f));
        back[i] = PackArgb(r, g, b);
      }
    }
  }

  surface.SwapBuffers();
}

void InitializeWatercubeRuntime(const WatercubeSceneAssets& watercube, WatercubeRuntime& runtime) {
  runtime.ripple_width = (!watercube.ripple_texture.Empty()) ? watercube.ripple_texture.width : 256;
  runtime.ripple_height = (!watercube.ripple_texture.Empty()) ? watercube.ripple_texture.height : 256;
  runtime.panel_width = 128;
  runtime.panel_height = 128;

  const size_t ripple_count =
      static_cast<size_t>(runtime.ripple_width) * static_cast<size_t>(runtime.ripple_height);
  runtime.ripple_a.assign(ripple_count, 0u);
  runtime.ripple_b.assign(ripple_count, 0u);
  runtime.ripple_combined.assign(ripple_count, 0u);

  runtime.ring_width = watercube.ring_texture.width;
  runtime.ring_height = watercube.ring_texture.height;
  ConvertArgbImageToPacked10(watercube.ring_texture, &runtime.ring_texture_10);

  ConvertArgbImageToPacked10(watercube.ripple_texture, &runtime.ripple_texture_10);
  if (runtime.ripple_texture_10.size() != ripple_count) {
    runtime.ripple_texture_10.assign(ripple_count, legacy10::PackRgb8To10(8, 22, 34));
  }

  runtime.panel_overlay_width = watercube.panel_overlay.width;
  runtime.panel_overlay_height = watercube.panel_overlay.height;
  ConvertArgbImageToPacked10(watercube.panel_overlay, &runtime.panel_overlay_10);
  runtime.panel_buffer_10.assign(
      static_cast<size_t>(runtime.panel_width) * static_cast<size_t>(runtime.panel_height), 0u);
  runtime.frame_packed_10.assign(
      static_cast<size_t>(kLogicalWidth) * static_cast<size_t>(kLogicalHeight), 0u);
  runtime.panel_scale = std::max(1, kLogicalHeight / 128);

  EnsureArgbImageStorage(&runtime.water_dynamic_argb, runtime.ripple_width, runtime.ripple_height);
  EnsureArgbImageStorage(&runtime.panel_dynamic_argb, runtime.panel_width, runtime.panel_height);

  InitJavaRandomStateRaw(&runtime.java_random_state, 0x1998u);
  runtime.rng_state = 0x57415445u;
  runtime.frame_counter = 0;
  runtime.source_is_b = true;
  runtime.kluns1_rot_x = 0.7f;
  runtime.kluns1_rot_z = 0.0f;
  runtime.kluns2_rot_x = -0.7f;
  runtime.kluns2_rot_z = 0.0f;
  runtime.flash_amount = 0.0f;
  runtime.flash_decay = 0.0f;
  runtime.roll_impulse = 0.0f;
  runtime.shock_amount = 0.0f;
  runtime.shock_decay = 0.0f;
  runtime.tex_strip_offset = 0;
  runtime.next_script_event = 0;
  runtime.last_order_row = -1;

  runtime.flash_lut_10.assign(1000, 0u);
  for (uint32_t& c : runtime.flash_lut_10) {
    const int r = static_cast<int>(JavaRandomNextDoubleRaw(&runtime.java_random_state) * 68.0);
    const int g = static_cast<int>(JavaRandomNextDoubleRaw(&runtime.java_random_state) * 56.0);
    const int b = static_cast<int>(JavaRandomNextDoubleRaw(&runtime.java_random_state) * 37.0);
    c = PackLegacy10(r, g, b);
  }
  runtime.flash_scanline_order.resize(static_cast<size_t>(kLogicalHeight));
  for (int i = 0; i < kLogicalHeight; ++i) {
    runtime.flash_scanline_order[static_cast<size_t>(i)] = i;
  }
  for (int i = 0; i < 3000; ++i) {
    const int n4 = i % static_cast<int>(runtime.flash_scanline_order.size());
    const int n8 = static_cast<int>(
        JavaRandomNextDoubleRaw(&runtime.java_random_state) *
        static_cast<double>(runtime.flash_scanline_order.size() - 2));
    std::swap(runtime.flash_scanline_order[static_cast<size_t>(n4)],
              runtime.flash_scanline_order[static_cast<size_t>(std::clamp(
                  n8, 0, static_cast<int>(runtime.flash_scanline_order.size()) - 1))]);
  }
  runtime.initialized = true;
}

void ApplyWatercubeMessage(WatercubeRuntime& runtime, const std::string& message) {
  if (message == "suh") {
    runtime.flash_amount = 50.0f;
    runtime.flash_decay = 200.0f;
    return;
  }
  if (message == "suh0") {
    runtime.flash_amount = 100.0f;
    runtime.flash_decay = 150.0f;
    return;
  }
  if (message == "suh1") {
    runtime.flash_amount = 128.0f;
    runtime.flash_decay = 120.0f;
    return;
  }
  if (message == "suh2") {
    runtime.flash_amount = 256.0f;
    runtime.flash_decay = 90.0f;
    return;
  }
  if (message == "rok") {
    runtime.roll_impulse = 1.0f;
    return;
  }
  if (message == "pum") {
    runtime.shock_amount = 100.0f;
    runtime.shock_decay = 130.0f;
    return;
  }
  if (message == "tex0") {
    runtime.tex_strip_offset = -80;
    return;
  }
  if (message == "tex1") {
    runtime.tex_strip_offset = -160;
    return;
  }
  if (message == "tex2") {
    runtime.tex_strip_offset = -240;
    return;
  }
  if (message == "tex3") {
    runtime.tex_strip_offset = -320;
  }
}

void RunWatercubeScriptAtOrderRow(WatercubeRuntime& runtime, int order_row) {
  if (order_row < 0) {
    return;
  }
  struct WatercubeEvent {
    int order_row;
    const char* message;
  };
  static const std::array<WatercubeEvent, 18> kEvents = {{
      {0x1004, "pum"},
      {0x1008, "rok"},
      {0x100C, "suh"},
      {0x1030, "pum"},
      {0x1100, "rok"},
      {0x1100, "pum"},
      {0x1110, "suh0"},
      {0x1128, "suh0"},
      {0x1130, "suh0"},
      {0x1200, "suh1"},
      {0x1200, "pum"},
      {0x1200, "rok"},
      {0x1210, "suh0"},
      {0x1210, "tex0"},
      {0x1220, "suh1"},
      {0x1220, "tex1"},
      {0x1230, "suh0"},
      {0x1230, "tex2"},
  }};

  if (runtime.last_order_row < 0) {
    runtime.last_order_row = order_row;
  }

  while (runtime.next_script_event < static_cast<int>(kEvents.size())) {
    const WatercubeEvent& ev = kEvents[static_cast<size_t>(runtime.next_script_event)];
    const bool reached = (order_row == ev.order_row) ||
                         RowCrossed(runtime.last_order_row, order_row, ev.order_row);
    if (!reached) {
      break;
    }
    ApplyWatercubeMessage(runtime, ev.message);
    ++runtime.next_script_event;
  }
  runtime.last_order_row = order_row;
}

void WatercubeInjectRing(WatercubeRuntime& runtime) {
  if (runtime.ring_texture_10.empty() || runtime.ripple_b.empty()) {
    return;
  }
  for (int i = 0; i < 1; ++i) {
    const int x = 106 + static_cast<int>(
                              10.0 * -std::sin(static_cast<double>(i + runtime.frame_counter) / 6.24));
    const int y = 106 + static_cast<int>(
                              15.0 * std::cos(static_cast<double>(2 * i + runtime.frame_counter) / 6.24));
    legacy10::AdditiveBlit(runtime.ring_texture_10.data(),
                           runtime.ring_width,
                           runtime.ring_height,
                           0,
                           0,
                           runtime.ripple_b.data(),
                           runtime.ripple_width,
                           runtime.ripple_height,
                           x,
                           y,
                           runtime.ring_width,
                           runtime.ring_height);
  }
}

void WatercubeWaveStep(const std::vector<uint32_t>& src, std::vector<uint32_t>* dst, int width, int height) {
  if (!dst || src.empty() || dst->empty() || width < 4 || height < 4) {
    return;
  }

  const int n3_start = width * 2;
  const int n4 = width + width;
  const int n5 = static_cast<int>(legacy10::kCarryMask);
  const int n6 = n4 - 2;
  const int n7 = n4 + 2;
  const int n8 = n4 + n4;

  int n3 = n3_start;
  for (int n9 = 2; n9 < height - 2; n9 += 2) {
    int n10 = n3 - n4 + 1;
    int n11 = n3 + 1;
    for (int n12 = 1; n12 < width - 1; n12 += 2) {
      const int n14 = static_cast<int>(src[static_cast<size_t>(n10)]) +
                      static_cast<int>(src[static_cast<size_t>(n10 + n6)]) +
                      static_cast<int>(src[static_cast<size_t>(n10 + n7)]) +
                      static_cast<int>(src[static_cast<size_t>(n10 + n8)]);
      const int n15 = static_cast<int>((*dst)[static_cast<size_t>(n11)]);
      const int n16 = (n14 >> 1) + n5 - n15;
      const int n17 = n16 & n5;
      const int n13 = n16 & (n17 - (n17 >> 8));
      (*dst)[static_cast<size_t>(n11 - width)] = static_cast<uint32_t>(n13);
      (*dst)[static_cast<size_t>(n11 - width + 1)] = static_cast<uint32_t>(n13);
      (*dst)[static_cast<size_t>(n11++)] = static_cast<uint32_t>(n13);
      (*dst)[static_cast<size_t>(n11++)] = static_cast<uint32_t>(n13);
      n10 += 2;
    }
    n3 += 2 * width;
  }
}

void ApplyWatercubeFlashNoise(Surface32& surface, WatercubeRuntime& runtime, int amount_signed) {
  if (amount_signed == 0 || runtime.flash_lut_10.empty() || runtime.flash_scanline_order.empty()) {
    return;
  }

  int amount = std::abs(amount_signed);
  if (amount > kLogicalHeight) {
    amount = kLogicalHeight - 1;
  }
  if (amount <= 0) {
    return;
  }

  uint32_t* back = surface.BackPixelsMutable();
  if (!back) {
    return;
  }

  const size_t count = static_cast<size_t>(kLogicalWidth) * static_cast<size_t>(kLogicalHeight);
  if (runtime.frame_packed_10.size() != count) {
    runtime.frame_packed_10.resize(count);
  }
  for (size_t i = 0; i < count; ++i) {
    const uint32_t c = back[i];
    runtime.frame_packed_10[i] = legacy10::PackRgb8To10(
        static_cast<uint8_t>((c >> 16u) & 0xFFu),
        static_cast<uint8_t>((c >> 8u) & 0xFFu),
        static_cast<uint8_t>(c & 0xFFu));
  }

  const int lut_len = static_cast<int>(runtime.flash_lut_10.size());
  const int line_perm_len = static_cast<int>(runtime.flash_scanline_order.size());
  const int random_line_offset = JavaRandomNextIntBoundRaw(&runtime.java_random_state, lut_len);

  for (int i = 0; i < amount; ++i) {
    const int y = runtime.flash_scanline_order[static_cast<size_t>((i + random_line_offset) % line_perm_len)];
    const int noise_start =
        JavaRandomNextIntBoundRaw(&runtime.java_random_state, std::max(1, lut_len - 1 - kLogicalWidth));
    int dst_idx = y * kLogicalWidth;
    int src_idx = noise_start;
    const int src_end = noise_start + kLogicalWidth;
    if (amount_signed > 0) {
      while (src_idx < src_end) {
        runtime.frame_packed_10[static_cast<size_t>(dst_idx)] =
            legacy10::AddSaturating(runtime.frame_packed_10[static_cast<size_t>(dst_idx)],
                                    runtime.flash_lut_10[static_cast<size_t>(src_idx)]);
        ++dst_idx;
        ++src_idx;
      }
    } else {
      while (src_idx < src_end) {
        runtime.frame_packed_10[static_cast<size_t>(dst_idx)] =
            legacy10::SubSaturating(runtime.frame_packed_10[static_cast<size_t>(dst_idx)],
                                    runtime.flash_lut_10[static_cast<size_t>(src_idx)]);
        ++dst_idx;
        ++src_idx;
      }
    }
  }

  legacy10::ConvertBufferToArgb(runtime.frame_packed_10.data(), back, count);
}

void AdditiveBlitAdditiveMode49(Surface32& surface,
                                Surface32& layer_surface,
                                const Mesh& mesh,
                                const Camera& camera,
                                const RenderInstance& instance,
                                Renderer3D& renderer) {
  layer_surface.ClearBack(PackArgb(0, 0, 0));
  renderer.DrawMesh(layer_surface, mesh, camera, instance);
  layer_surface.SwapBuffers();
  surface.AdditiveBlitToBack(layer_surface.FrontPixels(),
                             kLogicalWidth,
                             kLogicalHeight,
                             0,
                             0,
                             0,
                             0,
                             kLogicalWidth,
                             kLogicalHeight,
                             255);
}

void ComposeWatercubePanelBuffer(WatercubeRuntime& runtime) {
  if (runtime.panel_overlay_10.empty() || runtime.panel_buffer_10.empty()) {
    return;
  }
  const int panel_x =
      -292 + static_cast<int>(JavaRandomNextDoubleRaw(&runtime.java_random_state) * 20.0) - 20;
  const int panel_y =
      -80 + static_cast<int>(JavaRandomNextDoubleRaw(&runtime.java_random_state) * 40.0) - 20;
  legacy10::AdditiveBlit(runtime.panel_overlay_10.data(),
                         runtime.panel_overlay_width,
                         runtime.panel_overlay_height,
                         0,
                         0,
                         runtime.panel_buffer_10.data(),
                         runtime.panel_width,
                         runtime.panel_height,
                         panel_x,
                         panel_y,
                         runtime.panel_overlay_width,
                         runtime.panel_overlay_height);
  legacy10::ShiftChannelsRight(runtime.panel_buffer_10.data(), runtime.panel_buffer_10.size(), 1);
  legacy10::ConvertBufferToArgb(runtime.panel_buffer_10.data(),
                                runtime.panel_dynamic_argb.pixels.data(),
                                runtime.panel_buffer_10.size());
}

void ApplyWatercubeShockOverlay(Surface32& surface,
                                const WatercubeSceneAssets& watercube,
                                WatercubeRuntime& runtime) {
  if (runtime.shock_amount <= 0.0f) {
    return;
  }
  const int n = -static_cast<int>(JavaRandomNextDoubleRaw(&runtime.java_random_state) * 384.0);
  const int n2 = -static_cast<int>(JavaRandomNextDoubleRaw(&runtime.java_random_state) * 352.0);
  surface.AdditiveBlitToBack(watercube.scroll_texture.pixels.data(),
                             watercube.scroll_texture.width,
                             watercube.scroll_texture.height,
                             0,
                             0,
                             n,
                             n2,
                             watercube.scroll_texture.width,
                             watercube.scroll_texture.height,
                             255);
  surface.AdditiveBlitToBack(watercube.scroll_texture.pixels.data(),
                             watercube.scroll_texture.width,
                             watercube.scroll_texture.height,
                             0,
                             0,
                             n + 640,
                             n2,
                             watercube.scroll_texture.width,
                             watercube.scroll_texture.height,
                             255);
  surface.AdditiveBlitToBack(watercube.scroll_texture.pixels.data(),
                             watercube.scroll_texture.width,
                             watercube.scroll_texture.height,
                             0,
                             0,
                             n + 640,
                             n2 + 480,
                             watercube.scroll_texture.width,
                             watercube.scroll_texture.height,
                             255);
  surface.AdditiveBlitToBack(watercube.scroll_texture.pixels.data(),
                             watercube.scroll_texture.width,
                             watercube.scroll_texture.height,
                             0,
                             0,
                             n,
                             n2 + 480,
                             watercube.scroll_texture.width,
                             watercube.scroll_texture.height,
                             255);
}

void DrawWatercubeFrameAtTime(Surface32& surface,
                              Surface32& watercube_layer_surface,
                              const DemoState& state,
                              const WatercubeSceneAssets& watercube,
                              WatercubeRuntime& runtime,
                              Camera& camera,
                              Renderer3D& renderer,
                              RenderInstance& object_instance,
                              double scene_seconds,
                              bool trigger_script_messages) {
  if (!watercube.enabled || watercube.animated_objects.empty() || watercube.scroll_texture.Empty() ||
      watercube.box_texture.Empty() || watercube.panel_overlay.Empty() || watercube.ring_texture.Empty() ||
      watercube.ripple_texture.Empty()) {
    surface.ClearBack(PackArgb(0, 0, 0));
    surface.SwapBuffers();
    return;
  }
  if (!runtime.initialized) {
    InitializeWatercubeRuntime(watercube, runtime);
  }

  const int order_row = (state.music_module_slot == 2) ? state.music_order_row : -1;
  if (trigger_script_messages) {
    RunWatercubeScriptAtOrderRow(runtime, order_row);
  }

  const float dt = static_cast<float>(std::clamp(state.frame_dt_seconds, 1.0 / 240.0, 0.1));
  runtime.frame_counter += 1;
  runtime.kluns1_rot_x += 0.02f;
  runtime.kluns1_rot_z += 0.07f;
  if (watercube.has_kluns2) {
    runtime.kluns2_rot_x -= 0.02f;
    runtime.kluns2_rot_z += 0.07f;
  }

  WatercubeInjectRing(runtime);
  if (runtime.source_is_b) {
    WatercubeWaveStep(runtime.ripple_b, &runtime.ripple_a, runtime.ripple_width, runtime.ripple_height);
    runtime.ripple_combined = runtime.ripple_texture_10;
    legacy10::AdditiveBlit(runtime.ripple_a.data(),
                           runtime.ripple_width,
                           runtime.ripple_height,
                           0,
                           0,
                           runtime.ripple_combined.data(),
                           runtime.ripple_width,
                           runtime.ripple_height,
                           0,
                           0,
                           runtime.ripple_width,
                           runtime.ripple_height);
  } else {
    WatercubeWaveStep(runtime.ripple_a, &runtime.ripple_b, runtime.ripple_width, runtime.ripple_height);
    runtime.ripple_combined = runtime.ripple_texture_10;
    legacy10::AdditiveBlit(runtime.ripple_b.data(),
                           runtime.ripple_width,
                           runtime.ripple_height,
                           0,
                           0,
                           runtime.ripple_combined.data(),
                           runtime.ripple_width,
                           runtime.ripple_height,
                           0,
                           0,
                           runtime.ripple_width,
                           runtime.ripple_height);
  }
  runtime.source_is_b = !runtime.source_is_b;
  legacy10::ConvertBufferToArgb(runtime.ripple_combined.data(),
                                runtime.water_dynamic_argb.pixels.data(),
                                runtime.ripple_combined.size());

  const double t_eval_seconds = scene_seconds * 1.8 + 2.0;
  const double t_ms = t_eval_seconds * 1000.0;

  Vec3 cam_pos(0.0f, -80.0f, 20.0f);
  Vec3 cam_target(0.0f, 0.0f, 0.0f);
  if (!watercube.camera_track.empty() && !watercube.target_track.empty()) {
    cam_pos = SampleSaariTrackAtMs(watercube.camera_track, t_ms);
    cam_target = SampleSaariTrackAtMs(watercube.target_track, t_ms);
  }
  SetCameraLookAt(camera, cam_pos, cam_target, Vec3(0.0f, 0.0f, 1.0f));
  ApplyCameraRoll(camera, runtime.roll_impulse * 2.0f * kPi);
  camera.fov_degrees = watercube.camera_fov_degrees;

  surface.ClearBack(PackArgb(0, 0, 0));

  object_instance.uniform_scale = 1.0f;
  object_instance.fill_color = PackArgb(255, 255, 255);
  object_instance.wire_color = 0;
  object_instance.draw_fill = true;
  object_instance.draw_wire = false;
  object_instance.use_mesh_uv = true;
  object_instance.texture_wrap = true;
  object_instance.texture_unlit = false;
  object_instance.enable_backface_culling = true;

  for (const SaariSceneAssets::AnimatedObject& obj : watercube.animated_objects) {
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
    object_instance.translation = obj_pos;
    SetRenderInstanceBasisFromQuat(object_instance, obj_rot);
    if (obj.name == "TriPatch01") {
      object_instance.texture = &runtime.water_dynamic_argb;
      object_instance.texture_unlit = true;
      AdditiveBlitAdditiveMode49(surface, watercube_layer_surface, obj.mesh, camera, object_instance, renderer);
    } else {
      object_instance.texture = &watercube.box_texture;
      object_instance.texture_unlit = false;
      renderer.DrawMesh(surface, obj.mesh, camera, object_instance);
    }
  }

  object_instance.use_basis_rotation = false;
  object_instance.uniform_scale = 0.45f;
  object_instance.texture = &watercube.env_texture;
  object_instance.texture_unlit = false;
  if (!watercube.kluns1.Empty()) {
    object_instance.translation = Vec3(0.0f, 0.0f, 20.0f);
    object_instance.rotation_radians = Vec3(runtime.kluns1_rot_x, 0.0f, runtime.kluns1_rot_z);
    renderer.DrawMesh(surface, watercube.kluns1, camera, object_instance);
  }
  if (watercube.has_kluns2 && !watercube.kluns2.Empty()) {
    object_instance.translation = Vec3(0.0f, 0.0f, -20.0f);
    object_instance.rotation_radians = Vec3(runtime.kluns2_rot_x, 0.0f, runtime.kluns2_rot_z);
    renderer.DrawMesh(surface, watercube.kluns2, camera, object_instance);
  }

  ComposeWatercubePanelBuffer(runtime);
  if (runtime.panel_scale == 2) {
    surface.AdditiveBlitScaledToBack(runtime.panel_dynamic_argb.pixels.data(),
                                     runtime.panel_dynamic_argb.width,
                                     runtime.panel_dynamic_argb.height,
                                     126 * runtime.panel_scale,
                                     0,
                                     128 * runtime.panel_scale,
                                     128 * runtime.panel_scale,
                                     255);
  } else {
    surface.AdditiveBlitScaledToBack(runtime.panel_dynamic_argb.pixels.data(),
                                     runtime.panel_dynamic_argb.width,
                                     runtime.panel_dynamic_argb.height,
                                     126 * runtime.panel_scale,
                                     0,
                                     128,
                                     128,
                                     255);
  }

  surface.AdditiveBlitScaledToBack(watercube.scroll_texture.pixels.data(),
                                   watercube.scroll_texture.width,
                                   watercube.scroll_texture.height,
                                   static_cast<int>(-scene_seconds * 135.0),
                                   -260,
                                   1280,
                                   960,
                                   255);
  if (runtime.tex_strip_offset != 0) {
    surface.AdditiveBlitToBack(watercube.scroll_texture.pixels.data(),
                               watercube.scroll_texture.width,
                               watercube.scroll_texture.height,
                               0,
                               0,
                               -200,
                               runtime.tex_strip_offset,
                               watercube.scroll_texture.width,
                               watercube.scroll_texture.height,
                               255);
  }

  runtime.roll_impulse *= 0.917f;
  if (runtime.flash_amount > 0.0f) {
    ApplyWatercubeFlashNoise(surface, runtime, static_cast<int>(runtime.flash_amount));
    runtime.flash_amount = std::max(0.0f, runtime.flash_amount - runtime.flash_decay * dt);
  }
  if (runtime.shock_amount > 0.0f) {
    ApplyWatercubeShockOverlay(surface, watercube, runtime);
    runtime.shock_amount = std::max(0.0f, runtime.shock_amount - runtime.shock_decay * dt);
  }

  surface.SwapBuffers();
}

void DrawWatercubeFrameAtTime(Surface32& surface,
                              const DemoState& state,
                              const WatercubeSceneAssets& watercube,
                              WatercubeRuntime& runtime,
                              Camera& camera,
                              Renderer3D& renderer,
                              RenderInstance& object_instance,
                              double scene_seconds,
                              bool trigger_script_messages) {
  static Surface32 watercube_layer_surface(kLogicalWidth, kLogicalHeight, true);
  DrawWatercubeFrameAtTime(surface,
                           watercube_layer_surface,
                           state,
                           watercube,
                           runtime,
                           camera,
                           renderer,
                           object_instance,
                           scene_seconds,
                           trigger_script_messages);
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

void SetFetaPalette(FetaRuntime& runtime, bool force_index_255_black) {
  runtime.palette_index_255_black = force_index_255_black;
  for (int i = 0; i < 256; ++i) {
    int r = std::min(255, i * 2);
    int g = std::min(255, i * 3);
    int b = i;
    if (force_index_255_black && i == 255) {
      r = 0;
      g = 0;
      b = 0;
    }
    runtime.palette_packed10[static_cast<size_t>(i)] =
        legacy10::PackRgb8To10(static_cast<uint8_t>(r),
                               static_cast<uint8_t>(g),
                               static_cast<uint8_t>(b));
  }
}

void InitializeFetaRuntime(FetaRuntime& runtime) {
  const size_t pixel_count = static_cast<size_t>(kLogicalWidth) * static_cast<size_t>(kLogicalHeight);
  runtime.indices_a.resize(pixel_count);
  runtime.indices_b.resize(pixel_count);
  runtime.mesh_mask.assign(pixel_count, 0u);
  runtime.packed_frame.assign(pixel_count, 0u);
  for (size_t i = 0; i < pixel_count; ++i) {
    runtime.indices_a[i] = static_cast<uint8_t>(i & 0xFFu);
    runtime.indices_b[i] = static_cast<uint8_t>(i & 0xFFu);
  }
  runtime.current_indices_a = true;
  runtime.blackfeta_start_seconds = 0.0;
  runtime.blackmuna_start_seconds = 0.0;
  runtime.last_order_row = -1;
  runtime.next_script_event = 0;
  SetFetaPalette(runtime, true);
  runtime.initialized = true;
}

void ApplyFetaMessage(FetaRuntime& runtime, const std::string& message, double scene_seconds) {
  if (message == "1") {
    SetFetaPalette(runtime, true);
    return;
  }
  if (message == "2") {
    SetFetaPalette(runtime, false);
    return;
  }
  if (message == "blackfeta") {
    runtime.blackfeta_start_seconds = scene_seconds;
    return;
  }
  if (message == "blackmuna") {
    runtime.blackmuna_start_seconds = scene_seconds;
  }
}

void RunFetaScriptAtOrderRow(FetaRuntime& runtime, int order_row, double scene_seconds) {
  if (order_row < 0) {
    return;
  }
  struct FetaEvent {
    int order_row;
    const char* message;
  };
  static const std::array<FetaEvent, 3> kEvents = {{
      {0x1230, "1"},
      {0x1520, "blackfeta"},
      {0x1530, "blackmuna"},
  }};

  const int previous_row = runtime.last_order_row;
  if (runtime.last_order_row < 0) {
    runtime.last_order_row = order_row;
  }

  while (runtime.next_script_event < static_cast<int>(kEvents.size())) {
    const FetaEvent& ev = kEvents[static_cast<size_t>(runtime.next_script_event)];
    bool reached = false;
    if (previous_row < 0) {
      reached = order_row >= ev.order_row;
    } else {
      reached = (order_row == ev.order_row) ||
                RowCrossed(runtime.last_order_row, order_row, ev.order_row);
    }
    if (!reached) {
      break;
    }
    ApplyFetaMessage(runtime, ev.message, scene_seconds);
    ++runtime.next_script_event;
  }
  runtime.last_order_row = order_row;
}

void BuildFetaMeshMask(Surface32& mask_surface,
                       const Mesh& mesh,
                       const Camera& camera,
                       Renderer3D& renderer,
                       const RenderInstance& mesh_instance,
                       FetaRuntime& runtime) {
  mask_surface.ClearBack(PackArgb(0, 0, 0));
  RenderInstance mask_instance = mesh_instance;
  mask_instance.texture = nullptr;
  mask_instance.use_mesh_uv = false;
  mask_instance.texture_wrap = false;
  mask_instance.texture_unlit = true;
  mask_instance.fill_color = PackArgb(255, 255, 255);
  mask_instance.wire_color = 0;
  mask_instance.draw_fill = true;
  mask_instance.draw_wire = false;
  renderer.DrawMesh(mask_surface, mesh, camera, mask_instance);

  const uint32_t* mask_pixels = mask_surface.BackPixels();
  const size_t pixel_count = static_cast<size_t>(kLogicalWidth) * static_cast<size_t>(kLogicalHeight);
  for (size_t i = 0; i < pixel_count; ++i) {
    runtime.mesh_mask[i] = ((mask_pixels[i] & 0x00FFFFFFu) != 0u) ? 1u : 0u;
  }
}

void ApplyFetaIndexedPostComposite(Surface32& surface,
                                   FetaRuntime& runtime,
                                   double scene_seconds) {
  uint32_t* back = surface.BackPixelsMutable();
  if (!back) {
    return;
  }

  const size_t pixel_count = static_cast<size_t>(kLogicalWidth) * static_cast<size_t>(kLogicalHeight);
  for (size_t i = 0; i < pixel_count; ++i) {
    const uint32_t c = back[i];
    runtime.packed_frame[i] = legacy10::PackRgb8To10(static_cast<uint8_t>((c >> 16u) & 0xFFu),
                                                      static_cast<uint8_t>((c >> 8u) & 0xFFu),
                                                      static_cast<uint8_t>(c & 0xFFu));
  }

  const std::vector<uint8_t>& src =
      runtime.current_indices_a ? runtime.indices_a : runtime.indices_b;
  std::vector<uint8_t>& dst =
      runtime.current_indices_a ? runtime.indices_b : runtime.indices_a;

  const double scale = 1.0 / 1.100000023841858;
  const int n26 = static_cast<int>(scale * 65536.0);
  const int n27 = 0;
  const int n28 = 0;
  const int n29 = static_cast<int>(scale * 65536.0);
  const int cx = kLogicalWidth / 2;
  const int cy = kLogicalHeight / 2;
  int row_u = static_cast<int>((-(cx * scale) * 65536.0) + (cx * 65536.0));
  int row_v = static_cast<int>((-(cy * scale) * 65536.0) + (cy * 65536.0));

  const bool masked_mode = runtime.blackfeta_start_seconds == 0.0;
  int dst_index = 0;
  for (int y = 0; y < kLogicalHeight; ++y) {
    int u = row_u;
    int v = row_v;
    for (int x = 0; x < kLogicalWidth; ++x) {
      (void)x;
      if (masked_mode && runtime.mesh_mask[static_cast<size_t>(dst_index)] != 0u) {
        dst[static_cast<size_t>(dst_index)] = 255u;
      } else {
        const int sx = (u >> 16) & 0x1FF;
        const int sy = (v >> 16) & 0x0FF;
        const uint8_t idx =
            static_cast<uint8_t>(src[static_cast<size_t>((sy << 9) | sx)] >> 1u);
        dst[static_cast<size_t>(dst_index)] = idx;
        if (idx != 0u) {
          runtime.packed_frame[static_cast<size_t>(dst_index)] = legacy10::AddSaturating(
              runtime.packed_frame[static_cast<size_t>(dst_index)],
              runtime.palette_packed10[static_cast<size_t>(idx)]);
        }
      }
      ++dst_index;
      u += n26;
      v += n27;
    }
    row_u += n28;
    row_v += n29;
  }
  runtime.current_indices_a = !runtime.current_indices_a;

  if (runtime.blackfeta_start_seconds != 0.0) {
    int n = static_cast<int>(std::min(255.0, std::max(0.0, (scene_seconds - runtime.blackfeta_start_seconds) *
                                                                0.7 * 255.0)));
    int n2 = 0;
    if (runtime.blackmuna_start_seconds != 0.0) {
      n2 = static_cast<int>(std::min(
          255.0, std::max(0.0, (scene_seconds - runtime.blackmuna_start_seconds) * 0.4 * 255.0)));
    }
    const uint32_t dark_feta = legacy10::PackColor24To10(static_cast<uint32_t>(n * 65793));
    const uint32_t dark_muna = legacy10::PackColor24To10(static_cast<uint32_t>(n2 * 65793));
    for (size_t i = 0; i < pixel_count; ++i) {
      const uint32_t dark = (runtime.mesh_mask[i] != 0u) ? dark_feta : dark_muna;
      runtime.packed_frame[i] = legacy10::SubSaturating(runtime.packed_frame[i], dark);
    }
  }

  for (size_t i = 0; i < pixel_count; ++i) {
    back[i] = legacy10::Unpack10ToArgb(runtime.packed_frame[i]);
  }
}

void DrawFetaFrame(Surface32& surface,
                   const DemoState& state,
                   const Mesh& mesh,
                   const KaaakmaBackgroundPass& background,
                   MmaamkaParticlePass& particles,
                   FetaRuntime& feta_runtime,
                   Camera& camera,
                   Renderer3D& renderer,
                   RenderInstance& mesh_instance,
                   RenderInstance& halo_instance,
                   RenderInstance& background_instance,
                   Surface32& halo_surface,
                   const FetaSceneAssets& feta,
                   const QuickWinPostLayer& post) {
  if (!feta_runtime.initialized) {
    InitializeFetaRuntime(feta_runtime);
  }

  surface.ClearBack(PackArgb(2, 3, 8));

  const float t = static_cast<float>(state.timeline_seconds);
  const double scene_seconds = std::max(0.0, state.timeline_seconds - state.scene_start_seconds);

  camera.position = Vec3(0.0f, 0.0f, 0.0f);
  camera.right = Vec3(1.0f, 0.0f, 0.0f);
  camera.up = Vec3(0.0f, 1.0f, 0.0f);
  camera.forward = Vec3(0.0f, 0.0f, 1.0f);
  camera.fov_degrees = state.feta_fov_degrees;

  ConfigureFetaInstance(mesh_instance, feta, t);

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

  renderer.DrawMesh(surface, mesh, camera, mesh_instance);

  StepMmaamkaParticles(particles, state.timeline_seconds);
  DrawMmaamkaParticles(surface, camera, particles, state.timeline_seconds);

  static Surface32 feta_mask_surface(kLogicalWidth, kLogicalHeight, true);
  BuildFetaMeshMask(feta_mask_surface, mesh, camera, renderer, mesh_instance, feta_runtime);
  ApplyFetaIndexedPostComposite(surface, feta_runtime, scene_seconds);

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
                                   const KukotSceneAssets& kukot_assets,
                                   KukotRuntime& kukot_runtime,
                                   const MakuSceneAssets& maku_assets,
                                   MakuRuntime& maku_runtime,
                                   const WatercubeSceneAssets& watercube_assets,
                                   WatercubeRuntime& watercube_runtime,
                                   Camera& camera,
                                   Renderer3D& renderer,
                                   RenderInstance& saari_backdrop_instance,
                                   RenderInstance& saari_terrain_instance,
                                   RenderInstance& saari_object_instance,
                                   RenderInstance& watercube_object_instance) {
  const double sequence_seconds = std::max(0.0, state.timeline_seconds - state.scene_start_seconds);

  if (state.sequence_stage == SequenceStage::kMute95) {
    const int order_row = (state.music_module_slot == 1) ? state.music_order_row : -1;
    DrawMute95FrameAtTime(
        surface, mute95_assets, mute95_runtime, sequence_seconds, state.frame_dt_seconds, order_row);
    return;
  }
  if (state.sequence_stage == SequenceStage::kDomina || !saari_assets.enabled) {
    DrawDominaFrameAtTime(surface, domina_assets, domina_runtime, sequence_seconds, true);
    return;
  }
  if (state.sequence_stage == SequenceStage::kKukot) {
    DrawKukotFrameAtTime(surface,
                         state,
                         kukot_assets,
                         kukot_runtime,
                         camera,
                         renderer,
                         saari_object_instance,
                         sequence_seconds,
                         true);
    return;
  }
  if (state.sequence_stage == SequenceStage::kMaku) {
    DrawMakuFrameAtTime(surface,
                        state,
                        maku_assets,
                        maku_runtime,
                        camera,
                        renderer,
                        saari_terrain_instance,
                        sequence_seconds,
                        true);
    return;
  }
  if (state.sequence_stage == SequenceStage::kWatercube) {
    DrawWatercubeFrameAtTime(surface,
                             state,
                             watercube_assets,
                             watercube_runtime,
                             camera,
                             renderer,
                             watercube_object_instance,
                             sequence_seconds,
                             true);
    return;
  }
  DrawSaariFrameAtTime(surface,
                       saari_assets,
                       saari_runtime,
                       camera,
                       renderer,
                       saari_backdrop_instance,
                       saari_terrain_instance,
                       saari_object_instance,
                       sequence_seconds,
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
               const KukotSceneAssets& kukot_assets,
               KukotRuntime& kukot_runtime,
               const MakuSceneAssets& maku_assets,
               MakuRuntime& maku_runtime,
               const WatercubeSceneAssets& watercube_assets,
               WatercubeRuntime& watercube_runtime,
               const UppolSceneAssets& uppol_assets,
               UppolRuntime& uppol_runtime,
               const Mesh& mesh,
               const KaaakmaBackgroundPass& background,
               MmaamkaParticlePass& particles,
               FetaRuntime& feta_runtime,
               Camera& camera,
               Renderer3D& renderer,
               RenderInstance& mesh_instance,
               RenderInstance& halo_instance,
               RenderInstance& background_instance,
               RenderInstance& saari_backdrop_instance,
               RenderInstance& saari_terrain_instance,
               RenderInstance& saari_object_instance,
               RenderInstance& watercube_object_instance,
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
  if (state.scene_mode == SceneMode::kUppol) {
    DrawUppolFrame(surface, state, uppol_assets, uppol_runtime);
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
                                  kukot_assets,
                                  kukot_runtime,
                                  maku_assets,
                                  maku_runtime,
                                  watercube_assets,
                                  watercube_runtime,
                                  camera,
                                  renderer,
                                  saari_backdrop_instance,
                                  saari_terrain_instance,
                                  saari_object_instance,
                                  watercube_object_instance);
    return;
  }

  DrawFetaFrame(surface,
                state,
                mesh,
                background,
                particles,
                feta_runtime,
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
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
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

  bool disable_audio = false;
  bool verbose_audio = false;
  WatercubeValidationHarness watercube_harness;
  FetaValidationHarness feta_harness;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--nosound" || arg == "nosound") {
      disable_audio = true;
    } else if (arg == "--verbose-audio") {
      verbose_audio = true;
    } else if (arg == "--watercube-capture") {
      watercube_harness.enabled = true;
      watercube_harness.output_dir =
          std::filesystem::path("documentation") / "watercube-checkpoints";
    } else if (arg.rfind("--watercube-capture-dir=", 0) == 0) {
      watercube_harness.enabled = true;
      watercube_harness.output_dir = arg.substr(std::string("--watercube-capture-dir=").size());
    } else if (arg.rfind("--watercube-reference-dir=", 0) == 0) {
      watercube_harness.reference_dir =
          std::filesystem::path(arg.substr(std::string("--watercube-reference-dir=").size()));
      watercube_harness.has_reference_dir = true;
    } else if (arg == "--feta-capture") {
      feta_harness.enabled = true;
      feta_harness.output_dir = std::filesystem::path("documentation") / "feta-checkpoints";
    } else if (arg.rfind("--feta-capture-dir=", 0) == 0) {
      feta_harness.enabled = true;
      feta_harness.output_dir = arg.substr(std::string("--feta-capture-dir=").size());
    } else if (arg.rfind("--feta-reference-dir=", 0) == 0) {
      feta_harness.reference_dir =
          std::filesystem::path(arg.substr(std::string("--feta-reference-dir=").size()));
      feta_harness.has_reference_dir = true;
    }
  }
  if (watercube_harness.enabled && watercube_harness.output_dir.empty()) {
    watercube_harness.output_dir = std::filesystem::path("documentation") / "watercube-checkpoints";
  }
  if (watercube_harness.enabled) {
    std::error_code ec;
    std::filesystem::create_directories(watercube_harness.output_dir, ec);
    if (ec) {
      std::cerr << "watercube capture disabled: cannot create output dir "
                << watercube_harness.output_dir.string() << "\n";
      watercube_harness.enabled = false;
    } else {
      std::cerr << "watercube capture enabled: " << watercube_harness.output_dir.string() << "\n";
    }
  }
  if (feta_harness.enabled && feta_harness.output_dir.empty()) {
    feta_harness.output_dir = std::filesystem::path("documentation") / "feta-checkpoints";
  }
  if (feta_harness.enabled) {
    std::error_code ec;
    std::filesystem::create_directories(feta_harness.output_dir, ec);
    if (ec) {
      std::cerr << "feta capture disabled: cannot create output dir "
                << feta_harness.output_dir.string() << "\n";
      feta_harness.enabled = false;
    } else {
      std::cerr << "feta capture enabled: " << feta_harness.output_dir.string() << "\n";
    }
  }
  watercube_harness.captured_rows.clear();
  watercube_harness.last_order_row = -1;
  feta_harness.captured_rows.clear();
  feta_harness.last_order_row = -1;
  if (disable_audio && verbose_audio) {
    std::cerr << "audio disabled by command line (--nosound)\n";
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
  RenderInstance watercube_object_instance;
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
  FetaRuntime feta_runtime;
  Mute95SceneAssets mute95;
  Mute95Runtime mute95_runtime;
  DominaSceneAssets domina;
  DominaRuntime domina_runtime;
  SaariSceneAssets saari;
  SaariRuntime saari_runtime;
  KukotSceneAssets kukot;
  KukotRuntime kukot_runtime;
  MakuSceneAssets maku;
  MakuRuntime maku_runtime;
  WatercubeSceneAssets watercube;
  WatercubeRuntime watercube_runtime;
  UppolSceneAssets uppol;
  UppolRuntime uppol_runtime;
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

  {
    std::string image_error;
    std::array<uint32_t, 256> env_palette{};
    bool has_env_palette = false;

    const std::string envplane_path = ResolveForwardAssetPath("images/envplane.gif");
    if (!envplane_path.empty()) {
      has_env_palette = LoadGifGlobalPalette(envplane_path, &env_palette);
      if (!has_env_palette) {
        Image32 envplane_image;
        if (LoadForwardImage("images/envplane.gif", &envplane_image, &image_error) &&
            !envplane_image.Empty()) {
          const int y = 0;
          for (int i = 0; i < 256; ++i) {
            const int x = (i * std::max(1, envplane_image.width - 1)) / 255;
            env_palette[static_cast<size_t>(i)] =
                envplane_image.pixels[static_cast<size_t>(y) *
                                          static_cast<size_t>(envplane_image.width) +
                                      static_cast<size_t>(x)];
          }
          has_env_palette = true;
        }
      }
    }
    if (!has_env_palette) {
      std::cerr << "kukot envplane load failed\n";
    } else {
      kukot.object_texture = BuildKukotEnvTextureFromPalette(env_palette, 48.0f, 192.0f, 80.0f);
    }

    kukot.random_tile = BuildKukotRandomTile(0x06C0FFEEu);

    if (!LoadForwardImage("images/flare1.jpg", &kukot.flare, &image_error)) {
      std::cerr << "kukot flare load failed: " << image_error << "\n";
    }

    const std::string kukot_ase_path = ResolveForwardAssetPath("asses/under1.ase");
    bool tracks_ok = false;
    bool objects_ok = false;
    if (!kukot_ase_path.empty()) {
      tracks_ok = ParseSaariAseCameraTracks(kukot_ase_path,
                                            &kukot.camera_track,
                                            &kukot.target_track,
                                            &kukot.camera_fov_degrees);
      static const std::vector<std::string> kKukotObjectNames = {"kellu", "kellu01", "kellu02"};
      objects_ok = ParseAseAnimatedObjects(kukot_ase_path, kKukotObjectNames, &kukot.animated_objects);
    }
    if (!tracks_ok) {
      std::cerr << "kukot camera tracks parse failed\n";
    }
    if (!objects_ok) {
      std::cerr << "kukot ASE object parse failed\n";
    } else {
      std::cerr << "kukot ASE objects loaded: " << kukot.animated_objects.size() << "\n";
    }

    kukot.enabled = !kukot.object_texture.Empty() && !kukot.random_tile.Empty() &&
                    !kukot.flare.Empty() && tracks_ok && objects_ok;
  }

  {
    std::string image_error;
    Image32 maku_height;
    Image32 maku_texture;
    bool ok = LoadForwardImage("images/scape/loopk40.gif", &maku_height, &image_error);
    if (!ok) {
      std::cerr << "maku heightmap load failed: " << image_error << "\n";
    }
    ok = LoadForwardImage("images/scape/loopa2.gif", &maku_texture, &image_error);
    if (!ok) {
      std::cerr << "maku texture load failed: " << image_error << "\n";
    }
    bool mesh_ok = false;
    if (!maku_height.Empty()) {
      mesh_ok = BuildTerrainMeshFromHeightmap(maku_height, 200.0f, 1.94f, 0, &maku.terrain);
      if (!mesh_ok) {
        std::cerr << "maku terrain mesh build failed\n";
      }
    }
    if (!maku_texture.Empty()) {
      maku.terrain_texture = std::move(maku_texture);
    }

    bool tracks_ok = false;
    const std::string maku_ase_path = ResolveForwardAssetPath("asses/vuori5.ase");
    if (!maku_ase_path.empty()) {
      tracks_ok = ParseSaariAseCameraTracks(
          maku_ase_path, &maku.camera_track, &maku.target_track, &maku.camera_fov_degrees);
    }
    if (!tracks_ok) {
      std::cerr << "maku camera tracks parse failed\n";
    }
    maku.enabled = mesh_ok && !maku.terrain_texture.Empty() && tracks_ok;
  }

  {
    std::string image_error;
    bool textures_ok = true;
    textures_ok &= LoadForwardImage("images/1.jpg", &watercube.panel_overlay, &image_error);
    if (watercube.panel_overlay.Empty()) {
      std::cerr << "watercube panel overlay load failed: " << image_error << "\n";
      textures_ok = false;
    }
    textures_ok &= LoadForwardImage("images/txt1.jpg", &watercube.scroll_texture, &image_error);
    if (watercube.scroll_texture.Empty()) {
      std::cerr << "watercube scroll texture load failed: " << image_error << "\n";
      textures_ok = false;
    }
    textures_ok &= LoadForwardImage("images/reunus2.jpg", &watercube.box_texture, &image_error);
    if (watercube.box_texture.Empty()) {
      std::cerr << "watercube box texture load failed: " << image_error << "\n";
      textures_ok = false;
    }
    textures_ok &= LoadForwardImage("images/rinku2.jpg", &watercube.ring_texture, &image_error);
    if (watercube.ring_texture.Empty()) {
      std::cerr << "watercube ring texture load failed: " << image_error << "\n";
      textures_ok = false;
    }
    textures_ok &= LoadForwardImage("images/riple2.jpg", &watercube.ripple_texture, &image_error);
    if (watercube.ripple_texture.Empty()) {
      std::cerr << "watercube ripple texture load failed: " << image_error << "\n";
      textures_ok = false;
    }

    if (!LoadForwardImage("images/env3.jpg", &watercube.env_texture, &image_error)) {
      std::cerr << "watercube env texture load failed: " << image_error << "\n";
    }

    const std::string watercube_ase_path = ResolveForwardAssetPath("asses/nosto3.ase");
    bool tracks_ok = false;
    bool objects_ok = false;
    if (!watercube_ase_path.empty()) {
      tracks_ok = ParseSaariAseCameraTracks(watercube_ase_path,
                                            &watercube.camera_track,
                                            &watercube.target_track,
                                            &watercube.camera_fov_degrees);
      static const std::vector<std::string> kWatercubeObjectNames = {"Box01", "TriPatch01"};
      objects_ok =
          ParseAseAnimatedObjects(watercube_ase_path, kWatercubeObjectNames, &watercube.animated_objects);
    }
    if (!tracks_ok) {
      std::cerr << "watercube camera tracks parse failed\n";
    }
    if (!objects_ok) {
      std::cerr << "watercube ASE object parse failed\n";
    } else {
      std::cerr << "watercube ASE objects loaded: " << watercube.animated_objects.size() << "\n";
    }

    const std::string kluns1_path = ResolveForwardAssetPath("meshes/kluns1.igu");
    if (!kluns1_path.empty() && !forward::core::LoadIguMesh(kluns1_path, watercube.kluns1, &mesh_error)) {
      std::cerr << "watercube kluns1 load failed: " << mesh_error << "\n";
      watercube.kluns1.Clear();
    }
    const std::string kluns2_path = ResolveForwardAssetPath("meshes/kluns2.igu");
    if (!kluns2_path.empty() && forward::core::LoadIguMesh(kluns2_path, watercube.kluns2, &mesh_error)) {
      watercube.has_kluns2 = !watercube.kluns2.Empty();
    }

    watercube.enabled = textures_ok && tracks_ok && objects_ok;
  }

  {
    const std::string uppol_path = ResolveForwardAssetPath("images/phorward.gif");
    std::string uppol_error;
    if (!uppol_path.empty() &&
        forward::core::LoadGifIndexed8FirstFrame(uppol_path, &uppol.phorward, &uppol_error) &&
        !uppol.phorward.Empty()) {
      uppol.enabled = true;
    } else {
      if (uppol_path.empty()) {
        std::cerr << "uppol source load failed: images/phorward.gif not found\n";
      } else {
        std::cerr << "uppol source load failed: " << uppol_error << "\n";
      }
    }
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

  XmPlayer xm_player;
  MusicState music;
  XmTiming xm_timing;
  if (!disable_audio) {
    std::string audio_error;
    const std::string mod1_path = ResolveForwardAssetPath("mods/kuninga.xm");
    const std::string mod2_path = ResolveForwardAssetPath("mods/jarnomix.xm");
    music.has_mod1 = !mod1_path.empty();
    music.has_mod2 = !mod2_path.empty();

    if (!music.has_mod1) {
      std::cerr << "audio init: missing mods/kuninga.xm\n";
    }
    if (!music.has_mod2) {
      std::cerr << "audio init: missing mods/jarnomix.xm\n";
    }

    if (music.has_mod1 && music.has_mod2) {
      if (!xm_player.Initialize(44100, 1024, &audio_error)) {
        std::cerr << "audio init failed: " << audio_error << "\n";
      } else if (!xm_player.LoadModule(1, mod1_path, &audio_error) ||
                 !xm_player.LoadModule(2, mod2_path, &audio_error) ||
                 !xm_player.StartModule(1, false, &audio_error)) {
        std::cerr << "audio module setup failed: " << audio_error << "\n";
      } else {
        music.enabled = true;
        if (verbose_audio) {
          const char* driver = SDL_GetCurrentAudioDriver();
          std::cerr << "audio enabled via SDL driver: " << (driver ? driver : "unknown") << "\n";
        }
      }
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
    state.sequence_stage = SequenceStage::kMute95;
    state.script_driven = true;
    state.scene_label =
        (maku.enabled && watercube.enabled)
            ? "mute95->domina->saari->kukot->maku->watercube->feta->uppol"
            : (maku.enabled) ? "mute95->domina->saari->kukot->maku->feta->uppol"
                             : "mute95->domina->saari->kukot->feta->uppol";
  } else if (mute95.enabled && domina.enabled) {
    state.scene_mode = SceneMode::kMute95DominaSequence;
    state.sequence_stage = SequenceStage::kMute95;
    state.script_driven = true;
    state.scene_label = "mute95->domina";
  } else if (mute95.enabled) {
    state.scene_mode = SceneMode::kMute95;
    state.script_driven = false;
    state.scene_label = "mute95";
  } else if (domina.enabled) {
    state.scene_mode = SceneMode::kDomina;
    state.script_driven = false;
    state.scene_label = "domina";
  } else if (saari.enabled) {
    state.scene_mode = SceneMode::kSaari;
    state.script_driven = false;
    state.scene_label = "saari";
  } else if (feta.enabled && background.enabled && particles.enabled) {
    state.scene_mode = SceneMode::kFeta;
    state.script_driven = false;
    state.scene_label = "feta+kaaakma+mmaamka";
  } else if (feta.enabled) {
    state.scene_mode = SceneMode::kFeta;
    state.script_driven = false;
    state.scene_label = "feta";
  } else {
    state.scene_mode = SceneMode::kFeta;
    state.script_driven = false;
    state.scene_label = "fallback";
  }
  state.mesh_label = std::filesystem::path(mesh_path).filename().string();
  state.post_label = state.show_post && post.enabled ? "phorward" : "off";
  double sequence_script_start_seconds = state.timeline_seconds;

  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--scene=mute95" || arg == "--mute95") {
      if (mute95.enabled) {
        state.scene_mode = SceneMode::kMute95;
        state.script_driven = false;
        state.scene_label = "mute95";
      }
    } else if (arg == "--scene=domina" || arg == "--domina") {
      if (domina.enabled) {
        state.scene_mode = SceneMode::kDomina;
        state.script_driven = false;
        state.scene_label = "domina";
      }
    } else if (arg == "--scene=saari" || arg == "--saari") {
      if (saari.enabled) {
        state.scene_mode = SceneMode::kSaari;
        state.script_driven = false;
        state.scene_label = "saari";
      }
    } else if (arg == "--scene=uppol" || arg == "--uppol") {
      if (uppol.enabled) {
        state.scene_mode = SceneMode::kUppol;
        state.script_driven = false;
        state.scene_label = "uppol";
      }
    } else if (arg == "--scene=row" || arg == "--row" || arg == "--scene=script" ||
               arg == "--script") {
      if (mute95.enabled && domina.enabled) {
        state.scene_mode = SceneMode::kMute95DominaSequence;
        state.sequence_stage = SequenceStage::kMute95;
        state.script_driven = true;
        state.scene_label = saari.enabled ? ((maku.enabled && watercube.enabled)
                                                 ? "mute95->domina->saari->kukot->maku->watercube->feta->uppol"
                                                 : (maku.enabled
                                                        ? "mute95->domina->saari->kukot->maku->feta->uppol"
                                                        : "mute95->domina->saari->kukot->feta->uppol"))
                                          : "mute95->domina";
        sequence_script_start_seconds = state.timeline_seconds;
        watercube_harness.captured_rows.clear();
        watercube_harness.last_order_row = -1;
        feta_harness.captured_rows.clear();
        feta_harness.last_order_row = -1;
        feta_runtime.initialized = false;
      }
    } else if (arg == "--scene=feta" || arg == "--feta") {
      state.scene_mode = SceneMode::kFeta;
      state.script_driven = false;
      state.scene_label = feta.enabled ? "feta+kaaakma+mmaamka" : "feta-fallback";
      state.scene_start_seconds = state.timeline_seconds;
      particles.initialized = false;
      feta_runtime.initialized = false;
    }
  }

  RuntimeStats stats;

  uint64_t perf_prev = SDL_GetPerformanceCounter();
  const uint64_t perf_freq = SDL_GetPerformanceFrequency();
  double accumulator = 0.0;
  double title_elapsed = 0.0;
  bool running = true;

  auto restart_sequence_audio = [&]() {
    if (!music.enabled) {
      return;
    }
    std::string audio_error;
    if (!xm_player.StartModule(1, false, &audio_error)) {
      std::cerr << "audio restart failed: " << audio_error << "\n";
      music.enabled = false;
      return;
    }
    music.module2_started = false;
    xm_player.SetPaused(state.paused);
    xm_timing = xm_player.GetTiming();
  };

  if (music.enabled) {
    xm_player.SetPaused(state.paused);
    if (state.scene_mode == SceneMode::kMute95DominaSequence) {
      restart_sequence_audio();
    }
  }

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
            if (music.enabled) {
              xm_player.SetPaused(state.paused);
            }
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
              state.script_driven = false;
              state.scene_label = "mute95";
              state.scene_start_seconds = state.timeline_seconds;
              mute95_runtime.initialized = false;
            }
            break;
          case SDLK_2:
            state.scene_mode = SceneMode::kFeta;
            state.script_driven = false;
            state.scene_label = feta.enabled ? "feta+kaaakma+mmaamka" : "feta-fallback";
            state.scene_start_seconds = state.timeline_seconds;
            particles.initialized = false;
            feta_runtime.initialized = false;
            break;
          case SDLK_3:
            if (domina.enabled) {
              state.scene_mode = SceneMode::kDomina;
              state.script_driven = false;
              state.scene_label = "domina";
              state.scene_start_seconds = state.timeline_seconds;
              domina_runtime.initialized = false;
            }
            break;
          case SDLK_4:
            if (mute95.enabled && domina.enabled) {
              state.scene_mode = SceneMode::kMute95DominaSequence;
              state.sequence_stage = SequenceStage::kMute95;
              state.script_driven = true;
              state.scene_label = saari.enabled ? ((maku.enabled && watercube.enabled)
                                                       ? "mute95->domina->saari->kukot->maku->watercube->feta->uppol"
                                                       : (maku.enabled
                                                              ? "mute95->domina->saari->kukot->maku->feta->uppol"
                                                              : "mute95->domina->saari->kukot->feta->uppol"))
                                                : "mute95->domina";
              state.scene_start_seconds = state.timeline_seconds;
              sequence_script_start_seconds = state.timeline_seconds;
              mute95_runtime.initialized = false;
              domina_runtime.initialized = false;
              saari_runtime.initialized = false;
              kukot_runtime.initialized = false;
              maku_runtime.initialized = false;
              watercube_runtime.initialized = false;
              feta_runtime.initialized = false;
              watercube_harness.captured_rows.clear();
              watercube_harness.last_order_row = -1;
              feta_harness.captured_rows.clear();
              feta_harness.last_order_row = -1;
              restart_sequence_audio();
            }
            break;
          case SDLK_5:
            if (saari.enabled) {
              state.scene_mode = SceneMode::kSaari;
              state.script_driven = false;
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
          case SDLK_8:
            if (uppol.enabled) {
              state.scene_mode = SceneMode::kUppol;
              state.script_driven = false;
              state.scene_label = "uppol";
              state.scene_start_seconds = state.timeline_seconds;
              uppol_runtime.initialized = false;
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
    state.frame_dt_seconds = frame_dt;

    accumulator += frame_dt;
    title_elapsed += frame_dt;

    int ticks_this_frame = 0;
    while (accumulator >= kTickDtSeconds) {
      if (!state.paused && !music.enabled) {
        state.timeline_seconds += kTickDtSeconds;
      }
      accumulator -= kTickDtSeconds;
      ++ticks_this_frame;
    }
    stats.simulated_ticks += static_cast<uint64_t>(ticks_this_frame);

    if (music.enabled) {
      xm_timing = xm_player.GetTiming();

      if (xm_timing.valid && xm_timing.module_slot == 1 && !music.module2_started &&
          PackOrderRow(xm_timing.order, xm_timing.row) >= kMod1ToMod2Row) {
        std::string audio_error;
        if (!xm_player.StartModule(2, true, &audio_error)) {
          std::cerr << "audio switch-to-mod2 failed: " << audio_error << "\n";
          music.enabled = false;
        } else {
          music.module2_started = true;
          xm_timing = xm_player.GetTiming();
        }
      }

      if (!state.paused && xm_timing.valid) {
        state.timeline_seconds = static_cast<double>(xm_timing.clock_time_ms) / 1000.0;
      }
    }
    state.music_module_slot = xm_timing.valid ? xm_timing.module_slot : 0;
    state.music_order_row = xm_timing.valid ? PackOrderRow(xm_timing.order, xm_timing.row) : -1;

    // Feta script messages are emitted before feta is shown in the original script.
    // Consume them at the global script level so palette/message state is ready at show row 0x1300.
    if (state.script_driven && feta.enabled && state.music_module_slot == 2 && state.music_order_row >= 0) {
      if (!feta_runtime.initialized) {
        InitializeFetaRuntime(feta_runtime);
      }
      const double feta_scene_seconds = (state.scene_mode == SceneMode::kFeta)
                                            ? std::max(0.0, state.timeline_seconds - state.scene_start_seconds)
                                            : 0.0;
      RunFetaScriptAtOrderRow(feta_runtime, state.music_order_row, feta_scene_seconds);
    }

    if (state.scene_mode == SceneMode::kMute95DominaSequence) {
      SequenceStage desired = state.sequence_stage;
      if (music.enabled) {
        if (xm_timing.valid) {
          desired =
              DetermineSequenceStage(xm_timing, saari.enabled, kukot.enabled, maku.enabled, watercube.enabled, 0.0);
        }
      } else {
        const double fallback_script_seconds =
            std::max(0.0, state.timeline_seconds - sequence_script_start_seconds);
        desired = DetermineSequenceStage(
            xm_timing, saari.enabled, kukot.enabled, maku.enabled, watercube.enabled, fallback_script_seconds);
      }
      if (desired != state.sequence_stage) {
        state.sequence_stage = desired;
        state.scene_start_seconds = state.timeline_seconds;
        if (desired == SequenceStage::kMute95) {
          state.scene_label = "mute95->domina->saari->kukot->maku->watercube->feta->uppol [mute95]";
        } else if (desired == SequenceStage::kDomina) {
          state.scene_label = "mute95->domina->saari->kukot->maku->watercube->feta->uppol [domina]";
        } else if (desired == SequenceStage::kSaari) {
          state.scene_label = "mute95->domina->saari->kukot->maku->watercube->feta->uppol [saari]";
        } else if (desired == SequenceStage::kKukot) {
          state.scene_label = "mute95->domina->saari->kukot->maku->watercube->feta->uppol [kukot]";
        } else if (desired == SequenceStage::kMaku) {
          state.scene_label = "mute95->domina->saari->kukot->maku->watercube->feta->uppol [maku]";
        } else {
          state.scene_label = "mute95->domina->saari->kukot->maku->watercube->feta->uppol [watercube]";
        }
        if (desired == SequenceStage::kMute95) {
          mute95_runtime.initialized = false;
        } else if (desired == SequenceStage::kDomina) {
          domina_runtime.initialized = false;
        } else if (desired == SequenceStage::kSaari) {
          saari_runtime.initialized = false;
        } else if (desired == SequenceStage::kKukot) {
          kukot_runtime.initialized = false;
        } else if (desired == SequenceStage::kMaku) {
          maku_runtime.initialized = false;
        } else {
          watercube_runtime.initialized = false;
        }
      }

      // Original script switches from watercube to feta at module 2 row 0x1300.
      const bool should_switch_to_feta =
          feta.enabled &&
          ((xm_timing.valid && xm_timing.module_slot == 2 &&
            PackOrderRow(xm_timing.order, xm_timing.row) >= kMod2ToFetaRow) ||
           (!music.enabled &&
            std::max(0.0, state.timeline_seconds - sequence_script_start_seconds) >=
                kScriptFallbackToFetaSeconds));
      if (should_switch_to_feta) {
        state.scene_mode = SceneMode::kFeta;
        state.scene_label = "feta+kaaakma+mmaamka [script]";
        state.scene_start_seconds = state.timeline_seconds;
        particles.initialized = false;
      }
    }

    if (state.script_driven && state.scene_mode == SceneMode::kFeta) {
      // Original script switches from feta to uppol at module 2 row 0x1600.
      const bool should_switch_to_uppol =
          uppol.enabled &&
          ((xm_timing.valid && xm_timing.module_slot == 2 &&
            PackOrderRow(xm_timing.order, xm_timing.row) >= kMod2ToUppolRow) ||
           (!music.enabled &&
            std::max(0.0, state.timeline_seconds - sequence_script_start_seconds) >=
                kScriptFallbackToUppolSeconds));
      if (should_switch_to_uppol) {
        state.scene_mode = SceneMode::kUppol;
        state.scene_label = "uppol [script]";
        state.scene_start_seconds = state.timeline_seconds;
        uppol_runtime.initialized = false;
      }
    }

    DrawFrame(surface,
              state,
              mute95,
              mute95_runtime,
              domina,
              domina_runtime,
              saari,
              saari_runtime,
              kukot,
              kukot_runtime,
              maku,
              maku_runtime,
              watercube,
              watercube_runtime,
              uppol,
              uppol_runtime,
              mesh,
              background,
              particles,
              feta_runtime,
              camera,
              renderer_3d,
              mesh_instance,
              halo_instance,
              background_instance,
              saari_backdrop_instance,
              saari_terrain_instance,
              saari_object_instance,
              watercube_object_instance,
              halo_surface,
              feta,
              post);

    MaybeCaptureWatercubeCheckpoint(
        &watercube_harness, state, xm_timing, surface, watercube_runtime);
    MaybeCaptureFetaCheckpoint(&feta_harness, state, xm_timing, surface, feta_runtime);

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
      UpdateWindowTitle(window, state, stats, music, xm_timing, title_elapsed);
      stats.rendered_frames = 0;
      stats.simulated_ticks = 0;
      title_elapsed = 0.0;
    }
  }

  xm_player.Shutdown();
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer_sdl);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
