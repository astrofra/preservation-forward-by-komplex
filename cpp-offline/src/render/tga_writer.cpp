#include "render/tga_writer.h"

#include <fstream>

#include "render/rgb_surface.h"

namespace forward_offline {

bool write_tga24(const std::string& path, const RgbSurface& surface, std::string* error_message) {
    std::ofstream stream(path.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    if (!stream) {
        if (error_message != NULL) {
            *error_message = "unable to write tga frame: " + path;
        }
        return false;
    }

    unsigned char header[18] = {0};
    header[2] = 2;
    header[12] = static_cast<unsigned char>(surface.width() & 0xff);
    header[13] = static_cast<unsigned char>((surface.width() >> 8) & 0xff);
    header[14] = static_cast<unsigned char>(surface.height() & 0xff);
    header[15] = static_cast<unsigned char>((surface.height() >> 8) & 0xff);
    header[16] = 24;
    header[17] = 0x20;
    stream.write(reinterpret_cast<const char*>(header), sizeof(header));

    const std::vector<std::uint32_t>& pixels = surface.pixels();
    for (std::size_t index = 0; index < pixels.size(); ++index) {
        const std::uint32_t rgb = pixels[index];
        const char bytes[3] = {
            static_cast<char>(rgb & 0xff),
            static_cast<char>((rgb >> 8) & 0xff),
            static_cast<char>((rgb >> 16) & 0xff)
        };
        stream.write(bytes, 3);
    }

    if (!stream) {
        if (error_message != NULL) {
            *error_message = "unable to finish tga frame: " + path;
        }
        return false;
    }

    return true;
}

}  // namespace forward_offline
