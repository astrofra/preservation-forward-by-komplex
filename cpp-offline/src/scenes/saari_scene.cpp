#include "scenes/saari_scene.h"

#include <algorithm>
#include <cmath>

namespace forward_offline {

namespace {

const int kSurfaceWidth = 512;
const int kSurfaceHeight = 256;
const int kSaariTextureSize = 256;
const int kShockPatternLength = 1000;
const int kShockSeed = 195;
const int kShockFrameSeed = 1337;

float clamp_unit(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }
    return value;
}

int clamp_int(int value, int minimum, int maximum) {
    if (value < minimum) {
        return minimum;
    }
    if (value > maximum) {
        return maximum;
    }
    return value;
}

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

std::uint32_t pack_rgb(int red, int green, int blue) {
    return static_cast<std::uint32_t>((clamp_int(red, 0, 255) << 16) |
                                      (clamp_int(green, 0, 255) << 8) |
                                      clamp_int(blue, 0, 255));
}

std::uint32_t blend_rgb(std::uint32_t dst, std::uint32_t src, float alpha) {
    const float clamped_alpha = clamp_unit(alpha);
    const float inverse_alpha = 1.0f - clamped_alpha;

    const int dst_red = static_cast<int>((dst >> 16) & 0xffU);
    const int dst_green = static_cast<int>((dst >> 8) & 0xffU);
    const int dst_blue = static_cast<int>(dst & 0xffU);

    const int src_red = static_cast<int>((src >> 16) & 0xffU);
    const int src_green = static_cast<int>((src >> 8) & 0xffU);
    const int src_blue = static_cast<int>(src & 0xffU);

    return pack_rgb(static_cast<int>(src_red * clamped_alpha + dst_red * inverse_alpha),
                    static_cast<int>(src_green * clamped_alpha + dst_green * inverse_alpha),
                    static_cast<int>(src_blue * clamped_alpha + dst_blue * inverse_alpha));
}

std::uint32_t sample_packed_rgb_asset(const PackedRgbAsset& asset, float u, float v) {
    if (asset.width <= 0 || asset.height <= 0 || asset.packed_pixels.empty()) {
        return 0;
    }

    while (u < 0.0f) {
        u += 1.0f;
    }
    while (v < 0.0f) {
        v += 1.0f;
    }
    u -= std::floor(u);
    v -= std::floor(v);

    const int x = clamp_int(static_cast<int>(u * static_cast<float>(asset.width)), 0, asset.width - 1);
    const int y = clamp_int(static_cast<int>(v * static_cast<float>(asset.height)), 0, asset.height - 1);
    const std::uint32_t packed = asset.packed_pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(asset.width) +
                                                     static_cast<std::size_t>(x)];
    return static_cast<std::uint32_t>(((packed >> 20) & 0xffU) << 16 |
                                      ((packed >> 10) & 0xffU) << 8 |
                                      (packed & 0xffU));
}

std::uint32_t sample_indexed_asset(const IndexedAsset& asset, float u, float v) {
    if (asset.width <= 0 || asset.height <= 0 || asset.pixels.empty()) {
        return 0;
    }

    while (u < 0.0f) {
        u += 1.0f;
    }
    while (v < 0.0f) {
        v += 1.0f;
    }
    u -= std::floor(u);
    v -= std::floor(v);

    const int x = clamp_int(static_cast<int>(u * static_cast<float>(asset.width)), 0, asset.width - 1);
    const int y = clamp_int(static_cast<int>(v * static_cast<float>(asset.height)), 0, asset.height - 1);
    const std::size_t index = static_cast<std::size_t>(y) * static_cast<std::size_t>(asset.width) +
                              static_cast<std::size_t>(x);
    const int palette_index = asset.pixels[index];
    return pack_rgb(asset.palette_red[static_cast<std::size_t>(palette_index)],
                    asset.palette_green[static_cast<std::size_t>(palette_index)],
                    asset.palette_blue[static_cast<std::size_t>(palette_index)]);
}

int sample_height_value(const IndexedAsset& asset, float u, float v) {
    if (asset.width <= 0 || asset.height <= 0 || asset.pixels.empty()) {
        return 0;
    }

    u = clamp_unit(u);
    v = clamp_unit(v);
    const int x = clamp_int(static_cast<int>(u * static_cast<float>(asset.width)), 0, asset.width - 1);
    const int y = clamp_int(static_cast<int>(v * static_cast<float>(asset.height)), 0, asset.height - 1);
    const int palette_index = asset.pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(asset.width) +
                                           static_cast<std::size_t>(x)];
    return static_cast<int>(asset.palette_red[static_cast<std::size_t>(palette_index)]);
}

}  // namespace

