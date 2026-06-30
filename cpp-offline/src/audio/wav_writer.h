#ifndef FORWARD_OFFLINE_AUDIO_WAV_WRITER_H
#define FORWARD_OFFLINE_AUDIO_WAV_WRITER_H

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>

namespace forward_offline {

class WavWriter {
public:
    WavWriter();
    ~WavWriter();

    bool open(const std::string& path,
              int sample_rate,
              int channel_count,
              int bits_per_sample,
              std::string* error_message);
    bool write_silence(std::size_t sample_frames, std::string* error_message);
    bool finalize(std::string* error_message);

private:
    bool write_header_placeholder(std::string* error_message);
    bool patch_header(std::string* error_message);
    static void write_u16(std::ofstream& stream, std::uint16_t value);
    static void write_u32(std::ofstream& stream, std::uint32_t value);

    std::ofstream stream_;
    std::uint64_t data_bytes_;
    int sample_rate_;
    int channel_count_;
    int bits_per_sample_;
    bool is_open_;
    bool finalized_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_AUDIO_WAV_WRITER_H
