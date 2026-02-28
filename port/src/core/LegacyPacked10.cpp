#include "LegacyPacked10.h"

#include <algorithm>

namespace forward::core::legacy10 {
namespace {

int ClampInt(int v, int lo, int hi) { return std::max(lo, std::min(v, hi)); }

}  // namespace

uint32_t PackRgb8To10(uint8_t r, uint8_t g, uint8_t b) {
  return (static_cast<uint32_t>(r) << 20u) | (static_cast<uint32_t>(g) << 10u) |
         static_cast<uint32_t>(b);
}

uint32_t PackColor24To10(uint32_t rgb24) {
  const uint8_t r = static_cast<uint8_t>((rgb24 >> 16u) & 0xFFu);
  const uint8_t g = static_cast<uint8_t>((rgb24 >> 8u) & 0xFFu);
  const uint8_t b = static_cast<uint8_t>(rgb24 & 0xFFu);
  return PackRgb8To10(r, g, b);
}

uint32_t Unpack10ToArgb(uint32_t packed10) {
  const uint8_t r = static_cast<uint8_t>((packed10 >> 20u) & 0xFFu);
  const uint8_t g = static_cast<uint8_t>((packed10 >> 10u) & 0xFFu);
  const uint8_t b = static_cast<uint8_t>(packed10 & 0xFFu);
  return (0xFFu << 24u) | (static_cast<uint32_t>(r) << 16u) |
         (static_cast<uint32_t>(g) << 8u) | static_cast<uint32_t>(b);
}

uint32_t AddSaturating(uint32_t a, uint32_t b) {
  const uint32_t sum = a + b;
  const uint32_t carry = sum & kCarryMask;
  return (sum - carry) | (carry - (carry >> 8u));
}

uint32_t SubSaturating(uint32_t a, uint32_t b) {
  const uint32_t diff = a + kCarryMask - b;
  const uint32_t carry = diff & kCarryMask;
  return diff & (carry - (carry >> 8u));
}

void AddConstant(uint32_t* pixels, size_t count, uint32_t rgb24) {
  if (!pixels) {
    return;
  }
  const uint32_t c = PackColor24To10(rgb24);
  for (size_t i = 0; i < count; ++i) {
    pixels[i] = AddSaturating(pixels[i], c);
  }
}

void SubConstant(uint32_t* pixels, size_t count, uint32_t rgb24) {
  if (!pixels) {
    return;
  }
  const uint32_t c = PackColor24To10(rgb24);
  for (size_t i = 0; i < count; ++i) {
    pixels[i] = SubSaturating(pixels[i], c);
  }
}

void ShiftChannelsRight(uint32_t* pixels, size_t count, int shift) {
  if (!pixels || shift <= 0) {
    return;
  }
  shift = ClampInt(shift, 0, 8);
  if (shift == 0) {
    return;
  }
  const uint32_t keep = 255u - ((1u << static_cast<uint32_t>(shift)) - 1u);
  const uint32_t mask = keep | (keep << 10u) | (keep << 20u);
  for (size_t i = 0; i < count; ++i) {
    pixels[i] = (pixels[i] & mask) >> static_cast<uint32_t>(shift);
  }
}

void AverageNoSaturation(uint32_t* dst, const uint32_t* src, size_t count) {
  if (!dst || !src) {
    return;
  }
  for (size_t i = 0; i < count; ++i) {
    dst[i] = ((dst[i] + src[i]) >> 1u) & kPackMask;
  }
}

void AddHalfSaturating(uint32_t* dst, const uint32_t* src, size_t count) {
  if (!dst || !src) {
    return;
  }
  for (size_t i = 0; i < count; ++i) {
    const uint32_t sum = dst[i] + ((src[i] >> 1u) & kPackMask);
    const uint32_t carry = sum & kCarryMask;
    dst[i] = (sum - carry) | (carry - (carry >> 8u));
  }
}

void AdditiveBlit(const uint32_t* src_pixels,
                  int src_width,
                  int src_height,
                  int src_x,
                  int src_y,
                  uint32_t* dst_pixels,
                  int dst_width,
                  int dst_height,
                  int dst_x,
                  int dst_y,
                  int w,
                  int h) {
  if (!src_pixels || !dst_pixels || src_width <= 0 || src_height <= 0 || dst_width <= 0 ||
      dst_height <= 0 || w <= 0 || h <= 0) {
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
  copy_w = std::min(copy_w, dst_width - copy_dst_x);
  copy_h = std::min(copy_h, dst_height - copy_dst_y);
  if (copy_w <= 0 || copy_h <= 0) {
    return;
  }

  int src_row_index = copy_src_y * src_width + copy_src_x;
  int dst_row_index = copy_dst_y * dst_width + copy_dst_x;
  for (int y = 0; y < copy_h; ++y) {
    int src_i = src_row_index;
    int dst_i = dst_row_index;
    for (int x = 0; x < copy_w; ++x) {
      dst_pixels[dst_i] = AddSaturating(dst_pixels[dst_i], src_pixels[src_i]);
      ++src_i;
      ++dst_i;
    }
    src_row_index += src_width;
    dst_row_index += dst_width;
  }
}

void AdditiveBlitScaled(const uint32_t* src_pixels,
                        int src_width,
                        int src_height,
                        uint32_t* dst_pixels,
                        int dst_width,
                        int dst_height,
                        int dst_x,
                        int dst_y,
                        int dst_w,
                        int dst_h) {
  if (!src_pixels || !dst_pixels || src_width <= 0 || src_height <= 0 || dst_width <= 0 ||
      dst_height <= 0) {
    return;
  }

  int n = dst_x;
  int n2 = dst_y;
  int n3 = dst_w;
  int n4 = dst_h;
  int src_off_x = 0;
  int src_off_y = 0;

  if (n < 0) {
    n3 += n;
    src_off_x = -n;
    n = 0;
  }
  if (n3 <= 0) {
    return;
  }
  if (n + n3 > dst_width) {
    n3 = dst_width - n;
  }
  if (n3 <= 0) {
    return;
  }

  if (n2 < 0) {
    n4 += n2;
    src_off_y = -n2;
    n2 = 0;
  }
  if (n4 <= 0) {
    return;
  }
  if (n2 + n4 > dst_height) {
    n4 = dst_height - n2;
  }
  if (n4 <= 0) {
    return;
  }

  int dst_row_start = n2 * dst_width + n;
  const int step_x = (1024 * src_width) / dst_w;
  const int step_y = (1024 * src_height) / dst_h;
  const int base_x = step_x * src_off_x;
  int y_fp = step_y * src_off_y;

  for (int y = 0; y < n4; ++y) {
    int dst_i = dst_row_start;
    int x_fp = base_x + (y_fp & 0xFFFFFC00) * src_width;
    for (int x = 0; x < n3; ++x) {
      dst_pixels[dst_i] = AddSaturating(dst_pixels[dst_i], src_pixels[x_fp >> 10]);
      ++dst_i;
      x_fp += step_x;
    }
    dst_row_start += dst_width;
    y_fp += step_y;
  }
}

void HorizontalFeedbackBlur(uint32_t* pixels, int width, int height, float blend) {
  if (!pixels || width <= 0 || height <= 0) {
    return;
  }
  const int n = ClampInt(static_cast<int>(31.0f * blend), 0, 31);
  const int n2 = 32 - n;

  size_t idx = 0;
  for (int y = 0; y < height; ++y) {
    uint32_t run = (pixels[idx] >> 1u) & kHalfMask;
    for (int x = 0; x < width; ++x) {
      const uint32_t src = pixels[idx];
      run = (((run >> 3u) & kRunMask) * static_cast<uint32_t>(n) +
             ((src >> 3u) & kRunMask) * static_cast<uint32_t>(n2)) >>
            2u;
      pixels[idx] = run & kPackMask;
      ++idx;
    }
  }
}

void ConvertBufferToArgb(const uint32_t* packed10, uint32_t* argb, size_t count) {
  if (!packed10 || !argb) {
    return;
  }
  for (size_t i = 0; i < count; ++i) {
    argb[i] = Unpack10ToArgb(packed10[i]);
  }
}

}  // namespace forward::core::legacy10

