#include "scenes/saari_scene.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <limits>
#include <sstream>

namespace forward_offline {

namespace {

const int kSurfaceWidth = 512;
const int kSurfaceHeight = 256;
const int kSaariTextureSize = 256;
const int kShockPatternLength = 1000;
const int kShockSeed = 195;
const int kShockFrameSeed = 1337;
const float kSceneTimeScale = 1.16f;
const float kCameraFieldOfView = 1.4f;
const float kTrackTickScale = 1000.0f;
const float kNearPlane = 0.1f;
const float kCameraFarPlane = 250.0f;
const float kSaariWaterPlaneHeight = -0.001f;
const float kSaariWaterCoverageDistance = 1000.0f;
const float kDepthRampNear = 15.0f;
const float kDepthRampFar = 250.0f;
const float kTerrainHeightScale = 0.16f;
const float kBackdropUvRotation = 0.7853982f;
const float kSaariFogNear = 100.0f;
const float kSaariMaxFadeFactor = 255.0f / 256.0f;

struct CameraState {
    SaariVec3 position;
    SaariVec3 target;
    SaariVec3 forward;
    SaariVec3 right;
    SaariVec3 up;
    float focal_length;
    float half_width;
    float half_height;
};

struct ScreenVertex {
    float x;
    float y;
    float depth;
    float inv_depth;
    float u;
    float v;
    float shade;
};

struct SaariScreenVertex {
    float x;
    float y;
    float depth;
    float inv_depth;
    float u;
    float v;
    float fade;
};

struct SaariClipVertex {
    float view_x;
    float view_y;
    float view_z;
    float u;
    float v;
    float fade;
};

enum SaariCompositeMode {
    kSaariCompositeOpaque = 0,
    kSaariCompositeReflectAdd = 1
};

struct SaariPrimitive {
    std::vector<SaariClipVertex> polygon;
    const IndexedAsset* index_texture;
    const std::vector<std::uint32_t>* ramp_pixels;
    const std::vector<std::uint8_t>* reflective_palette_mask;
    SaariCompositeMode composite_mode;
    float sort_key;
};

struct SaariMatrix3 {
    float m00;
    float m01;
    float m02;
    float m10;
    float m11;
    float m12;
    float m20;
    float m21;
    float m22;
};

enum DepthFadeMode {
    kDepthFadeNone = 0,
    kDepthFadeFogBlue = 1,
    kDepthFadeToWhite = 2,
    kDepthFadeToBlack = 3
};

bool starts_with(const std::string& text, const std::string& prefix) {
    return text.size() >= prefix.size() && text.compare(0, prefix.size(), prefix) == 0;
}

float clamp_unit(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }
    return value;
}

int java_trunc_to_int(float value) {
    return static_cast<int>(value);
}

int clamp_int(int value, int minimum, int maximum) {
    if (value < minimum) {
        return minimum;
    }
    if (value > maximum) {
        return maximum;
    }
    return value;
}

float wrap_unit(float value) {
    value -= std::floor(value);
    if (value < 0.0f) {
        value += 1.0f;
    }
    return value;
}

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

SaariVec3 make_vec3(float x, float y, float z) {
    SaariVec3 value;
    value.x = x;
    value.y = y;
    value.z = z;
    return value;
}

SaariVec3 add(const SaariVec3& a, const SaariVec3& b) {
    return make_vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

SaariVec3 subtract(const SaariVec3& a, const SaariVec3& b) {
    return make_vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

SaariVec3 scale(const SaariVec3& value, float factor) {
    return make_vec3(value.x * factor, value.y * factor, value.z * factor);
}

float dot(const SaariVec3& a, const SaariVec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

SaariVec3 cross(const SaariVec3& a, const SaariVec3& b) {
    return make_vec3(a.y * b.z - a.z * b.y,
                     a.z * b.x - a.x * b.z,
                     a.x * b.y - a.y * b.x);
}

float length_sq(const SaariVec3& value) {
    return dot(value, value);
}

float length(const SaariVec3& value) {
    return std::sqrt(length_sq(value));
}

SaariVec3 normalize(const SaariVec3& value) {
    const float value_length = length(value);
    if (value_length <= 1.0e-6f) {
        return make_vec3(0.0f, 0.0f, 0.0f);
    }
    return scale(value, 1.0f / value_length);
}

SaariVec3 reflect(const SaariVec3& incident, const SaariVec3& normal) {
    return subtract(incident, scale(normal, 2.0f * dot(incident, normal)));
}

SaariVec3 rotate_x(const SaariVec3& value, float angle) {
    const float sine = std::sin(angle);
    const float cosine = std::cos(angle);
    return make_vec3(value.x,
                     value.y * cosine - value.z * sine,
                     value.z * cosine + value.y * sine);
}

SaariVec3 rotate_y(const SaariVec3& value, float angle) {
    const float sine = std::sin(angle);
    const float cosine = std::cos(angle);
    return make_vec3(value.x * cosine + value.z * sine,
                     value.y,
                     value.z * cosine - value.x * sine);
}

SaariVec3 rotate_z(const SaariVec3& value, float angle) {
    const float sine = std::sin(angle);
    const float cosine = std::cos(angle);
    return make_vec3(value.x * cosine - value.y * sine,
                     value.y * cosine + value.x * sine,
                     value.z);
}

SaariVec3 rotate_xyz(const SaariVec3& value, float angle_x, float angle_y, float angle_z) {
    return rotate_z(rotate_y(rotate_x(value, angle_x), angle_y), angle_z);
}

SaariMatrix3 identity_matrix3() {
    SaariMatrix3 matrix;
    matrix.m00 = 1.0f;
    matrix.m01 = 0.0f;
    matrix.m02 = 0.0f;
    matrix.m10 = 0.0f;
    matrix.m11 = 1.0f;
    matrix.m12 = 0.0f;
    matrix.m20 = 0.0f;
    matrix.m21 = 0.0f;
    matrix.m22 = 1.0f;
    return matrix;
}

SaariVec3 transform_matrix3(const SaariMatrix3& matrix, const SaariVec3& value) {
    return make_vec3(matrix.m00 * value.x + matrix.m10 * value.y + matrix.m20 * value.z,
                     matrix.m01 * value.x + matrix.m11 * value.y + matrix.m21 * value.z,
                     matrix.m02 * value.x + matrix.m12 * value.y + matrix.m22 * value.z);
}

void matrix_rotate_x_in_place(SaariMatrix3* matrix, float angle) {
    const float sine = std::sin(angle);
    const float cosine = std::cos(angle);

    float value0 = matrix->m01 * cosine - sine * matrix->m02;
    float value1 = matrix->m02 * cosine + sine * matrix->m01;
    matrix->m01 = value0;
    matrix->m02 = value1;

    value0 = matrix->m11 * cosine - sine * matrix->m12;
    value1 = matrix->m12 * cosine + sine * matrix->m11;
    matrix->m11 = value0;
    matrix->m12 = value1;

    value0 = matrix->m21 * cosine - sine * matrix->m22;
    value1 = matrix->m22 * cosine + sine * matrix->m21;
    matrix->m21 = value0;
    matrix->m22 = value1;
}

void matrix_rotate_y_in_place(SaariMatrix3* matrix, float angle) {
    const float sine = std::sin(angle);
    const float cosine = std::cos(angle);

    float value0 = matrix->m00 * cosine + sine * matrix->m02;
    float value1 = matrix->m02 * cosine - sine * matrix->m00;
    matrix->m00 = value0;
    matrix->m02 = value1;

    value0 = matrix->m10 * cosine + sine * matrix->m12;
    value1 = matrix->m12 * cosine - sine * matrix->m10;
    matrix->m10 = value0;
    matrix->m12 = value1;

    value0 = matrix->m20 * cosine + sine * matrix->m22;
    value1 = matrix->m22 * cosine - sine * matrix->m20;
    matrix->m20 = value0;
    matrix->m22 = value1;
}

void matrix_rotate_z_in_place(SaariMatrix3* matrix, float angle) {
    const float sine = std::sin(angle);
    const float cosine = std::cos(angle);

    float value0 = matrix->m00 * cosine - sine * matrix->m01;
    float value1 = matrix->m01 * cosine + sine * matrix->m00;
    matrix->m00 = value0;
    matrix->m01 = value1;

    value0 = matrix->m10 * cosine - sine * matrix->m11;
    value1 = matrix->m11 * cosine + sine * matrix->m10;
    matrix->m10 = value0;
    matrix->m11 = value1;

    value0 = matrix->m20 * cosine - sine * matrix->m21;
    value1 = matrix->m21 * cosine + sine * matrix->m20;
    matrix->m20 = value0;
    matrix->m21 = value1;
}

SaariMatrix3 build_saari_rotation_matrix(float angle_x, float angle_y, float angle_z) {
    SaariMatrix3 matrix = identity_matrix3();
    matrix_rotate_x_in_place(&matrix, angle_x);
    matrix_rotate_y_in_place(&matrix, angle_y);
    matrix_rotate_z_in_place(&matrix, angle_z);
    return matrix;
}

SaariMatrix3 multiply_matrix3(const SaariMatrix3& left, const SaariMatrix3& right) {
    SaariMatrix3 result;
    result.m00 = left.m00 * right.m00 + left.m01 * right.m10 + left.m02 * right.m20;
    result.m01 = left.m00 * right.m01 + left.m01 * right.m11 + left.m02 * right.m21;
    result.m02 = left.m00 * right.m02 + left.m01 * right.m12 + left.m02 * right.m22;
    result.m10 = left.m10 * right.m00 + left.m11 * right.m10 + left.m12 * right.m20;
    result.m11 = left.m10 * right.m01 + left.m11 * right.m11 + left.m12 * right.m21;
    result.m12 = left.m10 * right.m02 + left.m11 * right.m12 + left.m12 * right.m22;
    result.m20 = left.m20 * right.m00 + left.m21 * right.m10 + left.m22 * right.m20;
    result.m21 = left.m20 * right.m01 + left.m21 * right.m11 + left.m22 * right.m21;
    result.m22 = left.m20 * right.m02 + left.m21 * right.m12 + left.m22 * right.m22;
    return result;
}

SaariMatrix3 camera_env_matrix(const CameraState& camera) {
    SaariMatrix3 matrix;
    // Java's env-mapped meshes use camera.jAKkaMa.AmajAKk(), i.e. the transposed
    // camera basis, so each UV axis comes from dot(normal, right/up/forward).
    matrix.m00 = camera.right.x;
    matrix.m01 = camera.up.x;
    matrix.m02 = camera.forward.x;
    matrix.m10 = camera.right.y;
    matrix.m11 = camera.up.y;
    matrix.m12 = camera.forward.y;
    matrix.m20 = camera.right.z;
    matrix.m21 = camera.up.z;
    matrix.m22 = camera.forward.z;
    return matrix;
}

SaariMatrix3 mirror_matrix_along_z_output(const SaariMatrix3& matrix) {
    SaariMatrix3 mirrored = matrix;
    mirrored.m02 = -mirrored.m02;
    mirrored.m12 = -mirrored.m12;
    mirrored.m22 = -mirrored.m22;
    return mirrored;
}

float rgb_hue_unit(int red, int green, int blue) {
    const float red_unit = static_cast<float>(red) / 255.0f;
    const float green_unit = static_cast<float>(green) / 255.0f;
    const float blue_unit = static_cast<float>(blue) / 255.0f;
    const float max_channel = std::max(red_unit, std::max(green_unit, blue_unit));
    const float min_channel = std::min(red_unit, std::min(green_unit, blue_unit));
    const float delta = max_channel - min_channel;
    if (delta <= 1.0e-6f) {
        return 0.0f;
    }

    float hue = 0.0f;
    if (max_channel == red_unit) {
        hue = std::fmod((green_unit - blue_unit) / delta, 6.0f);
    } else if (max_channel == green_unit) {
        hue = ((blue_unit - red_unit) / delta) + 2.0f;
    } else {
        hue = ((red_unit - green_unit) / delta) + 4.0f;
    }
    hue /= 6.0f;
    if (hue < 0.0f) {
        hue += 1.0f;
    }
    return hue;
}

std::uint32_t pack_rgb(int red, int green, int blue) {
    return static_cast<std::uint32_t>((clamp_int(red, 0, 255) << 16) |
                                      (clamp_int(green, 0, 255) << 8) |
                                      clamp_int(blue, 0, 255));
}

std::uint32_t multiply_rgb(std::uint32_t color, float factor) {
    const int red = static_cast<int>(((color >> 16) & 0xffU) * factor);
    const int green = static_cast<int>(((color >> 8) & 0xffU) * factor);
    const int blue = static_cast<int>((color & 0xffU) * factor);
    return pack_rgb(red, green, blue);
}

std::uint32_t tint_rgb(std::uint32_t color, float red_factor, float green_factor, float blue_factor) {
    const int red = static_cast<int>(((color >> 16) & 0xffU) * red_factor);
    const int green = static_cast<int>(((color >> 8) & 0xffU) * green_factor);
    const int blue = static_cast<int>((color & 0xffU) * blue_factor);
    return pack_rgb(red, green, blue);
}

std::uint32_t blend_rgb(std::uint32_t dst, std::uint32_t src, float alpha) {
    const float clamped_alpha = clamp_unit(alpha);
    const float inverse_alpha = 1.0f - clamped_alpha;

    const int dst_red = static_cast<int>((dst >> 16) & 0xffU);
    const int dst_green = static_cast<int>((dst >> 8) & 0xffU);
    const int dst_blue = static_cast<int>(dst & 0xffU);

    const int src_red = static_cast<int>((src >> 16) & 0xffU);
    const int src_green = static_cast<int>((src >> 8) & 0xffU);
    const int src_blue = static_cast<int>(src & 0xffU);

    return pack_rgb(static_cast<int>(src_red * clamped_alpha + dst_red * inverse_alpha),
                    static_cast<int>(src_green * clamped_alpha + dst_green * inverse_alpha),
                    static_cast<int>(src_blue * clamped_alpha + dst_blue * inverse_alpha));
}

std::uint32_t add_rgb_saturate(std::uint32_t dst, std::uint32_t src) {
    const int red = clamp_int(static_cast<int>((dst >> 16) & 0xffU) + static_cast<int>((src >> 16) & 0xffU), 0, 255);
    const int green = clamp_int(static_cast<int>((dst >> 8) & 0xffU) + static_cast<int>((src >> 8) & 0xffU), 0, 255);
    const int blue = clamp_int(static_cast<int>(dst & 0xffU) + static_cast<int>(src & 0xffU), 0, 255);
    return pack_rgb(red, green, blue);
}

std::uint32_t sample_packed_rgb_asset(const PackedRgbAsset& asset, float u, float v) {
    if (asset.width <= 0 || asset.height <= 0 || asset.packed_pixels.empty()) {
        return 0;
    }

    u = wrap_unit(u);
    v = clamp_unit(v);

    const int x = clamp_int(static_cast<int>(u * static_cast<float>(asset.width)), 0, asset.width - 1);
    const int y = clamp_int(static_cast<int>(v * static_cast<float>(asset.height)), 0, asset.height - 1);
    const std::uint32_t packed = asset.packed_pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(asset.width) +
                                                     static_cast<std::size_t>(x)];
    return static_cast<std::uint32_t>(((packed >> 20) & 0xffU) << 16 |
                                      ((packed >> 10) & 0xffU) << 8 |
                                      (packed & 0xffU));
}

std::uint32_t sample_indexed_asset(const IndexedAsset& asset, float u, float v) {
    if (asset.width <= 0 || asset.height <= 0 || asset.pixels.empty()) {
        return 0;
    }

    u = wrap_unit(u);
    v = wrap_unit(v);

    const int x = clamp_int(static_cast<int>(u * static_cast<float>(asset.width)), 0, asset.width - 1);
    const int y = clamp_int(static_cast<int>(v * static_cast<float>(asset.height)), 0, asset.height - 1);
    const std::size_t index = static_cast<std::size_t>(y) * static_cast<std::size_t>(asset.width) +
                              static_cast<std::size_t>(x);
    const int palette_index = asset.pixels[index];
    return pack_rgb(asset.palette_red[static_cast<std::size_t>(palette_index)],
                    asset.palette_green[static_cast<std::size_t>(palette_index)],
                    asset.palette_blue[static_cast<std::size_t>(palette_index)]);
}

int sample_indexed_asset_palette_index(const IndexedAsset& asset, float u, float v) {
    if (asset.width <= 0 || asset.height <= 0 || asset.pixels.empty()) {
        return 0;
    }

    u = wrap_unit(u);
    v = wrap_unit(v);

    const int x = clamp_int(static_cast<int>(u * static_cast<float>(asset.width)), 0, asset.width - 1);
    const int y = clamp_int(static_cast<int>(v * static_cast<float>(asset.height)), 0, asset.height - 1);
    const std::size_t index = static_cast<std::size_t>(y) * static_cast<std::size_t>(asset.width) +
                              static_cast<std::size_t>(x);
    return asset.pixels[index];
}

float clamp_saari_fade_factor(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > kSaariMaxFadeFactor) {
        return kSaariMaxFadeFactor;
    }
    return value;
}

float saari_depth_fade(float depth) {
    return clamp_saari_fade_factor((depth - kSaariFogNear) / (kCameraFarPlane - kSaariFogNear));
}

void build_white_black_ramps(const IndexedAsset& asset,
                             std::vector<std::uint32_t>* white_ramp,
                             std::vector<std::uint32_t>* black_ramp) {
    white_ramp->assign(256U * 256U, 0U);
    black_ramp->assign(256U * 256U, 0U);

    for (int row = 0; row < 256; ++row) {
        const float source_weight = 1.0f - static_cast<float>(row) / 255.0f;
        const float target_weight = 1.0f - source_weight;
        for (int palette_index = 0; palette_index < 256; ++palette_index) {
            const int red = asset.palette_red[static_cast<std::size_t>(palette_index)];
            const int green = asset.palette_green[static_cast<std::size_t>(palette_index)];
            const int blue = asset.palette_blue[static_cast<std::size_t>(palette_index)];

            const std::size_t dst_index =
                static_cast<std::size_t>(row) * 256U + static_cast<std::size_t>(palette_index);
            (*white_ramp)[dst_index] = pack_rgb(static_cast<int>(std::min(255.0f, red * source_weight + 255.0f * target_weight)),
                                                static_cast<int>(std::min(255.0f, green * source_weight + 255.0f * target_weight)),
                                                static_cast<int>(std::min(255.0f, blue * source_weight + 255.0f * target_weight)));
            (*black_ramp)[dst_index] = pack_rgb(static_cast<int>(red * source_weight),
                                                static_cast<int>(green * source_weight),
                                                static_cast<int>(blue * source_weight));
        }
    }
}

void build_saari_reflective_palette_mask(const IndexedAsset& asset,
                                         std::vector<std::uint8_t>* reflective_palette_mask) {
    // The Java renderer tags hue-selected texels with the sign bit in its white fog ramp.
    // Reflections are then additively composited only where that bit survived previous opaque passes.
    reflective_palette_mask->assign(256U, 0U);
    for (int palette_index = 0; palette_index < 256; ++palette_index) {
        const float hue =
            rgb_hue_unit(asset.palette_red[static_cast<std::size_t>(palette_index)],
                         asset.palette_green[static_cast<std::size_t>(palette_index)],
                         asset.palette_blue[static_cast<std::size_t>(palette_index)]);
        if (hue > 0.5f && hue < 0.7f) {
            (*reflective_palette_mask)[static_cast<std::size_t>(palette_index)] = 1U;
        }
    }
}

float sample_height_value(const IndexedAsset& asset, int x, int y) {
    const int clamped_x = clamp_int(x, 0, asset.width - 1);
    const int clamped_y = clamp_int(y, 0, asset.height - 1);
    const std::size_t index = static_cast<std::size_t>(clamped_y) * static_cast<std::size_t>(asset.width) +
                              static_cast<std::size_t>(clamped_x);
    const int palette_index = asset.pixels[index];
    const float height =
        static_cast<float>(asset.palette_red[static_cast<std::size_t>(palette_index)]) - 16.0f;
    return std::max(0.0f, height);
}

IndexedAsset slice_vertical_asset(const IndexedAsset& source, int start_y, int height) {
    IndexedAsset slice;
    slice.width = source.width;
    slice.height = height;
    slice.palette_red = source.palette_red;
    slice.palette_green = source.palette_green;
    slice.palette_blue = source.palette_blue;
    slice.pixels.resize(static_cast<std::size_t>(slice.width) * static_cast<std::size_t>(slice.height));

    for (int y = 0; y < height; ++y) {
        const std::size_t src_offset =
            static_cast<std::size_t>(start_y + y) * static_cast<std::size_t>(source.width);
        const std::size_t dst_offset = static_cast<std::size_t>(y) * static_cast<std::size_t>(slice.width);
        std::copy(source.pixels.begin() + static_cast<std::ptrdiff_t>(src_offset),
                  source.pixels.begin() + static_cast<std::ptrdiff_t>(src_offset + static_cast<std::size_t>(slice.width)),
                  slice.pixels.begin() + static_cast<std::ptrdiff_t>(dst_offset));
    }

    return slice;
}

PackedRgbAsset slice_packed_rgb_asset(const PackedRgbAsset& source,
                                      int start_x,
                                      int start_y,
                                      int width,
                                      int height) {
    PackedRgbAsset slice;
    slice.width = width;
    slice.height = height;
    slice.packed_pixels.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const int src_x = clamp_int(start_x + x, 0, source.width - 1);
            const int src_y = clamp_int(start_y + y, 0, source.height - 1);
            const std::size_t src_index =
                static_cast<std::size_t>(src_y) * static_cast<std::size_t>(source.width) +
                static_cast<std::size_t>(src_x);
            const std::size_t dst_index =
                static_cast<std::size_t>(y) * static_cast<std::size_t>(width) +
                static_cast<std::size_t>(x);
            slice.packed_pixels[dst_index] = source.packed_pixels[src_index];
        }
    }

