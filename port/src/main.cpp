#include <SDL.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
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
  Vec3 velocity;
  float life = 0.0f;
  float size = 1.0f;
  float energy = 1.0f;
};

struct MmaamkaParticlePass {
  Image32 flare;
  std::vector<Particle> particles;
  size_t emit_cursor = 0;
  float phase = 0.0f;
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

uint32_t PackArgb(uint8_t r, uint8_t g, uint8_t b) {
  return (0xFFu << 24u) | (static_cast<uint32_t>(r) << 16u) |
         (static_cast<uint32_t>(g) << 8u) | static_cast<uint32_t>(b);
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

void DrawMute95Frame(Surface32& surface,
                     const DemoState& state,
                     const Mute95SceneAssets& assets,
                     Mute95Runtime& runtime) {
  if (!assets.enabled) {
    surface.ClearBack(PackArgb(0, 0, 0));
    surface.SwapBuffers();
    return;
  }
  if (!runtime.initialized) {
    InitializeMute95Runtime(runtime);
  }

  const double scene_seconds = std::max(0.0, state.timeline_seconds - state.scene_start_seconds);

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

void EmitOneParticle(MmaamkaParticlePass& pass, const Vec3& emitter_world, float angle) {
  if (pass.particles.empty()) {
    return;
  }

  Particle& p = pass.particles[pass.emit_cursor];
  p.position = emitter_world;
  const float radial_speed = RandomRange(&pass.rng_state, 0.2f, 0.6f);
  const float up_speed = RandomRange(&pass.rng_state, 0.2f, 0.7f);
  const float depth_speed = RandomRange(&pass.rng_state, 0.7f, 1.5f);
  p.velocity.Set(std::cos(angle) * radial_speed, up_speed, std::sin(angle) * radial_speed + depth_speed);
  p.life = RandomRange(&pass.rng_state, 0.9f, 2.4f);
  p.size = RandomRange(&pass.rng_state, 0.35f, 1.0f);
  p.energy = RandomRange(&pass.rng_state, 0.5f, 1.0f);

  ++pass.emit_cursor;
  if (pass.emit_cursor >= pass.particles.size()) {
    pass.emit_cursor = 0;
  }
}

void InitializeMmaamkaParticles(MmaamkaParticlePass& pass,
                                int count,
                                const Vec3& emitter_world,
                                double timeline_seconds) {
  pass.particles.assign(static_cast<size_t>(count), Particle{});
  pass.emit_cursor = 0;
  pass.phase = 0.0f;
  pass.last_timeline_seconds = timeline_seconds;
  pass.rng_state = 0x1998u;
  pass.initialized = true;

  for (Particle& p : pass.particles) {
    p.position.Set(emitter_world.x + RandomRange(&pass.rng_state, -1.2f, 1.2f),
                   emitter_world.y + RandomRange(&pass.rng_state, -0.8f, 0.8f),
                   emitter_world.z + RandomRange(&pass.rng_state, -0.9f, 0.9f));
    p.velocity.Set(RandomRange(&pass.rng_state, -0.35f, 0.35f),
                   RandomRange(&pass.rng_state, -0.2f, 0.6f),
                   RandomRange(&pass.rng_state, 0.4f, 1.3f));
    p.life = RandomRange(&pass.rng_state, 0.1f, 1.8f);
    p.size = RandomRange(&pass.rng_state, 0.35f, 0.95f);
    p.energy = RandomRange(&pass.rng_state, 0.4f, 1.0f);
  }
}

void StepMmaamkaParticles(MmaamkaParticlePass& pass,
                          const Camera& camera,
                          double timeline_seconds,
                          const Vec3& emitter_world) {
  if (!pass.enabled || pass.flare.Empty()) {
    return;
  }

  if (!pass.initialized) {
    InitializeMmaamkaParticles(pass, 300, emitter_world, timeline_seconds);
    return;
  }

  double dt = timeline_seconds - pass.last_timeline_seconds;
  pass.last_timeline_seconds = timeline_seconds;
  if (dt <= 0.0) {
    return;
  }
  dt = std::min(dt, 0.15);
  const float dtf = static_cast<float>(dt);

  const Vec3 gravity(0.0f, -0.85f, -0.42f);
  for (Particle& p : pass.particles) {
    p.velocity = p.velocity + gravity * dtf;
    p.position = p.position + p.velocity * dtf;
    p.life -= dtf;

    if (p.position.z <= camera.near_plane + 0.05f) {
      p.life = -1.0f;
    }
    if (p.life <= 0.0f) {
      p.position.Set(emitter_world.x + RandomRange(&pass.rng_state, -0.4f, 0.4f),
                     emitter_world.y + RandomRange(&pass.rng_state, -0.2f, 0.2f),
                     emitter_world.z + RandomRange(&pass.rng_state, -0.35f, 0.35f));
      p.velocity.Set(RandomRange(&pass.rng_state, -0.2f, 0.2f),
                     RandomRange(&pass.rng_state, 0.1f, 0.8f),
                     RandomRange(&pass.rng_state, 0.5f, 1.4f));
      p.life = RandomRange(&pass.rng_state, 0.8f, 2.2f);
      p.size = RandomRange(&pass.rng_state, 0.35f, 1.0f);
      p.energy = RandomRange(&pass.rng_state, 0.5f, 1.0f);
    }
  }

  for (int i = 0; i < 7; ++i) {
    const float ring_t = static_cast<float>(i) / 7.0f;
    const float angle = pass.phase + ring_t * (kPi * 2.0f);
    EmitOneParticle(pass, emitter_world, angle);
  }
  pass.phase += 0.05f;
}

bool ProjectPointToScreen(const Camera& camera,
                          const Vec3& world_pos,
                          int* out_x,
                          int* out_y,
                          float* out_depth) {
  const Vec3 view = world_pos - camera.position;
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
                          const MmaamkaParticlePass& pass) {
  if (!pass.enabled || pass.flare.Empty()) {
    return;
  }

  for (const Particle& p : pass.particles) {
    int sx = 0;
    int sy = 0;
    float depth = 1.0f;
    if (!ProjectPointToScreen(camera, p.position, &sx, &sy, &depth)) {
      continue;
    }

    const float projected = (20.0f / std::max(depth, 0.2f)) * p.size;
    const int sprite_size = std::clamp(static_cast<int>(std::lround(projected)), 2, 46);
    const float intensity_f = (18.0f / std::max(depth, 0.3f)) * p.energy;
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
                   RenderInstance& background_instance,
                   const FetaSceneAssets& feta,
                   const QuickWinPostLayer& post) {
  surface.ClearBack(PackArgb(2, 3, 8));

  const float t = static_cast<float>(state.timeline_seconds);
  camera.fov_degrees = state.feta_fov_degrees;

  if (background.enabled) {
    ConfigureKaaakmaBackgroundInstance(background_instance, background, camera, t);
    renderer.DrawMesh(surface, background.mesh, camera, background_instance);
  }

  ConfigureFetaInstance(mesh_instance, feta, t);
  renderer.DrawMesh(surface, mesh, camera, mesh_instance);

  const Vec3 emitter = FetaTranslationAtTime(t);
  StepMmaamkaParticles(particles, camera, state.timeline_seconds, emitter);
  DrawMmaamkaParticles(surface, camera, particles);

  DrawQuickWinPostLayer(surface, state, post);
  surface.SwapBuffers();
}

void DrawFrame(Surface32& surface,
               const DemoState& state,
               const Mute95SceneAssets& mute95_assets,
               Mute95Runtime& mute95_runtime,
               const Mesh& mesh,
               const KaaakmaBackgroundPass& background,
               MmaamkaParticlePass& particles,
               Camera& camera,
               Renderer3D& renderer,
               RenderInstance& mesh_instance,
               RenderInstance& background_instance,
               const FetaSceneAssets& feta,
               const QuickWinPostLayer& post) {
  if (state.scene_mode == SceneMode::kMute95) {
    DrawMute95Frame(surface, state, mute95_assets, mute95_runtime);
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
                background_instance,
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

  RenderInstance background_instance;

  FetaSceneAssets feta;
  Mute95SceneAssets mute95;
  Mute95Runtime mute95_runtime;
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

  QuickWinPostLayer post;

  const std::string phorward_path = ResolveForwardAssetPath("images/phorward.gif");
  std::string image_error;
  if (!phorward_path.empty() &&
      forward::core::LoadImage32(phorward_path, post.primary, &image_error)) {
    post.enabled = true;
  } else if (!phorward_path.empty()) {
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
  Renderer3D renderer_3d(kLogicalWidth, kLogicalHeight);

  DemoState state;
  if (mute95.enabled) {
    state.scene_mode = SceneMode::kMute95;
    state.scene_label = "mute95";
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
              mesh,
              background,
              particles,
              camera,
              renderer_3d,
              mesh_instance,
              background_instance,
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
