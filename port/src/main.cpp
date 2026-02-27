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

void DrawFetaFlare(Surface32& surface, const Image32& flare, double timeline_seconds) {
  if (flare.Empty()) {
    return;
  }

  const float t = static_cast<float>(timeline_seconds);
  const int center_x = kLogicalWidth / 2 + static_cast<int>(std::sin(t * 0.7f) * 64.0f);
  const int center_y = kLogicalHeight / 2 + static_cast<int>(std::cos(t * 0.5f) * 26.0f);
  const int x = center_x - flare.width / 2;
  const int y = center_y - flare.height / 2;
  const uint8_t intensity = static_cast<uint8_t>(120 + 100 * (0.5f + 0.5f * std::sin(t * 1.3f)));

  surface.AdditiveBlitToBack(
      flare.pixels.data(), flare.width, flare.height, 0, 0, x, y, flare.width, flare.height, intensity);
}

void DrawFrame(Surface32& surface,
               const DemoState& state,
               const Mesh& mesh,
               Camera& camera,
               Renderer3D& renderer,
               RenderInstance& instance,
               const FetaSceneAssets& feta,
               const QuickWinPostLayer& post) {
  surface.ClearBack(PackArgb(2, 3, 8));

  const float t = static_cast<float>(state.timeline_seconds);
  instance.rotation_radians.Set(0.28f * std::sin(t * 0.14f), -t * 0.52f, t * 0.11f);
  instance.translation.Set(0.0f, 0.12f * std::sin(t * 0.37f), 2.55f + 0.35f * std::sin(t * 0.21f));
  instance.fill_color = PackArgb(220, 220, 220);
  instance.wire_color = PackArgb(110, 255, 220);
  instance.draw_fill = true;
  instance.draw_wire = false;
  instance.texture = feta.enabled ? &feta.babyenv : nullptr;
  instance.use_mesh_uv = true;
  instance.texture_wrap = true;
  camera.fov_degrees = 62.0f + 6.0f * std::sin(t * 0.31f);

  renderer.DrawMesh(surface, mesh, camera, instance);
  if (feta.enabled) {
    DrawFetaFlare(surface, feta.flare, state.timeline_seconds);
  }
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

  RenderInstance instance;
  const float radius = mesh.BoundingRadius();
  instance.uniform_scale = (radius > 0.001f) ? (1.0f / radius) : 1.0f;
  instance.translation = Vec3(0.0f, 0.0f, 2.6f);
  instance.draw_fill = true;
  instance.draw_wire = true;
  instance.enable_backface_culling = true;

  FetaSceneAssets feta;
  if (std::filesystem::path(mesh_path).filename().string() == "fetus.igu") {
    std::string image_error;
    const std::string babyenv_path = ResolveForwardAssetPath("images/babyenv.jpg");
    const std::string flare_path = ResolveForwardAssetPath("images/flare1.jpg");

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

    feta.enabled = has_babyenv;
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
  state.scene_label = feta.enabled ? "feta" : "fallback";
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

    DrawFrame(surface, state, mesh, camera, renderer_3d, instance, feta, post);

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