    return slice;
}

std::uint32_t sample_packed_rgb_asset_clamped(const PackedRgbAsset& asset, float u, float v) {
    if (asset.width <= 0 || asset.height <= 0 || asset.packed_pixels.empty()) {
        return 0;
    }

    u = clamp_unit(u);
    v = clamp_unit(v);

    const int x = clamp_int(static_cast<int>(u * static_cast<float>(asset.width)), 0, asset.width - 1);
    const int y = clamp_int(static_cast<int>(v * static_cast<float>(asset.height)), 0, asset.height - 1);
    const std::uint32_t packed = asset.packed_pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(asset.width) +
                                                     static_cast<std::size_t>(x)];
    return static_cast<std::uint32_t>(((packed >> 20) & 0xffU) << 16 |
                                      ((packed >> 10) & 0xffU) << 8 |
                                      (packed & 0xffU));
}

std::string trim(const std::string& text) {
    std::string::size_type start = 0;
    while (start < text.size() && (text[start] == ' ' || text[start] == '\t' ||
                                   text[start] == '\r' || text[start] == '\n')) {
        ++start;
    }

    std::string::size_type end = text.size();
    while (end > start && (text[end - 1] == ' ' || text[end - 1] == '\t' ||
                           text[end - 1] == '\r' || text[end - 1] == '\n')) {
        --end;
    }

    return text.substr(start, end - start);
}

