#ifndef FORWARD_OFFLINE_SCENES_SAARI_SCENE_H
#define FORWARD_OFFLINE_SCENES_SAARI_SCENE_H

#include <cstdint>
#include <string>
#include <vector>

#include "assets/original_asset_loader.h"
#include "core/java_random.h"
#include "scenes/scene.h"
#include "scenes/scene3d_shared.h"
#include "scenes/capture_types.h"

namespace forward_offline {

typedef Scene3dVec3 SaariVec3;
typedef Scene3dTriangle SaariTriangle;
typedef Scene3dTrackSample SaariTrackSample;
typedef Scene3dRotationSample SaariRotationSample;
typedef Scene3dStaticMesh SaariStaticMesh;

typedef CaptureCamera SaariCaptureCamera;
typedef CapturePoint SaariCapturePoint;

SaariCaptureCamera make_saari_capture_camera(const SaariVec3& position,
    const SaariVec3& target, int width, int height, float horizontal_fov);

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

    // Static scene, explicit camera, and optional final opaque surface IDs.
    // IDs follow painter/compositing order, not a replacement z-buffer.
    void render_capture(RgbSurface& surface, const SaariCaptureCamera& camera,
                        float frozen_time, std::vector<int>* surface_ids);
    void capture_geometry(float frozen_time, std::vector<SaariCapturePoint>* points,
                          SaariVec3* bounds_min, SaariVec3* bounds_max,
                          SaariVec3* meditate_center) const;

private:
    void render_view(RgbSurface& surface, const SaariCaptureCamera& camera,
                     float scene_time_seconds, std::vector<int>* surface_ids);
    bool load_assets();
    bool load_ase_scene();
    void build_shock_tables(int max_gray_value);
    void apply_shock(RgbSurface& surface, int line_count);
    std::string ase_asset_path(const std::string& file_name) const;
    std::string jpeg_asset_path(const std::string& file_name) const;
    std::string gif_asset_path(const std::string& file_name) const;

    PackedRgbAsset sky_asset_;
    PackedRgbAsset backdrop_asset_;
    IndexedAsset saari_asset_;
    IndexedAsset terrain_asset_;
    IndexedAsset water_asset_;
    IndexedAsset env_asset_;
    IndexedAsset height_asset_;
    std::vector<std::uint32_t> saari_white_ramp_;
    std::vector<std::uint32_t> saari_black_ramp_;
    std::vector<std::uint32_t> env_white_ramp_;
    std::vector<std::uint32_t> env_black_ramp_;
    std::vector<std::uint8_t> saari_reflective_palette_mask_;
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
