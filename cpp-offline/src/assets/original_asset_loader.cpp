#include "assets/original_asset_loader.h"

#include <fstream>
#include <vector>

#define STBI_ONLY_JPEG
#define STBI_NO_STDIO
#include "stb/stb_image.h"

namespace forward_offline {

namespace {

std::uint32_t pack_rgb(std::uint8_t red, std::uint8_t green, std::uint8_t blue) {
    return static_cast<std::uint32_t>((static_cast<unsigned int>(red) << 20) |
                                      (static_cast<unsigned int>(green) << 10) |
                                      static_cast<unsigned int>(blue));
}

bool read_file(const std::string& path, std::vector<unsigned char>* bytes) {
    std::ifstream stream(path.c_str(), std::ios::binary);
    if (!stream) {
        return false;
    }

    stream.seekg(0, std::ios::end);
    const std::streamoff size = stream.tellg();
    stream.seekg(0, std::ios::beg);
    if (size < 0) {
        return false;
    }

    bytes->resize(static_cast<std::size_t>(size));
    if (size == 0) {
        return true;
    }

    stream.read(reinterpret_cast<char*>(&(*bytes)[0]), size);
    return stream.good();
}

std::uint16_t read_u16_le(const std::vector<unsigned char>& bytes, std::size_t offset) {
    return static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(bytes[offset]) |
        static_cast<std::uint16_t>(bytes[offset + 1]) << 8);
}

void skip_sub_blocks(const std::vector<unsigned char>& bytes, std::size_t* offset) {
    while (*offset < bytes.size()) {
        const std::size_t block_size = bytes[*offset];
        *offset += 1;
        if (block_size == 0) {
            return;
        }
        *offset += block_size;
    }
}

}  // namespace

bool load_original_jpeg_packed_rgb(const std::string& path,
                                   PackedRgbAsset* asset,
                                   std::string* error_message) {
    std::vector<unsigned char> file_bytes;
    if (!read_file(path, &file_bytes)) {
        if (error_message != NULL) {
            *error_message = "unable to read jpeg asset: " + path;
        }
        return false;
    }

    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* pixels = stbi_load_from_memory(&file_bytes[0],
                                                  static_cast<int>(file_bytes.size()),
                                                  &width,
                                                  &height,
                                                  &channels,
                                                  3);
    if (pixels == NULL) {
        if (error_message != NULL) {
            *error_message = "stb_image failed to decode jpeg asset: " + path;
        }
        return false;
    }

    asset->width = width;
    asset->height = height;
    const std::size_t pixel_count = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
    asset->packed_pixels.resize(pixel_count, 0);

    for (std::size_t index = 0; index < pixel_count; ++index) {
        const unsigned char red = pixels[index * 3];
        const unsigned char green = pixels[index * 3 + 1];
        const unsigned char blue = pixels[index * 3 + 2];
        asset->packed_pixels[index] = pack_rgb(red, green, blue);
    }

    stbi_image_free(pixels);
    return true;
}

bool load_original_gif_palette(const std::string& path,
                               std::vector<std::uint8_t>* palette_red,
                               std::vector<std::uint8_t>* palette_green,
                               std::vector<std::uint8_t>* palette_blue,
                               std::string* error_message) {
    std::vector<unsigned char> bytes;
    if (!read_file(path, &bytes) || bytes.size() < 13) {
        if (error_message != NULL) {
            *error_message = "unable to read gif asset: " + path;
        }
        return false;
    }

    if (!(bytes[0] == 'G' && bytes[1] == 'I' && bytes[2] == 'F')) {
        if (error_message != NULL) {
            *error_message = "invalid gif header: " + path;
        }
        return false;
    }

    palette_red->assign(256, 0);
    palette_green->assign(256, 0);
    palette_blue->assign(256, 0);

    std::size_t offset = 13;
    bool has_global_palette = (bytes[10] & 0x80U) != 0;
    std::size_t global_palette_entries = 0;
    if (has_global_palette) {
        global_palette_entries = static_cast<std::size_t>(1ULL << ((bytes[10] & 0x07U) + 1U));
        if (offset + global_palette_entries * 3 > bytes.size()) {
            if (error_message != NULL) {
                *error_message = "truncated global gif palette: " + path;
            }
            return false;
        }
        for (std::size_t index = 0; index < global_palette_entries; ++index) {
            (*palette_red)[index] = bytes[offset + index * 3];
            (*palette_green)[index] = bytes[offset + index * 3 + 1];
            (*palette_blue)[index] = bytes[offset + index * 3 + 2];
        }
        offset += global_palette_entries * 3;
        return true;
    }

    while (offset < bytes.size()) {
        const unsigned char marker = bytes[offset++];
        if (marker == 0x21U) {
            if (offset >= bytes.size()) {
                break;
            }
            offset += 1;
            skip_sub_blocks(bytes, &offset);
            continue;
        }
        if (marker == 0x2CU) {
            if (offset + 9 > bytes.size()) {
                break;
            }
            const unsigned char packed = bytes[offset + 8];
            offset += 9;
            if ((packed & 0x80U) == 0) {
                break;
            }

            const std::size_t local_palette_entries = static_cast<std::size_t>(1ULL << ((packed & 0x07U) + 1U));
            if (offset + local_palette_entries * 3 > bytes.size()) {
                break;
            }
            for (std::size_t index = 0; index < local_palette_entries; ++index) {
                (*palette_red)[index] = bytes[offset + index * 3];
                (*palette_green)[index] = bytes[offset + index * 3 + 1];
                (*palette_blue)[index] = bytes[offset + index * 3 + 2];
            }
            return true;
        }
        if (marker == 0x3BU) {
            break;
        }
    }

    if (error_message != NULL) {
        *error_message = "no usable gif palette found: " + path;
    }
    return false;
}

}  // namespace forward_offline
