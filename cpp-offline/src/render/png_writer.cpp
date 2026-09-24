#include "render/png_writer.h"
#include "render/rgb_surface.h"
#include <fstream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_WRITE_NO_STDIO
#if defined(_MSC_VER)
#pragma warning(push, 0)
#endif
#include "stb/stb_image_write.h"
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace forward_offline {
namespace {
void write_bytes(void* context, void* data, int size) {
    static_cast<std::ofstream*>(context)->write(static_cast<const char*>(data), size);
}
}

bool write_png24(const std::string& path, const RgbSurface& surface, std::string* error) {
    std::ofstream stream(path.c_str(), std::ios::binary | std::ios::trunc);
    if (!stream) {
        *error = "unable to open PNG: " + path;
        return false;
    }
    std::vector<unsigned char> rgb(surface.pixels().size() * 3);
    for (std::size_t i = 0; i < surface.pixels().size(); ++i) {
        const std::uint32_t p = surface.pixels()[i];
        rgb[i * 3] = static_cast<unsigned char>(p >> 16);
        rgb[i * 3 + 1] = static_cast<unsigned char>(p >> 8);
        rgb[i * 3 + 2] = static_cast<unsigned char>(p);
    }
    const int result = stbi_write_png_to_func(write_bytes, &stream, surface.width(), surface.height(),
                                               3, rgb.data(), surface.width() * 3);
    stream.close();
    if (!result || !stream) {
        *error = "unable to finish PNG: " + path;
        return false;
    }
    return true;
}
}