SaariScene::SaariScene()
    : sky_asset_(),
      saari_asset_(),
      env_asset_(),
      height_asset_(),
      shock_pattern_(kShockPatternLength, 0),
      shock_rows_(kSurfaceHeight, 0),
      shock_init_random_(kShockSeed),
      shock_frame_random_(kShockFrameSeed),
      shock_amount_(0.0f),
      shock_decay_(0.0f),
      ready_(false),
      error_message_() {
}

const char* SaariScene::script_name() const {
    return "saari";
}

void SaariScene::init() {
    ready_ = load_assets();
    if (ready_) {
        build_shock_tables(195);
    }
    on_show();
}

void SaariScene::on_show() {
    shock_amount_ = 0.0f;
    shock_decay_ = 0.0f;
    shock_frame_random_ = JavaRandom(kShockFrameSeed);
}

void SaariScene::render(RgbSurface& surface, float scene_time_seconds, float delta_seconds) {
    surface.clear(0);
    if (!ready_) {
        return;
    }

    draw_sky(surface, scene_time_seconds);
    draw_ocean(surface, scene_time_seconds);
    draw_island(surface, scene_time_seconds);

    const float primary_blob_radius =
        scene_time_seconds < 10.0f ? 0.0f : lerp(92.0f, 66.0f, clamp_unit((scene_time_seconds - 18.0f) / 22.0f));
    if (primary_blob_radius > 0.0f) {
        draw_env_blob(surface,
                      lerp(265.0f, 44.0f, clamp_unit((scene_time_seconds - 22.0f) / 18.0f)),
                      lerp(42.0f, 112.0f, clamp_unit((scene_time_seconds - 10.0f) / 18.0f)),
                      primary_blob_radius,
                      0.9f);
        draw_env_blob(surface,
                      244.0f + std::sin(scene_time_seconds * 0.25f) * 18.0f,
                      262.0f - scene_time_seconds * 3.4f,
                      48.0f,
                      0.75f);
    }

    if (shock_amount_ > 0.0f) {
        if (shock_decay_ > 0.0f) {
            shock_amount_ -= shock_decay_ * delta_seconds;
            if (shock_amount_ < 0.0f) {
                shock_amount_ = 0.0f;
            }
        }
        const int line_count = static_cast<int>(shock_amount_ * static_cast<float>(surface.height()) / 100.0f);
        apply_shock(surface, line_count);
    }
}

void SaariScene::handle_message(const std::string& message, float scene_time_seconds) {
    (void)scene_time_seconds;
    if (message == "suh") {
        shock_amount_ = 100.0f;
        shock_decay_ = 200.0f;
    } else if (message == "suh0") {
        shock_amount_ = 68.0f;
        shock_decay_ = 0.0f;
    }
}

bool SaariScene::is_ready() const {
    return ready_;
}

const std::string& SaariScene::error_message() const {
    return error_message_;
}

bool SaariScene::load_assets() {
    error_message_.clear();

    if (!load_original_jpeg_packed_rgb(jpeg_asset_path("tai1sp.jpg"), &sky_asset_, &error_message_)) {
        return false;
    }
    if (!load_original_gif_indexed(gif_asset_path("saari.gif"), &saari_asset_, &error_message_)) {
        return false;
    }
    if (!load_original_gif_indexed(gif_asset_path("envi_klu.gif"), &env_asset_, &error_message_)) {
        return false;
    }
    if (!load_original_gif_indexed(gif_asset_path("saarih15.gif"), &height_asset_, &error_message_)) {
        return false;
    }

    if (sky_asset_.width != kSurfaceWidth || sky_asset_.height != kSurfaceHeight) {
        error_message_ = "unexpected saari sky dimensions: " + jpeg_asset_path("tai1sp.jpg");
        return false;
    }
    if (saari_asset_.width != kSaariTextureSize || saari_asset_.height != kSaariTextureSize * 2) {
        error_message_ = "unexpected saari terrain dimensions: " + gif_asset_path("saari.gif");
        return false;
    }
    if (env_asset_.width != kSaariTextureSize || env_asset_.height != kSaariTextureSize) {
        error_message_ = "unexpected saari env-map dimensions: " + gif_asset_path("envi_klu.gif");
        return false;
    }

    return true;
}

