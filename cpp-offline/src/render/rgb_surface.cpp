#include "render/rgb_surface.h"

#include <algorithm>

namespace forward_offline {

RgbSurface::RgbSurface(int width, int height)
    : width_(width),
      height_(height),
      pixels_(static_cast<std::size_t>(width) * static_cast<std::size_t>(height), 0) {
}

int RgbSurface::width() const {
    return width_;
}

int RgbSurface::height() const {
    return height_;
}

std::vector<std::uint32_t>& RgbSurface::pixels() {
    return pixels_;
}

const std::vector<std::uint32_t>& RgbSurface::pixels() const {
    return pixels_;
}

void RgbSurface::clear(std::uint32_t rgb) {
    std::fill(pixels_.begin(), pixels_.end(), rgb);
}

void RgbSurface::set_pixel(int x, int y, std::uint32_t rgb) {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) {
        return;
    }

    pixels_[static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) +
            static_cast<std::size_t>(x)] = rgb;
}

void RgbSurface::fill_rect(int x, int y, int width, int height, std::uint32_t rgb) {
    const int start_x = std::max(0, x);
    const int start_y = std::max(0, y);
    const int end_x = std::min(width_, x + width);
    const int end_y = std::min(height_, y + height);

    for (int row = start_y; row < end_y; ++row) {
        for (int column = start_x; column < end_x; ++column) {
            set_pixel(column, row, rgb);
        }
    }
}

}  // namespace forward_offline
