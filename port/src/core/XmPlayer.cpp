#include "XmPlayer.h"

#include <xmp.h>

#include <cstring>

namespace forward::core {
namespace {

void SetError(std::string* out_error, const std::string& value) {
  if (out_error) {
    *out_error = value;
  }
}

}  // namespace

XmPlayer::~XmPlayer() { Shutdown(); }

bool XmPlayer::Initialize(int sample_rate, int buffer_frames, std::string* out_error) {
  Shutdown();

  xmp_ctx_ = xmp_create_context();
  if (!xmp_ctx_) {
    SetError(out_error, "xmp_create_context failed");
    return false;
  }

  SDL_AudioSpec desired{};
  desired.freq = sample_rate;
  desired.format = AUDIO_S16SYS;
  desired.channels = 2;
  desired.samples = static_cast<Uint16>(buffer_frames);
  desired.callback = &XmPlayer::SDLAudioCallback;
  desired.userdata = this;

  audio_device_ = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained_spec_, 0);
  if (audio_device_ == 0) {
    SetError(out_error, std::string("SDL_OpenAudioDevice failed: ") + SDL_GetError());
    xmp_free_context(static_cast<xmp_context>(xmp_ctx_));
    xmp_ctx_ = nullptr;
    return false;
  }
  return true;
}

bool XmPlayer::LoadModule(int slot, const std::string& path, std::string* out_error) {
  if (slot < 1 || slot >= static_cast<int>(module_paths_.size())) {
    SetError(out_error, "invalid module slot");
    return false;
  }
  if (path.empty()) {
    SetError(out_error, "module path is empty");
    return false;
  }
  module_paths_[slot] = path;
  return true;
}

bool XmPlayer::StartModule(int slot, bool loop, std::string* out_error) {
  if (audio_device_ == 0 || !xmp_ctx_) {
    SetError(out_error, "XmPlayer not initialized");
    return false;
  }
  SDL_LockAudioDevice(audio_device_);
  const bool ok = StartModuleLocked(slot, loop, out_error);
  SDL_UnlockAudioDevice(audio_device_);
  return ok;
}

bool XmPlayer::StartModuleLocked(int slot, bool loop, std::string* out_error) {
  if (slot < 1 || slot >= static_cast<int>(module_paths_.size())) {
    SetError(out_error, "invalid module slot");
    return false;
  }
  if (module_paths_[slot].empty()) {
    SetError(out_error, "module path not loaded for requested slot");
    return false;
  }

  xmp_context ctx = static_cast<xmp_context>(xmp_ctx_);
  if (module_loaded_in_context_.load(std::memory_order_acquire)) {
    xmp_end_player(ctx);
    xmp_release_module(ctx);
    module_loaded_in_context_.store(false, std::memory_order_release);
  }

  if (xmp_load_module(ctx, module_paths_[slot].c_str()) != 0) {
    SetError(out_error, "xmp_load_module failed");
    return false;
  }
  if (xmp_start_player(ctx, obtained_spec_.freq, 0) != 0) {
    xmp_release_module(ctx);
    SetError(out_error, "xmp_start_player failed");
    return false;
  }

  loop_current_module_.store(loop, std::memory_order_release);
  module_loaded_in_context_.store(true, std::memory_order_release);
  active_module_slot_.store(slot, std::memory_order_release);

  const int64_t current_clock_ms = clock_time_ms_.load(std::memory_order_acquire);
  module_base_time_ms_.store(current_clock_ms, std::memory_order_release);
  module_time_ms_.store(0, std::memory_order_release);
  order_.store(0, std::memory_order_release);
  row_.store(0, std::memory_order_release);
  speed_.store(0, std::memory_order_release);
  bpm_.store(0, std::memory_order_release);
  timing_valid_.store(false, std::memory_order_release);
  SDL_PauseAudioDevice(audio_device_, paused_.load(std::memory_order_acquire) ? 1 : 0);
  return true;
}

void XmPlayer::SetPaused(bool paused) {
  paused_.store(paused, std::memory_order_release);
  if (audio_device_ != 0) {
    SDL_PauseAudioDevice(audio_device_, paused ? 1 : 0);
  }
}