void SaariScene::build_shock_tables(int max_gray_value) {
    shock_init_random_ = JavaRandom(kShockSeed);
    for (std::size_t index = 0; index < shock_pattern_.size(); ++index) {
        shock_pattern_[index] = static_cast<int>(shock_init_random_.next_float() * static_cast<float>(max_gray_value));
    }

    for (std::size_t index = 0; index < shock_rows_.size(); ++index) {
        shock_rows_[index] = static_cast<int>(index);
    }

    for (int iteration = 0; iteration < 3000; ++iteration) {
        const int first = iteration % static_cast<int>(shock_rows_.size());
        const int second =
            static_cast<int>(shock_init_random_.next_float() * static_cast<float>(shock_rows_.size() - 2));
        std::swap(shock_rows_[static_cast<std::size_t>(first)], shock_rows_[static_cast<std::size_t>(second)]);
    }
}

void SaariScene::draw_sky(RgbSurface& surface, float scene_time_seconds) const {
    std::vector<std::uint32_t>& pixels = surface.pixels();
    for (int y = 0; y < surface.height(); ++y) {
        const float fy = static_cast<float>(y) / static_cast<float>(surface.height() - 1);
        const float horizon_glow = clamp_unit(1.0f - std::fabs((fy - 0.48f) * 3.4f));
        for (int x = 0; x < surface.width(); ++x) {
            const float fx = static_cast<float>(x) / static_cast<float>(surface.width() - 1);
            const float u = clamp_unit(0.02f + fx * 0.47f + std::sin(scene_time_seconds * 0.05f) * 0.01f);
            const float v = clamp_unit(0.02f + fy * 0.80f);
            std::uint32_t sky = sample_packed_rgb_asset(sky_asset_, u, v);

            const int red = static_cast<int>((sky >> 16) & 0xffU) + static_cast<int>(horizon_glow * 16.0f);
            const int green = static_cast<int>((sky >> 8) & 0xffU) + static_cast<int>(horizon_glow * 14.0f);
            const int blue = static_cast<int>(sky & 0xffU) + static_cast<int>(horizon_glow * 24.0f);
            pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(surface.width()) +
                   static_cast<std::size_t>(x)] = pack_rgb(red, green, blue);
        }
    }
}

void SaariScene::draw_ocean(RgbSurface& surface, float scene_time_seconds) const {
    std::vector<std::uint32_t>& pixels = surface.pixels();
    const float horizon_y = 104.0f + std::sin(scene_time_seconds * 0.14f) * 4.0f;
    for (int y = 0; y < surface.height(); ++y) {
        if (static_cast<float>(y) < horizon_y) {
            continue;
        }

        const float depth = clamp_unit((static_cast<float>(y) - horizon_y) /
                                       static_cast<float>(surface.height() - static_cast<int>(horizon_y)));
        const float tex_v = 0.5f + depth * depth * 0.5f + scene_time_seconds * 0.002f;
        const float blend = 0.55f + depth * 0.35f;
        for (int x = 0; x < surface.width(); ++x) {
            const float tex_u = static_cast<float>(x) / 256.0f + scene_time_seconds * 0.01f + depth * 0.08f;
            const std::uint32_t ocean = sample_indexed_asset(saari_asset_, tex_u, tex_v);
            const std::size_t index = static_cast<std::size_t>(y) * static_cast<std::size_t>(surface.width()) +
                                      static_cast<std::size_t>(x);
            pixels[index] = blend_rgb(pixels[index], ocean, blend);
        }
    }
}

void SaariScene::draw_island(RgbSurface& surface, float scene_time_seconds) const {
    if (scene_time_seconds < 24.0f) {
        return;
    }

    const float reveal = clamp_unit((scene_time_seconds - 24.0f) / 16.0f);
    const float scale = lerp(0.85f, 1.35f, clamp_unit((scene_time_seconds - 28.0f) / 18.0f));
    const float center_x = lerp(318.0f, 240.0f, clamp_unit((scene_time_seconds - 30.0f) / 18.0f));
    const float center_y = lerp(132.0f, 160.0f, clamp_unit((scene_time_seconds - 28.0f) / 16.0f));
    const int draw_width = static_cast<int>(256.0f * scale);
    const int draw_height = static_cast<int>(256.0f * scale * 0.82f);
    const int start_x = static_cast<int>(center_x - draw_width * 0.5f);
    const int start_y = static_cast<int>(center_y - draw_height * 0.5f);
    std::vector<std::uint32_t>& pixels = surface.pixels();

    for (int y = 0; y < draw_height; ++y) {
        const int dst_y = start_y + y;
        if (dst_y < 0 || dst_y >= surface.height()) {
            continue;
        }

        const float src_v = clamp_unit(static_cast<float>(y) / static_cast<float>(draw_height - 1));
        for (int x = 0; x < draw_width; ++x) {
            const int dst_x = start_x + x;
            if (dst_x < 0 || dst_x >= surface.width()) {
                continue;
            }

            const float src_u = clamp_unit(static_cast<float>(x) / static_cast<float>(draw_width - 1));
            const float height_bias =
                static_cast<float>(sample_height_value(height_asset_, src_u, src_v)) / 255.0f * 0.12f;
            const std::uint32_t island =
                sample_indexed_asset(saari_asset_, src_u, clamp_unit(src_v * 0.5f - height_bias));
            const std::size_t index = static_cast<std::size_t>(dst_y) * static_cast<std::size_t>(surface.width()) +
                                      static_cast<std::size_t>(dst_x);
            pixels[index] = blend_rgb(pixels[index], island, reveal * 0.94f);
        }
    }
}

