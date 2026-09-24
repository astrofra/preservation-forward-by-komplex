#include "scenes/domina_routine.h"

#include <algorithm>
#include <cstring>

namespace forward_offline {

DominaRoutine::DominaRoutine()
    : source_asset_(),
      frame_(512, 256),
      fade_to_black_(false),
      fade_start_seconds_(0.0f),
      ready_(false),
      error_message_() {
}

const char* DominaRoutine::script_name() const {
    return "domina";
}

void DominaRoutine::init() {
    fade_to_black_ = false;
    fade_start_seconds_ = 0.0f;
    ready_ = load_assets();
}

void DominaRoutine::on_show() {
    fade_to_black_ = false;
    fade_start_seconds_ = 0.0f;
}

void DominaRoutine::render(RgbSurface& surface, float scene_time_seconds, float delta_seconds) {
    (void)delta_seconds;
    surface.clear(0);
    if (!ready_) {
        return;
    }

    const int frame_index = static_cast<int>(std::max(0.0f, scene_time_seconds) * 50.0f);
    populate_frame(frame_index);
    build_palette(scene_time_seconds);
    frame_.render_to_rgb(surface);
}

void DominaRoutine::handle_message(const std::string& message, float scene_time_seconds) {
    if (message == "fade2black") {
        fade_to_black_ = true;
        fade_start_seconds_ = scene_time_seconds;
    }
}

bool DominaRoutine::is_ready() const {
    return ready_;
}

const std::string& DominaRoutine::error_message() const {
    return error_message_;
}

bool DominaRoutine::load_assets() {
    error_message_.clear();
    if (!load_original_gif_indexed(gif_asset_path("phorward.gif"), &source_asset_, &error_message_)) {
        return false;
    }

    if (source_asset_.width != 512 || source_asset_.height < 256) {
        error_message_ = "unexpected domina gif dimensions: " + gif_asset_path("phorward.gif");
        return false;
    }

    return true;
}

void DominaRoutine::populate_frame(int frame_index) {
    std::vector<std::uint8_t>& frame_pixels = frame_.pixels();
    const std::vector<std::uint8_t>& source_pixels = source_asset_.pixels;
    const int source_width = source_asset_.width;
    const int source_height = source_asset_.height;
    const int frame_height = frame_.height();
    const int source_y = (frame_index * frame_height) % source_height;

    for (int row = 0; row < frame_height; ++row) {
        const std::size_t src_offset =
            static_cast<std::size_t>(source_y + row) * static_cast<std::size_t>(source_width);
        const std::size_t dst_offset =
            static_cast<std::size_t>(row) * static_cast<std::size_t>(frame_.width());
        std::memcpy(&frame_pixels[dst_offset],
                    &source_pixels[src_offset],
                    static_cast<std::size_t>(frame_.width()));
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
        const int base_red = static_cast<int>(source_asset_.palette_red[static_cast<std::size_t>(index)]);
        const int base_green = static_cast<int>(source_asset_.palette_green[static_cast<std::size_t>(index)]);
        const int base_blue = static_cast<int>(source_asset_.palette_blue[static_cast<std::size_t>(index)]);

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

std::string DominaRoutine::gif_asset_path(const std::string& file_name) const {
    return std::string("original/forward/images/") + file_name;
}

}  // namespace forward_offline
