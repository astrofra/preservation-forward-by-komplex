#include "assets/original_asset_loader.h"

#include <cstring>
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

bool read_sub_blocks(const std::vector<unsigned char>& bytes,
                     std::size_t* offset,
                     std::vector<unsigned char>* output) {
    output->clear();
    while (*offset < bytes.size()) {
        const std::size_t block_size = bytes[*offset];
        *offset += 1;
        if (block_size == 0) {
            return true;
        }
        if (*offset + block_size > bytes.size()) {
            return false;
        }
        output->insert(output->end(),
                       bytes.begin() + static_cast<std::ptrdiff_t>(*offset),
                       bytes.begin() + static_cast<std::ptrdiff_t>(*offset + block_size));
        *offset += block_size;
    }
    return false;
}

bool read_gif_code(const std::vector<unsigned char>& data,
                   std::size_t bit_offset,
                   int code_size,
                   int* code) {
    const std::size_t available_bits = data.size() * 8;
    if (bit_offset + static_cast<std::size_t>(code_size) > available_bits) {
        return false;
    }

    int value = 0;
    for (int bit_index = 0; bit_index < code_size; ++bit_index) {
        const std::size_t current_bit = bit_offset + static_cast<std::size_t>(bit_index);
        const unsigned char byte = data[current_bit / 8];
        const int bit = (byte >> (current_bit % 8)) & 1;
        value |= bit << bit_index;
    }
    *code = value;
    return true;
}

bool decode_gif_lzw(const std::vector<unsigned char>& compressed,
                    int minimum_code_size,
                    std::size_t expected_size,
                    std::vector<std::uint8_t>* output) {
    if (minimum_code_size < 2 || minimum_code_size > 8) {
        return false;
    }

    const int clear_code = 1 << minimum_code_size;
    const int end_code = clear_code + 1;
    int code_size = minimum_code_size + 1;
    int next_code = end_code + 1;
    int previous_code = -1;
    std::uint8_t first_value = 0;
    std::size_t bit_offset = 0;

    std::vector<int> prefixes(4096, -1);
    std::vector<std::uint8_t> suffixes(4096, 0);
    std::vector<std::uint8_t> stack(4096, 0);

    for (int code = 0; code < clear_code; ++code) {
        suffixes[code] = static_cast<std::uint8_t>(code);
    }

    output->clear();
    output->reserve(expected_size);

    while (true) {
        int code = 0;
        if (!read_gif_code(compressed, bit_offset, code_size, &code)) {
            break;
        }
        bit_offset += static_cast<std::size_t>(code_size);

        if (code == clear_code) {
            code_size = minimum_code_size + 1;
            next_code = end_code + 1;
            previous_code = -1;
            continue;
        }
        if (code == end_code) {
            break;
        }
        if (code > 4095) {
            return false;
        }

        int decoded_code = code;
        int stack_size = 0;
        if (code >= next_code) {
            if (previous_code < 0) {
                return false;
            }
            stack[stack_size++] = first_value;
            decoded_code = previous_code;
        }

        while (decoded_code >= clear_code) {
            if (decoded_code > 4095 || stack_size >= 4096) {
                return false;
            }
            stack[stack_size++] = suffixes[decoded_code];
            decoded_code = prefixes[decoded_code];
            if (decoded_code < 0) {
                return false;
            }
        }

        first_value = suffixes[decoded_code];
        stack[stack_size++] = first_value;

        while (stack_size > 0) {
            output->push_back(stack[--stack_size]);
            if (output->size() == expected_size) {
                break;
            }
        }
        if (output->size() == expected_size) {
            break;
        }

        if (previous_code >= 0 && next_code < 4096) {
            prefixes[next_code] = previous_code;
            suffixes[next_code] = first_value;
            ++next_code;
            if (next_code == (1 << code_size) && code_size < 12) {
                ++code_size;
            }
        }

        previous_code = code;
    }

    return output->size() >= expected_size;
}