void SaariScene::draw_env_blob(RgbSurface& surface,
                               float center_x,
                               float center_y,
                               float radius,
                               float alpha) const {
    if (radius <= 1.0f) {
        return;
    }

    const int start_x = static_cast<int>(center_x - radius);
    const int start_y = static_cast<int>(center_y - radius);
    const int end_x = static_cast<int>(center_x + radius);
    const int end_y = static_cast<int>(center_y + radius);
    std::vector<std::uint32_t>& pixels = surface.pixels();

    for (int y = start_y; y <= end_y; ++y) {
        if (y < 0 || y >= surface.height()) {
            continue;
        }
        for (int x = start_x; x <= end_x; ++x) {
            if (x < 0 || x >= surface.width()) {
                continue;
            }

            const float dx = (static_cast<float>(x) - center_x) / radius;
            const float dy = (static_cast<float>(y) - center_y) / radius;
            const float dist2 = dx * dx + dy * dy;
            if (dist2 > 1.0f) {
                continue;
            }

            const std::uint32_t color = sample_indexed_asset(env_asset_, dx * 0.5f + 0.5f, dy * 0.5f + 0.5f);
            const int brightness = static_cast<int>((color >> 16) & 0xffU) +
                                   static_cast<int>((color >> 8) & 0xffU) +
                                   static_cast<int>(color & 0xffU);
            if (brightness < 48) {
                continue;
            }

            const float edge = 1.0f - std::sqrt(dist2);
            const std::size_t index = static_cast<std::size_t>(y) * static_cast<std::size_t>(surface.width()) +
                                      static_cast<std::size_t>(x);
            pixels[index] = blend_rgb(pixels[index], color, alpha * clamp_unit(edge * 1.8f));
        }
    }
}

void SaariScene::apply_shock(RgbSurface& surface, int line_count) {
    if (line_count <= 0) {
        return;
    }

    const int clamped_lines = std::min(line_count, surface.height() - 1);
    const int row_offset = static_cast<int>(shock_frame_random_.next_float() * 1000.0f);
    std::vector<std::uint32_t>& pixels = surface.pixels();

    for (int line = 0; line < clamped_lines; ++line) {
        const int row = shock_rows_[static_cast<std::size_t>((line + row_offset) % shock_rows_.size())];
        const int pattern_offset = static_cast<int>(shock_frame_random_.next_float() *
                                                    static_cast<float>(shock_pattern_.size() - 1 - surface.width()));
        std::size_t pixel_index = static_cast<std::size_t>(row) * static_cast<std::size_t>(surface.width());
        for (int x = 0; x < surface.width(); ++x) {
            const int delta = shock_pattern_[static_cast<std::size_t>(pattern_offset + x)];
            const std::uint32_t pixel = pixels[pixel_index];
            const int red = clamp_int(static_cast<int>((pixel >> 16) & 0xffU) - delta, 0, 255);
            const int green = clamp_int(static_cast<int>((pixel >> 8) & 0xffU) - delta, 0, 255);
            const int blue = clamp_int(static_cast<int>(pixel & 0xffU) - delta, 0, 255);
            pixels[pixel_index++] = pack_rgb(red, green, blue);
        }
    }
}

std::string SaariScene::jpeg_asset_path(const std::string& file_name) const {
    return std::string("original/forward/images/verax/") + file_name;
}

std::string SaariScene::gif_asset_path(const std::string& file_name) const {
    return std::string("original/forward/images/scape/") + file_name;
}

}  // namespace forward_offline
