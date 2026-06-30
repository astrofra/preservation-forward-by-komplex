#include "assets/normalized_asset_loader.h"

#include <fstream>

namespace forward_offline {

namespace {

bool read_exact(std::ifstream& stream, char* buffer, std::streamsize size) {
    stream.read(buffer, size);
    return stream.good();
}

std::uint16_t read_u16_le(std::ifstream& stream) {
    unsigned char bytes[2] = {0};
    stream.read(reinterpret_cast<char*>(bytes), 2);
    return static_cast<std::uint16_t>(bytes[0] | (bytes[1] << 8));
}

std::uint32_t read_u32_le(std::ifstream& stream) {
    unsigned char bytes[4] = {0};
    stream.read(reinterpret_cast<char*>(bytes), 4);
    return static_cast<std::uint32_t>(bytes[0] |
                                      (bytes[1] << 8) |
                                      (bytes[2] << 16) |
                                      (bytes[3] << 24));
}

}  // namespace

bool load_packed_rgb_asset(const std::string& path, PackedRgbAsset* asset, std::string* error_message) {
    std::ifstream stream(path.c_str(), std::ios::binary);
    if (!stream) {
        if (error_message != NULL) {
            *error_message = "unable to open packed rgb asset: " + path;
        }
        return false;
    }

    char magic[4] = {0};
    if (!read_exact(stream, magic, 4) ||
        magic[0] != 'F' || magic[1] != 'R' || magic[2] != 'G' || magic[3] != 'B') {
        if (error_message != NULL) {
            *error_message = "invalid packed rgb asset header: " + path;
        }
        return false;
    }

    asset->width = static_cast<int>(read_u16_le(stream));
    asset->height = static_cast<int>(read_u16_le(stream));
    const std::size_t pixel_count =
        static_cast<std::size_t>(asset->width) * static_cast<std::size_t>(asset->height);
    asset->packed_pixels.resize(pixel_count, 0);

    for (std::size_t index = 0; index < pixel_count; ++index) {
        asset->packed_pixels[index] = read_u32_le(stream);
    }

    if (!stream) {
        if (error_message != NULL) {
            *error_message = "truncated packed rgb asset: " + path;
        }
        return false;
    }

    return true;
}

bool load_indexed_asset(const std::string& path, IndexedAsset* asset, std::string* error_message) {
    std::ifstream stream(path.c_str(), std::ios::binary);
    if (!stream) {
        if (error_message != NULL) {
            *error_message = "unable to open indexed asset: " + path;
        }
        return false;
    }

    char magic[4] = {0};
    if (!read_exact(stream, magic, 4) ||
        magic[0] != 'F' || magic[1] != 'I' || magic[2] != 'D' || magic[3] != 'X') {
        if (error_message != NULL) {
            *error_message = "invalid indexed asset header: " + path;
        }
        return false;
    }

    asset->width = static_cast<int>(read_u16_le(stream));
    asset->height = static_cast<int>(read_u16_le(stream));
    asset->palette_red.resize(256, 0);
    asset->palette_green.resize(256, 0);
    asset->palette_blue.resize(256, 0);
    asset->pixels.resize(static_cast<std::size_t>(asset->width) * static_cast<std::size_t>(asset->height), 0);

    stream.read(reinterpret_cast<char*>(&asset->palette_red[0]), 256);
    stream.read(reinterpret_cast<char*>(&asset->palette_green[0]), 256);
    stream.read(reinterpret_cast<char*>(&asset->palette_blue[0]), 256);
    stream.read(reinterpret_cast<char*>(&asset->pixels[0]),
                static_cast<std::streamsize>(asset->pixels.size()));

    if (!stream) {
        if (error_message != NULL) {
            *error_message = "truncated indexed asset: " + path;
        }
        return false;
    }

    return true;
}

}  // namespace forward_offline
