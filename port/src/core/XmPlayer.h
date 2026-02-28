#pragma once

#include <SDL.h>

#include <array>
#include <atomic>
#include <cstdint>
#include <string>

namespace forward::core {

struct XmTiming {
  bool valid = false;
  int module_slot = 0;
  int order = 0;
  int row = 0;
  int speed = 0;
  int bpm = 0;
  int64_t module_time_ms = 0;
  int64_t clock_time_ms = 0;
};

class XmPlayer {
 public:
  XmPlayer() = default;
  ~XmPlayer();

  XmPlayer(const XmPlayer&) = delete;
  XmPlayer& operator=(const XmPlayer&) = delete;

  bool Initialize(int sample_rate, int buffer_frames, std::string* out_error);
  bool LoadModule(int slot, const std::string& path, std::string* out_error);
  bool StartModule(int slot, bool loop, std::string* out_error);
  void SetPaused(bool paused);

  XmTiming GetTiming() const;
  bool IsReady() const;

  void Shutdown();

 private:
  static void SDLAudioCallback(void* userdata, Uint8* stream, int len);
  void OnAudio(Uint8* stream, int len);
  bool StartModuleLocked(int slot, bool loop, std::string* out_error);

  void* xmp_ctx_ = nullptr;
  SDL_AudioDeviceID audio_device_ = 0;
  SDL_AudioSpec obtained_spec_{};
  std::array<std::string, 3> module_paths_;
  std::atomic<bool> module_loaded_in_context_{false};
  std::atomic<bool> loop_current_module_{false};

  std::atomic<bool> paused_{false};
  std::atomic<bool> timing_valid_{false};
  std::atomic<int> active_module_slot_{0};
  std::atomic<int> order_{0};
  std::atomic<int> row_{0};
  std::atomic<int> speed_{0};
  std::atomic<int> bpm_{0};
  std::atomic<int64_t> module_time_ms_{0};
  std::atomic<int64_t> clock_time_ms_{0};
  std::atomic<int64_t> module_base_time_ms_{0};
};

}  // namespace forward::core