bool extract_quoted_name(const std::string& line, std::string* name) {
    const std::string::size_type first_quote = line.find('"');
    if (first_quote == std::string::npos) {
        return false;
    }

    const std::string::size_type second_quote = line.find('"', first_quote + 1);
    if (second_quote == std::string::npos) {
        return false;
    }

    *name = line.substr(first_quote + 1, second_quote - first_quote - 1);
    return true;
}

std::size_t find_matching_brace(const std::string& text, std::size_t opening_brace) {
    int depth = 0;
    for (std::size_t index = opening_brace; index < text.size(); ++index) {
        if (text[index] == '{') {
            ++depth;
        } else if (text[index] == '}') {
            --depth;
            if (depth == 0) {
                return index;
            }
        }
    }

    return std::string::npos;
}

std::vector<std::string> extract_braced_blocks(const std::string& content, const std::string& header) {
    std::vector<std::string> blocks;
    std::string::size_type search_position = 0;
    while (true) {
        const std::string::size_type header_position = content.find(header, search_position);
        if (header_position == std::string::npos) {
            break;
        }

        const std::string::size_type brace_position = content.find('{', header_position + header.size());
        if (brace_position == std::string::npos) {
            break;
        }

        const std::size_t end_position = find_matching_brace(content, brace_position);
        if (end_position == std::string::npos) {
            break;
        }

        blocks.push_back(content.substr(header_position, end_position - header_position + 1));
        search_position = end_position + 1;
    }

    return blocks;
}

void axis_angle_to_rows(const SaariVec3& axis,
                        float angle,
                        SaariVec3* row0,
                        SaariVec3* row1,
                        SaariVec3* row2) {
    const SaariVec3 normalized_axis = normalize(axis);
    const float x = normalized_axis.x;
    const float y = normalized_axis.y;
    const float z = normalized_axis.z;
    const float cosine = std::cos(angle);
    const float sine = std::sin(angle);
    const float one_minus_cosine = 1.0f - cosine;

    *row0 = make_vec3(one_minus_cosine * x * x + cosine,
                      one_minus_cosine * x * y - sine * z,
                      one_minus_cosine * x * z + sine * y);
    *row1 = make_vec3(one_minus_cosine * x * y + sine * z,
                      one_minus_cosine * y * y + cosine,
                      one_minus_cosine * y * z - sine * x);
    *row2 = make_vec3(one_minus_cosine * x * z - sine * y,
                      one_minus_cosine * y * z + sine * x,
                      one_minus_cosine * z * z + cosine);
}

void build_mesh_normals(SaariStaticMesh* mesh) {
    mesh->normals.assign(mesh->vertices.size(), make_vec3(0.0f, 0.0f, 0.0f));
    for (std::size_t index = 0; index < mesh->triangles.size(); ++index) {
        const SaariTriangle& triangle = mesh->triangles[index];
        const SaariVec3& a = mesh->vertices[static_cast<std::size_t>(triangle.a)];
        const SaariVec3& b = mesh->vertices[static_cast<std::size_t>(triangle.b)];
        const SaariVec3& c = mesh->vertices[static_cast<std::size_t>(triangle.c)];
        const SaariVec3 normal = normalize(cross(subtract(b, a), subtract(c, a)));
        mesh->normals[static_cast<std::size_t>(triangle.a)] =
            add(mesh->normals[static_cast<std::size_t>(triangle.a)], normal);
        mesh->normals[static_cast<std::size_t>(triangle.b)] =
            add(mesh->normals[static_cast<std::size_t>(triangle.b)], normal);
        mesh->normals[static_cast<std::size_t>(triangle.c)] =
            add(mesh->normals[static_cast<std::size_t>(triangle.c)], normal);
    }

    for (std::size_t index = 0; index < mesh->normals.size(); ++index) {
        mesh->normals[index] = normalize(mesh->normals[index]);
    }
}

bool parse_mesh_vertices_and_faces(const std::string& block, SaariStaticMesh* mesh) {
    std::vector<SaariVec3> world_vertices;
    std::istringstream stream(block);
    std::string line;

    while (std::getline(stream, line)) {
        const std::string trimmed = trim(line);
        if (starts_with(trimmed, "*MESH_VERTEX")) {
            int index = 0;
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            if (sscanf_s(trimmed.c_str(), "*MESH_VERTEX %d %f %f %f", &index, &x, &y, &z) == 4) {
                if (index >= static_cast<int>(world_vertices.size())) {
                    world_vertices.resize(static_cast<std::size_t>(index + 1));
                }
                world_vertices[static_cast<std::size_t>(index)] = make_vec3(x, y, z);
            }
        } else if (starts_with(trimmed, "*MESH_FACE")) {
            int face_index = 0;
            int a = 0;
            int b = 0;
            int c = 0;
            if (sscanf_s(trimmed.c_str(), "*MESH_FACE %d: A: %d B: %d C: %d", &face_index, &a, &b, &c) == 4) {
                (void)face_index;
                SaariTriangle triangle;
                triangle.a = a;
                triangle.b = b;
                triangle.c = c;
                mesh->triangles.push_back(triangle);
            }
        } else if (starts_with(trimmed, "*TM_POS")) {
            sscanf_s(trimmed.c_str(), "*TM_POS %f %f %f", &mesh->pivot.x, &mesh->pivot.y, &mesh->pivot.z);
        }
    }

    if (world_vertices.empty() || mesh->triangles.empty()) {
        return false;
    }

    mesh->vertices.resize(world_vertices.size());
    for (std::size_t index = 0; index < world_vertices.size(); ++index) {
        mesh->vertices[index] = subtract(world_vertices[index], mesh->pivot);
    }

    build_mesh_normals(mesh);
    return true;
}

void parse_position_track(const std::string& block,
                          const std::string& node_name,
                          std::vector<SaariTrackSample>* track) {
    track->clear();
    bool inside_tm_animation = false;
    std::string active_track_name;
    std::istringstream stream(block);
    std::string line;

    while (std::getline(stream, line)) {
        const std::string trimmed = trim(line);
        if (trimmed == "*TM_ANIMATION {") {
            inside_tm_animation = true;
            active_track_name.clear();
            continue;
        }
        if (!inside_tm_animation) {
            continue;
        }
        if (starts_with(trimmed, "*NODE_NAME")) {
            extract_quoted_name(trimmed, &active_track_name);
            continue;
        }
        if (starts_with(trimmed, "*CONTROL_POS_SAMPLE") && active_track_name == node_name) {
            SaariTrackSample sample;
            if (sscanf_s(trimmed.c_str(), "*CONTROL_POS_SAMPLE %d %f %f %f",
                         &sample.tick, &sample.value.x, &sample.value.y, &sample.value.z) == 4) {
                track->push_back(sample);
            }
        }
    }
}

void parse_rotation_track(const std::string& block,
                          const std::string& node_name,
                          std::vector<SaariRotationSample>* track) {
    track->clear();
    bool inside_tm_animation = false;
    std::string active_track_name;
    std::istringstream stream(block);
    std::string line;

    while (std::getline(stream, line)) {
        const std::string trimmed = trim(line);
        if (trimmed == "*TM_ANIMATION {") {
            inside_tm_animation = true;
            active_track_name.clear();
            continue;
        }
        if (!inside_tm_animation) {
            continue;
        }
        if (starts_with(trimmed, "*NODE_NAME")) {
            extract_quoted_name(trimmed, &active_track_name);
            continue;
        }
        if (starts_with(trimmed, "*CONTROL_ROT_SAMPLE") && active_track_name == node_name) {
            SaariRotationSample sample;
            if (sscanf_s(trimmed.c_str(), "*CONTROL_ROT_SAMPLE %d %f %f %f %f",
                         &sample.tick, &sample.axis.x, &sample.axis.y, &sample.axis.z, &sample.angle) == 5) {
                track->push_back(sample);
            }
        }
    }
}

