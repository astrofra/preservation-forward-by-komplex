#pragma once

#include <vector>

#include "Vec2.h"
#include "Vec3.h"

namespace forward::core {

struct Triangle {
  int a = 0;
  int b = 0;
  int c = 0;
};

class Mesh {
 public:
  void Clear();
  bool Empty() const;
  float BoundingRadius() const;
  void RebuildVertexNormals();

  std::vector<Vec3> positions;
  std::vector<Vec3> normals;
  std::vector<Vec2> texcoords;
  std::vector<Triangle> triangles;
};

}  // namespace forward::core
