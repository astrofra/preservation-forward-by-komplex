#ifndef FORWARD_OFFLINE_RENDER_INDEXED_SURFACE_H
#define FORWARD_OFFLINE_RENDER_INDEXED_SURFACE_H

#include <cstdint>
#include <vector>

namespace forward_offline {

class RgbSurface;

class IndexedSurface {
public:
    IndexedSurface(int width, int height);

    int width() const;
    int height() const;

    std::vector<std::uint8_t>& pixels();
    const std::vector<std::uint8_t>& pixels() const;

    void clear(std::uint8_t index);
    void set_pixel(int x, int y, std::uint8_t index);
    std::uint8_t pixel_at(int x, int y) const;

    void set_palette_entry(int index, std::uint8_t red, std::uint8_t green, std::uint8_t blue);
    std::uint32_t palette_rgb(int index) const;
    void blit_wrapped_y(const IndexedSurface& source, int y_offset);
    void render_to_rgb(RgbSurface& target) const;

private:
    static std::uint32_t pack_rgb(std::uint8_t red, std::uint8_t green, std::uint8_t blue);

    int width_;
    int height_;
    std::vector<std::uint8_t> pixels_;
    std::vector<std::uint32_t> palette_rgb_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_RENDER_INDEXED_SURFACE_H
