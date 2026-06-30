#ifndef FORWARD_OFFLINE_RENDER_TGA_WRITER_H
#define FORWARD_OFFLINE_RENDER_TGA_WRITER_H

#include <string>

namespace forward_offline {

class RgbSurface;

bool write_tga24(const std::string& path, const RgbSurface& surface, std::string* error_message);

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_RENDER_TGA_WRITER_H