SaariVec3 sample_track(const std::vector<SaariTrackSample>& track, float tick) {
    if (track.empty()) {
        return make_vec3(0.0f, 0.0f, 0.0f);
    }
    if (tick <= static_cast<float>(track.front().tick)) {
        return track.front().value;
    }
    if (tick >= static_cast<float>(track.back().tick)) {
        return track.back().value;
    }

    for (std::size_t index = 1; index < track.size(); ++index) {
        if (tick <= static_cast<float>(track[index].tick)) {
            const SaariTrackSample& previous = track[index - 1];
            const SaariTrackSample& next = track[index];
            const float range = static_cast<float>(next.tick - previous.tick);
            const float t = range <= 0.0f ? 0.0f : (tick - static_cast<float>(previous.tick)) / range;
            return make_vec3(lerp(previous.value.x, next.value.x, t),
                             lerp(previous.value.y, next.value.y, t),
                             lerp(previous.value.z, next.value.z, t));
        }
    }

    return track.back().value;
}

SaariRotationSample sample_rotation_track(const std::vector<SaariRotationSample>& track, float tick) {
    SaariRotationSample identity;
    identity.tick = 0;
    identity.axis = make_vec3(0.0f, 0.0f, 1.0f);
    identity.angle = 0.0f;
    if (track.empty()) {
        return identity;
    }
    if (tick <= static_cast<float>(track.front().tick)) {
        return track.front();
    }
    if (tick >= static_cast<float>(track.back().tick)) {
        return track.back();
    }

    for (std::size_t index = 1; index < track.size(); ++index) {
        if (tick <= static_cast<float>(track[index].tick)) {
            const SaariRotationSample& previous = track[index - 1];
            const SaariRotationSample& next = track[index];
            const float range = static_cast<float>(next.tick - previous.tick);
            const float t = range <= 0.0f ? 0.0f : (tick - static_cast<float>(previous.tick)) / range;
            SaariRotationSample sample;
            sample.tick = static_cast<int>(tick);
            sample.axis = normalize(make_vec3(lerp(previous.axis.x, next.axis.x, t),
                                              lerp(previous.axis.y, next.axis.y, t),
                                              lerp(previous.axis.z, next.axis.z, t)));
            sample.angle = lerp(previous.angle, next.angle, t);
            return sample;
        }
    }

    return track.back();
}

CameraState make_camera_state(const SaariVec3& position,
                              const SaariVec3& target) {
    const SaariVec3 forward = normalize(subtract(target, position));
    const SaariVec3 world_up = make_vec3(0.0f, 0.0f, 1.0f);
    // Match the Java camera basis: right = worldUp x forward, up = forward x right.
    SaariVec3 right = normalize(cross(world_up, forward));
    if (length_sq(right) <= 1.0e-6f) {
        right = make_vec3(1.0f, 0.0f, 0.0f);
    }
    const SaariVec3 up = normalize(cross(forward, right));
    CameraState camera;
    camera.position = position;
    camera.target = target;
    camera.forward = normalize(forward);
    camera.right = normalize(right);
    camera.up = normalize(up);
    camera.half_width = static_cast<float>(kSurfaceWidth) * 0.5f;
    camera.half_height = static_cast<float>(kSurfaceHeight) * 0.5f;
    camera.focal_length = camera.half_width / std::tan(kCameraFieldOfView * 0.5f);
    return camera;
}

SaariVec3 camera_ray_direction(const CameraState& camera, float screen_x, float screen_y) {
    const float local_x = (screen_x - camera.half_width) / camera.focal_length;
    const float local_y = (camera.half_height - screen_y) / camera.focal_length;
    const SaariVec3 direction = add(add(scale(camera.right, local_x),
                                        scale(camera.up, local_y)),
                                    camera.forward);
    return normalize(direction);
}

std::uint32_t sample_saari_backdrop(const PackedRgbAsset& backdrop_asset, const SaariVec3& ray) {
    // Match Java BackdropMesh "procedural" mode for saari:
    // tai1sp.jpg is cropped to its left 256x256 half, then the hemisphere UVs are
    // square-projected after a hard-coded +45 degree Z rotation and a final U flip.
    SaariVec3 rotated = rotate_z(normalize(ray), kBackdropUvRotation);
    rotated = normalize(rotated);

    const float planar_length = std::sqrt(rotated.x * rotated.x + rotated.y * rotated.y);
    if (planar_length <= 1.0e-6f) {
        return sample_packed_rgb_asset_clamped(backdrop_asset, 0.5f, 0.5f);
    }

    const float angle = std::atan2(rotated.y, rotated.x);
    const float cosine = std::cos(angle);
    const float sine = std::sin(angle);
    const float secant = std::fabs(cosine) > 1.0e-6f ? std::fabs(1.0f / cosine) : FLT_MAX;
    const float cosecant = std::fabs(sine) > 1.0e-6f ? std::fabs(1.0f / sine) : FLT_MAX;
    const float square_scale = std::min(secant, cosecant);
    const float height_scale = std::acos(clamp_unit(std::fabs(rotated.z))) / (0.5f * 3.14159265f);
    const float mapped_x = rotated.x / planar_length * height_scale * square_scale;
    const float mapped_y = rotated.y / planar_length * height_scale * square_scale;

    const float u = 0.5f * (1.0f - mapped_x);
    const float v = 0.5f * (mapped_y + 1.0f);
    return sample_packed_rgb_asset_clamped(backdrop_asset, u, v);
}

bool project_point(const CameraState& camera,
                   const SaariVec3& world_position,
                   float* screen_x,
                   float* screen_y,
                   float* depth) {
    const SaariVec3 relative = subtract(world_position, camera.position);
    const float view_x = dot(relative, camera.right);
    const float view_y = dot(relative, camera.up);
    const float view_z = dot(relative, camera.forward);
    if (view_z <= kNearPlane) {
        return false;
    }

    *screen_x = camera.half_width + view_x * camera.focal_length / view_z;
    *screen_y = camera.half_height - view_y * camera.focal_length / view_z;
    *depth = view_z;
    return true;
}

void view_space_coordinates(const CameraState& camera,
                            const SaariVec3& world_position,
                            float* view_x,
                            float* view_y,
                            float* view_z) {
    const SaariVec3 relative = subtract(world_position, camera.position);
    *view_x = dot(relative, camera.right);
    *view_y = dot(relative, camera.up);
    *view_z = dot(relative, camera.forward);
}

SaariClipVertex make_clip_vertex(float view_x,
                                 float view_y,
                                 float view_z,
                                 float u,
                                 float v,
                                 float fade) {
    SaariClipVertex vertex;
    vertex.view_x = view_x;
    vertex.view_y = view_y;
    vertex.view_z = view_z;
    vertex.u = u;
    vertex.v = v;
    vertex.fade = fade;
    return vertex;
}

SaariClipVertex interpolate_clip_vertex(const SaariClipVertex& a,
                                        const SaariClipVertex& b,
                                        float t) {
    return make_clip_vertex(lerp(a.view_x, b.view_x, t),
                            lerp(a.view_y, b.view_y, t),
                            lerp(a.view_z, b.view_z, t),
                            lerp(a.u, b.u, t),
                            lerp(a.v, b.v, t),
                            lerp(a.fade, b.fade, t));
}

void clip_polygon_against_near_plane(std::vector<SaariClipVertex>* polygon) {
    if (polygon == NULL || polygon->empty()) {
        return;
    }

    const std::vector<SaariClipVertex> input = *polygon;
    polygon->clear();
    polygon->reserve(4U);

    SaariClipVertex previous = input.back();
    bool previous_inside = previous.view_z > kNearPlane;

    for (std::size_t index = 0; index < input.size(); ++index) {
        const SaariClipVertex current = input[index];
        const bool current_inside = current.view_z > kNearPlane;

        if (current_inside != previous_inside) {
            const float denominator = current.view_z - previous.view_z;
            const float t = std::fabs(denominator) <= 1.0e-6f
                ? 0.0f
                : (kNearPlane - previous.view_z) / denominator;
            polygon->push_back(interpolate_clip_vertex(previous, current, t));
        }
        if (current_inside) {
            polygon->push_back(current);
        }

        previous = current;
        previous_inside = current_inside;
    }
}

SaariScreenVertex project_clip_vertex(const CameraState& camera,
                                      const SaariClipVertex& clip_vertex) {
    SaariScreenVertex vertex;
    vertex.depth = clip_vertex.view_z;
    vertex.inv_depth = 1.0f / clip_vertex.view_z;
    vertex.x = camera.half_width + clip_vertex.view_x * camera.focal_length * vertex.inv_depth;
    vertex.y = camera.half_height - clip_vertex.view_y * camera.focal_length * vertex.inv_depth;
    vertex.u = clip_vertex.u;
    vertex.v = clip_vertex.v;
    vertex.fade = clip_vertex.fade;
    return vertex;
}

void draw_background(RgbSurface& surface,
                     const CameraState& camera,
                     const PackedRgbAsset& backdrop_asset) {
    std::vector<std::uint32_t>& pixels = surface.pixels();
    const SaariVec3 fog_color = make_vec3(234.0f, 239.0f, 255.0f);

    for (int y = 0; y < surface.height(); ++y) {
        for (int x = 0; x < surface.width(); ++x) {
            const SaariVec3 ray = camera_ray_direction(camera,
                                                       static_cast<float>(x) + 0.5f,
                                                       static_cast<float>(y) + 0.5f);
            std::uint32_t color = sample_saari_backdrop(backdrop_asset, ray);
            const float zenith_boost = clamp_unit(ray.z * 0.75f + 0.25f);
            color = blend_rgb(color, pack_rgb(static_cast<int>(fog_color.x),
                                              static_cast<int>(fog_color.y),
                                              static_cast<int>(fog_color.z)),
                              zenith_boost * 0.12f);

            pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(surface.width()) +
                   static_cast<std::size_t>(x)] = color;
        }
    }
}

