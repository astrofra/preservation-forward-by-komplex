#ifndef FORWARD_OFFLINE_SCENES_MUTE95_SCENE_H
#define FORWARD_OFFLINE_SCENES_MUTE95_SCENE_H

#include <string>
#include <vector>

#include "assets/original_asset_loader.h"
#include "core/java_random.h"
#include "scenes/scene.h"

namespace forward_offline {

class Mute95Scene : public Scene {
public:
    Mute95Scene();

    virtual const char* script_name() const;
    virtual void init();
    virtual void on_show();
    virtual void render(RgbSurface& surface, float scene_time_seconds, float delta_seconds);
    virtual void handle_message(const std::string& message, float scene_time_seconds);

    bool is_ready() const;
    const std::string& error_message() const;

private:
    struct CreditPair {
        std::string message_name;
        PackedRgbAsset first;
        PackedRgbAsset second;
    };

    static std::uint32_t pack_rgb(std::uint8_t red, std::uint8_t green, std::uint8_t blue);
    static std::uint32_t packed_to_standard_rgb(std::uint32_t packed);
    static std::uint32_t standard_to_packed_rgb(std::uint32_t rgb);
    void apply_warp(float scale);
    void apply_noise(float scene_time_seconds, float delta_seconds);
    void blend_buffers();
    void render_indexed_to_rgb(RgbSurface& surface) const;
    void render_credit_overlay(RgbSurface& surface, float scene_time_seconds);
    void blend_credit_region(RgbSurface& surface,
                             const PackedRgbAsset& first,
                             const PackedRgbAsset& second,
                             float blend_first,
                             float blend_second) const;
    void select_credit(const std::string& message, float scene_time_seconds);
    bool load_assets();
    bool load_credit_pair(const std::string& base_name, const std::string& message_name, CreditPair* pair);
    std::string jpeg_asset_path(const std::string& file_name) const;
    std::string gif_asset_path(const std::string& file_name) const;

    std::vector<std::uint8_t> palette_red_;
    std::vector<std::uint8_t> palette_green_;
    std::vector<std::uint8_t> palette_blue_;
    std::vector<std::uint8_t> active_pixels_;
    std::vector<std::uint8_t> passive_pixels_;
    std::vector<CreditPair> credits_;
    JavaRandom random_;
    int active_credit_;
    float message_start_seconds_;
    int phase_ticks_;
    double desktop_phase_ticks_;
    double desktop_noise_writes_;
    std::vector<std::vector<float> > horizontal_offsets_;
    std::vector<std::vector<float> > vertical_offsets_;
    bool ready_;
    std::string error_message_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_SCENES_MUTE95_SCENE_H
