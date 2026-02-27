#pragma once

#include <cstdint>
#include <vector>

namespace forward::core {

class Surface32 {
 public:
  Surface32(int width, int height, bool double_buffered);

  int width() const { return width_; }
  int height() const { return height_; }

  void ClearBack(uint32_t argb);
  void ClearFront(uint32_t argb);
  void SetBackPixel(int x, int y, uint32_t argb);

  void AddBackRgb(uint8_t add_r, uint8_t add_g, uint8_t add_b);
  void SubBackRgb(uint8_t sub_r, uint8_t sub_g, uint8_t sub_b);

  void BlitToBack(const Surface32& src,
                  int src_x,
                  int src_y,
                  int dst_x,
                  int dst_y,
                  int w,
                  int h);
  void AlphaBlitToBack(const uint32_t* src_pixels,
                       int src_width,
                       int src_height,
                       int src_x,
                       int src_y,
                       int dst_x,
                       int dst_y,
                       int w,
                       int h,
                       uint8_t global_alpha);
  void AdditiveBlitToBack(const uint32_t* src_pixels,
                          int src_width,
                          int src_height,
                          int src_x,
                          int src_y,
                          int dst_x,
                          int dst_y,
                          int w,
                          int h,
                          uint8_t intensity);
  void AdditiveBlitScaledToBack(const uint32_t* src_pixels,
                                int src_width,
                                int src_height,
                                int dst_x,
                                int dst_y,
                                int dst_w,
                                int dst_h,
                                uint8_t intensity);

  void SwapBuffers();

  const uint32_t* FrontPixels() const { return front_.data(); }
  const uint32_t* BackPixels() const { return back_.data(); }
  uint32_t* BackPixelsMutable() { return back_.data(); }

 private:
  int width_ = 0;
  int height_ = 0;
  bool double_buffered_ = true;
  std::vector<uint32_t> front_;
  std::vector<uint32_t> back_;
};

}  // namespace forward::core
