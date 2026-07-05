#ifndef FORWARD_OFFLINE_SCENES_UPPOL_ROUTINE_H
#define FORWARD_OFFLINE_SCENES_UPPOL_ROUTINE_H

#include <string>

#include "assets/original_asset_loader.h"
#include "render/indexed_surface.h"
#include "scenes/routine.h"

namespace forward_offline {

class UppolRoutine : public Routine {
public:
    UppolRoutine();

    virtual const char* script_name() const;
    virtual void init();
    virtual void on_show();
    virtual void render(RgbSurface& surface, float scene_time_seconds, float delta_seconds);
    bool is_ready() const;
    const std::string& error_message() const;

private:
    bool load_assets();
    bool validate_credit_bitmaps();
    void populate_frame(int frame_index);
    void render_credits(RgbSurface& surface, float scene_time_seconds) const;
    std::string gif_asset_path(const std::string& file_name) const;

    IndexedAsset source_asset_;
    IndexedSurface frame_;
    int frame_counter_;
    bool ready_;
    std::string error_message_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_SCENES_UPPOL_ROUTINE_H
