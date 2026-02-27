#include "Image32.h"

#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "../../third_party/stb_image.h"

namespace forward::core {

bool LoadImage32(const std::string& path, Image32& out_image, std::string* out_error) {
  int width = 0;
  int height = 0;
  int channels = 0;

  stbi_uc* rgba = stbi_load(path.c_str(), &width, &height, &channels, 4);
  if (!rgba) {
    if (out_error) {
      const char* reason = stbi_failure_reason();
      *out_error = "stb_image failed for " + path + ": " +
                   (reason ? std::string(reason) : std::string("unknown"));
    }
    return false;
  }

  out_image.width = width;
  out_image.height = height;
  out_image.pixels.resize(static_cast<size_t>(width) * static_cast<size_t>(height));

  const size_t pixel_count = static_cast<size_t>(width) * static_cast<size_t>(height);
  for (size_t i = 0; i < pixel_count; ++i) {
    const uint8_t r = rgba[i * 4 + 0];
    const uint8_t g = rgba[i * 4 + 1];
    const uint8_t b = rgba[i * 4 + 2];
    const uint8_t a = rgba[i * 4 + 3];
    out_image.pixels[i] = (static_cast<uint32_t>(a) << 24u) |
                          (static_cast<uint32_t>(r) << 16u) |
                          (static_cast<uint32_t>(g) << 8u) | static_cast<uint32_t>(b);
  }

  stbi_image_free(rgba);
  return true;
}

}  // namespace forward::core