void rasterize_saari_triangle(RgbSurface& surface,
                              std::vector<float>* depth_buffer,
                              std::vector<std::uint8_t>* reflection_mask_buffer,
                              const SaariScreenVertex& a,
                              const SaariScreenVertex& b,
                              const SaariScreenVertex& c,
                              const IndexedAsset& index_texture,
                              const std::vector<std::uint32_t>& ramp_pixels,
                              const std::vector<std::uint8_t>* reflective_palette_mask,
                              SaariCompositeMode composite_mode) {
    (void)depth_buffer;
    const float min_x = std::floor(std::min(a.x, std::min(b.x, c.x)));
    const float max_x = std::ceil(std::max(a.x, std::max(b.x, c.x)));
    const float min_y = std::floor(std::min(a.y, std::min(b.y, c.y)));
    const float max_y = std::ceil(std::max(a.y, std::max(b.y, c.y)));
    const int start_x = clamp_int(static_cast<int>(min_x), 0, surface.width() - 1);
    const int end_x = clamp_int(static_cast<int>(max_x), 0, surface.width() - 1);
    const int start_y = clamp_int(static_cast<int>(min_y), 0, surface.height() - 1);
    const int end_y = clamp_int(static_cast<int>(max_y), 0, surface.height() - 1);

    const float denominator = ((b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y));
    if (std::fabs(denominator) <= 1.0e-6f) {
        return;
    }

    std::vector<std::uint32_t>& pixels = surface.pixels();
    for (int y = start_y; y <= end_y; ++y) {
        for (int x = start_x; x <= end_x; ++x) {
            const float sample_x = static_cast<float>(x) + 0.5f;
            const float sample_y = static_cast<float>(y) + 0.5f;

            const float w0 = ((b.y - c.y) * (sample_x - c.x) + (c.x - b.x) * (sample_y - c.y)) / denominator;
            const float w1 = ((c.y - a.y) * (sample_x - c.x) + (a.x - c.x) * (sample_y - c.y)) / denominator;
            const float w2 = 1.0f - w0 - w1;
            if (w0 < 0.0f || w1 < 0.0f || w2 < 0.0f) {
                continue;
            }

            const std::size_t pixel_index = static_cast<std::size_t>(y) * static_cast<std::size_t>(surface.width()) +
                                            static_cast<std::size_t>(x);
            const float u = w0 * a.u + w1 * b.u + w2 * c.u;
            const float v = w0 * a.v + w1 * b.v + w2 * c.v;
            const float fade = clamp_saari_fade_factor(w0 * a.fade + w1 * b.fade + w2 * c.fade);
            const int palette_index = sample_indexed_asset_palette_index(index_texture, u, v);
            const int fade_row = clamp_int(static_cast<int>(fade * 256.0f), 0, 255);
            const std::uint32_t color =
                ramp_pixels[static_cast<std::size_t>(fade_row) * 256U + static_cast<std::size_t>(palette_index)];

            if (composite_mode == kSaariCompositeOpaque) {
                pixels[pixel_index] = color;
                if (reflection_mask_buffer != NULL) {
                    (*reflection_mask_buffer)[pixel_index] =
                        (reflective_palette_mask != NULL)
                            ? (*reflective_palette_mask)[static_cast<std::size_t>(palette_index)]
                            : 0U;
                }
            } else if (reflection_mask_buffer != NULL && (*reflection_mask_buffer)[pixel_index] != 0U) {
                pixels[pixel_index] = add_rgb_saturate(pixels[pixel_index], color);
                (*reflection_mask_buffer)[pixel_index] = 0U;
            }
        }
    }
}

void rasterize_saari_polygon(RgbSurface& surface,
                             std::vector<float>* depth_buffer,
                             std::vector<std::uint8_t>* reflection_mask_buffer,
                             const CameraState& camera,
                             const std::vector<SaariClipVertex>& polygon,
                             const IndexedAsset& index_texture,
                             const std::vector<std::uint32_t>& ramp_pixels,
                             const std::vector<std::uint8_t>* reflective_palette_mask,
                             SaariCompositeMode composite_mode) {
    if (polygon.size() < 3U) {
        return;
    }

    std::vector<SaariScreenVertex> projected_vertices;
    projected_vertices.reserve(polygon.size());
    for (std::size_t index = 0; index < polygon.size(); ++index) {
        projected_vertices.push_back(project_clip_vertex(camera, polygon[index]));
    }

    for (std::size_t index = 1; index + 1 < projected_vertices.size(); ++index) {
        rasterize_saari_triangle(surface,
                                 depth_buffer,
                                 reflection_mask_buffer,
                                 projected_vertices[0],
                                 projected_vertices[index],
                                 projected_vertices[index + 1U],
                                 index_texture,
                                 ramp_pixels,
                                 reflective_palette_mask,
                                 composite_mode);
    }
}

void emit_saari_primitive(std::vector<SaariPrimitive>* primitives,
                          const std::vector<SaariClipVertex>& polygon,
                          const IndexedAsset& index_texture,
                          const std::vector<std::uint32_t>& ramp_pixels,
                          const std::vector<std::uint8_t>* reflective_palette_mask,
                          SaariCompositeMode composite_mode,
                          float sort_key) {
    if (primitives == NULL || polygon.size() < 3U) {
        return;
    }

    SaariPrimitive primitive;
    primitive.polygon = polygon;
    primitive.index_texture = &index_texture;
    primitive.ramp_pixels = &ramp_pixels;
    primitive.reflective_palette_mask = reflective_palette_mask;
    primitive.composite_mode = composite_mode;
    primitive.sort_key = sort_key;
    primitives->push_back(primitive);
}

void render_saari_primitives(RgbSurface& surface,
                             std::vector<std::uint8_t>* reflection_mask_buffer,
                             const CameraState& camera,
                             std::vector<SaariPrimitive>* primitives) {
    if (primitives == NULL || primitives->empty()) {
        return;
    }

    std::sort(primitives->begin(),
              primitives->end(),
              [](const SaariPrimitive& left, const SaariPrimitive& right) {
                  return left.sort_key < right.sort_key;
              });

    for (std::size_t index = 0; index < primitives->size(); ++index) {
        const SaariPrimitive& primitive = (*primitives)[index];
        rasterize_saari_polygon(surface,
                                NULL,
                                reflection_mask_buffer,
                                camera,
                                primitive.polygon,
                                *primitive.index_texture,
                                *primitive.ramp_pixels,
                                primitive.reflective_palette_mask,
                                primitive.composite_mode);
    }
}

void rasterize_textured_triangle(RgbSurface& surface,
                                 std::vector<float>* depth_buffer,
                                 const ScreenVertex& a,
                                 const ScreenVertex& b,
                                 const ScreenVertex& c,
                                 const IndexedAsset& texture,
                                 float alpha,
                                 bool write_depth,
                                 DepthFadeMode depth_fade_mode,
                                 bool reflection_tint) {
    const float min_x = std::floor(std::min(a.x, std::min(b.x, c.x)));
    const float max_x = std::ceil(std::max(a.x, std::max(b.x, c.x)));
    const float min_y = std::floor(std::min(a.y, std::min(b.y, c.y)));
    const float max_y = std::ceil(std::max(a.y, std::max(b.y, c.y)));
    const int start_x = clamp_int(static_cast<int>(min_x), 0, surface.width() - 1);
    const int end_x = clamp_int(static_cast<int>(max_x), 0, surface.width() - 1);
    const int start_y = clamp_int(static_cast<int>(min_y), 0, surface.height() - 1);
    const int end_y = clamp_int(static_cast<int>(max_y), 0, surface.height() - 1);

    const float denominator = ((b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y));
    if (std::fabs(denominator) <= 1.0e-6f) {
        return;
    }

    std::vector<std::uint32_t>& pixels = surface.pixels();
    const SaariVec3 fog_rgb = make_vec3(214.0f, 223.0f, 245.0f);

    for (int y = start_y; y <= end_y; ++y) {
        for (int x = start_x; x <= end_x; ++x) {
            const float sample_x = static_cast<float>(x) + 0.5f;
            const float sample_y = static_cast<float>(y) + 0.5f;

            const float w0 = ((b.y - c.y) * (sample_x - c.x) + (c.x - b.x) * (sample_y - c.y)) / denominator;
            const float w1 = ((c.y - a.y) * (sample_x - c.x) + (a.x - c.x) * (sample_y - c.y)) / denominator;
            const float w2 = 1.0f - w0 - w1;
            if (w0 < 0.0f || w1 < 0.0f || w2 < 0.0f) {
                continue;
            }

            const float inv_depth = w0 * a.inv_depth + w1 * b.inv_depth + w2 * c.inv_depth;
            if (inv_depth <= 1.0e-6f) {
                continue;
            }

            const float depth = 1.0f / inv_depth;
            const std::size_t pixel_index = static_cast<std::size_t>(y) * static_cast<std::size_t>(surface.width()) +
                                            static_cast<std::size_t>(x);
            if (depth_buffer != NULL && depth >= (*depth_buffer)[pixel_index]) {
                continue;
            }

            const float u = (w0 * a.u * a.inv_depth + w1 * b.u * b.inv_depth + w2 * c.u * c.inv_depth) * depth;
            const float v = (w0 * a.v * a.inv_depth + w1 * b.v * b.inv_depth + w2 * c.v * c.inv_depth) * depth;
            const float shade =
                (w0 * a.shade * a.inv_depth + w1 * b.shade * b.inv_depth + w2 * c.shade * c.inv_depth) * depth;

            std::uint32_t color = sample_indexed_asset(texture, u, v);
            color = multiply_rgb(color, shade);

            if (depth_fade_mode != kDepthFadeNone) {
                const float fade_amount = clamp_unit((depth - kDepthRampNear) / (kDepthRampFar - kDepthRampNear));
                if (depth_fade_mode == kDepthFadeFogBlue) {
                    color = blend_rgb(color,
                                      pack_rgb(static_cast<int>(fog_rgb.x),
                                               static_cast<int>(fog_rgb.y),
                                               static_cast<int>(fog_rgb.z)),
                                      fade_amount);
                } else if (depth_fade_mode == kDepthFadeToWhite) {
                    color = blend_rgb(color, pack_rgb(255, 255, 255), fade_amount);
                } else if (depth_fade_mode == kDepthFadeToBlack) {
                    color = blend_rgb(color, pack_rgb(0, 0, 0), fade_amount);
                }
            }
            if (reflection_tint) {
                color = tint_rgb(color, 0.42f, 0.55f, 0.92f);
            }

            if (alpha >= 0.999f) {
                pixels[pixel_index] = color;
            } else {
                pixels[pixel_index] = blend_rgb(pixels[pixel_index], color, alpha);
            }

            if (depth_buffer != NULL && write_depth) {
                (*depth_buffer)[pixel_index] = depth;
            }
        }
    }
}

