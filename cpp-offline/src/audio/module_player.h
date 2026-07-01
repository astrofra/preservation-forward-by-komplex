#ifndef FORWARD_OFFLINE_AUDIO_MODULE_PLAYER_H
#define FORWARD_OFFLINE_AUDIO_MODULE_PLAYER_H

#include <cstddef>
#include <cstdint>
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
