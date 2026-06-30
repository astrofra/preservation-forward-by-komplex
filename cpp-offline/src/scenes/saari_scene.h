#ifndef FORWARD_OFFLINE_SCENES_SAARI_SCENE_H
#define FORWARD_OFFLINE_SCENES_SAARI_SCENE_H

#include <string>
#include <vector>

#include "assets/original_asset_loader.h"
#include "core/java_random.h"
#include "scenes/scene.h"

namespace forward_offline {

class SaariScene : public Scene {
public:
    SaariScene();

    virtual const char* script_name() const;
    virtual void init();
    virtual void on_show();
    virtual void render(RgbSurface& surface, float scene_time_seconds, float delta_seconds);
    virtual void handle_message(const std::string& message, float scene_time_seconds);

    bool is_ready() const;
    const std::string& error_message() const;

private:
    bool load_assets();
    void build_shock_tables(int max_gray_value);
    void draw_sky(RgbSurface& surface, float scene_time_seconds) const;
    void draw_ocean(RgbSurface& surface, float scene_time_seconds) const;
    void draw_island(RgbSurface& surface, float scene_time_seconds) const;
    void draw_env_blob(RgbSurface& surface,
                       float center_x,
                       float center_y,
                       float radius,
                       float alpha) const;
    void apply_shock(RgbSurface& surface, int line_count);
    std::string jpeg_asset_path(const std::string& file_name) const;
    std::string gif_asset_path(const std::string& file_name) const;

    PackedRgbAsset sky_asset_;
    IndexedAsset saari_asset_;
    IndexedAsset env_asset_;
    IndexedAsset height_asset_;
    std::vector<int> shock_pattern_;
    std::vector<int> shock_rows_;
    JavaRandom shock_init_random_;
    JavaRandom shock_frame_random_;
    float shock_amount_;
    float shock_decay_;
    bool ready_;
    std::string error_message_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_SCENES_SAARI_SCENE_H
