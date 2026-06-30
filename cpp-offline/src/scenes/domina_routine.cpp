#include "scenes/domina_routine.h"

#include <algorithm>
#include <cmath>

namespace forward_offline {

DominaRoutine::DominaRoutine()
    : source_(512, 512),
      frame_(512, 256),
      fade_to_black_(false),
      fade_start_seconds_(0.0f) {
}

const char* DominaRoutine::script_name() const {
    return "domina";
}

void DominaRoutine::init() {
    build_source();
    fade_to_black_ = false;
    fade_start_seconds_ = 0.0f;
}

void DominaRoutine::on_show() {
    fade_to_black_ = false;
    fade_start_seconds_ = 0.0f;
}

void DominaRoutine::render(RgbSurface& surface, float scene_time_seconds, float delta_seconds) {
    (void)delta_seconds;

    const int scroll = static_cast<int>(std::max(0.0f, scene_time_seconds) * 50.0f);
    frame_.blit_wrapped_y(source_, -scroll);
    build_palette(scene_time_seconds);
    frame_.render_to_rgb(surface);
}

void DominaRoutine::handle_message(const std::string& message, float scene_time_seconds) {
    if (message == "fade2black") {
        fade_to_black_ = true;
        fade_start_seconds_ = scene_time_seconds;
    }
}

void DominaRoutine::build_source() {
    for (int index = 0; index < 256; ++index) {
        const int red = 50 + (index * 3) / 4;
        const int green = 20 + index / 2;
        const int blue = 70 + index / 3;
        source_.set_palette_entry(index,
                                  static_cast<std::uint8_t>(red),
                                  static_cast<std::uint8_t>(green),
                                  static_cast<std::uint8_t>(blue));
        frame_.set_palette_entry(index,
                                 static_cast<std::uint8_t>(red),
                                 static_cast<std::uint8_t>(green),
                                 static_cast<std::uint8_t>(blue));
    }

    for (int y = 0; y < source_.height(); ++y) {
        for (int x = 0; x < source_.width(); ++x) {
            const float fx = static_cast<float>(x) * 0.014f;
            const float fy = static_cast<float>(y) * 0.023f;
            const float bands = 0.5f + 0.5f * std::sin(fy * 4.0f);
            const float wave = 0.5f + 0.5f * std::sin(fx + fy);
            const int value = std::max(0, std::min(255,
                                                   32 + static_cast<int>(bands * 120.0f) +
                                                       static_cast<int>(wave * 100.0f)));
            source_.set_pixel(x, y, static_cast<std::uint8_t>(value));
        }
    }
}

void DominaRoutine::build_palette(float scene_time_seconds) {
    float blend = (scene_time_seconds - 0.2f) / 8.0f;
    if (fade_to_black_) {
        blend = 1.0f - (scene_time_seconds - fade_start_seconds_) * 0.1f;
    }
    if (blend < 0.0f) {
        blend = 0.0f;
    }
    if (blend > 1.0f) {
        blend = 1.0f;
    }

    for (int index = 0; index < 256; ++index) {
        const std::uint32_t base = source_.palette_rgb(index);
        const int base_red = static_cast<int>((base >> 16) & 0xff);
        const int base_green = static_cast<int>((base >> 8) & 0xff);
        const int base_blue = static_cast<int>(base & 0xff);

        const int target_red = fade_to_black_ ? 0 : 255;
        const int target_green = fade_to_black_ ? 0 : 255;
        const int target_blue = fade_to_black_ ? 0 : 255;

        const int red = static_cast<int>((1.0f - blend) * static_cast<float>(target_red) +
                                         blend * static_cast<float>(base_red));
        const int green = static_cast<int>((1.0f - blend) * static_cast<float>(target_green) +
                                           blend * static_cast<float>(base_green));
        const int blue = static_cast<int>((1.0f - blend) * static_cast<float>(target_blue) +
                                          blend * static_cast<float>(base_blue));

        frame_.set_palette_entry(index,
                                 static_cast<std::uint8_t>(red),
                                 static_cast<std::uint8_t>(green),
                                 static_cast<std::uint8_t>(blue));
    }
}

std::uint32_t DominaRoutine::pack_rgb(int red, int green, int blue) {
    const int r = std::max(0, std::min(255, red));
    const int g = std::max(0, std::min(255, green));
    const int b = std::max(0, std::min(255, blue));
    return static_cast<std::uint32_t>((r << 16) | (g << 8) | b);
}

}  // namespace forward_offline
