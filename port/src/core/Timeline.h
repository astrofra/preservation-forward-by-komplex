#pragma once

#include <cstdint>
#include <vector>

#include "Vec3.h"

namespace forward::core {

struct TimelineOutput {
  Vec3 rotation_radians{0.0f, 0.0f, 0.0f};
  Vec3 translation{0.0f, 0.0f, 2.4f};
  float camera_fov_degrees = 70.0f;
  uint32_t fill_color = 0xFF6CA7E0u;
  uint32_t wire_color = 0xFFC5EEFFu;
  bool draw_fill = true;
  bool draw_wire = true;
};

class TimelineDriver {
 public:
  TimelineDriver();

  void Evaluate(double timeline_seconds, TimelineOutput& out) const;

 private:
  struct Keyframe {
    float time_seconds = 0.0f;
    TimelineOutput value;
  };

  std::vector<Keyframe> keyframes_;
  float loop_seconds_ = 0.0f;
};

}  // namespace forward::core
