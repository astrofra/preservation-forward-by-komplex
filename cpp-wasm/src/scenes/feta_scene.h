#ifndef FORWARD_OFFLINE_SCENES_FETA_SCENE_H
#define FORWARD_OFFLINE_SCENES_FETA_SCENE_H

#include <cstdint>
#include <string>
#include <vector>

#include "assets/original_asset_loader.h"
#include "core/java_random.h"
#include "scenes/scene.h"
#include "scenes/scene3d_shared.h"

namespace forward_offline {

struct FetaParticle {
    Scene3dVec3 local_position;
};

struct FetaIguMesh {
    std::vector<Scene3dVec3> vertices;
    std::vector<Scene3dVec3> normals;
    std::vector<Scene3dTriangle> triangles;
};

class FetaScene : public Scene {
public:
    FetaScene();

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
    bool load_igu_mesh(const std::string& path,
                       FetaIguMesh* mesh,
                       std::string* error_message) const;
    void build_particle_cloud();
    void initialize_feedback_buffers();
    void update_feedback_palette(bool black_index_255);
    void apply_feedback_composite(std::vector<std::uint32_t>* packed_surface,
                                  float scene_time_seconds);
    std::string image_asset_path(const std::string& file_name) const;
    std::string mesh_asset_path(const std::string& file_name) const;

    PackedRgbAsset backdrop_texture_;
    PackedRgbAsset env_texture_;
    PackedRgbAsset flare_texture_;
    FetaIguMesh fetus_mesh_;
    std::vector<FetaParticle> particles_;
    std::vector<std::uint8_t> feedback_current_;
    std::vector<std::uint8_t> feedback_next_;
    std::vector<std::uint32_t> feedback_palette_;
    std::vector<std::uint32_t> frame_history_;
    JavaRandom particle_random_;
    float black_feta_time_;
    float black_muna_time_;
    bool ready_;
    std::string error_message_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_SCENES_FETA_SCENE_H
