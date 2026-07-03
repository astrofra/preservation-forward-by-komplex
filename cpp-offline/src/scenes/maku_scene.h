#ifndef FORWARD_OFFLINE_SCENES_MAKU_SCENE_H
#define FORWARD_OFFLINE_SCENES_MAKU_SCENE_H

#include <cstdint>
#include <string>
#include <vector>

#include "assets/original_asset_loader.h"
#include "core/java_random.h"
#include "scenes/scene.h"
#include "scenes/scene3d_shared.h"

namespace forward_offline {

class MakuScene : public Scene {
public:
    MakuScene();

    virtual const char* script_name() const;
    virtual void init();
    virtual void on_show();
    virtual void dispose();
    virtual void render(RgbSurface& surface, float scene_time_seconds, float delta_seconds);
    virtual void handle_message(const std::string& message, float scene_time_seconds);

    bool is_ready() const;
    const std::string& error_message() const;

private:
    bool load_assets();
    bool load_ase_scene();
    void build_terrain_surface();
    void build_shock_tables(int max_gray_value);
    void apply_shock(RgbSurface& surface, int line_count);
    std::string ase_asset_path(const std::string& file_name) const;
    std::string image_asset_path(const std::string& file_name) const;

    IndexedAsset height_asset_;
    IndexedAsset terrain_asset_;
    PackedRgbAsset terrain_surface_;
    std::vector<Scene3dTrackSample> camera_track_;
    std::vector<Scene3dTrackSample> camera_target_track_;
    std::vector<int> shock_pattern_;
    std::vector<int> shock_rows_;
    JavaRandom shock_init_random_;
    JavaRandom shock_frame_random_;
    std::vector<std::uint32_t> frame_history_;
    bool ksor_enabled_;
    bool low_enabled_;
    bool roll_enabled_;
    float roll_angle_;
    float track_speed_;
    float track_offset_seconds_;
    float track_reference_seconds_;
    float shock_amount_;
    float shock_decay_;
    bool ready_;
    std::string error_message_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_SCENES_MAKU_SCENE_H