XmTiming XmPlayer::GetTiming() const {
  XmTiming out;
  out.valid = timing_valid_.load(std::memory_order_acquire);
  out.module_slot = active_module_slot_.load(std::memory_order_acquire);
  out.order = order_.load(std::memory_order_acquire);
  out.row = row_.load(std::memory_order_acquire);
  out.speed = speed_.load(std::memory_order_acquire);
  out.bpm = bpm_.load(std::memory_order_acquire);
  out.module_time_ms = module_time_ms_.load(std::memory_order_acquire);
  out.clock_time_ms = clock_time_ms_.load(std::memory_order_acquire);
  return out;
}

bool XmPlayer::IsReady() const { return audio_device_ != 0 && xmp_ctx_ != nullptr; }

void XmPlayer::Shutdown() {
  if (audio_device_ != 0) {
    SDL_PauseAudioDevice(audio_device_, 1);
    SDL_LockAudioDevice(audio_device_);
    if (xmp_ctx_ && module_loaded_in_context_.load(std::memory_order_acquire)) {
      xmp_end_player(static_cast<xmp_context>(xmp_ctx_));
      xmp_release_module(static_cast<xmp_context>(xmp_ctx_));
      module_loaded_in_context_.store(false, std::memory_order_release);
    }
    SDL_UnlockAudioDevice(audio_device_);
    SDL_CloseAudioDevice(audio_device_);
    audio_device_ = 0;
  }

  if (xmp_ctx_) {
    xmp_free_context(static_cast<xmp_context>(xmp_ctx_));
    xmp_ctx_ = nullptr;
  }
  module_paths_.fill({});
  loop_current_module_.store(false, std::memory_order_release);
  paused_.store(false, std::memory_order_release);
  timing_valid_.store(false, std::memory_order_release);
  active_module_slot_.store(0, std::memory_order_release);
  order_.store(0, std::memory_order_release);
  row_.store(0, std::memory_order_release);
  speed_.store(0, std::memory_order_release);
  bpm_.store(0, std::memory_order_release);
  module_time_ms_.store(0, std::memory_order_release);
  clock_time_ms_.store(0, std::memory_order_release);
  module_base_time_ms_.store(0, std::memory_order_release);
}

void XmPlayer::SDLAudioCallback(void* userdata, Uint8* stream, int len) {
  if (!userdata || !stream || len <= 0) {
    return;
  }
  static_cast<XmPlayer*>(userdata)->OnAudio(stream, len);
}

void XmPlayer::OnAudio(Uint8* stream, int len) {
  std::memset(stream, 0, static_cast<size_t>(len));
  if (!xmp_ctx_ || !module_loaded_in_context_.load(std::memory_order_acquire) ||
      paused_.load(std::memory_order_acquire)) {
    return;
  }

  xmp_context ctx = static_cast<xmp_context>(xmp_ctx_);
  const int loop_flag = loop_current_module_.load(std::memory_order_acquire) ? 1 : 0;
  const int result = xmp_play_buffer(ctx, stream, len, loop_flag);
  if (result < 0 && result != -XMP_END) {
    timing_valid_.store(false, std::memory_order_release);
    return;
  }

  xmp_frame_info info{};
  xmp_get_frame_info(ctx, &info);

  const int64_t module_ms = static_cast<int64_t>(info.time);
  const int64_t absolute_ms = module_base_time_ms_.load(std::memory_order_acquire) + module_ms;
  module_time_ms_.store(module_ms, std::memory_order_release);
  clock_time_ms_.store(absolute_ms, std::memory_order_release);
  order_.store(info.pos, std::memory_order_release);
  row_.store(info.row, std::memory_order_release);
  speed_.store(info.speed, std::memory_order_release);
  bpm_.store(info.bpm, std::memory_order_release);
  timing_valid_.store(true, std::memory_order_release);

  if (result == -XMP_END && !loop_current_module_.load(std::memory_order_acquire)) {
    module_loaded_in_context_.store(false, std::memory_order_release);
    xmp_end_player(ctx);
    xmp_release_module(ctx);
  }
}

}  // namespace forward::core
