#ifndef FORWARD_OFFLINE_AUDIO_MODULE_PLAYER_H
#define FORWARD_OFFLINE_AUDIO_MODULE_PLAYER_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace forward_offline {

bool render_sequence_module_audio(const std::string& sequence_name,
                                  int sample_rate,
                                  std::size_t sample_frames,
                                  std::vector<std::int16_t>* interleaved_samples,
                                  std::string* error_message);

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_AUDIO_MODULE_PLAYER_H
