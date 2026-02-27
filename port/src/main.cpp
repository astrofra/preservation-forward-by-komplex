#include <SDL.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "core/Surface32.h"
#include "core/Vec2.h"
#include "core/Vec3.h"
#include "core/Vertex.h"

namespace {

using forward::core::Surface32;
using forward::core::Vec2;
using forward::core::Vec3;
using forward::core::Vertex;

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
        << " | logical " << kLogicalWidth << "x" << kLogicalHeight
        << " | audio pending";

  SDL_SetWindowTitle(window, title.str().c_str());
}

void BuildOverlayPattern(Surface32& overlay) {
  overlay.ClearBack(PackArgb(12, 12, 16));

  for (int y = 0; y < overlay.height(); ++y) {
    for (int x = 0; x < overlay.width(); ++x) {
      const bool checker = ((x / 8) + (y / 8)) % 2 == 0;
      if (!checker) {
        continue;
      }
      const uint8_t r = static_cast<uint8_t>(48 + (x % 64));
      const uint8_t g = static_cast<uint8_t>(24 + (y % 64));
      const uint8_t b = static_cast<uint8_t>(96 + ((x + y) % 64));
      overlay.SetBackPixel(x, y, PackArgb(r, g, b));
    }
  }

  overlay.SwapBuffers();
}

void DrawMarker(Surface32& surface, const DemoState& state) {
  Vertex marker;
  marker.Set(static_cast<float>(std::cos(state.timeline_seconds * 1.3) * 0.75),
             static_cast<float>(std::sin(state.timeline_seconds * 0.9) * 0.45),
             0.0f);
  marker.sx = (marker.x * 0.5f + 0.5f) * static_cast<float>(kLogicalWidth - 1);
  marker.sy = (marker.y * 0.5f + 0.5f) * static_cast<float>(kLogicalHeight - 1);

  const int mx = static_cast<int>(marker.sx);
  const int my = static_cast<int>(marker.sy);
  const uint32_t color = PackArgb(255, 255, 255);

  for (int d = -2; d <= 2; ++d) {
    surface.SetBackPixel(mx + d, my, color);
    surface.SetBackPixel(mx, my + d, color);
  }
}

void DrawFrame(Surface32& surface, const Surface32& overlay, const DemoState& state) {
  const double t = state.timeline_seconds;
  surface.ClearBack(PackArgb(0, 0, 0));

  const Vec3 base_color(0.25f, 0.22f, 0.28f);

  for (int y = 0; y < kLogicalHeight; ++y) {
    for (int x = 0; x < kLogicalWidth; ++x) {
      const Vec2 uv(
          static_cast<float>(x) / static_cast<float>(kLogicalWidth - 1),
          static_cast<float>(y) / static_cast<float>(kLogicalHeight - 1));

      const float nx = uv.x * 2.0f - 1.0f;
      const float ny = uv.y * 2.0f - 1.0f;
      const float radius = std::sqrt(nx * nx + ny * ny);

      const float wave = static_cast<float>(std::sin((nx * 10.0f) + (t * 2.2)) +
                                            std::cos((ny * 14.0f) - (t * 1.7)));
      const float ring = static_cast<float>(std::sin((radius * 24.0f) - (t * 3.3)));

      Vec3 color(
          base_color.x + 0.28f * wave + 0.20f * ring,
          base_color.y + 0.30f * std::sin((nx + ny) * 8.0f + t),
          base_color.z + 0.45f * std::cos((radius * 9.0f) - t));

      color.x = std::clamp(color.x, 0.0f, 1.0f);
      color.y = std::clamp(color.y, 0.0f, 1.0f);
      color.z = std::clamp(color.z, 0.0f, 1.0f);

      surface.SetBackPixel(
          x,
          y,
          PackArgb(static_cast<uint8_t>(color.x * 255.0f),
                   static_cast<uint8_t>(color.y * 255.0f),
                   static_cast<uint8_t>(color.z * 255.0f)));
    }
  }

  if ((static_cast<int>(t * 2.0) & 1) == 0) {
    surface.AddBackRgb(8, 4, 0);
  } else {
    surface.SubBackRgb(4, 1, 0);
  }

  const int overlay_x = static_cast<int>(
      (std::sin(t * 0.8) * 0.5 + 0.5) * static_cast<double>(kLogicalWidth - overlay.width()));
  const int overlay_y = static_cast<int>(
      (std::cos(t * 0.5) * 0.5 + 0.5) * static_cast<double>(kLogicalHeight - overlay.height()));
  surface.BlitToBack(overlay, 0, 0, overlay_x, overlay_y, overlay.width(), overlay.height());

  DrawMarker(surface, state);

  surface.SwapBuffers();
}

}  // namespace

int main() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
    return 1;
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

  SDL_Renderer* renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  }
  if (!renderer) {
    std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_Texture* texture = SDL_CreateTexture(renderer,
                                           SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           kLogicalWidth,
                                           kLogicalHeight);
  if (!texture) {
    std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << "\n";
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  Surface32 surface(kLogicalWidth, kLogicalHeight, true);
  Surface32 overlay(160, 72, false);
  BuildOverlayPattern(overlay);

  DemoState state;
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

    DrawFrame(surface, overlay, state);

    if (SDL_UpdateTexture(texture,
                          nullptr,
                          surface.FrontPixels(),
                          kLogicalWidth * static_cast<int>(sizeof(uint32_t))) != 0) {
      std::cerr << "SDL_UpdateTexture failed: " << SDL_GetError() << "\n";
      running = false;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    const SDL_Rect dst = ComputePresentationRect(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
    SDL_RenderPresent(renderer);

    ++stats.rendered_frames;

    if (title_elapsed >= 0.5) {
      UpdateWindowTitle(window, state, stats, title_elapsed);
      stats.rendered_frames = 0;
      stats.simulated_ticks = 0;
      title_elapsed = 0.0;
    }
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
