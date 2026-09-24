#ifndef FORWARD_OFFLINE_RENDER_PNG_WRITER_H
#define FORWARD_OFFLINE_RENDER_PNG_WRITER_H

#include <string>

namespace forward_offline {
class RgbSurface;
bool write_png24(const std::string& path, const RgbSurface& surface, std::string* error);
}

#endif
