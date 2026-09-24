#ifndef FORWARD_OFFLINE_RENDER_RGB_SURFACE_H
#define FORWARD_OFFLINE_RENDER_RGB_SURFACE_H

#include <cstdint>
#include <vector>

namespace forward_offline {

class RgbSurface {
public:
    RgbSurface(int width, int height);

    int width() const;
    int height() const;

    std::vector<std::uint32_t>& pixels();
    const std::vector<std::uint32_t>& pixels() const;

    void clear(std::uint32_t rgb);
    void set_pixel(int x, int y, std::uint32_t rgb);
    void fill_rect(int x, int y, int width, int height, std::uint32_t rgb);

private:
    int width_;
    int height_;
    std::vector<std::uint32_t> pixels_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_RENDER_RGB_SURFACE_H
