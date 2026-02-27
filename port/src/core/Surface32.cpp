#include "Surface32.h"

#include <algorithm>

namespace forward::core {
namespace {

uint8_t ChannelR(uint32_t argb) { return static_cast<uint8_t>((argb >> 16u) & 0xFFu); }
uint8_t ChannelG(uint32_t argb) { return static_cast<uint8_t>((argb >> 8u) & 0xFFu); }
uint8_t ChannelB(uint32_t argb) { return static_cast<uint8_t>(argb & 0xFFu); }
uint8_t ChannelA(uint32_t argb) { return static_cast<uint8_t>((argb >> 24u) & 0xFFu); }

uint32_t PackArgb(uint8_t r, uint8_t g, uint8_t b) {
  return (0xFFu << 24u) | (static_cast<uint32_t>(r) << 16u) |
         (static_cast<uint32_t>(g) << 8u) | static_cast<uint32_t>(b);
}

int ClampToByte(int value) {
  return std::max(0, std::min(255, value));
}

}  // namespace

Surface32::Surface32(int width, int height, bool double_buffered)
    : width_(width),
      height_(height),
      double_buffered_(double_buffered),
      front_(static_cast<size_t>(width) * static_cast<size_t>(height), 0xFF000000u),
      back_(static_cast<size_t>(width) * static_cast<size_t>(height), 0xFF000000u) {}

void Surface32::ClearBack(uint32_t argb) {
  std::fill(back_.begin(), back_.end(), argb);
}

void Surface32::ClearFront(uint32_t argb) {
  std::fill(front_.begin(), front_.end(), argb);
}

void Surface32::SetBackPixel(int x, int y, uint32_t argb) {
  if (x < 0 || y < 0 || x >= width_ || y >= height_) {
    return;
  }
  back_[static_cast<size_t>(y) * static_cast<size_t>(width_) + static_cast<size_t>(x)] = argb;
}

void Surface32::AddBackRgb(uint8_t add_r, uint8_t add_g, uint8_t add_b) {
  for (uint32_t& pixel : back_) {
    const int r = ClampToByte(static_cast<int>(ChannelR(pixel)) + add_r);
    const int g = ClampToByte(static_cast<int>(ChannelG(pixel)) + add_g);
    const int b = ClampToByte(static_cast<int>(ChannelB(pixel)) + add_b);
    pixel = PackArgb(static_cast<uint8_t>(r), static_cast<uint8_t>(g),
                     static_cast<uint8_t>(b));
  }
}

void Surface32::SubBackRgb(uint8_t sub_r, uint8_t sub_g, uint8_t sub_b) {
  for (uint32_t& pixel : back_) {
    const int r = ClampToByte(static_cast<int>(ChannelR(pixel)) - sub_r);
    const int g = ClampToByte(static_cast<int>(ChannelG(pixel)) - sub_g);
    const int b = ClampToByte(static_cast<int>(ChannelB(pixel)) - sub_b);
    pixel = PackArgb(static_cast<uint8_t>(r), static_cast<uint8_t>(g),
                     static_cast<uint8_t>(b));
  }
}

void Surface32::BlitToBack(const Surface32& src,
                           int src_x,
                           int src_y,
                           int dst_x,
                           int dst_y,
                           int w,
                           int h) {
  if (w <= 0 || h <= 0) {
    return;
  }

  int copy_src_x = src_x;
  int copy_src_y = src_y;
  int copy_dst_x = dst_x;
  int copy_dst_y = dst_y;
  int copy_w = w;
  int copy_h = h;

  if (copy_src_x < 0) {
    copy_w += copy_src_x;
    copy_dst_x -= copy_src_x;
    copy_src_x = 0;
  }
  if (copy_src_y < 0) {
    copy_h += copy_src_y;
    copy_dst_y -= copy_src_y;
    copy_src_y = 0;
  }
  if (copy_dst_x < 0) {
    copy_w += copy_dst_x;
    copy_src_x -= copy_dst_x;
    copy_dst_x = 0;
  }
  if (copy_dst_y < 0) {
    copy_h += copy_dst_y;
    copy_src_y -= copy_dst_y;
    copy_dst_y = 0;
  }

  copy_w = std::min(copy_w, src.width_ - copy_src_x);
  copy_h = std::min(copy_h, src.height_ - copy_src_y);
  copy_w = std::min(copy_w, width_ - copy_dst_x);
  copy_h = std::min(copy_h, height_ - copy_dst_y);

  if (copy_w <= 0 || copy_h <= 0) {
    return;
  }

  for (int row = 0; row < copy_h; ++row) {
    const uint32_t* src_row =
        src.front_.data() + static_cast<size_t>(copy_src_y + row) * src.width_ + copy_src_x;
    uint32_t* dst_row =
        back_.data() + static_cast<size_t>(copy_dst_y + row) * width_ + copy_dst_x;
    std::copy_n(src_row, copy_w, dst_row);
  }
}

void Surface32::AlphaBlitToBack(const uint32_t* src_pixels,
                                int src_width,
                                int src_height,
                                int src_x,
                                int src_y,
                                int dst_x,
                                int dst_y,
                                int w,
                                int h,
                                uint8_t global_alpha) {
  if (!src_pixels || src_width <= 0 || src_height <= 0 || w <= 0 || h <= 0 ||
      global_alpha == 0) {
    return;
  }

  int copy_src_x = src_x;
  int copy_src_y = src_y;
  int copy_dst_x = dst_x;
  int copy_dst_y = dst_y;
  int copy_w = w;
  int copy_h = h;

  if (copy_src_x < 0) {
    copy_w += copy_src_x;
    copy_dst_x -= copy_src_x;
    copy_src_x = 0;
  }
  if (copy_src_y < 0) {
    copy_h += copy_src_y;
    copy_dst_y -= copy_src_y;
    copy_src_y = 0;
  }
  if (copy_dst_x < 0) {
    copy_w += copy_dst_x;
    copy_src_x -= copy_dst_x;
    copy_dst_x = 0;
  }
  if (copy_dst_y < 0) {
    copy_h += copy_dst_y;
    copy_src_y -= copy_dst_y;
    copy_dst_y = 0;
  }

  copy_w = std::min(copy_w, src_width - copy_src_x);
  copy_h = std::min(copy_h, src_height - copy_src_y);
  copy_w = std::min(copy_w, width_ - copy_dst_x);
  copy_h = std::min(copy_h, height_ - copy_dst_y);

  if (copy_w <= 0 || copy_h <= 0) {
    return;
  }

  for (int row = 0; row < copy_h; ++row) {
    const uint32_t* src_row = src_pixels +
                              static_cast<size_t>(copy_src_y + row) * src_width + copy_src_x;
    uint32_t* dst_row =
        back_.data() + static_cast<size_t>(copy_dst_y + row) * width_ + copy_dst_x;
    for (int col = 0; col < copy_w; ++col) {
      const uint32_t src = src_row[col];
      const uint32_t dst = dst_row[col];

      const int src_a = (static_cast<int>(ChannelA(src)) * global_alpha) / 255;
      if (src_a <= 0) {
        continue;
      }
      if (src_a >= 255) {
        dst_row[col] = (0xFFu << 24u) | (src & 0x00FFFFFFu);
        continue;
      }

      const int inv_a = 255 - src_a;
      const int r = (static_cast<int>(ChannelR(src)) * src_a +
                     static_cast<int>(ChannelR(dst)) * inv_a) /
                    255;
      const int g = (static_cast<int>(ChannelG(src)) * src_a +
                     static_cast<int>(ChannelG(dst)) * inv_a) /
                    255;
      const int b = (static_cast<int>(ChannelB(src)) * src_a +
                     static_cast<int>(ChannelB(dst)) * inv_a) /
                    255;
      dst_row[col] = PackArgb(static_cast<uint8_t>(r), static_cast<uint8_t>(g),
                              static_cast<uint8_t>(b));
    }
  }
}

void Surface32::AdditiveBlitToBack(const uint32_t* src_pixels,
                                   int src_width,
                                   int src_height,
                                   int src_x,
                                   int src_y,
                                   int dst_x,
                                   int dst_y,
                                   int w,
                                   int h,
                                   uint8_t intensity) {
  if (!src_pixels || src_width <= 0 || src_height <= 0 || w <= 0 || h <= 0 || intensity == 0) {
    return;
  }

  int copy_src_x = src_x;
  int copy_src_y = src_y;
  int copy_dst_x = dst_x;
  int copy_dst_y = dst_y;
  int copy_w = w;
  int copy_h = h;

  if (copy_src_x < 0) {
    copy_w += copy_src_x;
    copy_dst_x -= copy_src_x;
    copy_src_x = 0;
  }
  if (copy_src_y < 0) {
    copy_h += copy_src_y;
    copy_dst_y -= copy_src_y;
    copy_src_y = 0;
  }
  if (copy_dst_x < 0) {
    copy_w += copy_dst_x;
    copy_src_x -= copy_dst_x;
    copy_dst_x = 0;
  }
  if (copy_dst_y < 0) {
    copy_h += copy_dst_y;
    copy_src_y -= copy_dst_y;
    copy_dst_y = 0;
  }

  copy_w = std::min(copy_w, src_width - copy_src_x);
  copy_h = std::min(copy_h, src_height - copy_src_y);
  copy_w = std::min(copy_w, width_ - copy_dst_x);
  copy_h = std::min(copy_h, height_ - copy_dst_y);

  if (copy_w <= 0 || copy_h <= 0) {
    return;
  }

  for (int row = 0; row < copy_h; ++row) {
    const uint32_t* src_row = src_pixels +
                              static_cast<size_t>(copy_src_y + row) * src_width + copy_src_x;
    uint32_t* dst_row =
        back_.data() + static_cast<size_t>(copy_dst_y + row) * width_ + copy_dst_x;
    for (int col = 0; col < copy_w; ++col) {
      const uint32_t src = src_row[col];
      const uint32_t dst = dst_row[col];

      const int r = ClampToByte(static_cast<int>(ChannelR(dst)) +
                                (static_cast<int>(ChannelR(src)) * intensity) / 255);
      const int g = ClampToByte(static_cast<int>(ChannelG(dst)) +
                                (static_cast<int>(ChannelG(src)) * intensity) / 255);
      const int b = ClampToByte(static_cast<int>(ChannelB(dst)) +
                                (static_cast<int>(ChannelB(src)) * intensity) / 255);
      dst_row[col] = PackArgb(static_cast<uint8_t>(r), static_cast<uint8_t>(g),
                              static_cast<uint8_t>(b));
    }
  }
}

void Surface32::SwapBuffers() {
  if (double_buffered_) {
    std::swap(front_, back_);
  } else {
    front_ = back_;
  }
}

}  // namespace forward::core
