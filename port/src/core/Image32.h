#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace forward::core {

struct Image32 {
  int width = 0;
  int height = 0;
  std::vector<uint32_t> pixels;

  bool Empty() const { return width <= 0 || height <= 0 || pixels.empty(); }
};

bool LoadImage32(const std::string& path, Image32& out_image, std::string* out_error);

}  // namespace forward::core
