#ifndef FORWARD_OFFLINE_ASSETS_ORIGINAL_ASSET_LOADER_H
#define FORWARD_OFFLINE_ASSETS_ORIGINAL_ASSET_LOADER_H

#include <cstdint>
#include <string>
#include <vector>

#include "assets/normalized_asset_loader.h"

namespace forward_offline {

std::uint32_t unpack_original_packed_rgb(std::uint32_t packed);
void convert_original_packed_rgb_asset(PackedRgbAsset* asset);

bool load_original_jpeg_packed_rgb(const std::string& path,
                                   PackedRgbAsset* asset,
                                   std::string* error_message);

bool load_original_gif_palette(const std::string& path,
                               std::vector<std::uint8_t>* palette_red,
                               std::vector<std::uint8_t>* palette_green,
                               std::vector<std::uint8_t>* palette_blue,
                               std::string* error_message);

bool load_original_gif_indexed(const std::string& path,
                               IndexedAsset* asset,
                               std::string* error_message);

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_ASSETS_ORIGINAL_ASSET_LOADER_H
