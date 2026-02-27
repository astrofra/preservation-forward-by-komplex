#include "Mesh.h"

#include <algorithm>
#include <cmath>

namespace forward::core {

void Mesh::Clear() {
  positions.clear();
  normals.clear();
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

void Mesh::RebuildVertexNormals() {
  normals.assign(positions.size(), Vec3{});
  if (positions.empty() || triangles.empty()) {
    return;
  }

  for (const Triangle& tri : triangles) {
    const size_t ia = static_cast<size_t>(tri.a);
    const size_t ib = static_cast<size_t>(tri.b);
    const size_t ic = static_cast<size_t>(tri.c);
    if (ia >= positions.size() || ib >= positions.size() || ic >= positions.size()) {
      continue;
    }

    const Vec3& a = positions[ia];
    const Vec3& b = positions[ib];
    const Vec3& c = positions[ic];
    const Vec3 face = (b - a).Cross(c - a);
    normals[ia] = normals[ia] + face;
    normals[ib] = normals[ib] + face;
    normals[ic] = normals[ic] + face;
  }

  for (Vec3& n : normals) {
    n = n.Normalized();
  }
}

}  // namespace forward::core
