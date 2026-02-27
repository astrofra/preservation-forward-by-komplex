#pragma once

#include <cstdint>
#include <vector>

#include "Camera.h"
#include "Mesh.h"
#include "Surface32.h"
#include "Vec3.h"

namespace forward::core {

struct RenderInstance {
  Vec3 rotation_radians{0.0f, 0.0f, 0.0f};
  Vec3 translation{0.0f, 0.0f, 0.0f};
  float uniform_scale = 1.0f;
  uint32_t fill_color = 0xFFB0D0FFu;
  uint32_t wire_color = 0xFFFFFFFFu;
  bool draw_fill = true;
  bool draw_wire = true;
};

class Renderer3D {
 public:
  Renderer3D(int target_width, int target_height);

  void DrawMesh(Surface32& target,
                const Mesh& mesh,
                const Camera& camera,
                const RenderInstance& instance);

 private:
  struct ProjectedVertex {
    Vec3 view_pos;
    float fx = 0.0f;
    float fy = 0.0f;
    float z = 0.0f;
    int x = 0;
    int y = 0;
    bool visible = false;
  };

  void EnsureDepthBuffer();
  void ClearDepthBuffer();

  void DrawFilledTriangle(Surface32& target,
                          const ProjectedVertex& a,
                          const ProjectedVertex& b,
                          const ProjectedVertex& c,
                          uint32_t fill_color);

  void DrawLine(Surface32& target, int x0, int y0, int x1, int y1, uint32_t color) const;

  int target_width_ = 0;
  int target_height_ = 0;
  std::vector<float> depth_buffer_;
};

}  // namespace forward::core
