#include "scenes/placeholder_scene.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace forward_offline {

namespace {

std::uint32_t pack_rgb(int red, int green, int blue) {
    const int r = std::max(0, std::min(255, red));
    const int g = std::max(0, std::min(255, green));
    const int b = std::max(0, std::min(255, blue));
    return static_cast<std::uint32_t>((r << 16) | (g << 8) | b);
}

float clamp_unit(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }
    return value;
}

}  // namespace

PlaceholderScene::PlaceholderScene() : phase_(0.0f) {
}

const char* PlaceholderScene::script_name() const {
    return "bootstrap";
}

void PlaceholderScene::on_show() {
    phase_ = 0.0f;
}

void PlaceholderScene::render(RgbSurface& surface, float scene_time_seconds, float delta_seconds) {
    phase_ += delta_seconds;

    const int width = surface.width();
    const int height = surface.height();
    std::vector<std::uint32_t>& pixels = surface.pixels();

    const float sweep = std::fmod(scene_time_seconds * 64.0f, static_cast<float>(width + 96)) - 48.0f;

    for (int y = 0; y < height; ++y) {
        const float fy = static_cast<float>(y) / static_cast<float>(height > 1 ? height - 1 : 1);
        for (int x = 0; x < width; ++x) {
            const float fx = static_cast<float>(x) / static_cast<float>(width > 1 ? width - 1 : 1);
            const float glow = clamp_unit(1.0f - std::fabs((static_cast<float>(x) - sweep) / 72.0f));
            const float wave = 0.5f + 0.5f * std::sin((fx * 9.0f) + phase_ * 1.3f);

            int red = 10 + static_cast<int>(70.0f * fx) + static_cast<int>(80.0f * glow);
            int green = 18 + static_cast<int>(110.0f * (1.0f - fy)) + static_cast<int>(45.0f * wave);
            int blue = 30 + static_cast<int>(150.0f * fy);

            if (y > (height * 2) / 3) {
                const int checker = ((x / 16) + (y / 16) + static_cast<int>(scene_time_seconds * 6.0f)) & 1;
                red += checker * 18;
                green += checker * 12;
                blue += checker * 6;
            }

            pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(width) +
                   static_cast<std::size_t>(x)] = pack_rgb(red, green, blue);
        }
    }

    surface.fill_rect(0, height / 2, width, 3, pack_rgb(210, 164, 72));
    surface.fill_rect(0, (height / 2) + 10, width, 1, pack_rgb(255, 220, 124));

    for (int index = 0; index < 3; ++index) {
        const float offset = scene_time_seconds * (0.8f + static_cast<float>(index) * 0.25f);
        const float normalized = 0.5f + 0.5f * std::sin(offset + static_cast<float>(index));
        const int bar_x = static_cast<int>(normalized * static_cast<float>(width - 36));
        const int bar_y = height - 28 - index * 18;
        const std::uint32_t color = index == 0
                                        ? pack_rgb(255, 110, 72)
                                        : (index == 1 ? pack_rgb(80, 220, 180) : pack_rgb(84, 132, 255));
        surface.fill_rect(bar_x, bar_y, 28, 10, color);
    }
}

}  // namespace forward_offline
