#include "audio/wav_writer.h"

#include <vector>

namespace forward_offline {

WavWriter::WavWriter()
    : stream_(),
      data_bytes_(0),
      sample_rate_(0),
      channel_count_(0),
      bits_per_sample_(0),
      is_open_(false),
      finalized_(false) {
}

WavWriter::~WavWriter() {
    if (is_open_ && !finalized_) {
        finalize(NULL);
    }
}

bool WavWriter::open(const std::string& path,
                     int sample_rate,
                     int channel_count,
                     int bits_per_sample,
                     std::string* error_message) {
    stream_.open(path.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    if (!stream_) {
        if (error_message != NULL) {
            *error_message = "unable to open wav output: " + path;
        }
        return false;
    }

    sample_rate_ = sample_rate;
    channel_count_ = channel_count;
    bits_per_sample_ = bits_per_sample;
    data_bytes_ = 0;
    finalized_ = false;
    is_open_ = true;

    return write_header_placeholder(error_message);
}

bool WavWriter::write_pcm_s16(const std::vector<std::int16_t>& interleaved_samples,
                              std::string* error_message) {
    if (!is_open_) {
        if (error_message != NULL) {
            *error_message = "wav writer is not open";
        }
        return false;
    }
    if (bits_per_sample_ != 16) {
        if (error_message != NULL) {
            *error_message = "write_pcm_s16 requires a 16-bit wav output";
        }
        return false;
    }

    if (interleaved_samples.empty()) {
        return true;
    }

    std::vector<char> bytes(interleaved_samples.size() * 2U, 0);
    for (std::size_t index = 0; index < interleaved_samples.size(); ++index) {
        const std::uint16_t sample_bits =
            static_cast<std::uint16_t>(interleaved_samples[index]);
        bytes[index * 2U] = static_cast<char>(sample_bits & 0xffU);
        bytes[index * 2U + 1U] = static_cast<char>((sample_bits >> 8) & 0xffU);
    }

    stream_.write(&bytes[0], static_cast<std::streamsize>(bytes.size()));
    if (!stream_) {
        if (error_message != NULL) {
            *error_message = "unable to write wav pcm data";
        }
        return false;
    }

    data_bytes_ += static_cast<std::uint64_t>(bytes.size());
    return true;
}

bool WavWriter::write_silence(std::size_t sample_frames, std::string* error_message) {
    if (!is_open_) {
        if (error_message != NULL) {
            *error_message = "wav writer is not open";
        }
        return false;
    }

    const std::size_t bytes_per_frame =
        static_cast<std::size_t>(channel_count_ * (bits_per_sample_ / 8));
    const std::size_t chunk_frames = 4096;
    std::vector<char> zeros(chunk_frames * bytes_per_frame, 0);

    std::size_t remaining = sample_frames;
    while (remaining > 0) {
        const std::size_t frames_to_write = remaining > chunk_frames ? chunk_frames : remaining;
        const std::size_t bytes_to_write = frames_to_write * bytes_per_frame;
        stream_.write(&zeros[0], static_cast<std::streamsize>(bytes_to_write));
        if (!stream_) {
            if (error_message != NULL) {
                *error_message = "unable to write wav data";
            }
            return false;
        }

        data_bytes_ += bytes_to_write;
        remaining -= frames_to_write;
    }

    return true;
}

bool WavWriter::finalize(std::string* error_message) {
    if (!is_open_) {
        return true;
    }
    if (finalized_) {
        return true;
    }

    if (!patch_header(error_message)) {
        return false;
    }

    stream_.flush();
    stream_.close();
    finalized_ = true;
    is_open_ = false;
    return true;
}

bool WavWriter::write_header_placeholder(std::string* error_message) {
    stream_.write("RIFF", 4);
    write_u32(stream_, 0);
    stream_.write("WAVE", 4);

    stream_.write("fmt ", 4);
    write_u32(stream_, 16);
    write_u16(stream_, 1);
    write_u16(stream_, static_cast<std::uint16_t>(channel_count_));
    write_u32(stream_, static_cast<std::uint32_t>(sample_rate_));
    write_u32(stream_, static_cast<std::uint32_t>(
                           sample_rate_ * channel_count_ * (bits_per_sample_ / 8)));
    write_u16(stream_, static_cast<std::uint16_t>(channel_count_ * (bits_per_sample_ / 8)));
    write_u16(stream_, static_cast<std::uint16_t>(bits_per_sample_));

    stream_.write("data", 4);
    write_u32(stream_, 0);

    if (!stream_) {
        if (error_message != NULL) {
            *error_message = "unable to write wav header";
        }
        return false;
    }

    return true;
}

bool WavWriter::patch_header(std::string* error_message) {
    if (data_bytes_ > 0xffffffffULL - 36ULL) {
        if (error_message != NULL) {
            *error_message = "wav output too large for RIFF/WAVE";
        }
        return false;
    }

    stream_.seekp(4, std::ios::beg);
    write_u32(stream_, static_cast<std::uint32_t>(36ULL + data_bytes_));
    stream_.seekp(40, std::ios::beg);
    write_u32(stream_, static_cast<std::uint32_t>(data_bytes_));
    stream_.seekp(0, std::ios::end);

    if (!stream_) {
        if (error_message != NULL) {
            *error_message = "unable to finalize wav header";
        }
        return false;
    }

    return true;
}

void WavWriter::write_u16(std::ofstream& stream, std::uint16_t value) {
    const char bytes[2] = {
        static_cast<char>(value & 0xff),
        static_cast<char>((value >> 8) & 0xff)
    };
    stream.write(bytes, 2);
}

void WavWriter::write_u32(std::ofstream& stream, std::uint32_t value) {
    const char bytes[4] = {
        static_cast<char>(value & 0xff),
        static_cast<char>((value >> 8) & 0xff),
        static_cast<char>((value >> 16) & 0xff),
        static_cast<char>((value >> 24) & 0xff)
    };
    stream.write(bytes, 4);
}

}  // namespace forward_offline
