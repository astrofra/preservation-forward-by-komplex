#include <SDL.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr int kLogicalWidth = 512;
constexpr int kLogicalHeight = 256;
constexpr double kTickHz = 50.0;
constexpr double kTickDtSeconds = 1.0 / kTickHz;

struct Settings {
  bool nosound = true;
  bool one_to_one = false;
  int window_scale = 2;
};

struct RuntimeStats {
  uint64_t rendered_frames = 0;
  uint64_t simulated_ticks = 0;
  double title_timer = 0.0;
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

int ParseInt(const char* value, int fallback) {
  if (!value) {
    return fallback;
  }
  char* end = nullptr;
  long parsed = std::strtol(value, &end, 10);
  if (end == value || *end != '\0') {
    return fallback;
  }
  if (parsed < 1) {
    return 1;
  }
  if (parsed > 16) {
    return 16;
  }
  return static_cast<int>(parsed);
}

Settings ParseArgs(int argc, char** argv) {
  Settings settings;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--nosound") {
      settings.nosound = true;
      continue;
    }
    if (arg == "--1x1") {
      settings.one_to_one = true;
      settings.window_scale = 1;
      continue;
    }
    if (arg == "--scale" && i + 1 < argc) {
      settings.window_scale = ParseInt(argv[++i], settings.window_scale);
      continue;
    }
  }
  return settings;
}

void DrawFrame(std::vector<uint32_t>& framebuffer, const DemoState& state) {
  const double t = state.timeline_seconds;
  for (int y = 0; y < kLogicalHeight; ++y) {
    for (int x = 0; x < kLogicalWidth; ++x) {
      const double nx = (2.0 * x / static_cast<double>(kLogicalWidth)) - 1.0;
      const double ny = (2.0 * y / static_cast<double>(kLogicalHeight)) - 1.0;

      const double radius = std::sqrt(nx * nx + ny * ny);
      const double wave = std::sin((nx * 10.0) + (t * 2.2)) +
                          std::cos((ny * 14.0) - (t * 1.7));
      const double ring = std::sin((radius * 24.0) - (t * 3.3));

      const double r = std::clamp(0.5 + 0.3 * wave + 0.2 * ring, 0.0, 1.0);
      const double g = std::clamp(0.4 + 0.4 * std::sin((nx + ny) * 8.0 + t), 0.0,
                                  1.0);
      const double b = std::clamp(0.25 + 0.55 * std::cos((radius * 9.0) - t),
                                  0.0, 1.0);

      framebuffer[(y * kLogicalWidth) + x] =
          PackArgb(static_cast<uint8_t>(r * 255.0),
                   static_cast<uint8_t>(g * 255.0),
                   static_cast<uint8_t>(b * 255.0));
    }
  }
}

SDL_Rect ComputePresentationRect(SDL_Renderer* renderer) {
  int window_w = 0;
  int window_h = 0;
  SDL_GetRendererOutputSize(renderer, &window_w, &window_h);

  const int scale_x = std::max(1, window_w / kLogicalWidth);
  const int scale_y = std::max(1, window_h / kLogicalHeight);
  const int scale = std::max(1, std::min(scale_x, scale_y));

  const int out_w = kLogicalWidth * scale;
  const int out_h = kLogicalHeight * scale;
  const int out_x = (window_w - out_w) / 2;
  const int out_y = (window_h - out_h) / 2;

  return SDL_Rect{out_x, out_y, out_w, out_h};
}

void UpdateWindowTitle(SDL_Window* window,
                       const Settings& settings,
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
        << " | nosound " << (settings.nosound ? "on" : "off")
        << " | logical " << kLogicalWidth << "x" << kLogicalHeight;

  SDL_SetWindowTitle(window, title.str().c_str());
}

}  // namespace

int main(int argc, char** argv) {
  const Settings settings = ParseArgs(argc, argv);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
    return 1;
  }

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

  const int initial_w = kLogicalWidth * settings.window_scale;
  const int initial_h = kLogicalHeight * settings.window_scale;

  SDL_Window* window = SDL_CreateWindow(
      "forward native harness",
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

  SDL_Renderer* renderer = SDL_CreateRenderer(
      window,
      -1,
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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

  std::vector<uint32_t> framebuffer(kLogicalWidth * kLogicalHeight,
                                    PackArgb(0, 0, 0));

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
            SDL_SetWindowFullscreen(
                window,
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

    DrawFrame(framebuffer, state);

    if (SDL_UpdateTexture(texture,
                          nullptr,
                          framebuffer.data(),
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
      UpdateWindowTitle(window, settings, state, stats, title_elapsed);
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
