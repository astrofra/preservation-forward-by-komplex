#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace forward::core {

struct IndexedImage8 {
  int width = 0;
  int height = 0;
  std::vector<uint8_t> indices;
  std::array<uint8_t, 256> palette_r{};
  std::array<uint8_t, 256> palette_g{};
  std::array<uint8_t, 256> palette_b{};

  bool Empty() const { return width <= 0 || height <= 0 || indices.empty(); }
};

// Loads first GIF image block as palette-indexed 8-bit data.
bool LoadGifIndexed8FirstFrame(const std::string& path,
                               IndexedImage8* out_image,
                               std::string* out_error);

}  // namespace forward::core

