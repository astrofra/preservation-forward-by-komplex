#ifndef FORWARD_OFFLINE_APP_SONG_POSITION_TRANSPORT_H
#define FORWARD_OFFLINE_APP_SONG_POSITION_TRANSPORT_H

#include <string>

namespace forward_offline {

class SongPositionTransport {
public:
    SongPositionTransport(int frames_per_row, int rows_per_order);

    bool is_valid() const;
    const std::string& error_message() const;

    unsigned int song_position_for_frame(unsigned int frame_index) const;
    std::string song_position_string(unsigned int song_position) const;

private:
    int frames_per_row_;
    int rows_per_order_;
    std::string error_message_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_APP_SONG_POSITION_TRANSPORT_H
