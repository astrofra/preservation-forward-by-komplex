#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "GifIndexed.h"

namespace forward::core {

class Surface32;

class IndexedSurface8 {
 public:
  IndexedSurface8(int width, int height);

  int width() const { return width_; }
  int height() const { return height_; }

  void SetPalette(const std::array<uint8_t, 256>& r,
                  const std::array<uint8_t, 256>& g,
                  const std::array<uint8_t, 256>& b);

  void BlitImageAt(const IndexedImage8& src, int dst_x, int dst_y);

  void PresentToBack(Surface32& destination) const;

 private:
  int width_ = 0;
  int height_ = 0;
  std::vector<uint8_t> indices_;
  std::array<uint8_t, 256> palette_r_{};
  std::array<uint8_t, 256> palette_g_{};
  std::array<uint8_t, 256> palette_b_{};
  std::array<uint32_t, 256> palette_argb_{};
};

}  // namespace forward::core

