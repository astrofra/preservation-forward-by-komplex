#ifndef FORWARD_OFFLINE_SCENES_KUKOT_SCENE_H
#define FORWARD_OFFLINE_SCENES_KUKOT_SCENE_H

#include <cstdint>
#include <string>
#include <vector>

#include "assets/original_asset_loader.h"
#include "core/java_random.h"
#include "scenes/scene.h"
#include "scenes/scene3d_shared.h"

namespace forward_offline {

struct KukotMeshActor {
    std::string name;
    Scene3dStaticMesh mesh;
    std::vector<Scene3dTrackSample> position_track;
    std::vector<Scene3dOrientationSample> orientation_track;
};

struct KukotParticle {
    Scene3dVec3 local_position;
};

class KukotScene : public Scene {
public:
    KukotScene();

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
    void build_environment_surface(const IndexedAsset& env_palette_asset);
    void build_background_noise();
    void build_flash_tables();
    void build_particle_cloud();
    std::string ase_asset_path(const std::string& file_name) const;
    std::string image_asset_path(const std::string& file_name) const;

    PackedRgbAsset env_surface_;
    IndexedAsset env_indexed_asset_;
    PackedRgbAsset flare_asset_;
    PackedRgbAsset background_noise_;
    std::vector<KukotMeshActor> actors_;
    std::vector<Scene3dTrackSample> camera_track_;
    std::vector<Scene3dTrackSample> camera_target_track_;
    std::vector<KukotParticle> particles_;
    std::vector<std::uint32_t> flash_pattern_;
    std::vector<int> flash_rows_;
    JavaRandom background_init_random_;
    JavaRandom background_frame_random_;
    JavaRandom flash_init_random_;
    JavaRandom flash_frame_random_;
    JavaRandom particle_random_;
    float flash_amount_;
    float flash_decay_;
    bool ready_;
    std::string error_message_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_SCENES_KUKOT_SCENE_H