void assign_palette_entries(const std::vector<unsigned char>& palette_bytes,
                            std::vector<std::uint8_t>* red,
                            std::vector<std::uint8_t>* green,
                            std::vector<std::uint8_t>* blue) {
    red->assign(256, 0);
    green->assign(256, 0);
    blue->assign(256, 0);

    const std::size_t entry_count = palette_bytes.size() / 3;
    for (std::size_t index = 0; index < entry_count && index < 256; ++index) {
        (*red)[index] = palette_bytes[index * 3];
        (*green)[index] = palette_bytes[index * 3 + 1];
        (*blue)[index] = palette_bytes[index * 3 + 2];
    }
}

bool decode_gif_indexed(const std::vector<unsigned char>& bytes,
                        IndexedAsset* asset,
                        std::string* error_message,
                        const std::string& path) {
    if (bytes.size() < 13) {
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

    const int logical_width = static_cast<int>(read_u16_le(bytes, 6));
    const int logical_height = static_cast<int>(read_u16_le(bytes, 8));
    const unsigned char packed_fields = bytes[10];
    const std::uint8_t background_index = bytes[11];

    std::vector<unsigned char> global_palette_bytes;
    std::size_t offset = 13;
    if ((packed_fields & 0x80U) != 0) {
        const std::size_t palette_entries = static_cast<std::size_t>(1ULL << ((packed_fields & 0x07U) + 1U));
        if (offset + palette_entries * 3 > bytes.size()) {
            if (error_message != NULL) {
                *error_message = "truncated global gif palette: " + path;
            }
            return false;
        }
        global_palette_bytes.assign(bytes.begin() + static_cast<std::ptrdiff_t>(offset),
                                    bytes.begin() + static_cast<std::ptrdiff_t>(offset + palette_entries * 3));
        offset += palette_entries * 3;
    }

    int transparent_index = -1;
    while (offset < bytes.size()) {
        const unsigned char marker = bytes[offset++];
        if (marker == 0x21U) {
            if (offset >= bytes.size()) {
                break;
            }
            const unsigned char extension_label = bytes[offset++];
            if (extension_label == 0xF9U) {
                if (offset >= bytes.size()) {
                    break;
                }
                const std::size_t block_size = bytes[offset++];
                if (block_size < 4 || offset + block_size > bytes.size()) {
                    break;
                }
                const unsigned char graphic_control_packed = bytes[offset];
                if ((graphic_control_packed & 0x01U) != 0 && block_size >= 4) {
                    transparent_index = static_cast<int>(bytes[offset + 3]);
                }
                offset += block_size;
                if (offset < bytes.size() && bytes[offset] == 0) {
                    ++offset;
                }
                continue;
            }
            skip_sub_blocks(bytes, &offset);
            continue;
        }

        if (marker == 0x2CU) {
            if (offset + 9 > bytes.size()) {
                break;
            }

            const int image_left = static_cast<int>(read_u16_le(bytes, offset));
            const int image_top = static_cast<int>(read_u16_le(bytes, offset + 2));
            const int image_width = static_cast<int>(read_u16_le(bytes, offset + 4));
            const int image_height = static_cast<int>(read_u16_le(bytes, offset + 6));
            const unsigned char image_packed = bytes[offset + 8];
            const bool interlaced = (image_packed & 0x40U) != 0;
            offset += 9;

            std::vector<unsigned char> active_palette_bytes = global_palette_bytes;
            if ((image_packed & 0x80U) != 0) {
                const std::size_t palette_entries = static_cast<std::size_t>(1ULL << ((image_packed & 0x07U) + 1U));
                if (offset + palette_entries * 3 > bytes.size()) {
                    break;
                }
                active_palette_bytes.assign(bytes.begin() + static_cast<std::ptrdiff_t>(offset),
                                            bytes.begin() + static_cast<std::ptrdiff_t>(offset + palette_entries * 3));
                offset += palette_entries * 3;
            }

            if (offset >= bytes.size()) {
                break;
            }
            const int minimum_code_size = bytes[offset++];

            std::vector<unsigned char> compressed_image_data;
            if (!read_sub_blocks(bytes, &offset, &compressed_image_data)) {
                break;
            }

            std::vector<std::uint8_t> decoded_indices;
            const std::size_t image_pixel_count =
                static_cast<std::size_t>(image_width) * static_cast<std::size_t>(image_height);
            if (!decode_gif_lzw(compressed_image_data,
                                minimum_code_size,
                                image_pixel_count,
                                &decoded_indices)) {
                break;
            }

            asset->width = logical_width;
            asset->height = logical_height;
            assign_palette_entries(active_palette_bytes,
                                   &asset->palette_red,
                                   &asset->palette_green,
                                   &asset->palette_blue);
            asset->pixels.assign(static_cast<std::size_t>(logical_width) *
                                     static_cast<std::size_t>(logical_height),
                                 background_index);

            static const int kInterlaceStarts[4] = {0, 4, 2, 1};
            static const int kInterlaceSteps[4] = {8, 8, 4, 2};
            std::size_t source_index = 0;
            if (interlaced) {
                for (int pass = 0; pass < 4; ++pass) {
                    for (int row = kInterlaceStarts[pass]; row < image_height; row += kInterlaceSteps[pass]) {
                        const int dst_y = image_top + row;
                        if (dst_y < 0 || dst_y >= logical_height) {
                            source_index += static_cast<std::size_t>(image_width);
                            continue;
                        }
                        for (int column = 0; column < image_width; ++column) {
                            const int dst_x = image_left + column;
                            const std::uint8_t pixel = decoded_indices[source_index++];
                            if (dst_x < 0 || dst_x >= logical_width) {
                                continue;
                            }
                            if (transparent_index >= 0 && pixel == static_cast<std::uint8_t>(transparent_index)) {
                                continue;
                            }
                            asset->pixels[static_cast<std::size_t>(dst_y) * static_cast<std::size_t>(logical_width) +
                                          static_cast<std::size_t>(dst_x)] = pixel;
                        }
                    }
                }
            } else {
                for (int row = 0; row < image_height; ++row) {
                    const int dst_y = image_top + row;
                    for (int column = 0; column < image_width; ++column) {
                        const int dst_x = image_left + column;
                        const std::uint8_t pixel = decoded_indices[source_index++];
                        if (dst_x < 0 || dst_x >= logical_width || dst_y < 0 || dst_y >= logical_height) {
                            continue;
                        }
                        if (transparent_index >= 0 && pixel == static_cast<std::uint8_t>(transparent_index)) {
                            continue;
                        }
                        asset->pixels[static_cast<std::size_t>(dst_y) * static_cast<std::size_t>(logical_width) +
                                      static_cast<std::size_t>(dst_x)] = pixel;
                    }
                }
            }

            return true;
        }

        if (marker == 0x3BU) {
            break;
        }
    }

    if (error_message != NULL) {
        *error_message = "unable to decode indexed gif asset: " + path;
    }
    return false;
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
    IndexedAsset asset;
    if (!read_file(path, &bytes) || !decode_gif_indexed(bytes, &asset, error_message, path)) {
        return false;
    }

    *palette_red = asset.palette_red;
    *palette_green = asset.palette_green;
    *palette_blue = asset.palette_blue;
    return true;
}

bool load_original_gif_indexed(const std::string& path,
                               IndexedAsset* asset,
                               std::string* error_message) {
    std::vector<unsigned char> bytes;
    if (!read_file(path, &bytes)) {
        if (error_message != NULL) {
            *error_message = "unable to read gif asset: " + path;
        }
        return false;
    }

    return decode_gif_indexed(bytes, asset, error_message, path);
}

}  // namespace forward_offline