// Saari terrain pass: the heightfield island/mountain and its water/reflection behavior.
void render_terrain(std::vector<SaariPrimitive>* primitives,
                    const CameraState& camera,
                    const IndexedAsset& height_asset,
                    const IndexedAsset& terrain_asset,
                    const IndexedAsset& water_asset,
                    const std::vector<std::uint32_t>& white_ramp,
                    const std::vector<std::uint32_t>& black_ramp,
                    const std::vector<std::uint8_t>& reflective_palette_mask,
                    bool reflection_pass) {
    if (height_asset.width <= 1 || height_asset.height <= 1) {
        return;
    }

    const int terrain_width = height_asset.width;
    const int terrain_height = height_asset.height;
    const float cell_size = 200.0f / static_cast<float>(terrain_width);

    const float tan_half_fov = std::tan(kCameraFieldOfView * 0.5f);
    const float far_half_width = kCameraFarPlane * tan_half_fov;
    const float far_half_height = far_half_width * static_cast<float>(kSurfaceHeight) / static_cast<float>(kSurfaceWidth);
    const SaariVec3 frustum_corners[4] = {
        add(scale(camera.forward, kCameraFarPlane),
            add(scale(camera.right, -far_half_width), scale(camera.up, far_half_height))),
        add(scale(camera.forward, kCameraFarPlane),
            add(scale(camera.right, far_half_width), scale(camera.up, far_half_height))),
        add(scale(camera.forward, kCameraFarPlane),
            add(scale(camera.right, -far_half_width), scale(camera.up, -far_half_height))),
        add(scale(camera.forward, kCameraFarPlane),
            add(scale(camera.right, far_half_width), scale(camera.up, -far_half_height)))
    };

    float min_world_x = camera.position.x;
    float max_world_x = camera.position.x;
    float min_world_y = camera.position.y;
    float max_world_y = camera.position.y;
    for (int corner_index = 0; corner_index < 4; ++corner_index) {
        const SaariVec3 corner = add(camera.position, frustum_corners[corner_index]);
        min_world_x = std::min(min_world_x, corner.x);
        max_world_x = std::max(max_world_x, corner.x);
        min_world_y = std::min(min_world_y, corner.y);
        max_world_y = std::max(max_world_y, corner.y);
    }

    // Cover the visible water footprint itself, not just the finite far-plane rectangle.
    // Without this extra screen-to-water sampling, shallow Saari shots can reveal a backdrop leak
    // where the finite terrain patch ends before the apparent sea horizon.
    for (int sample_row = 0; sample_row < 7; ++sample_row) {
        const float row_lerp = static_cast<float>(sample_row) / 6.0f;
        const float screen_y = lerp(0.5f, static_cast<float>(kSurfaceHeight) - 0.5f, row_lerp);
        for (int sample_column = 0; sample_column < 9; ++sample_column) {
            const float column_lerp = static_cast<float>(sample_column) / 8.0f;
            const float screen_x = lerp(0.5f, static_cast<float>(kSurfaceWidth) - 0.5f, column_lerp);
            const SaariVec3 ray = camera_ray_direction(camera, screen_x, screen_y);
            if (std::fabs(ray.z) <= 1.0e-6f) {
                continue;
            }

            float distance = (kSaariWaterPlaneHeight - camera.position.z) / ray.z;
            if (distance <= 0.0f) {
                continue;
            }
            distance = std::min(distance, kSaariWaterCoverageDistance);

            const SaariVec3 water_hit = add(camera.position, scale(ray, distance));
            min_world_x = std::min(min_world_x, water_hit.x);
            max_world_x = std::max(max_world_x, water_hit.x);
            min_world_y = std::min(min_world_y, water_hit.y);
            max_world_y = std::max(max_world_y, water_hit.y);
        }
    }

    const int start_grid_x = java_trunc_to_int(min_world_x / cell_size) - 1;
    const int end_grid_x = java_trunc_to_int(max_world_x / cell_size) + 2;
    const int start_grid_y = java_trunc_to_int(min_world_y / cell_size) - 1;
    const int end_grid_y = java_trunc_to_int(max_world_y / cell_size) + 2;

    const int patch_width = std::max(2, end_grid_x - start_grid_x);
    const int patch_height = std::max(2, end_grid_y - start_grid_y);

    std::vector<SaariVec3> world_vertices(static_cast<std::size_t>(patch_width * patch_height));
    std::vector<float> source_heights(static_cast<std::size_t>(patch_width * patch_height), -0.001f);
    std::vector<float> tex_u(static_cast<std::size_t>(patch_width * patch_height), 0.0f);
    std::vector<float> tex_v(static_cast<std::size_t>(patch_width * patch_height), 0.0f);
    std::vector<float> view_x(static_cast<std::size_t>(patch_width * patch_height), 0.0f);
    std::vector<float> view_y(static_cast<std::size_t>(patch_width * patch_height), 0.0f);
    std::vector<float> view_z(static_cast<std::size_t>(patch_width * patch_height), 0.0f);
    std::vector<float> fade_values(static_cast<std::size_t>(patch_width * patch_height), 0.0f);

    const int half_width = terrain_width / 2;
    const int half_height = terrain_height / 2;

    for (int row = 0; row < patch_height; ++row) {
        const int grid_y = start_grid_y + row;
        for (int column = 0; column < patch_width; ++column) {
            const int grid_x = start_grid_x + column;
            const std::size_t index = static_cast<std::size_t>(row) * static_cast<std::size_t>(patch_width) +
                                      static_cast<std::size_t>(column);

            const int sample_x = grid_x + half_width;
            const int sample_y_unflipped = grid_y + half_height;
            float height = -0.001f;
            if (sample_x >= 0 && sample_x < terrain_width &&
                sample_y_unflipped >= 0 && sample_y_unflipped < terrain_height) {
                const int sample_y = terrain_height - 1 - sample_y_unflipped;
                height = sample_height_value(height_asset, sample_x, sample_y) * kTerrainHeightScale;
            }

            source_heights[index] = height;
            tex_u[index] = static_cast<float>(sample_x) / static_cast<float>(terrain_width);
            tex_v[index] = static_cast<float>(-sample_y_unflipped) / static_cast<float>(terrain_height);

            const float z = reflection_pass ? -height : height;
            const SaariVec3 position = make_vec3(static_cast<float>(grid_x) * cell_size,
                                                 static_cast<float>(grid_y) * cell_size,
                                                 z);
            world_vertices[index] = position;
            view_space_coordinates(camera, position, &view_x[index], &view_y[index], &view_z[index]);
            fade_values[index] = saari_depth_fade(view_z[index]);
        }
    }

    for (int row = 0; row < patch_height - 1; ++row) {
        for (int column = 0; column < patch_width - 1; ++column) {
            const int a = row * patch_width + column;
            const int b = row * patch_width + column + 1;
            const int c = (row + 1) * patch_width + column;
            const int d = (row + 1) * patch_width + column + 1;
            const int triangle_indices[2][3] = {
                {a, d, b},
                {d, a, c}
            };

            for (int triangle_index = 0; triangle_index < 2; ++triangle_index) {
                const int ia = triangle_indices[triangle_index][0];
                const int ib = triangle_indices[triangle_index][1];
                const int ic = triangle_indices[triangle_index][2];
                if (view_z[static_cast<std::size_t>(ia)] <= kNearPlane &&
                    view_z[static_cast<std::size_t>(ib)] <= kNearPlane &&
                    view_z[static_cast<std::size_t>(ic)] <= kNearPlane) {
                    continue;
                }

                const SaariVec3& world_a = world_vertices[static_cast<std::size_t>(ia)];
                const SaariVec3& world_b = world_vertices[static_cast<std::size_t>(ib)];
                const SaariVec3& world_c = world_vertices[static_cast<std::size_t>(ic)];
                const float slope_x = (triangle_index == 0)
                    ? (world_c.z - world_a.z)
                    : (world_a.z - world_c.z);
                const float slope_y = (triangle_index == 0)
                    ? (world_b.z - world_c.z)
                    : (world_c.z - world_b.z);
                const float relative_x = world_a.x - camera.position.x;
                const float relative_y = world_a.y - camera.position.y;
                const float relative_z = world_a.z - camera.position.z;
                if (!reflection_pass) {
                    const float facing_value =
                        relative_x * slope_x + relative_y * slope_y + relative_z * -cell_size;
                    if (facing_value <= 0.0f) {
                        continue;
                    }
                } else {
                    // `world_vertices` already carry mirrored Z for the reflection pass,
                    // so the terrain slopes are already sign-flipped versus the opaque pass.
                    // Re-negating them here shows reflection faces that Java culls.
                    const float facing_value =
                        relative_x * slope_x + relative_y * slope_y + relative_z * -cell_size;
                    if (facing_value >= 0.0f) {
                        continue;
                    }
                }

                const float height_a = source_heights[static_cast<std::size_t>(ia)];
                const float height_b = source_heights[static_cast<std::size_t>(ib)];
                const float height_c = source_heights[static_cast<std::size_t>(ic)];
                const bool is_water_triangle = (height_a < 0.0f || height_b < 0.0f || height_c < 0.0f);

                std::vector<SaariClipVertex> polygon;
                polygon.reserve(4U);
                const int indices[3] = {ia, ib, ic};
                for (int vertex_index = 0; vertex_index < 3; ++vertex_index) {
                    const int grid_index = indices[vertex_index];
                    polygon.push_back(make_clip_vertex(view_x[static_cast<std::size_t>(grid_index)],
                                                       view_y[static_cast<std::size_t>(grid_index)],
                                                       view_z[static_cast<std::size_t>(grid_index)],
                                                       tex_u[static_cast<std::size_t>(grid_index)],
                                                       tex_v[static_cast<std::size_t>(grid_index)],
                                                       fade_values[static_cast<std::size_t>(grid_index)]));
                }
                clip_polygon_against_near_plane(&polygon);
                if (polygon.size() < 3U) {
                    continue;
                }

                const IndexedAsset& index_texture =
                    reflection_pass ? terrain_asset : (is_water_triangle ? water_asset : terrain_asset);
                const std::vector<std::uint32_t>& ramp_pixels = reflection_pass ? black_ramp : white_ramp;
                const std::vector<std::uint8_t>* triangle_mask =
                    reflection_pass ? NULL : &reflective_palette_mask;
                const float sort_key =
                    reflection_pass
                        ? (view_z[static_cast<std::size_t>(ia)] +
                           view_z[static_cast<std::size_t>(ib)] +
                           view_z[static_cast<std::size_t>(ic)])
                        : -(view_z[static_cast<std::size_t>(ia)] +
                            view_z[static_cast<std::size_t>(ib)] +
                            view_z[static_cast<std::size_t>(ic)]);

                emit_saari_primitive(primitives,
                                     polygon,
                                     index_texture,
                                     ramp_pixels,
                                     triangle_mask,
                                     reflection_pass ? kSaariCompositeReflectAdd : kSaariCompositeOpaque,
                                     sort_key);
            }
        }
    }
}

