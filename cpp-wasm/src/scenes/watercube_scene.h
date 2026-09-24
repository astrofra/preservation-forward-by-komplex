#ifndef FORWARD_OFFLINE_SCENES_WATERCUBE_SCENE_H
#define FORWARD_OFFLINE_SCENES_WATERCUBE_SCENE_H

#include <cstdint>
#include <string>
#include <vector>

#include "assets/original_asset_loader.h"
#include "core/java_random.h"
#include "scenes/scene.h"
#include "scenes/scene3d_shared.h"

namespace forward_offline {

struct WatercubeUvCoord {
    float u;
    float v;
};

struct WatercubeTexturedTriangle {
    int a;
    int b;
    int c;
    int ta;
    int tb;
    int tc;
};

struct WatercubeTexturedMesh {
    std::vector<Scene3dVec3> vertices;
    std::vector<Scene3dVec3> normals;
    std::vector<WatercubeUvCoord> texcoords;
    std::vector<WatercubeTexturedTriangle> triangles;
    Scene3dVec3 pivot;
};

struct WatercubeIguMesh {
    std::vector<Scene3dVec3> vertices;
    std::vector<Scene3dVec3> normals;
    std::vector<Scene3dTriangle> triangles;
};

class WatercubeScene : public Scene {
public:
    WatercubeScene();

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
    bool load_igu_mesh(const std::string& path,
                       WatercubeIguMesh* mesh,
                       std::string* error_message) const;
    void build_flash_tables();
    void step_simulation();
    void inject_ring_stamp();
    void advance_wave_buffers();
    std::string ase_asset_path(const std::string& file_name) const;
    std::string image_asset_path(const std::string& file_name) const;
    std::string mesh_asset_path(const std::string& file_name) const;

    PackedRgbAsset overlay_panel_texture_;
    PackedRgbAsset overlay_scroll_texture_;
    PackedRgbAsset box_texture_;
    PackedRgbAsset env_texture_;
    PackedRgbAsset ring_texture_;
    PackedRgbAsset ripple_texture_;
    PackedRgbAsset wave_a_;
    PackedRgbAsset wave_b_;
    PackedRgbAsset water_texture_;
    PackedRgbAsset panel_surface_;
    WatercubeTexturedMesh water_mesh_;
    WatercubeTexturedMesh box_mesh_;
    WatercubeIguMesh kluns1_mesh_;
    WatercubeIguMesh kluns2_mesh_;
    std::vector<Scene3dTrackSample> camera_track_;
    std::vector<Scene3dTrackSample> camera_target_track_;
    std::vector<std::uint32_t> flash_pattern_;
    std::vector<int> flash_rows_;
    std::vector<std::uint32_t> frame_packed_;
    JavaRandom flash_init_random_;
    JavaRandom render_random_;
    bool second_kluns_enabled_;
    bool wave_toggle_;
    int mode_scale_;
    int ripple_phase_;
    int text_strip_offset_;
    std::int64_t last_tick_index_;
    std::int64_t simulation_tick_count_;
    float roll_impulse_;
    float flash_amount_;
    float flash_decay_;
    float shock_amount_;
    float shock_decay_;
    bool ready_;
    std::string error_message_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_SCENES_WATERCUBE_SCENE_H
