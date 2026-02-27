#include <SDL.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
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

struct DemoState {
  double timeline_seconds = 0.0;
  bool paused = false;
  bool fullscreen = false;
  bool show_post = false;
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
      (0.5f * static_cast<float>(kLogicalHeight)) / std::tan(half_fov);
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

void DrawFrame(Surface32& surface,
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
  camera.fov_degrees = 62.0f + 6.0f * std::sin(t * 0.31f);

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

}  // namespace

int main() {
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
  camera.fov_degrees = 70.0f;
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
  MmaamkaParticlePass particles;
  KaaakmaBackgroundPass background;
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
  if (feta.enabled && background.enabled && particles.enabled) {
    state.scene_label = "feta+kaaakma+mmaamka";
  } else if (feta.enabled) {
    state.scene_label = "feta";
  } else {
    state.scene_label = "fallback";
  }
  state.mesh_label = std::filesystem::path(mesh_path).filename().string();
  state.post_label = state.show_post && post.enabled ? "phorward" : "off";
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
