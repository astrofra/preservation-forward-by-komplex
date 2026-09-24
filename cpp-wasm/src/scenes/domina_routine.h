#ifndef FORWARD_OFFLINE_SCENES_DOMINA_ROUTINE_H
#define FORWARD_OFFLINE_SCENES_DOMINA_ROUTINE_H

#include <string>

#include "assets/original_asset_loader.h"
#include "render/indexed_surface.h"
#include "scenes/routine.h"

namespace forward_offline {

class DominaRoutine : public Routine {
public:
    DominaRoutine();

    virtual const char* script_name() const;
    virtual void init();
    virtual void on_show();
    virtual void render(RgbSurface& surface, float scene_time_seconds, float delta_seconds);
    virtual void handle_message(const std::string& message, float scene_time_seconds);
    bool is_ready() const;
    const std::string& error_message() const;

private:
    bool load_assets();
    void populate_frame(int frame_index);
    void build_palette(float scene_time_seconds);
    std::string gif_asset_path(const std::string& file_name) const;

    IndexedAsset source_asset_;
    IndexedSurface frame_;
    bool fade_to_black_;
    float fade_start_seconds_;
    bool ready_;
    std::string error_message_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_SCENES_DOMINA_ROUTINE_H
