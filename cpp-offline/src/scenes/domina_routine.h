#ifndef FORWARD_OFFLINE_SCENES_DOMINA_ROUTINE_H
#define FORWARD_OFFLINE_SCENES_DOMINA_ROUTINE_H

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

private:
    void build_source();
    void build_palette(float scene_time_seconds);
    static std::uint32_t pack_rgb(int red, int green, int blue);

    IndexedSurface source_;
    IndexedSurface frame_;
    bool fade_to_black_;
    float fade_start_seconds_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_SCENES_DOMINA_ROUTINE_H
