#ifndef FORWARD_OFFLINE_SCENES_MUTE95_SCENE_H
#define FORWARD_OFFLINE_SCENES_MUTE95_SCENE_H

#include <string>
#include <vector>

#include "render/indexed_surface.h"
#include "scenes/scene.h"

namespace forward_offline {

class Mute95Scene : public Scene {
public:
    Mute95Scene();

    virtual const char* script_name() const;
    virtual void init();
    virtual void on_show();
    virtual void render(RgbSurface& surface, float scene_time_seconds, float delta_seconds);
    virtual void handle_message(const std::string& message, float scene_time_seconds);

private:
    struct CreditPair {
        CreditPair(const std::string& label,
                   std::uint32_t primary_a,
                   std::uint32_t secondary_a,
                   std::uint32_t primary_b,
                   std::uint32_t secondary_b);

        std::string label;
        RgbSurface card_a;
        RgbSurface card_b;
    };

    static std::uint32_t pack_rgb(int red, int green, int blue);
    static float clamp_unit(float value);
    static void blend_card(const RgbSurface& source,
                           RgbSurface& destination,
                           int dst_x,
                           int dst_y,
                           float amount);
    static void draw_card(RgbSurface& surface,
                          const std::string& label,
                          std::uint32_t primary,
                          std::uint32_t secondary);
    static void draw_text(RgbSurface& surface,
                          int x,
                          int y,
                          const std::string& text,
                          std::uint32_t color);
    void build_palette();
    void build_background(float scene_time_seconds);
    void render_credit_overlay(RgbSurface& surface, float scene_time_seconds);
    void select_credit(const std::string& message, float scene_time_seconds);

    IndexedSurface background_;
    std::vector<CreditPair> credits_;
    int active_credit_;
    float message_start_seconds_;
    float phase_ticks_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_SCENES_MUTE95_SCENE_H
