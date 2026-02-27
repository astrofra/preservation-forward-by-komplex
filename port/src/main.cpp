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

#include "core/Camera.h"
#include "core/Mesh.h"
#include "core/MeshLoaderIgu.h"
#include "core/Renderer3D.h"
#include "core/Surface32.h"
#include "core/Timeline.h"
#include "core/Vec3.h"

namespace {

using forward::core::Camera;
using forward::core::Mesh;
using forward::core::RenderInstance;
using forward::core::Renderer3D;
using forward::core::Surface32;
using forward::core::TimelineDriver;
using forward::core::TimelineOutput;
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
  std::string mesh_label;
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
  const std::array<std::string, 2> mesh_names = {"half8.igu", "octa8.igu"};
  std::error_code ec;
  std::filesystem::path cursor = std::filesystem::current_path(ec);
  if (ec) {
    return {};
  }

  while (true) {
    for (const std::string& mesh_name : mesh_names) {
      const std::filesystem::path candidate =
          cursor / "original" / "forward" / "meshes" / mesh_name;
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
        std::filesystem::path("original") / "forward" / "meshes" / mesh_name;
    std::error_code ec;
    if (std::filesystem::exists(candidate, ec) && !ec) {
      return candidate.string();
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
        << " | mesh " << state.mesh_label << " | logical " << kLogicalWidth << "x"
        << kLogicalHeight << " | audio pending";

  SDL_SetWindowTitle(window, title.str().c_str());
}

void DrawFrame(Surface32& surface,
               const DemoState& state,
               const Mesh& mesh,
               Camera& camera,
               Renderer3D& renderer,
               RenderInstance& instance,
               const TimelineDriver& timeline) {
  surface.ClearBack(PackArgb(2, 3, 8));

  TimelineOutput timeline_output;
  timeline.Evaluate(state.timeline_seconds, timeline_output);
  instance.rotation_radians = timeline_output.rotation_radians;
  instance.translation = timeline_output.translation;
  instance.fill_color = timeline_output.fill_color;
  instance.wire_color = timeline_output.wire_color;
  instance.draw_fill = timeline_output.draw_fill;
  instance.draw_wire = timeline_output.draw_wire;
  camera.fov_degrees = timeline_output.camera_fov_degrees;

  renderer.DrawMesh(surface, mesh, camera, instance);
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

  TimelineDriver timeline;

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
  state.mesh_label = std::filesystem::path(mesh_path).filename().string();
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

    DrawFrame(surface, state, mesh, camera, renderer_3d, instance, timeline);

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
