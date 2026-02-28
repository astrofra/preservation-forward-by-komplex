#pragma once

#include <cstddef>
#include <cstdint>

namespace forward::core::legacy10 {

constexpr uint32_t kCarryMask = 0x10040100u;
constexpr uint32_t kPackMask = 0x0FF3FCFFu;
constexpr uint32_t kHalfMask = 0x07E1F87Eu;
constexpr uint32_t kRunMask = 0x01F07C1Fu;

uint32_t PackRgb8To10(uint8_t r, uint8_t g, uint8_t b);
uint32_t PackColor24To10(uint32_t rgb24);
uint32_t Unpack10ToArgb(uint32_t packed10);

uint32_t AddSaturating(uint32_t a, uint32_t b);
uint32_t SubSaturating(uint32_t a, uint32_t b);

void AddConstant(uint32_t* pixels, size_t count, uint32_t rgb24);
void SubConstant(uint32_t* pixels, size_t count, uint32_t rgb24);

void ShiftChannelsRight(uint32_t* pixels, size_t count, int shift);
void AverageNoSaturation(uint32_t* dst, const uint32_t* src, size_t count);
void AddHalfSaturating(uint32_t* dst, const uint32_t* src, size_t count);

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
                  int h);

void AdditiveBlitScaled(const uint32_t* src_pixels,
                        int src_width,
                        int src_height,
                        uint32_t* dst_pixels,
                        int dst_width,
                        int dst_height,
                        int dst_x,
                        int dst_y,
                        int dst_w,
                        int dst_h);

void HorizontalFeedbackBlur(uint32_t* pixels, int width, int height, float blend);

void ConvertBufferToArgb(const uint32_t* packed10, uint32_t* argb, size_t count);

}  // namespace forward::core::legacy10

