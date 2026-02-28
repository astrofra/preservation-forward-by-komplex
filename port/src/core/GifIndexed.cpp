#include "GifIndexed.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

namespace forward::core {
namespace {

bool ReadFileBytes(const std::string& path, std::vector<uint8_t>* out_bytes) {
  if (!out_bytes) {
    return false;
  }
  std::ifstream input(path, std::ios::binary);
  if (!input.is_open()) {
    return false;
  }
  input.seekg(0, std::ios::end);
  const std::streamoff size = input.tellg();
  if (size <= 0) {
    return false;
  }
  input.seekg(0, std::ios::beg);
  out_bytes->resize(static_cast<size_t>(size));
  input.read(reinterpret_cast<char*>(out_bytes->data()), size);
  return input.good() || input.gcount() == size;
}

bool ReadU16LE(const std::vector<uint8_t>& bytes, size_t* offset, uint16_t* out_value) {
  if (!offset || !out_value || *offset + 2 > bytes.size()) {
    return false;
  }
  *out_value =
      static_cast<uint16_t>(bytes[*offset]) |
      static_cast<uint16_t>(static_cast<uint16_t>(bytes[*offset + 1]) << 8u);
  *offset += 2;
  return true;
}

bool SkipGifSubBlocks(const std::vector<uint8_t>& bytes, size_t* offset) {
  if (!offset) {
    return false;
  }
  while (*offset < bytes.size()) {
    const uint8_t block_size = bytes[*offset];
    *offset += 1;
    if (block_size == 0) {
      return true;
    }
    if (*offset + block_size > bytes.size()) {
      return false;
    }
    *offset += block_size;
  }
  return false;
}

bool ReadColorTable(const std::vector<uint8_t>& bytes,
                    size_t* offset,
                    int color_count,
                    std::array<uint8_t, 256>* out_r,
                    std::array<uint8_t, 256>* out_g,
                    std::array<uint8_t, 256>* out_b) {
  if (!offset || !out_r || !out_g || !out_b || color_count <= 0 || color_count > 256) {
    return false;
  }
  const size_t bytes_needed = static_cast<size_t>(color_count) * 3u;
  if (*offset + bytes_needed > bytes.size()) {
    return false;
  }
  for (int i = 0; i < color_count; ++i) {
    const size_t base = *offset + static_cast<size_t>(i) * 3u;
    (*out_r)[static_cast<size_t>(i)] = bytes[base + 0];
    (*out_g)[static_cast<size_t>(i)] = bytes[base + 1];
    (*out_b)[static_cast<size_t>(i)] = bytes[base + 2];
  }
  *offset += bytes_needed;
  return true;
}

bool DecodeGifLzw(const std::vector<uint8_t>& compressed,
                  int min_code_size,
                  size_t expected_pixels,
                  std::vector<uint8_t>* out_indices,
                  std::string* out_error) {
  if (!out_indices || min_code_size < 2 || min_code_size > 8) {
    if (out_error) {
      *out_error = "unsupported GIF LZW minimum code size";
    }
    return false;
  }

  constexpr int kMaxCodes = 4096;
  const int clear_code = 1 << min_code_size;
  const int end_code = clear_code + 1;

  std::array<uint16_t, kMaxCodes> prefix{};
  std::array<uint8_t, kMaxCodes> suffix{};
  std::array<uint8_t, kMaxCodes> stack{};

  for (int i = 0; i < clear_code; ++i) {
    suffix[static_cast<size_t>(i)] = static_cast<uint8_t>(i);
  }

  int code_size = min_code_size + 1;
  int next_code = end_code + 1;
  size_t bit_pos = 0;

  auto read_code = [&]() -> int {
    if (bit_pos + static_cast<size_t>(code_size) > compressed.size() * 8u) {
      return -1;
    }
    int value = 0;
    for (int i = 0; i < code_size; ++i) {
      const size_t bit_index = bit_pos + static_cast<size_t>(i);
      const size_t byte_index = bit_index >> 3u;
      const int bit_in_byte = static_cast<int>(bit_index & 7u);
      value |= ((compressed[byte_index] >> bit_in_byte) & 1u) << i;
    }
    bit_pos += static_cast<size_t>(code_size);
    return value;
  };

  out_indices->clear();
  out_indices->reserve(expected_pixels);

  int old_code = -1;
  uint8_t first_char = 0;

  while (out_indices->size() < expected_pixels) {
    const int code = read_code();
    if (code < 0) {
      break;
    }

    if (code == clear_code) {
      code_size = min_code_size + 1;
      next_code = end_code + 1;
      old_code = -1;
      continue;
    }
    if (code == end_code) {
      break;
    }

    if (old_code == -1) {
      if (code >= clear_code) {
        if (out_error) {
          *out_error = "GIF LZW stream has invalid first code";
        }
        return false;
      }
      first_char = suffix[static_cast<size_t>(code)];
      out_indices->push_back(first_char);
      old_code = code;
      continue;
    }

    int in_code = code;
    int stack_size = 0;
    int traverse_code = code;

    if (traverse_code >= next_code) {
      stack[static_cast<size_t>(stack_size++)] = first_char;
      traverse_code = old_code;
    }

    while (traverse_code >= clear_code) {
      if (traverse_code < 0 || traverse_code >= next_code || stack_size >= kMaxCodes) {
        if (out_error) {
          *out_error = "GIF LZW stream traversal failed";
        }
        return false;
      }
      stack[static_cast<size_t>(stack_size++)] = suffix[static_cast<size_t>(traverse_code)];
      traverse_code = static_cast<int>(prefix[static_cast<size_t>(traverse_code)]);
    }

    if (traverse_code < 0 || traverse_code >= clear_code || stack_size >= kMaxCodes) {
      if (out_error) {
        *out_error = "GIF LZW stream root code failed";
      }
      return false;
    }
    first_char = suffix[static_cast<size_t>(traverse_code)];
    stack[static_cast<size_t>(stack_size++)] = first_char;

    while (stack_size > 0 && out_indices->size() < expected_pixels) {
      out_indices->push_back(stack[static_cast<size_t>(--stack_size)]);
    }

    if (next_code < kMaxCodes) {
      prefix[static_cast<size_t>(next_code)] = static_cast<uint16_t>(old_code);
      suffix[static_cast<size_t>(next_code)] = first_char;
      ++next_code;
      if (next_code == (1 << code_size) && code_size < 12) {
        ++code_size;
      }
    }

    old_code = in_code;
  }

  if (out_indices->size() < expected_pixels) {
    if (out_error) {
      *out_error = "GIF LZW stream ended before expected pixel count";
    }
    return false;
  }
  if (out_indices->size() > expected_pixels) {
    out_indices->resize(expected_pixels);
  }
  return true;
}

}  // namespace

bool LoadGifIndexed8FirstFrame(const std::string& path,
                               IndexedImage8* out_image,
                               std::string* out_error) {
  if (!out_image) {
    if (out_error) {
      *out_error = "LoadGifIndexed8FirstFrame: out_image is null";
    }
    return false;
  }

  std::vector<uint8_t> bytes;
  if (!ReadFileBytes(path, &bytes)) {
    if (out_error) {
      *out_error = "LoadGifIndexed8FirstFrame: unable to read " + path;
    }
    return false;
  }
  if (bytes.size() < 13) {
    if (out_error) {
      *out_error = "LoadGifIndexed8FirstFrame: file too small";
    }
    return false;
  }
  if (!(bytes[0] == 'G' && bytes[1] == 'I' && bytes[2] == 'F')) {
    if (out_error) {
      *out_error = "LoadGifIndexed8FirstFrame: not a GIF file";
    }
    return false;
  }

  size_t offset = 6;  // Signature/version.
  uint16_t logical_width = 0;
  uint16_t logical_height = 0;
  if (!ReadU16LE(bytes, &offset, &logical_width) || !ReadU16LE(bytes, &offset, &logical_height)) {
    if (out_error) {
      *out_error = "LoadGifIndexed8FirstFrame: invalid logical screen descriptor";
    }
    return false;
  }
  if (offset + 3 > bytes.size()) {
    if (out_error) {
      *out_error = "LoadGifIndexed8FirstFrame: truncated logical screen descriptor";
    }
    return false;
  }
  const uint8_t packed = bytes[offset++];
  offset += 2;  // background color index + pixel aspect ratio.

  std::array<uint8_t, 256> global_r{};
  std::array<uint8_t, 256> global_g{};
  std::array<uint8_t, 256> global_b{};
  int global_color_count = 0;
  const bool has_global_table = (packed & 0x80u) != 0u;
  if (has_global_table) {
    global_color_count = 1 << ((packed & 0x07u) + 1);
    if (!ReadColorTable(bytes, &offset, global_color_count, &global_r, &global_g, &global_b)) {
      if (out_error) {
        *out_error = "LoadGifIndexed8FirstFrame: invalid global color table";
      }
      return false;
    }
  }

  while (offset < bytes.size()) {
    const uint8_t block_id = bytes[offset++];
    if (block_id == 0x3Bu) {  // Trailer
      break;
    }
    if (block_id == 0x21u) {  // Extension
      if (offset >= bytes.size()) {
        break;
      }
      offset += 1;  // Extension label
      if (!SkipGifSubBlocks(bytes, &offset)) {
        if (out_error) {
          *out_error = "LoadGifIndexed8FirstFrame: invalid extension sub-blocks";
        }
        return false;
      }
      continue;
    }
    if (block_id != 0x2Cu) {  // Not image descriptor
      if (out_error) {
        *out_error = "LoadGifIndexed8FirstFrame: unknown GIF block";
      }
      return false;
    }

    uint16_t image_left = 0;
    uint16_t image_top = 0;
    uint16_t image_width = 0;
    uint16_t image_height = 0;
    if (!ReadU16LE(bytes, &offset, &image_left) || !ReadU16LE(bytes, &offset, &image_top) ||
        !ReadU16LE(bytes, &offset, &image_width) || !ReadU16LE(bytes, &offset, &image_height)) {
      if (out_error) {
        *out_error = "LoadGifIndexed8FirstFrame: invalid image descriptor";
      }
      return false;
    }
    if (offset >= bytes.size()) {
      if (out_error) {
        *out_error = "LoadGifIndexed8FirstFrame: truncated image descriptor";
      }
      return false;
    }
    const uint8_t image_packed = bytes[offset++];
    const bool has_local_table = (image_packed & 0x80u) != 0u;
    const bool is_interlaced = (image_packed & 0x40u) != 0u;

    std::array<uint8_t, 256> palette_r = global_r;
    std::array<uint8_t, 256> palette_g = global_g;
    std::array<uint8_t, 256> palette_b = global_b;
    int palette_count = global_color_count;

    if (has_local_table) {
      palette_count = 1 << ((image_packed & 0x07u) + 1);
      if (!ReadColorTable(bytes, &offset, palette_count, &palette_r, &palette_g, &palette_b)) {
        if (out_error) {
          *out_error = "LoadGifIndexed8FirstFrame: invalid local color table";
        }
        return false;
      }
    }
    if (palette_count <= 0) {
      if (out_error) {
        *out_error = "LoadGifIndexed8FirstFrame: no palette available";
      }
      return false;
    }
    if (offset >= bytes.size()) {
      if (out_error) {
        *out_error = "LoadGifIndexed8FirstFrame: missing LZW minimum code size";
      }
      return false;
    }
    const int min_code_size = static_cast<int>(bytes[offset++]);

    std::vector<uint8_t> compressed;
    while (offset < bytes.size()) {
      const uint8_t block_size = bytes[offset++];
      if (block_size == 0) {
        break;
      }
      if (offset + block_size > bytes.size()) {
        if (out_error) {
          *out_error = "LoadGifIndexed8FirstFrame: invalid image data block size";
        }
        return false;
      }
      compressed.insert(
          compressed.end(), bytes.begin() + static_cast<std::ptrdiff_t>(offset),
          bytes.begin() + static_cast<std::ptrdiff_t>(offset + block_size));
      offset += block_size;
    }

    const size_t pixel_count =
        static_cast<size_t>(std::max<uint16_t>(1, image_width)) *
        static_cast<size_t>(std::max<uint16_t>(1, image_height));
    std::vector<uint8_t> decoded;
    if (!DecodeGifLzw(compressed, min_code_size, pixel_count, &decoded, out_error)) {
      if (out_error && out_error->empty()) {
        *out_error = "LoadGifIndexed8FirstFrame: GIF LZW decode failed";
      }
      return false;
    }

    out_image->width = static_cast<int>(image_width);
    out_image->height = static_cast<int>(image_height);
    out_image->indices.assign(pixel_count, 0u);

    if (is_interlaced) {
      size_t src_index = 0;
      auto write_pass = [&](int start_row, int step) {
        for (int y = start_row; y < out_image->height; y += step) {
          const size_t row_offset = static_cast<size_t>(y) * static_cast<size_t>(out_image->width);
          for (int x = 0; x < out_image->width && src_index < decoded.size(); ++x) {
            out_image->indices[row_offset + static_cast<size_t>(x)] = decoded[src_index++];
          }
        }
      };
      write_pass(0, 8);
      write_pass(4, 8);
      write_pass(2, 4);
      write_pass(1, 2);
    } else {
      out_image->indices = std::move(decoded);
    }

    for (int i = 0; i < 256; ++i) {
      const int palette_index = std::min(i, palette_count - 1);
      out_image->palette_r[static_cast<size_t>(i)] = palette_r[static_cast<size_t>(palette_index)];
      out_image->palette_g[static_cast<size_t>(i)] = palette_g[static_cast<size_t>(palette_index)];
      out_image->palette_b[static_cast<size_t>(i)] = palette_b[static_cast<size_t>(palette_index)];
    }
    (void)logical_width;
    (void)logical_height;
    (void)image_left;
    (void)image_top;
    return true;
  }

  if (out_error) {
    *out_error = "LoadGifIndexed8FirstFrame: no image frame found";
  }
  return false;
}

}  // namespace forward::core

