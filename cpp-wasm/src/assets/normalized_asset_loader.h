#ifndef FORWARD_OFFLINE_ASSETS_NORMALIZED_ASSET_LOADER_H
#define FORWARD_OFFLINE_ASSETS_NORMALIZED_ASSET_LOADER_H

#include <cstdint>
#include <string>
#include <vector>

namespace forward_offline {

struct PackedRgbAsset {
    int width;
    int height;
    std::vector<std::uint32_t> packed_pixels;
};

struct IndexedAsset {
    int width;
    int height;
    std::vector<std::uint8_t> palette_red;
    std::vector<std::uint8_t> palette_green;
    std::vector<std::uint8_t> palette_blue;
    std::vector<std::uint8_t> pixels;
};

bool load_packed_rgb_asset(const std::string& path, PackedRgbAsset* asset, std::string* error_message);
bool load_indexed_asset(const std::string& path, IndexedAsset* asset, std::string* error_message);

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_ASSETS_NORMALIZED_ASSET_LOADER_H
