#include "Mesh.h"

#include <algorithm>
#include <cmath>

namespace forward::core {

void Mesh::Clear() {
  positions.clear();
  texcoords.clear();
  triangles.clear();
}

bool Mesh::Empty() const {
  return positions.empty() || triangles.empty();
}

float Mesh::BoundingRadius() const {
  float radius_sq = 0.0f;
  for (const Vec3& p : positions) {
    radius_sq = std::max(radius_sq, p.LengthSq());
  }
  return std::sqrt(radius_sq);
}

}  // namespace forward::core
