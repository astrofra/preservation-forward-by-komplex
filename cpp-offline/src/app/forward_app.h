#ifndef FORWARD_OFFLINE_APP_FORWARD_APP_H
#define FORWARD_OFFLINE_APP_FORWARD_APP_H

#include <string>

#include "app/export_config.h"
#include "app/intro_script.h"
#include "app/song_position_transport.h"
#include "core/offline_timeline.h"
#include "render/rgb_surface.h"
#include "scenes/domina_routine.h"
#include "scenes/mute95_scene.h"
#include "scenes/placeholder_scene.h"

namespace forward_offline {

class ForwardApp {
public:
    explicit ForwardApp(const ExportConfig& config);

    int run();

private:
    enum class ActiveRenderable {
        none,
        scene,
        routine
    };

    bool prepare_output(std::string* error_message) const;
    bool write_log(std::string* error_message) const;
    bool is_intro_sequence() const;
    bool initialize_sequence(std::string* error_message);
    void process_intro_script(unsigned int frame_index);
    void execute_script_command(const ScriptCommand& command, double demo_time_seconds);
    void show_scene(const std::string& scene_name, double demo_time_seconds);
    void show_routine(const std::string& routine_name, double demo_time_seconds);
    void kill_renderable(const std::string& name);
    std::string frame_file_name(unsigned int frame_index) const;
    std::string next_script_time_hex(unsigned int frame_index) const;

    ExportConfig config_;
    OfflineTimeline timeline_;
    SongPositionTransport intro_transport_;
    IntroScript intro_script_;
    RgbSurface frame_buffer_;
    Mute95Scene mute95_scene_;
    DominaRoutine domina_routine_;
    PlaceholderScene scene_;
    ActiveRenderable active_renderable_;
    std::string active_name_;
    double active_start_seconds_;
    std::size_t next_script_index_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_APP_FORWARD_APP_H
