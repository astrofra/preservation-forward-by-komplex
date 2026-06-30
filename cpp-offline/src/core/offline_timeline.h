#ifndef FORWARD_OFFLINE_CORE_OFFLINE_TIMELINE_H
#define FORWARD_OFFLINE_CORE_OFFLINE_TIMELINE_H

#include <cstdint>
#include <string>

namespace forward_offline {

class OfflineTimeline {
public:
    OfflineTimeline(int fps, int sample_rate);

    bool is_valid() const;
    const std::string& error_message() const;

    int fps() const;
    int sample_rate() const;
    int samples_per_frame() const;
    double frame_duration_seconds() const;
    std::uint64_t sample_index_for_frame(std::uint32_t frame_index) const;
    std::uint64_t total_samples_for_frames(std::uint32_t frame_count) const;
    std::uint64_t frame_time_ms(std::uint32_t frame_index) const;
    double frame_time_seconds(std::uint32_t frame_index) const;

private:
    int fps_;
    int sample_rate_;
    int samples_per_frame_;
    std::string error_message_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_CORE_OFFLINE_TIMELINE_H
