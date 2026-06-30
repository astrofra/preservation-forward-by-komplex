#include "render/indexed_surface.h"

#include "render/rgb_surface.h"

namespace forward_offline {

IndexedSurface::IndexedSurface(int width, int height)
    : width_(width),
      height_(height),
      pixels_(static_cast<std::size_t>(width) * static_cast<std::size_t>(height), 0),
      palette_rgb_(256, 0) {
    for (int index = 0; index < 256; ++index) {
        const std::uint8_t red = static_cast<std::uint8_t>((index * 3) / 2);
        const std::uint8_t green = static_cast<std::uint8_t>((index * 5) / 4);
        const std::uint8_t blue = static_cast<std::uint8_t>(index);
        set_palette_entry(index, red, green, blue);
    }
}

int IndexedSurface::width() const {
    return width_;
}

int IndexedSurface::height() const {
    return height_;
}

std::vector<std::uint8_t>& IndexedSurface::pixels() {
    return pixels_;
}

const std::vector<std::uint8_t>& IndexedSurface::pixels() const {
    return pixels_;
}

void IndexedSurface::clear(std::uint8_t index) {
    for (std::size_t pixel_index = 0; pixel_index < pixels_.size(); ++pixel_index) {
        pixels_[pixel_index] = index;
    }
}

void IndexedSurface::set_pixel(int x, int y, std::uint8_t index) {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) {
        return;
    }

    pixels_[static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) +
            static_cast<std::size_t>(x)] = index;
}

std::uint8_t IndexedSurface::pixel_at(int x, int y) const {
    if (width_ <= 0 || height_ <= 0) {
        return 0;
    }

    while (x < 0) {
        x += width_;
    }
    while (y < 0) {
        y += height_;
    }

    x %= width_;
    y %= height_;
    return pixels_[static_cast<std::size_t>(y) * static_cast<std::size_t>(width_) +
                   static_cast<std::size_t>(x)];
}

void IndexedSurface::set_palette_entry(int index, std::uint8_t red, std::uint8_t green, std::uint8_t blue) {
    if (index < 0 || index >= static_cast<int>(palette_rgb_.size())) {
        return;
    }

    palette_rgb_[static_cast<std::size_t>(index)] = pack_rgb(red, green, blue);
}

std::uint32_t IndexedSurface::palette_rgb(int index) const {
    if (index < 0 || index >= static_cast<int>(palette_rgb_.size())) {
        return 0;
    }
    return palette_rgb_[static_cast<std::size_t>(index)];
}

void IndexedSurface::blit_wrapped_y(const IndexedSurface& source, int y_offset) {
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            set_pixel(x, y, source.pixel_at(x, y + y_offset));
        }
    }
}

void IndexedSurface::render_to_rgb(RgbSurface& target) const {
    std::vector<std::uint32_t>& target_pixels = target.pixels();
    const std::size_t count = pixels_.size() < target_pixels.size() ? pixels_.size() : target_pixels.size();
    for (std::size_t index = 0; index < count; ++index) {
        target_pixels[index] = palette_rgb_[pixels_[index]];
    }
}

std::uint32_t IndexedSurface::pack_rgb(std::uint8_t red, std::uint8_t green, std::uint8_t blue) {
    return static_cast<std::uint32_t>((static_cast<unsigned int>(red) << 16) |
                                      (static_cast<unsigned int>(green) << 8) |
                                      static_cast<unsigned int>(blue));
}

}  // namespace forward_offline
