#include "IndexedSurface8.h"

#include <algorithm>
#include <cstddef>

#include "Surface32.h"

namespace forward::core {
namespace {

uint32_t PackArgb(uint8_t r, uint8_t g, uint8_t b) {
  return (0xFFu << 24u) | (static_cast<uint32_t>(r) << 16u) |
         (static_cast<uint32_t>(g) << 8u) | static_cast<uint32_t>(b);
}

}  // namespace

IndexedSurface8::IndexedSurface8(int width, int height)
    : width_(std::max(0, width)),
      height_(std::max(0, height)),
      indices_(static_cast<size_t>(std::max(0, width)) * static_cast<size_t>(std::max(0, height)), 0u) {
  // Mirrors kmajkka constructor behavior where default index buffer is a repeating ramp.
  for (size_t i = 0; i < indices_.size(); ++i) {
    indices_[i] = static_cast<uint8_t>(i & 0xFFu);
  }
  SetPalette(palette_r_, palette_g_, palette_b_);
}

void IndexedSurface8::SetPalette(const std::array<uint8_t, 256>& r,
                                 const std::array<uint8_t, 256>& g,
                                 const std::array<uint8_t, 256>& b) {
  palette_r_ = r;
  palette_g_ = g;
  palette_b_ = b;
  for (int i = 0; i < 256; ++i) {
    palette_argb_[static_cast<size_t>(i)] =
        PackArgb(palette_r_[static_cast<size_t>(i)],
                 palette_g_[static_cast<size_t>(i)],
                 palette_b_[static_cast<size_t>(i)]);
  }
}

void IndexedSurface8::BlitImageAt(const IndexedImage8& src, int dst_x, int dst_y) {
  if (src.Empty() || width_ <= 0 || height_ <= 0) {
    return;
  }

  int src_x = 0;
  int src_y = 0;
  int copy_w = src.width;
  int copy_h = src.height;

  if (dst_x < 0) {
    copy_w += dst_x;
    src_x -= dst_x;
    dst_x = 0;
  }
  if (copy_w <= 0) {
    return;
  }
  if (dst_x + copy_w > width_) {
    copy_w = width_ - dst_x;
  }
  if (copy_w <= 0) {
    return;
  }

  if (dst_y < 0) {
    copy_h += dst_y;
    src_y -= dst_y;
    dst_y = 0;
  }
  if (copy_h <= 0) {
    return;
  }
  if (dst_y + copy_h > height_) {
    copy_h = height_ - dst_y;
  }
  if (copy_h <= 0) {
    return;
  }

  if (src_x < 0) {
    copy_w += src_x;
    dst_x -= src_x;
    src_x = 0;
  }
  if (src_y < 0) {
    copy_h += src_y;
    dst_y -= src_y;
    src_y = 0;
  }
  if (copy_w <= 0 || copy_h <= 0 || src_x >= src.width || src_y >= src.height) {
    return;
  }

  copy_w = std::min(copy_w, src.width - src_x);
  copy_h = std::min(copy_h, src.height - src_y);
  if (copy_w <= 0 || copy_h <= 0) {
    return;
  }

  for (int row = 0; row < copy_h; ++row) {
    const size_t src_row = static_cast<size_t>(src_y + row) * static_cast<size_t>(src.width) +
                           static_cast<size_t>(src_x);
    const size_t dst_row = static_cast<size_t>(dst_y + row) * static_cast<size_t>(width_) +
                           static_cast<size_t>(dst_x);
    std::copy_n(src.indices.data() + src_row, copy_w, indices_.data() + dst_row);
  }
}

void IndexedSurface8::PresentToBack(Surface32& destination) const {
  if (width_ <= 0 || height_ <= 0 || destination.width() <= 0 || destination.height() <= 0) {
    return;
  }
  const int copy_w = std::min(width_, destination.width());
  const int copy_h = std::min(height_, destination.height());
  uint32_t* dst = destination.BackPixelsMutable();
  for (int y = 0; y < copy_h; ++y) {
    const size_t src_row = static_cast<size_t>(y) * static_cast<size_t>(width_);
    const size_t dst_row = static_cast<size_t>(y) * static_cast<size_t>(destination.width());
    for (int x = 0; x < copy_w; ++x) {
      const uint8_t idx = indices_[src_row + static_cast<size_t>(x)];
      dst[dst_row + static_cast<size_t>(x)] = palette_argb_[idx];
    }
  }
}

}  // namespace forward::core