void render_env_mesh(std::vector<SaariPrimitive>* primitives,
                     const CameraState& camera,
                     const SaariStaticMesh& mesh,
                     const IndexedAsset& env_asset,
                     const std::vector<std::uint32_t>& white_ramp,
                     const std::vector<std::uint32_t>& black_ramp,
                     const SaariVec3& translation,
                     float rotation_x,
                     float rotation_y,
                     float rotation_z,
                     bool camera_locked_env,
                     bool apply_klunssi_env_tweak,
                     bool reflection_pass,
                     bool allow_reflection) {
    if (mesh.vertices.empty() || mesh.normals.empty() || mesh.triangles.empty()) {
        return;
    }
    if (reflection_pass && !allow_reflection) {
        return;
    }

    const SaariMatrix3 rotation_matrix = build_saari_rotation_matrix(rotation_x, rotation_y, rotation_z);
    // Java mirrors klunssi's reflection by cloning the mesh and flipping its world-space Z output.
    // The reflected clone keeps env-map UVs from that mirrored transform, but it does not inherit
    // the original klunssi's extra JAKkama X-axis tweak from SaariScene/MeshObject.
    const SaariMatrix3 mirrored_rotation_matrix =
        reflection_pass ? mirror_matrix_along_z_output(rotation_matrix) : rotation_matrix;
    SaariMatrix3 env_matrix =
        camera_locked_env ? camera_env_matrix(camera) : mirrored_rotation_matrix;
    if (apply_klunssi_env_tweak && !reflection_pass) {
        SaariMatrix3 env_tweak = identity_matrix3();
        matrix_rotate_x_in_place(&env_tweak, -1.5707964f);
        env_matrix = multiply_matrix3(env_matrix, env_tweak);
    }

    std::vector<SaariVec3> world_vertices(mesh.vertices.size());
    std::vector<float> view_x(mesh.vertices.size(), 0.0f);
    std::vector<float> view_y(mesh.vertices.size(), 0.0f);
    std::vector<float> view_z(mesh.vertices.size(), 0.0f);
    std::vector<float> fade_values(mesh.vertices.size(), 0.0f);
    std::vector<float> env_u(mesh.vertices.size(), 0.0f);
    std::vector<float> env_v(mesh.vertices.size(), 0.0f);

    for (std::size_t index = 0; index < mesh.vertices.size(); ++index) {
        SaariVec3 world_position = transform_matrix3(rotation_matrix, mesh.vertices[index]);
        world_position = add(world_position, translation);
        SaariVec3 env_normal = normalize(transform_matrix3(env_matrix, mesh.normals[index]));

        if (reflection_pass) {
            world_position.z = -world_position.z;
        }

        world_vertices[index] = world_position;
        view_space_coordinates(camera, world_position, &view_x[index], &view_y[index], &view_z[index]);
        fade_values[index] = saari_depth_fade(view_z[index]);
        env_u[index] = 0.5f * (env_normal.x + 1.0f);
        env_v[index] = 0.5f * (env_normal.y + 1.0f);
    }

    for (std::size_t triangle_index = 0; triangle_index < mesh.triangles.size(); ++triangle_index) {
        const SaariTriangle& triangle = mesh.triangles[triangle_index];
        if (view_z[static_cast<std::size_t>(triangle.a)] <= kNearPlane &&
            view_z[static_cast<std::size_t>(triangle.b)] <= kNearPlane &&
            view_z[static_cast<std::size_t>(triangle.c)] <= kNearPlane) {
            continue;
        }

        const SaariVec3& world_a = world_vertices[static_cast<std::size_t>(triangle.a)];
        const SaariVec3& world_b = world_vertices[static_cast<std::size_t>(triangle.b)];
        const SaariVec3& world_c = world_vertices[static_cast<std::size_t>(triangle.c)];
        SaariVec3 face_normal =
            normalize(cross(subtract(world_b, world_a), subtract(world_c, world_a)));
        if (reflection_pass) {
            const SaariVec3 local_a = mesh.vertices[static_cast<std::size_t>(triangle.a)];
            const SaariVec3 local_b = mesh.vertices[static_cast<std::size_t>(triangle.b)];
            const SaariVec3 local_c = mesh.vertices[static_cast<std::size_t>(triangle.c)];
            const SaariVec3 local_face_normal =
                normalize(cross(subtract(local_b, local_a), subtract(local_c, local_a)));
            // Java keeps the clone's local triangle winding and evaluates visibility through
            // the mirrored object transform. Recomputing a world-space cross product after the
            // Z mirror flips the normal sign and can expose concave interior faces instead of
            // the single front-most reflected layer.
            face_normal = normalize(transform_matrix3(mirrored_rotation_matrix, local_face_normal));
        }
        const SaariVec3 face_center = scale(add(add(world_a, world_b), world_c), 1.0f / 3.0f);
        if (dot(face_normal, subtract(camera.position, face_center)) <= 0.0f) {
            continue;
        }

        std::vector<SaariClipVertex> polygon;
        polygon.reserve(4U);
        const int indices[3] = {triangle.a, triangle.b, triangle.c};
        for (int vertex_index = 0; vertex_index < 3; ++vertex_index) {
            const int mesh_index = indices[vertex_index];
            polygon.push_back(make_clip_vertex(view_x[static_cast<std::size_t>(mesh_index)],
                                               view_y[static_cast<std::size_t>(mesh_index)],
                                               view_z[static_cast<std::size_t>(mesh_index)],
                                               env_u[static_cast<std::size_t>(mesh_index)],
                                               env_v[static_cast<std::size_t>(mesh_index)],
                                               fade_values[static_cast<std::size_t>(mesh_index)]));
        }
        clip_polygon_against_near_plane(&polygon);
        if (polygon.size() < 3U) {
            continue;
        }

        const float sort_key =
            reflection_pass
                ? (view_z[static_cast<std::size_t>(triangle.a)] +
                   view_z[static_cast<std::size_t>(triangle.b)] +
                   view_z[static_cast<std::size_t>(triangle.c)])
                : -(view_z[static_cast<std::size_t>(triangle.a)] +
                    view_z[static_cast<std::size_t>(triangle.b)] +
                    view_z[static_cast<std::size_t>(triangle.c)]);

        emit_saari_primitive(primitives,
                             polygon,
                             env_asset,
                             reflection_pass ? black_ramp : white_ramp,
                             NULL,
                             reflection_pass ? kSaariCompositeReflectAdd : kSaariCompositeOpaque,
                             sort_key);
    }
}

}  // namespace

SaariScene::SaariScene()
    : sky_asset_(),
      backdrop_asset_(),
      saari_asset_(),
      terrain_asset_(),
      water_asset_(),
      env_asset_(),
      height_asset_(),
      saari_white_ramp_(),
      saari_black_ramp_(),
      env_white_ramp_(),
      env_black_ramp_(),
      saari_reflective_palette_mask_(),
      meditate_mesh_(),
      klunssi_mesh_(),
      camera_track_(),
      camera_target_track_(),
      camera_rotation_track_(),
      klunssi_track_(),
      meditate_position_(),
      klunssi_initial_position_(),
      shock_pattern_(kShockPatternLength, 0),
      shock_rows_(kSurfaceHeight, 0),
      shock_init_random_(kShockSeed),
      shock_frame_random_(kShockFrameSeed),
      shock_amount_(0.0f),
      shock_decay_(0.0f),
      ready_(false),
      error_message_() {
    meditate_position_ = make_vec3(0.0f, 0.0f, 0.0f);
    klunssi_initial_position_ = make_vec3(0.0f, 0.0f, 0.0f);
}

const char* SaariScene::script_name() const {
    return "saari";
}

void SaariScene::init() {
    ready_ = load_assets();
    if (ready_) {
        build_shock_tables(195);
    }
    on_show();
}

void SaariScene::on_show() {
    shock_amount_ = 0.0f;
    shock_decay_ = 0.0f;
    shock_frame_random_ = JavaRandom(kShockFrameSeed);
}

