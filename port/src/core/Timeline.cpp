#include "Timeline.h"

#include <algorithm>
#include <cmath>

namespace forward::core {
namespace {

float WrapTime(double t, float loop_seconds) {
  if (loop_seconds <= 0.0f) {
    return 0.0f;
  }
  const double wrapped = std::fmod(t, static_cast<double>(loop_seconds));
  if (wrapped < 0.0) {
    return static_cast<float>(wrapped + loop_seconds);
  }
  return static_cast<float>(wrapped);
}

float Lerp(float a, float b, float t) {
  return a + (b - a) * t;
}

Vec3 LerpVec3(const Vec3& a, const Vec3& b, float t) {
  return a + (b - a) * t;
}

uint8_t ChannelR(uint32_t argb) { return static_cast<uint8_t>((argb >> 16u) & 0xFFu); }
uint8_t ChannelG(uint32_t argb) { return static_cast<uint8_t>((argb >> 8u) & 0xFFu); }
uint8_t ChannelB(uint32_t argb) { return static_cast<uint8_t>(argb & 0xFFu); }

uint32_t PackArgb(uint8_t r, uint8_t g, uint8_t b) {
  return (0xFFu << 24u) | (static_cast<uint32_t>(r) << 16u) |
         (static_cast<uint32_t>(g) << 8u) | static_cast<uint32_t>(b);
}

uint32_t LerpColor(uint32_t a, uint32_t b, float t) {
  const uint8_t r = static_cast<uint8_t>(std::clamp(Lerp(ChannelR(a), ChannelR(b), t), 0.0f, 255.0f));
  const uint8_t g = static_cast<uint8_t>(std::clamp(Lerp(ChannelG(a), ChannelG(b), t), 0.0f, 255.0f));
  const uint8_t bch = static_cast<uint8_t>(std::clamp(Lerp(ChannelB(a), ChannelB(b), t), 0.0f, 255.0f));
  return PackArgb(r, g, bch);
}

}  // namespace

TimelineDriver::TimelineDriver() {
  keyframes_ = {
      {0.0f, TimelineOutput{
                 Vec3(0.0f, 0.0f, 0.0f),
                 Vec3(0.0f, 0.0f, 2.60f),
                 72.0f,
                 0xFF2A4CC8u,
                 0xFF93E8FFu,
                 true,
                 true,
             }},
      {6.0f, TimelineOutput{
                 Vec3(1.5f, 1.1f, 0.5f),
                 Vec3(0.0f, 0.1f, 2.25f),
                 66.0f,
                 0xFF4F67DAu,
                 0xFFC4F2FFu,
                 true,
                 true,
             }},
      {12.0f, TimelineOutput{
                  Vec3(2.7f, 2.2f, 1.1f),
                  Vec3(0.0f, -0.1f, 1.90f),
                  56.0f,
                  0xFFC06A2Cu,
                  0xFFF8CC9Fu,
                  true,
                  false,
              }},
      {18.0f, TimelineOutput{
                  Vec3(3.6f, 3.0f, 2.1f),
                  Vec3(0.0f, 0.0f, 2.35f),
                  64.0f,
                  0xFF3BA67Du,
                  0xFFA9FFE2u,
                  true,
                  true,
              }},
      {24.0f, TimelineOutput{
                  Vec3(5.2f, 4.2f, 3.1f),
                  Vec3(0.0f, 0.0f, 2.70f),
                  74.0f,
                  0xFF3345B0u,
                  0xFF90DFF9u,
                  true,
                  true,
              }},
  };
  loop_seconds_ = keyframes_.back().time_seconds;
}

void TimelineDriver::Evaluate(double timeline_seconds, TimelineOutput& out) const {
  if (keyframes_.size() < 2 || loop_seconds_ <= 0.0f) {
    return;
  }

  const float t = WrapTime(timeline_seconds, loop_seconds_);

  size_t next_index = 1;
  while (next_index < keyframes_.size() && keyframes_[next_index].time_seconds < t) {
    ++next_index;
  }
  if (next_index >= keyframes_.size()) {
    next_index = keyframes_.size() - 1;
  }
  const size_t prev_index = (next_index == 0) ? 0 : (next_index - 1);

  const Keyframe& prev = keyframes_[prev_index];
  const Keyframe& next = keyframes_[next_index];
  const float span = std::max(0.001f, next.time_seconds - prev.time_seconds);
  const float alpha = std::clamp((t - prev.time_seconds) / span, 0.0f, 1.0f);

  out.rotation_radians = LerpVec3(prev.value.rotation_radians, next.value.rotation_radians, alpha);
  out.translation = LerpVec3(prev.value.translation, next.value.translation, alpha);
  out.camera_fov_degrees = Lerp(prev.value.camera_fov_degrees, next.value.camera_fov_degrees, alpha);
  out.fill_color = LerpColor(prev.value.fill_color, next.value.fill_color, alpha);
  out.wire_color = LerpColor(prev.value.wire_color, next.value.wire_color, alpha);

  // Toggle-style channels stay piecewise for now; this is intentionally easy to replace
  // when the real forward timeline/state classes are ported.
  out.draw_fill = prev.value.draw_fill;
  out.draw_wire = prev.value.draw_wire;

  const float pulse = static_cast<float>(std::sin(t * 0.85f));
  out.translation.y += 0.10f * pulse;
  out.rotation_radians.z += 0.12f * pulse;
}

}  // namespace forward::core
