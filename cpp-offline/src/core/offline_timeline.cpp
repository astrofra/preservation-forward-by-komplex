#include "core/offline_timeline.h"

namespace forward_offline {

OfflineTimeline::OfflineTimeline(int fps, int sample_rate)
    : fps_(fps),
      sample_rate_(sample_rate),
      samples_per_frame_(0),
      error_message_() {
    if (fps_ <= 0) {
        error_message_ = "fps must be positive";
        return;
    }
    if (sample_rate_ <= 0) {
        error_message_ = "sample rate must be positive";
        return;
    }
    if (sample_rate_ % fps_ != 0) {
        error_message_ = "sample rate must be divisible by fps";
        return;
    }

    samples_per_frame_ = sample_rate_ / fps_;
}

bool OfflineTimeline::is_valid() const {
    return error_message_.empty();
}

const std::string& OfflineTimeline::error_message() const {
    return error_message_;
}

int OfflineTimeline::fps() const {
    return fps_;
}

int OfflineTimeline::sample_rate() const {
    return sample_rate_;
}

int OfflineTimeline::samples_per_frame() const {
    return samples_per_frame_;
}

double OfflineTimeline::frame_duration_seconds() const {
    return 1.0 / static_cast<double>(fps_);
}

std::uint64_t OfflineTimeline::sample_index_for_frame(std::uint32_t frame_index) const {
    return static_cast<std::uint64_t>(frame_index) * static_cast<std::uint64_t>(samples_per_frame_);
}

std::uint64_t OfflineTimeline::total_samples_for_frames(std::uint32_t frame_count) const {
    return static_cast<std::uint64_t>(frame_count) * static_cast<std::uint64_t>(samples_per_frame_);
}

std::uint64_t OfflineTimeline::frame_time_ms(std::uint32_t frame_index) const {
    const std::uint64_t sample_index = sample_index_for_frame(frame_index);
    return (sample_index * 1000ULL) / static_cast<std::uint64_t>(sample_rate_);
}

double OfflineTimeline::frame_time_seconds(std::uint32_t frame_index) const {
    return static_cast<double>(sample_index_for_frame(frame_index)) /
           static_cast<double>(sample_rate_);
}

}  // namespace forward_offline