void SaariScene::render(RgbSurface& surface, float scene_time_seconds, float delta_seconds) {
    surface.clear(0);
    if (!ready_) {
        return;
    }

    const float track_tick = scene_time_seconds * kSceneTimeScale * kTrackTickScale;
    const SaariVec3 camera_position = forward_offline::sample_track(camera_track_, track_tick);
    const SaariVec3 camera_target = forward_offline::sample_track(camera_target_track_, track_tick);
    CameraState camera = make_camera_state(camera_position, camera_target);
    if (camera.position.z < 0.3f) {
        camera.position.z = 0.3f;
    }

    draw_background(surface, camera, backdrop_asset_);

    std::vector<std::uint8_t> reflection_mask_buffer(
        static_cast<std::size_t>(surface.width()) * static_cast<std::size_t>(surface.height()), 0U);
    std::vector<SaariPrimitive> primitives;
    primitives.reserve(8192U);

    // Match the Java SceneRenderer flow:
    // 1. collect opaque and reflection primitives into one shared sorted list
    // 2. rasterize in depth-sorted order instead of z-buffering
    // 3. let additive reflection primitives consume the surviving water mask
    render_terrain(&primitives,
                   camera,
                   height_asset_,
                   terrain_asset_,
                   water_asset_,
                   saari_white_ramp_,
                   saari_black_ramp_,
                   saari_reflective_palette_mask_,
                   false);

    // `klunssi` is the spinning reflective blob-like object orbiting the scene.
    // The original mesh reads like a metaball-generated form rather than a rigid prop.
    const SaariVec3 klunssi_position = forward_offline::sample_track(klunssi_track_, track_tick);
    render_env_mesh(&primitives,
                    camera,
                    klunssi_mesh_,
                    env_asset_,
                    env_white_ramp_,
                    env_black_ramp_,
                    klunssi_position,
                    scene_time_seconds / 3.0f,
                    scene_time_seconds * 2.0f / 3.0f,
                    scene_time_seconds,
                    false,
                    true,
                    false,
                    true);

    // `meditate` is the seated meditating figure staged above the mountain summit.
    render_env_mesh(&primitives,
                    camera,
                    meditate_mesh_,
                    env_asset_,
                    env_white_ramp_,
                    env_black_ramp_,
                    meditate_position_,
                    0.0f,
                    0.0f,
                    3.14159265f,
                    true,
                    false,
                    false,
                    false);

    render_terrain(&primitives,
                   camera,
                   height_asset_,
                   terrain_asset_,
                   water_asset_,
                   saari_white_ramp_,
                   saari_black_ramp_,
                   saari_reflective_palette_mask_,
                   true);
    render_env_mesh(&primitives,
                    camera,
                    klunssi_mesh_,
                    env_asset_,
                    env_white_ramp_,
                    env_black_ramp_,
                    klunssi_position,
                    scene_time_seconds / 3.0f,
                    scene_time_seconds * 2.0f / 3.0f,
                    scene_time_seconds,
                    false,
                    true,
                    true,
                    true);
    render_saari_primitives(surface, &reflection_mask_buffer, camera, &primitives);

    if (shock_amount_ > 0.0f) {
        if (shock_decay_ > 0.0f) {
            shock_amount_ -= shock_decay_ * delta_seconds;
            if (shock_amount_ < 0.0f) {
                shock_amount_ = 0.0f;
            }
        }
        const int line_count = static_cast<int>(shock_amount_ * static_cast<float>(surface.height()) / 100.0f);
        apply_shock(surface, line_count);
    }
}

void SaariScene::handle_message(const std::string& message, float scene_time_seconds) {
    (void)scene_time_seconds;
    if (message == "suh") {
        shock_amount_ = 100.0f;
        shock_decay_ = 200.0f;
    } else if (message == "suh0") {
        shock_amount_ = 68.0f;
        shock_decay_ = 0.0f;
    }
}

bool SaariScene::is_ready() const {
    return ready_;
}

const std::string& SaariScene::error_message() const {
    return error_message_;
}

bool SaariScene::load_assets() {
    error_message_.clear();

    if (!load_original_jpeg_packed_rgb(jpeg_asset_path("tai1sp.jpg"), &sky_asset_, &error_message_)) {
        return false;
    }
    if (!load_original_gif_indexed(gif_asset_path("saari.gif"), &saari_asset_, &error_message_)) {
        return false;
    }
    if (!load_original_gif_indexed(gif_asset_path("envi_klu.gif"), &env_asset_, &error_message_)) {
        return false;
    }
    if (!load_original_gif_indexed(gif_asset_path("saarih15.gif"), &height_asset_, &error_message_)) {
        return false;
    }

    if (sky_asset_.width != kSurfaceWidth || sky_asset_.height != kSurfaceHeight) {
        error_message_ = "unexpected saari sky dimensions: " + jpeg_asset_path("tai1sp.jpg");
        return false;
    }
    backdrop_asset_ = slice_packed_rgb_asset(sky_asset_, 0, 0, kSaariTextureSize, kSaariTextureSize);
    if (saari_asset_.width != kSaariTextureSize || saari_asset_.height != kSaariTextureSize * 2) {
        error_message_ = "unexpected saari terrain dimensions: " + gif_asset_path("saari.gif");
        return false;
    }
    if (env_asset_.width != kSaariTextureSize || env_asset_.height != kSaariTextureSize) {
        error_message_ = "unexpected saari env-map dimensions: " + gif_asset_path("envi_klu.gif");
        return false;
    }

    terrain_asset_ = slice_vertical_asset(saari_asset_, 0, kSaariTextureSize);
    water_asset_ = slice_vertical_asset(saari_asset_, kSaariTextureSize, kSaariTextureSize);
    build_white_black_ramps(saari_asset_, &saari_white_ramp_, &saari_black_ramp_);
    build_white_black_ramps(env_asset_, &env_white_ramp_, &env_black_ramp_);
    build_saari_reflective_palette_mask(saari_asset_, &saari_reflective_palette_mask_);
    return load_ase_scene();
}

bool SaariScene::load_ase_scene() {
    const std::string path = ase_asset_path("alku6.ase");
    std::ifstream stream(path.c_str(), std::ios::in | std::ios::binary);
    if (!stream) {
        error_message_ = "unable to open saari ase scene: " + path;
        return false;
    }

    std::ostringstream content_builder;
    content_builder << stream.rdbuf();
    const std::string content = content_builder.str();

    const std::vector<std::string> camera_blocks =
        forward_offline::extract_braced_blocks(content, "*CAMERAOBJECT");
    if (camera_blocks.empty()) {
        error_message_ = "missing camera block in saari ase scene: " + path;
        return false;
    }

    forward_offline::parse_position_track(camera_blocks.front(), "Camera01", &camera_track_);
    forward_offline::parse_position_track(camera_blocks.front(), "Camera01.Target", &camera_target_track_);
    forward_offline::parse_rotation_track(camera_blocks.front(), "Camera01", &camera_rotation_track_);
    if (camera_track_.empty() || camera_target_track_.empty() || camera_rotation_track_.empty()) {
        error_message_ = "missing camera track data in saari ase scene: " + path;
        return false;
    }

    const std::vector<std::string> geom_blocks =
        forward_offline::extract_braced_blocks(content, "*GEOMOBJECT");
    bool found_meditate = false;
    bool found_klunssi = false;
    for (std::size_t index = 0; index < geom_blocks.size(); ++index) {
        const std::string& block = geom_blocks[index];
        if (!found_meditate && block.find("*NODE_NAME \"meditate\"") != std::string::npos) {
            // `meditate`: the meditating character placed above the island/mountain.
            if (!forward_offline::parse_mesh_vertices_and_faces(block, &meditate_mesh_)) {
                error_message_ = "failed to parse meditate mesh from saari ase scene: " + path;
                return false;
            }
            meditate_position_ = meditate_mesh_.pivot;
            found_meditate = true;
        } else if (!found_klunssi && block.find("*NODE_NAME \"klunssi\"") != std::string::npos) {
            // `klunssi`: the spinning blob-like object, likely authored from metaball-style forms.
            if (!forward_offline::parse_mesh_vertices_and_faces(block, &klunssi_mesh_)) {
                error_message_ = "failed to parse klunssi mesh from saari ase scene: " + path;
                return false;
            }
            klunssi_initial_position_ = klunssi_mesh_.pivot;
            forward_offline::parse_position_track(block, "klunssi", &klunssi_track_);
            found_klunssi = true;
        }
    }

    if (!found_meditate || !found_klunssi || klunssi_track_.empty()) {
        error_message_ = "missing geometry or animation blocks in saari ase scene: " + path;
        return false;
    }

    return true;
}

void SaariScene::build_shock_tables(int max_gray_value) {
    shock_init_random_ = JavaRandom(kShockSeed);
    for (std::size_t index = 0; index < shock_pattern_.size(); ++index) {
        shock_pattern_[index] = static_cast<int>(shock_init_random_.next_float() * static_cast<float>(max_gray_value));
    }

    for (std::size_t index = 0; index < shock_rows_.size(); ++index) {
        shock_rows_[index] = static_cast<int>(index);
    }

    for (int iteration = 0; iteration < 3000; ++iteration) {
        const int first = iteration % static_cast<int>(shock_rows_.size());
        const int second =
            static_cast<int>(shock_init_random_.next_float() * static_cast<float>(shock_rows_.size() - 2));
        std::swap(shock_rows_[static_cast<std::size_t>(first)], shock_rows_[static_cast<std::size_t>(second)]);
    }
}

void SaariScene::apply_shock(RgbSurface& surface, int line_count) {
    if (line_count <= 0) {
        return;
    }

    const int clamped_lines = std::min(line_count, surface.height() - 1);
    const int row_offset = static_cast<int>(shock_frame_random_.next_float() * 1000.0f);
    std::vector<std::uint32_t>& pixels = surface.pixels();

    for (int line = 0; line < clamped_lines; ++line) {
        const int row = shock_rows_[static_cast<std::size_t>((line + row_offset) % shock_rows_.size())];
        const int pattern_offset = static_cast<int>(shock_frame_random_.next_float() *
                                                    static_cast<float>(shock_pattern_.size() - 1 - surface.width()));
        std::size_t pixel_index = static_cast<std::size_t>(row) * static_cast<std::size_t>(surface.width());
        for (int x = 0; x < surface.width(); ++x) {
            const int delta = shock_pattern_[static_cast<std::size_t>(pattern_offset + x)];
            const std::uint32_t pixel = pixels[pixel_index];
            const int red = clamp_int(static_cast<int>((pixel >> 16) & 0xffU) - delta, 0, 255);
            const int green = clamp_int(static_cast<int>((pixel >> 8) & 0xffU) - delta, 0, 255);
            const int blue = clamp_int(static_cast<int>(pixel & 0xffU) - delta, 0, 255);
            pixels[pixel_index++] = pack_rgb(red, green, blue);
        }
    }
}

std::string SaariScene::ase_asset_path(const std::string& file_name) const {
    return std::string("original/forward/asses/") + file_name;
}

std::string SaariScene::jpeg_asset_path(const std::string& file_name) const {
    return std::string("original/forward/images/verax/") + file_name;
}

std::string SaariScene::gif_asset_path(const std::string& file_name) const {
    return std::string("original/forward/images/scape/") + file_name;
}

}  // namespace forward_offline
