#include "app/song_position_transport.h"

#include <sstream>

namespace forward_offline {

SongPositionTransport::SongPositionTransport(int frames_per_row, int rows_per_order)
    : frames_per_row_(frames_per_row),
      rows_per_order_(rows_per_order),
      error_message_() {
    if (frames_per_row_ <= 0) {
        error_message_ = "frames_per_row must be positive";
        return;
    }
    if (rows_per_order_ <= 0 || rows_per_order_ > 255) {
        error_message_ = "rows_per_order must be in 1..255";
    }
}

bool SongPositionTransport::is_valid() const {
    return error_message_.empty();
}

const std::string& SongPositionTransport::error_message() const {
    return error_message_;
}

unsigned int SongPositionTransport::song_position_for_frame(unsigned int frame_index) const {
    const unsigned int row_index = frame_index / static_cast<unsigned int>(frames_per_row_);
    const unsigned int order = row_index / static_cast<unsigned int>(rows_per_order_);
    const unsigned int row = row_index % static_cast<unsigned int>(rows_per_order_);
    return (order << 8) | row;
}

std::string SongPositionTransport::song_position_string(unsigned int song_position) const {
    std::ostringstream builder;
    builder << "0x" << std::hex << song_position;
    return builder.str();
}

}  // namespace forward_offline
