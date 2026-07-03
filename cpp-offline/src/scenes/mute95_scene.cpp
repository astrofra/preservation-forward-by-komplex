#include "scenes/mute95_scene.h"

#include <algorithm>

namespace forward_offline {

namespace {

const int kWidth = 512;
const int kHeight = 256;
const int kBlockWidth = 8;
const int kBlockHeight = 8;
const int kCreditWidth = 256;
const int kCreditHeight = 50;
const int kCreditSourceX = 8;
const int kCreditSourceY = 40;
const int kCreditDestX = (kWidth - kCreditWidth) / 2;
const int kCreditDestY = (kHeight - kCreditHeight) / 2;
const float kDesktopFrameDrivenHz = 50.0f;
const int kDesktopNoiseWriteDelta = 70;
const std::uint32_t kColorMask = 0x0F83E0F8U;
const std::uint32_t kCarryMask = 0x10040100U;

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

Mute95Scene::Mute95Scene()
    : palette_red_(256, 0),
      palette_green_(256, 0),
      palette_blue_(256, 0),
      active_pixels_(static_cast<std::size_t>(kWidth) * static_cast<std::size_t>(kHeight), 0),
      passive_pixels_(static_cast<std::size_t>(kWidth) * static_cast<std::size_t>(kHeight), 0),
      credits_(),
      random_(999U),
      active_credit_(-1),
      message_start_seconds_(-1.0f),
      phase_ticks_(0),
      desktop_phase_ticks_(0.0),
      desktop_noise_writes_(0.0),
      horizontal_offsets_(static_cast<std::size_t>(kHeight / kBlockHeight),
                          std::vector<float>(static_cast<std::size_t>(kWidth / kBlockWidth), 0.0f)),
      vertical_offsets_(static_cast<std::size_t>(kHeight / kBlockHeight),
                        std::vector<float>(static_cast<std::size_t>(kWidth / kBlockWidth), 0.0f)),
      ready_(false),
      error_message_() {
}

const char* Mute95Scene::script_name() const {
    return "mute95";
}

void Mute95Scene::init() {
    std::fill(active_pixels_.begin(), active_pixels_.end(), static_cast<std::uint8_t>(0));
    std::fill(passive_pixels_.begin(), passive_pixels_.end(), static_cast<std::uint8_t>(0));
    for (std::size_t y = 0; y < horizontal_offsets_.size(); ++y) {
        std::fill(horizontal_offsets_[y].begin(), horizontal_offsets_[y].end(), 0.0f);
        std::fill(vertical_offsets_[y].begin(), vertical_offsets_[y].end(), 0.0f);
    }
    credits_.clear();
    random_ = JavaRandom(999U);
    desktop_phase_ticks_ = 0.0;
    desktop_noise_writes_ = 0.0;
    phase_ticks_ = 0;
    active_credit_ = -1;
    message_start_seconds_ = -1.0f;
    ready_ = load_assets();
}

void Mute95Scene::on_show() {
    active_credit_ = -1;
    message_start_seconds_ = -1.0f;
}

void Mute95Scene::render(RgbSurface& surface, float scene_time_seconds, float delta_seconds) {
    surface.clear(0);
    if (!ready_) {
        return;
    }

    float scale = delta_seconds * 10.0f;
    if (scale < 0.05f) {
        scale = 0.05f;
    }

    desktop_phase_ticks_ += static_cast<double>(delta_seconds * kDesktopFrameDrivenHz);
    phase_ticks_ = static_cast<int>(desktop_phase_ticks_);
    apply_warp(scale);
    apply_noise(scene_time_seconds, delta_seconds);
    blend_buffers();
    swap_buffers();
    render_indexed_to_rgb(surface);
    render_credit_overlay(surface, scene_time_seconds);
}

void Mute95Scene::handle_message(const std::string& message, float scene_time_seconds) {
    select_credit(message, scene_time_seconds);
}

bool Mute95Scene::is_ready() const {
    return ready_;
}

const std::string& Mute95Scene::error_message() const {
    return error_message_;
}

std::uint32_t Mute95Scene::pack_rgb(std::uint8_t red, std::uint8_t green, std::uint8_t blue) {
    return static_cast<std::uint32_t>((static_cast<unsigned int>(red) << 16) |
                                      (static_cast<unsigned int>(green) << 8) |
                                      static_cast<unsigned int>(blue));
}

std::uint32_t Mute95Scene::packed_to_standard_rgb(std::uint32_t packed) {
    return unpack_original_packed_rgb(packed);
}

std::uint32_t Mute95Scene::standard_to_packed_rgb(std::uint32_t rgb) {
    return static_cast<std::uint32_t>((((rgb >> 16) & 0xffU) << 20) |
                                      (((rgb >> 8) & 0xffU) << 10) |
                                      (rgb & 0xffU));
}

void Mute95Scene::apply_warp(float scale) {
    const int grid_width = kWidth / kBlockWidth;
    const int grid_height = kHeight / kBlockHeight;
    const int center_x = grid_width / 2;
    const int center_y = grid_height / 2;
    const float phase_x = static_cast<float>(phase_ticks_ % 4) * 0.2f;
    const float phase_y = static_cast<float>(phase_ticks_ % 5) * 0.2f;

    for (int block_y = 0; block_y < grid_height; ++block_y) {
        for (int block_x = 0; block_x < grid_width; ++block_x) {
            const float delta_x = static_cast<float>(block_x - center_x) * scale + phase_x;
            const float delta_y = static_cast<float>(block_y - center_y) * scale + phase_y;

            const float previous_horizontal = horizontal_offsets_[static_cast<std::size_t>(block_y)][static_cast<std::size_t>(block_x)];
            const float previous_vertical = vertical_offsets_[static_cast<std::size_t>(block_y)][static_cast<std::size_t>(block_x)];
            const float next_horizontal = previous_horizontal + delta_x;
            const float next_vertical = previous_vertical + delta_y;
            horizontal_offsets_[static_cast<std::size_t>(block_y)][static_cast<std::size_t>(block_x)] = next_horizontal;
            vertical_offsets_[static_cast<std::size_t>(block_y)][static_cast<std::size_t>(block_x)] = next_vertical;

            const int shift_x = static_cast<int>(next_horizontal) - static_cast<int>(previous_horizontal);
            const int shift_y = static_cast<int>(next_vertical) - static_cast<int>(previous_vertical);

            const int dst_x = block_x * kBlockWidth;
            const int dst_y = block_y * kBlockHeight;
            const int src_x = dst_x - shift_x;
            const int src_y = dst_y - shift_y;

            if (src_x < 0 || src_y < 0 || src_x + kBlockWidth > kWidth || src_y + kBlockHeight > kHeight) {
                continue;
            }

            int src_index = src_y * kWidth + src_x;
            int dst_index = dst_y * kWidth + dst_x;
            for (int row = 0; row < kBlockHeight; ++row) {
                for (int column = 0; column < kBlockWidth; ++column) {
                    active_pixels_[static_cast<std::size_t>(dst_index + column)] =
                        passive_pixels_[static_cast<std::size_t>(src_index + column)];
                }
                src_index += kWidth;
                dst_index += kWidth;
            }
        }
    }
}

void Mute95Scene::apply_noise(float scene_time_seconds, float delta_seconds) {
    // This zoom-noise accumulation may still oversaturate the intro compared
    // with the original Java release. We do not currently have a reliable
    // reference capture of that original runtime, so keep this path under
    // review instead of treating the current saturation as final.
    const int max_value =
        static_cast<int>(std::min(scene_time_seconds * 1.8f + 22.0f, 255.0f));
    const int pixel_count = kWidth * kHeight;

    desktop_noise_writes_ += static_cast<double>(220.0f * delta_seconds * kDesktopFrameDrivenHz);
    int writes = static_cast<int>(desktop_noise_writes_);
    desktop_noise_writes_ -= static_cast<double>(writes);

    for (int index = 0; index < writes; ++index) {
        const int pixel_index = static_cast<int>(random_.next_float() * static_cast<float>(pixel_count - 1));
        const int value = static_cast<int>(active_pixels_[static_cast<std::size_t>(pixel_index)]) + kDesktopNoiseWriteDelta;
        active_pixels_[static_cast<std::size_t>(pixel_index)] =
            static_cast<std::uint8_t>(std::min(max_value, value));
    }
}

void Mute95Scene::blend_buffers() {
    for (std::size_t index = 0; index < active_pixels_.size(); ++index) {
        active_pixels_[index] = static_cast<std::uint8_t>(
            (static_cast<int>(passive_pixels_[index]) + static_cast<int>(active_pixels_[index])) >> 1);
    }
}

void Mute95Scene::swap_buffers() {
    active_pixels_.swap(passive_pixels_);
}

void Mute95Scene::render_indexed_to_rgb(RgbSurface& surface) const {
    std::vector<std::uint32_t>& pixels = surface.pixels();
    for (std::size_t index = 0; index < active_pixels_.size() && index < pixels.size(); ++index) {
        const int palette_index = active_pixels_[index];
        pixels[index] = pack_rgb(palette_red_[static_cast<std::size_t>(palette_index)],
                                 palette_green_[static_cast<std::size_t>(palette_index)],
                                 palette_blue_[static_cast<std::size_t>(palette_index)]);
    }
}

void Mute95Scene::render_credit_overlay(RgbSurface& surface, float scene_time_seconds) {
    if (active_credit_ < 0 || active_credit_ >= static_cast<int>(credits_.size()) ||
        message_start_seconds_ < 0.0f) {
        return;
    }

    const float elapsed = scene_time_seconds - message_start_seconds_;
    if (elapsed < 0.0f) {
        return;
    }

    float blend_first = 0.0f;
    float blend_second = 0.0f;
    if (elapsed < 1.5f) {
        blend_first = elapsed / 1.5f;
    } else if (elapsed < 4.0f) {
        blend_first = 1.0f;
        blend_second = (elapsed - 1.5f) / (4.0f - 1.5f);
    } else if (elapsed < 6.0f) {
        blend_first = 1.0f - (elapsed - 4.0f) / (6.0f - 4.0f);
        blend_second = 1.0f;
    } else if (elapsed < 9.0f) {
        blend_second = 1.0f - (elapsed - 6.0f) / (9.0f - 6.0f);
    }

    const CreditPair& pair = credits_[static_cast<std::size_t>(active_credit_)];
    blend_credit_region(surface, pair.first, pair.second, clamp_unit(blend_first), clamp_unit(blend_second));
}

void Mute95Scene::blend_credit_region(RgbSurface& surface,
                                      const PackedRgbAsset& first,
                                      const PackedRgbAsset& second,
                                      float blend_first,
                                      float blend_second) const {
    if (blend_first <= 0.0f && blend_second <= 0.0f) {
        return;
    }

    const int factor_first = static_cast<int>(blend_first * 32.9f);
    const int factor_second = static_cast<int>(blend_second * 32.9f);
    std::vector<std::uint32_t>& dst_pixels = surface.pixels();

    for (int row = 0; row < kCreditHeight; ++row) {
        for (int column = 0; column < kCreditWidth; ++column) {
            const int src_x = kCreditSourceX + column;
            const int src_y = kCreditSourceY + row;
            const std::size_t src_index =
                static_cast<std::size_t>(src_y) * static_cast<std::size_t>(first.width) +
                static_cast<std::size_t>(src_x);
            const std::size_t dst_index =
                static_cast<std::size_t>(kCreditDestY + row) * static_cast<std::size_t>(surface.width()) +
                static_cast<std::size_t>(kCreditDestX + column);

            std::uint32_t source_first = first.packed_pixels[src_index] & kColorMask;
            source_first = ((source_first >> 3) * static_cast<std::uint32_t>(factor_first)) >> 2;
            source_first &= kColorMask;

            std::uint32_t source_second = second.packed_pixels[src_index] & kColorMask;
            source_second = ((source_second >> 3) * static_cast<std::uint32_t>(factor_second)) >> 2;
            source_second &= kColorMask;

            std::uint32_t blended = source_first + source_second;
            std::uint32_t carry = blended & kCarryMask;
            blended = blended - carry | carry - (carry >> 8);

            std::uint32_t dst_packed = standard_to_packed_rgb(dst_pixels[dst_index]);
            dst_packed += blended;
            carry = dst_packed & kCarryMask;
            dst_packed = dst_packed - carry | carry - (carry >> 8);
            dst_pixels[dst_index] = packed_to_standard_rgb(dst_packed);
        }
    }
}

void Mute95Scene::select_credit(const std::string& message, float scene_time_seconds) {
    for (std::size_t index = 0; index < credits_.size(); ++index) {
        if (credits_[index].message_name == message) {
            active_credit_ = static_cast<int>(index);
            message_start_seconds_ = scene_time_seconds;
            return;
        }
    }
}

bool Mute95Scene::load_assets() {
    error_message_.clear();

    if (!load_original_gif_palette(gif_asset_path("krad3.gif"),
                                   &palette_red_,
                                   &palette_green_,
                                   &palette_blue_,
                                   &error_message_)) {
        return false;
    }

    CreditPair pair;
    if (!load_credit_pair("sav", "saviour", &pair)) {
        return false;
    }
    credits_.push_back(pair);
    if (!load_credit_pair("jmag", "jmagic", &pair)) {
        return false;
    }
    credits_.push_back(pair);
    if (!load_credit_pair("jugi", "jugi", &pair)) {
        return false;
    }
    credits_.push_back(pair);
    if (!load_credit_pair("anis", "anis", &pair)) {
        return false;
    }
    credits_.push_back(pair);
    if (!load_credit_pair("car", "carebear", &pair)) {
        return false;
    }
    credits_.push_back(pair);

    return true;
}

bool Mute95Scene::load_credit_pair(const std::string& base_name,
                                   const std::string& message_name,
                                   CreditPair* pair) {
    pair->message_name = message_name;
    if (!load_original_jpeg_packed_rgb(jpeg_asset_path(base_name + "1.jpg"),
                                       &pair->first,
                                       &error_message_)) {
        return false;
    }
    if (!load_original_jpeg_packed_rgb(jpeg_asset_path(base_name + "2.jpg"),
                                       &pair->second,
                                       &error_message_)) {
        return false;
    }
    return true;
}

std::string Mute95Scene::jpeg_asset_path(const std::string& file_name) const {
    return std::string("original/forward/images/kosmos/") + file_name;
}

std::string Mute95Scene::gif_asset_path(const std::string& file_name) const {
    return std::string("original/forward/images/kosmos/") + file_name;
}

}  // namespace forward_offline
