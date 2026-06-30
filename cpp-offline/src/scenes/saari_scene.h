#ifndef FORWARD_OFFLINE_SCENES_SAARI_SCENE_H
#define FORWARD_OFFLINE_SCENES_SAARI_SCENE_H

#include <cstdint>
#include <string>
#include <vector>

#include "assets/original_asset_loader.h"
#include "core/java_random.h"
#include "scenes/scene.h"

namespace forward_offline {

struct SaariVec3 {
    float x;
    float y;
    float z;
};

struct SaariTriangle {
    int a;
    int b;
    int c;
};

struct SaariTrackSample {
    int tick;
    SaariVec3 value;
};

struct SaariRotationSample {
    int tick;
    SaariVec3 axis;
    float angle;
};

struct SaariStaticMesh {
    std::vector<SaariVec3> vertices;
    std::vector<SaariVec3> normals;
    std::vector<SaariTriangle> triangles;
    SaariVec3 pivot;
};

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
    bool load_ase_scene();
    void build_shock_tables(int max_gray_value);
    void apply_shock(RgbSurface& surface, int line_count);
    std::string ase_asset_path(const std::string& file_name) const;
    std::string jpeg_asset_path(const std::string& file_name) const;
    std::string gif_asset_path(const std::string& file_name) const;

    PackedRgbAsset sky_asset_;
    IndexedAsset saari_asset_;
    IndexedAsset terrain_asset_;
    IndexedAsset water_asset_;
    IndexedAsset env_asset_;
    IndexedAsset height_asset_;
    SaariStaticMesh meditate_mesh_;
    SaariStaticMesh klunssi_mesh_;
    std::vector<SaariTrackSample> camera_track_;
    std::vector<SaariTrackSample> camera_target_track_;
    std::vector<SaariRotationSample> camera_rotation_track_;
    std::vector<SaariTrackSample> klunssi_track_;
    SaariVec3 meditate_position_;
    SaariVec3 klunssi_initial_position_;
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
