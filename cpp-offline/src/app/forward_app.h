#ifndef FORWARD_OFFLINE_APP_FORWARD_APP_H
#define FORWARD_OFFLINE_APP_FORWARD_APP_H

#include <cstdint>
#include <string>

#include "app/export_config.h"
#include "app/intro_script.h"
#include "audio/module_player.h"
#include "core/offline_timeline.h"
#include "render/rgb_surface.h"
#include "scenes/domina_routine.h"
#include "scenes/feta_scene.h"
#include "scenes/kukot_scene.h"
#include "scenes/maku_scene.h"
#include "scenes/mute95_scene.h"
#include "scenes/placeholder_scene.h"
#include "scenes/saari_scene.h"
#include "scenes/uppol_routine.h"
#include "scenes/watercube_scene.h"

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

    bool resolve_export_span(std::string* error_message);
    bool prepare_output(std::string* error_message) const;
    bool prepare_sequence_audio(std::string* error_message);
    bool write_log(std::string* error_message) const;
    bool write_sequence_audio(class WavWriter* wav_writer, std::string* error_message) const;
    bool is_intro_sequence() const;
    bool is_saari_sequence() const;
    bool is_kukot_sequence() const;
    bool is_maku_sequence() const;
    bool is_watercube_sequence() const;
    bool is_feta_sequence() const;
    bool is_uppol_sequence() const;
    bool initialize_sequence(std::string* error_message);
    void process_intro_script(std::uint64_t sample_index);
    void process_saari_script(std::uint64_t sample_index);
    void process_kukot_script(std::uint64_t sample_index);
    void process_maku_script(std::uint64_t sample_index);
    void process_watercube_script(std::uint64_t sample_index);
    void process_feta_script(std::uint64_t sample_index);
    void process_uppol_script(std::uint64_t sample_index);
    std::string song_position_string(unsigned int song_position) const;
    void execute_script_command(const ScriptCommand& command, double demo_time_seconds);
    void show_scene(const std::string& scene_name, double demo_time_seconds);
    void show_routine(const std::string& routine_name, double demo_time_seconds);
    void kill_renderable(const std::string& name);
    std::string frame_file_name(unsigned int frame_index) const;
    std::string next_script_time_hex(unsigned int frame_index) const;

    ExportConfig config_;
    OfflineTimeline timeline_;
    IntroScript intro_script_;
    KukotScript kukot_script_;
    MakuScript maku_script_;
    WatercubeScript watercube_script_;
    FetaScript feta_script_;
    SequenceAudioRender sequence_audio_render_;
    RgbSurface frame_buffer_;
    Mute95Scene mute95_scene_;
    DominaRoutine domina_routine_;
    SaariScene saari_scene_;
    KukotScene kukot_scene_;
    MakuScene maku_scene_;
    WatercubeScene watercube_scene_;
    FetaScene feta_scene_;
    UppolRoutine uppol_routine_;
    PlaceholderScene scene_;
    ActiveRenderable active_renderable_;
    std::string active_name_;
    double active_start_seconds_;
    std::size_t next_script_index_;
    std::size_t next_song_position_event_index_;
    unsigned int current_song_position_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_APP_FORWARD_APP_H
