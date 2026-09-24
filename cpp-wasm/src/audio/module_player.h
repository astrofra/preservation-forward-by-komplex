#ifndef FORWARD_OFFLINE_AUDIO_MODULE_PLAYER_H
#define FORWARD_OFFLINE_AUDIO_MODULE_PLAYER_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace forward_offline {

struct SongPositionEvent {
    std::uint64_t sample_index;
    unsigned int song_position_hex;
};

struct SequenceAudioRender {
    std::vector<std::int16_t> interleaved_samples;
    std::vector<SongPositionEvent> song_positions;
};

// Starts at module sample zero (use intro or saari). State survives block boundaries.
class ModuleAudioStream {
public:
    ModuleAudioStream();
    ~ModuleAudioStream();
    bool open(const std::string& sequence_name, int sample_rate, std::string* error);
    bool render(std::size_t frames, SequenceAudioRender* block, std::string* error);
    std::uint64_t position() const;
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

bool module_position_sample(const std::string& sequence_name, int sample_rate,
                            unsigned int position, std::uint64_t* sample,
                            std::string* error);

bool resolve_sequence_frame_count_for_song_position(const std::string& sequence_name,
                                                    int fps,
                                                    int sample_rate,
                                                    unsigned int song_position_hex,
                                                    int post_roll_frames,
                                                    int* frame_count,
                                                    std::string* error_message);

bool render_sequence_module_audio(const std::string& sequence_name,
                                  int sample_rate,
                                  std::size_t sample_frames,
                                  SequenceAudioRender* render,
                                  std::string* error_message);

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_AUDIO_MODULE_PLAYER_H
