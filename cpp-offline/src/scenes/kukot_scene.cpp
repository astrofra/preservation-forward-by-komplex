#include "scenes/kukot_scene.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>

namespace forward_offline {

namespace {

const int kSurfaceWidth = 512;
const int kSurfaceHeight = 256;
const int kTextureSize = 256;
const int kFlashPatternLength = 1000;
const float kSceneTimeScale = 1.9f;
const float kTrackTickScale = 1000.0f;
const float kFieldOfView = 1.4f;
const float kNearPlane = 0.1f;
const float kFarPlane = 150.0f;
const float kHorizontalSmearFactor = 0.875f;
const float kParticleSizeScale = 512.0f;
const float kParticleMinimumDepth = 0.5f;
const std::uint32_t kPackedCarryMask = 0x10040100U;
const std::uint32_t kPackedColorMask = 0x0FF3FCFFU;
const std::uint32_t kPackedHalfMask = 0x07E1F87EU;
const std::uint32_t kPackedBlendMask = 0x01F07C1FU;

struct KukotMatrix3 {
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

struct KukotCameraState {
    Scene3dVec3 position;
    Scene3dVec3 target;
    Scene3dVec3 forward;
    Scene3dVec3 right;
    Scene3dVec3 up;
    float focal_length;
    float half_width;
    float half_height;
};

struct KukotProjectedVertex {
    float x;
    float y;
    float depth;
    float u;
    float v;
    float shade;
};

struct KukotTrianglePrimitive {
    KukotProjectedVertex a;
    KukotProjectedVertex b;
    KukotProjectedVertex c;
    float depth;
};

struct KukotSpritePrimitive {
    float center_x;
    float center_y;
    float depth;
    float size;
};

enum KukotPrimitiveType {
    kKukotPrimitiveTriangle = 0,
    kKukotPrimitiveSprite = 1
};

struct KukotRenderPrimitive {
    KukotPrimitiveType type;
    float depth;
    KukotTrianglePrimitive triangle;
    KukotSpritePrimitive sprite;
};

bool starts_with(const std::string& text, const std::string& prefix) {
    return text.size() >= prefix.size() && text.compare(0, prefix.size(), prefix) == 0;
}

std::string trim(const std::string& text) {
    std::string::size_type begin = 0;
    while (begin < text.size() && static_cast<unsigned char>(text[begin]) <= 32U) {
        ++begin;
    }

    std::string::size_type end = text.size();
    while (end > begin && static_cast<unsigned char>(text[end - 1U]) <= 32U) {
        --end;
    }

    return text.substr(begin, end - begin);
}

bool extract_quoted_name(const std::string& text, std::string* name) {
    if (name == NULL) {
        return false;
    }

    const std::string::size_type first_quote = text.find('"');
    if (first_quote == std::string::npos) {
        return false;
    }

    const std::string::size_type second_quote = text.find('"', first_quote + 1U);
    if (second_quote == std::string::npos || second_quote <= first_quote + 1U) {
        return false;
    }

    *name = text.substr(first_quote + 1U, second_quote - first_quote - 1U);
    return true;
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

float clamp_unit(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }
    return value;
}

std::uint32_t pack_rgb(int red, int green, int blue) {
    return static_cast<std::uint32_t>((clamp_int(red, 0, 255) << 16) |
                                      (clamp_int(green, 0, 255) << 8) |
                                      clamp_int(blue, 0, 255));
}

std::uint32_t pack_original_packed_rgb(std::uint32_t rgb) {
    return (((rgb >> 16) & 0xffU) << 20) |
           (((rgb >> 8) & 0xffU) << 10) |
           (rgb & 0xffU);
}

std::uint32_t add_rgb_saturate(std::uint32_t left, std::uint32_t right) {
    return pack_rgb(static_cast<int>((left >> 16) & 0xffU) + static_cast<int>((right >> 16) & 0xffU),
                    static_cast<int>((left >> 8) & 0xffU) + static_cast<int>((right >> 8) & 0xffU),
                    static_cast<int>(left & 0xffU) + static_cast<int>(right & 0xffU));
}

std::uint32_t subtract_rgb_floor(std::uint32_t left, std::uint32_t right) {
    return pack_rgb(static_cast<int>((left >> 16) & 0xffU) - static_cast<int>((right >> 16) & 0xffU),
                    static_cast<int>((left >> 8) & 0xffU) - static_cast<int>((right >> 8) & 0xffU),
                    static_cast<int>(left & 0xffU) - static_cast<int>(right & 0xffU));
}

Scene3dVec3 make_vec3(float x, float y, float z) {
    Scene3dVec3 value;
    value.x = x;
    value.y = y;
    value.z = z;
    return value;
}

Scene3dVec3 add(const Scene3dVec3& a, const Scene3dVec3& b) {
    return make_vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Scene3dVec3 subtract(const Scene3dVec3& a, const Scene3dVec3& b) {
    return make_vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Scene3dVec3 scale(const Scene3dVec3& value, float factor) {
    return make_vec3(value.x * factor, value.y * factor, value.z * factor);
}

float dot(const Scene3dVec3& a, const Scene3dVec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Scene3dVec3 cross(const Scene3dVec3& a, const Scene3dVec3& b) {
    return make_vec3(a.y * b.z - a.z * b.y,
                     a.z * b.x - a.x * b.z,
                     a.x * b.y - a.y * b.x);
}

float length_sq(const Scene3dVec3& value) {
    return dot(value, value);
}

Scene3dVec3 normalize(const Scene3dVec3& value) {
    const float magnitude_sq = length_sq(value);
    if (magnitude_sq <= 1.0e-12f) {
        return make_vec3(0.0f, 0.0f, 0.0f);
    }

    const float inverse_magnitude = 1.0f / std::sqrt(magnitude_sq);
    return scale(value, inverse_magnitude);
}

KukotMatrix3 quaternion_matrix(const Scene3dQuaternion& quaternion) {
    const float length_sq =
        quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z +
        quaternion.w * quaternion.w;
    const float scale = length_sq <= 1.0e-12f ? 2.0f : 2.0f / length_sq;
    const float x2 = quaternion.x * scale;
    const float y2 = quaternion.y * scale;
    const float z2 = quaternion.z * scale;
    const float wx = quaternion.w * x2;
    const float wy = quaternion.w * y2;
    const float wz = quaternion.w * z2;
    const float xx = quaternion.x * x2;
    const float xy = quaternion.x * y2;
    const float xz = quaternion.x * z2;
    const float yy = quaternion.y * y2;
    const float yz = quaternion.y * z2;
    const float zz = quaternion.z * z2;

    KukotMatrix3 matrix;
    matrix.m00 = 1.0f - (yy + zz);
    matrix.m01 = xy - wz;
    matrix.m02 = xz + wy;
    matrix.m10 = xy + wz;
    matrix.m11 = 1.0f - (xx + zz);
    matrix.m12 = yz - wx;
    matrix.m20 = xz - wy;
    matrix.m21 = yz + wx;
    matrix.m22 = 1.0f - (xx + yy);
    return matrix;
}

Scene3dVec3 transform_matrix3(const KukotMatrix3& matrix, const Scene3dVec3& value) {
    return make_vec3(matrix.m00 * value.x + matrix.m10 * value.y + matrix.m20 * value.z,
                     matrix.m01 * value.x + matrix.m11 * value.y + matrix.m21 * value.z,
                     matrix.m02 * value.x + matrix.m12 * value.y + matrix.m22 * value.z);
}

Scene3dVec3 deform_kukot_vertex(const Scene3dVec3& vertex, float scene_time_seconds) {
    Scene3dVec3 value = vertex;
    value.y -= 0.8f;

    const float radius_sq = dot(value, value);
    // Accepted kukot-specific aesthetic override: keep the Java deformation shape but reverse the twist sense.
    const float twist_angle =
        -radius_sq * 0.015f * std::sin(scene_time_seconds + value.z * 0.1f);
    const float cosine = std::cos(twist_angle);
    const float sine = std::sin(twist_angle);
    const float x = value.x * cosine - value.y * sine;
    const float y = value.y * cosine + value.x * sine;

    value.x = x;
    value.y = y + 0.8f;
    return value;
}

KukotCameraState make_camera_state(const Scene3dVec3& position,
                                   const Scene3dVec3& target) {
    const Scene3dVec3 forward = normalize(subtract(target, position));
    const Scene3dVec3 world_up = make_vec3(0.0f, 0.0f, 1.0f);
    Scene3dVec3 right = normalize(cross(world_up, forward));
    if (length_sq(right) <= 1.0e-12f) {
        right = make_vec3(1.0f, 0.0f, 0.0f);
    }
    const Scene3dVec3 up = normalize(cross(forward, right));

    KukotCameraState camera;
    camera.position = position;
    camera.target = target;
    camera.forward = forward;
    camera.right = right;
    camera.up = up;
    camera.half_width = static_cast<float>(kSurfaceWidth) * 0.5f;
    camera.half_height = static_cast<float>(kSurfaceHeight) * 0.5f;
    camera.focal_length = camera.half_width / std::tan(kFieldOfView * 0.5f);
    return camera;
}

bool project_point(const KukotCameraState& camera,
                   const Scene3dVec3& world_position,
                   float* screen_x,
                   float* screen_y,
                   float* depth) {
    const Scene3dVec3 relative = subtract(world_position, camera.position);
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

std::uint32_t sample_packed_rgb_wrapped(const PackedRgbAsset& asset, float u, float v) {
    if (asset.width <= 0 || asset.height <= 0 || asset.packed_pixels.empty()) {
        return 0U;
    }

    u -= std::floor(u);
    v -= std::floor(v);
    if (u < 0.0f) {
        u += 1.0f;
    }
    if (v < 0.0f) {
        v += 1.0f;
    }

    const int x = clamp_int(static_cast<int>(u * static_cast<float>(asset.width)),
                            0,
                            asset.width - 1);
    const int y = clamp_int(static_cast<int>(v * static_cast<float>(asset.height)),
                            0,
                            asset.height - 1);
    return asset.packed_pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(asset.width) +
                               static_cast<std::size_t>(x)];
}

std::uint32_t sample_packed_rgb_clamped(const PackedRgbAsset& asset, float u, float v) {
    if (asset.width <= 0 || asset.height <= 0 || asset.packed_pixels.empty()) {
        return 0U;
    }

    const int x = clamp_int(static_cast<int>(clamp_unit(u) * static_cast<float>(asset.width)),
                            0,
                            asset.width - 1);
    const int y = clamp_int(static_cast<int>(clamp_unit(v) * static_cast<float>(asset.height)),
                            0,
                            asset.height - 1);
    return asset.packed_pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(asset.width) +
                               static_cast<std::size_t>(x)];
}

std::uint8_t sample_indexed_wrapped(const IndexedAsset& asset, float u, float v) {
    if (asset.width <= 0 || asset.height <= 0 || asset.pixels.empty()) {
        return 0U;
    }

    u -= std::floor(u);
    v -= std::floor(v);
    if (u < 0.0f) {
        u += 1.0f;
    }
    if (v < 0.0f) {
        v += 1.0f;
    }

    const int x = clamp_int(static_cast<int>(u * static_cast<float>(asset.width)),
                            0,
                            asset.width - 1);
    const int y = clamp_int(static_cast<int>(v * static_cast<float>(asset.height)),
                            0,
                            asset.height - 1);
    return asset.pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(asset.width) +
                        static_cast<std::size_t>(x)];
}

void apply_horizontal_smear(RgbSurface& surface, float factor) {
    std::vector<std::uint32_t>& pixels = surface.pixels();
    if (pixels.empty()) {
        return;
    }

    const int previous_weight = clamp_int(static_cast<int>(31.0f * factor), 0, 31);
    const int current_weight = 32 - previous_weight;
    for (int y = 0; y < surface.height(); ++y) {
        const std::size_t row_start =
            static_cast<std::size_t>(y) * static_cast<std::size_t>(surface.width());
        std::uint32_t smeared = (pack_original_packed_rgb(pixels[row_start]) >> 1) & kPackedHalfMask;
        for (int x = 0; x < surface.width(); ++x) {
            const std::size_t index = row_start + static_cast<std::size_t>(x);
            const std::uint32_t current = pack_original_packed_rgb(pixels[index]);
            smeared = ((((smeared >> 3) & kPackedBlendMask) * static_cast<std::uint32_t>(previous_weight)) +
                       (((current >> 3) & kPackedBlendMask) * static_cast<std::uint32_t>(current_weight))) >> 2;
            smeared &= kPackedColorMask;
            pixels[index] = unpack_original_packed_rgb(smeared);
        }
    }
}

void rasterize_triangle(RgbSurface& surface,
                        const PackedRgbAsset& env_surface,
                        const IndexedAsset& env_indexed,
                        const KukotTrianglePrimitive& primitive) {
    const float min_x = std::floor(std::min(primitive.a.x, std::min(primitive.b.x, primitive.c.x)));
    const float max_x = std::ceil(std::max(primitive.a.x, std::max(primitive.b.x, primitive.c.x)));
    const float min_y = std::floor(std::min(primitive.a.y, std::min(primitive.b.y, primitive.c.y)));
    const float max_y = std::ceil(std::max(primitive.a.y, std::max(primitive.b.y, primitive.c.y)));
    const int start_x = clamp_int(static_cast<int>(min_x), 0, surface.width() - 1);
    const int end_x = clamp_int(static_cast<int>(max_x), 0, surface.width() - 1);
    const int start_y = clamp_int(static_cast<int>(min_y), 0, surface.height() - 1);
    const int end_y = clamp_int(static_cast<int>(max_y), 0, surface.height() - 1);

    const float denominator =
        ((primitive.b.y - primitive.c.y) * (primitive.a.x - primitive.c.x)) +
        ((primitive.c.x - primitive.b.x) * (primitive.a.y - primitive.c.y));
    if (std::fabs(denominator) <= 1.0e-6f) {
        return;
    }

    std::vector<std::uint32_t>& pixels = surface.pixels();
    for (int y = start_y; y <= end_y; ++y) {
        for (int x = start_x; x <= end_x; ++x) {
            const float sample_x = static_cast<float>(x) + 0.5f;
            const float sample_y = static_cast<float>(y) + 0.5f;

            const float w0 =
                ((primitive.b.y - primitive.c.y) * (sample_x - primitive.c.x) +
                 (primitive.c.x - primitive.b.x) * (sample_y - primitive.c.y)) / denominator;
            const float w1 =
                ((primitive.c.y - primitive.a.y) * (sample_x - primitive.c.x) +
                 (primitive.a.x - primitive.c.x) * (sample_y - primitive.c.y)) / denominator;
            const float w2 = 1.0f - w0 - w1;
            if (w0 < 0.0f || w1 < 0.0f || w2 < 0.0f) {
                continue;
            }

            // Java material 3 keeps env UV and shade affine in screen space.
            const float u = w0 * primitive.a.u + w1 * primitive.b.u + w2 * primitive.c.u;
            const float v = w0 * primitive.a.v + w1 * primitive.b.v + w2 * primitive.c.v;
            const float shade = w0 * primitive.a.shade + w1 * primitive.b.shade + w2 * primitive.c.shade;

            const std::uint8_t env_index = sample_indexed_wrapped(env_indexed, u, v);
            const float shade_row = clamp_unit(shade) * (255.0f / 256.0f);
            const std::uint32_t color =
                sample_packed_rgb_clamped(env_surface,
                                          (static_cast<float>(env_index) + 0.5f) / 256.0f,
                                          shade_row);
            pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(surface.width()) +
                   static_cast<std::size_t>(x)] = color;
        }
    }
}

void draw_additive_sprite(RgbSurface& surface,
                          const PackedRgbAsset& asset,
                          float center_x,
                          float center_y,
                          float size) {
    const int draw_width = static_cast<int>(size);
    const int draw_height = static_cast<int>(size);
    if (draw_width <= 0 || draw_height <= 0 || asset.width <= 0 || asset.height <= 0) {
        return;
    }

    int start_x = static_cast<int>(center_x - size * 0.5f);
    int start_y = static_cast<int>(center_y - size * 0.5f);
    int clipped_width = draw_width;
    int clipped_height = draw_height;
    int source_offset_x = 0;
    int source_offset_y = 0;

    if (start_x < 0) {
        clipped_width += start_x;
        source_offset_x = -start_x;
        start_x = 0;
    }
    if (clipped_width <= 0) {
        return;
    }
    if (start_x + clipped_width > surface.width()) {
        clipped_width = surface.width() - start_x;
    }
    if (clipped_width <= 0) {
        return;
    }

    if (start_y < 0) {
        clipped_height += start_y;
        source_offset_y = -start_y;
        start_y = 0;
    }
    if (clipped_height <= 0) {
        return;
    }
    if (start_y + clipped_height > surface.height()) {
        clipped_height = surface.height() - start_y;
    }
    if (clipped_height <= 0) {
        return;
    }

    std::vector<std::uint32_t>& pixels = surface.pixels();
    const int step_x = static_cast<int>((1024.0f * static_cast<float>(asset.width)) / size);
    const int step_y = static_cast<int>((1024.0f * static_cast<float>(asset.height)) / size);
    const int source_x_fp_origin = step_x * source_offset_x;
    int source_y_fp = step_y * source_offset_y;

    for (int row = 0; row < clipped_height; ++row) {
        std::size_t pixel_index =
            static_cast<std::size_t>(start_y + row) * static_cast<std::size_t>(surface.width()) +
            static_cast<std::size_t>(start_x);
        int remaining = clipped_width;
        int source_index_fp =
            source_x_fp_origin + ((source_y_fp & ~1023) * asset.width);
        while (remaining-- > 0) {
            const std::uint32_t sample =
                asset.packed_pixels[static_cast<std::size_t>(source_index_fp >> 10)];
            pixels[pixel_index] = add_rgb_saturate(pixels[pixel_index], sample);
            ++pixel_index;
            source_index_fp += step_x;
        }
        source_y_fp += step_y;
    }
}

void apply_temporal_feedback(RgbSurface& surface,
                             const std::vector<std::uint32_t>& previous_frame) {
    std::vector<std::uint32_t>& pixels = surface.pixels();
    if (previous_frame.size() != pixels.size()) {
        return;
    }

    for (std::size_t index = 0; index < pixels.size(); ++index) {
        std::uint32_t current = pack_original_packed_rgb(pixels[index]);
        const std::uint32_t previous = pack_original_packed_rgb(previous_frame[index]);
        current += (previous >> 1) & kPackedColorMask;
        const std::uint32_t carry = current & kPackedCarryMask;
        current = current - carry | carry - (carry >> 8);
        pixels[index] = unpack_original_packed_rgb(current);
    }
}

bool parse_actor_name(const std::string& block, std::string* name) {
    std::istringstream stream(block);
    std::string line;
    while (std::getline(stream, line)) {
        const std::string trimmed = trim(line);
        if (starts_with(trimmed, "*NODE_NAME")) {
            return extract_quoted_name(trimmed, name);
        }
    }
    return false;
}

}  // namespace

KukotScene::KukotScene()
    : env_surface_(),
      env_indexed_asset_(),
      flare_asset_(),
      background_noise_(),
      actors_(),
      camera_track_(),
      camera_target_track_(),
      particles_(),
      flash_pattern_(kFlashPatternLength, 0U),
      flash_rows_(kSurfaceHeight, 0),
      background_init_random_(0x4B554B4FULL),
      background_frame_random_(0x4B554B50ULL),
      flash_init_random_(0x46534C48ULL),
      flash_frame_random_(0x46534C49ULL),
      particle_random_(0x50415254ULL),
      frame_history_(static_cast<std::size_t>(kSurfaceWidth) * static_cast<std::size_t>(kSurfaceHeight), 0U),
      flash_amount_(0.0f),
      flash_decay_(0.0f),
      ready_(false),
      error_message_() {
}

const char* KukotScene::script_name() const {
    return "kukot";
}

void KukotScene::init() {
    ready_ = load_assets();
    on_show();
}

void KukotScene::on_show() {
    flash_amount_ = 0.0f;
    flash_decay_ = 0.0f;
    background_frame_random_ = JavaRandom(0x4B554B50ULL);
    flash_frame_random_ = JavaRandom(0x46534C49ULL);
    std::fill(frame_history_.begin(), frame_history_.end(), 0U);
}

void KukotScene::dispose() {
    actors_.clear();
    particles_.clear();
    std::fill(frame_history_.begin(), frame_history_.end(), 0U);
}

void KukotScene::render(RgbSurface& surface, float scene_time_seconds, float delta_seconds) {
    surface.clear(0U);
    if (!ready_) {
        return;
    }

    const int offset_x = static_cast<int>(background_frame_random_.next_float() * 256.0f);
    const int offset_y = static_cast<int>(background_frame_random_.next_float() * 128.0f);
    std::vector<std::uint32_t>& pixels = surface.pixels();
    for (int y = 0; y < surface.height(); ++y) {
        const int source_y = (y + offset_y) & 255;
        for (int x = 0; x < surface.width(); ++x) {
            const int source_x = (x + offset_x) & 255;
            pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(surface.width()) +
                   static_cast<std::size_t>(x)] =
                background_noise_.packed_pixels[static_cast<std::size_t>(source_y) * 256U +
                                                static_cast<std::size_t>(source_x)];
        }
    }

    const float track_time_seconds = scene_time_seconds * kSceneTimeScale;
    const float track_tick = track_time_seconds * kTrackTickScale;
    const Scene3dVec3 camera_position = forward_offline::sample_track(camera_track_, track_tick);
    const Scene3dVec3 camera_target = forward_offline::sample_track(camera_target_track_, track_tick);
    const KukotCameraState camera = make_camera_state(camera_position, camera_target);

    std::vector<KukotRenderPrimitive> primitives;
    for (std::size_t actor_index = 0; actor_index < actors_.size(); ++actor_index) {
        const KukotMeshActor& actor = actors_[actor_index];
        const Scene3dVec3 translation =
            actor.position_track.empty()
                ? actor.mesh.pivot
                : forward_offline::sample_track(actor.position_track, track_tick);
        const Scene3dQuaternion orientation =
            forward_offline::sample_orientation_track(actor.orientation_track, track_tick);
        const KukotMatrix3 rotation_matrix = quaternion_matrix(orientation);

        std::vector<Scene3dVec3> world_vertices(actor.mesh.vertices.size());
        std::vector<Scene3dVec3> env_vectors(actor.mesh.vertices.size());
        std::vector<float> vertex_depths(actor.mesh.vertices.size(), 0.0f);
        std::vector<float> screen_x(actor.mesh.vertices.size(), 0.0f);
        std::vector<float> screen_y(actor.mesh.vertices.size(), 0.0f);
        std::vector<bool> projected(actor.mesh.vertices.size(), false);

        for (std::size_t vertex_index = 0; vertex_index < actor.mesh.vertices.size(); ++vertex_index) {
            const Scene3dVec3 deformed_vertex =
                deform_kukot_vertex(actor.mesh.vertices[vertex_index], track_time_seconds);
            const Scene3dVec3 rotated_vertex =
                transform_matrix3(rotation_matrix, deformed_vertex);
            const Scene3dVec3 world_position = add(rotated_vertex, translation);
            // Accepted kukot-specific aesthetic override: flip the env-map normal direction for closer body relief.
            const Scene3dVec3 env_vector =
                scale(normalize(transform_matrix3(rotation_matrix, actor.mesh.normals[vertex_index])),
                      -1.0f);

            world_vertices[vertex_index] = world_position;
            env_vectors[vertex_index] = env_vector;
            projected[vertex_index] = project_point(camera,
                                                    world_position,
                                                    &screen_x[vertex_index],
                                                    &screen_y[vertex_index],
                                                    &vertex_depths[vertex_index]);
        }

        for (std::size_t triangle_index = 0; triangle_index < actor.mesh.triangles.size(); ++triangle_index) {
            const Scene3dTriangle& triangle = actor.mesh.triangles[triangle_index];
            const std::size_t a_index = static_cast<std::size_t>(triangle.a);
            const std::size_t b_index = static_cast<std::size_t>(triangle.b);
            const std::size_t c_index = static_cast<std::size_t>(triangle.c);
            if (!projected[a_index] || !projected[b_index] || !projected[c_index]) {
                continue;
            }
            if (vertex_depths[a_index] > kFarPlane ||
                vertex_depths[b_index] > kFarPlane ||
                vertex_depths[c_index] > kFarPlane) {
                continue;
            }

            const Scene3dVec3 world_a = world_vertices[a_index];
            const Scene3dVec3 world_b = world_vertices[b_index];
            const Scene3dVec3 world_c = world_vertices[c_index];
            const Scene3dVec3 face_normal =
                normalize(cross(subtract(world_b, world_a), subtract(world_c, world_a)));
            const Scene3dVec3 face_center = scale(add(add(world_a, world_b), world_c), 1.0f / 3.0f);
            if (dot(face_normal, subtract(camera.position, face_center)) <= 0.0f) {
                continue;
            }

            KukotRenderPrimitive primitive;
            primitive.type = kKukotPrimitiveTriangle;
            primitive.triangle.a.x = screen_x[a_index];
            primitive.triangle.a.y = screen_y[a_index];
            primitive.triangle.a.depth = vertex_depths[a_index];
            primitive.triangle.a.u = 0.5f * (env_vectors[a_index].x + 1.0f);
            primitive.triangle.a.v = 0.5f * (env_vectors[a_index].y + 1.0f);
            primitive.triangle.a.shade =
                clamp_unit((vertex_depths[a_index] - kNearPlane) / (kFarPlane - kNearPlane));

            primitive.triangle.b.x = screen_x[b_index];
            primitive.triangle.b.y = screen_y[b_index];
            primitive.triangle.b.depth = vertex_depths[b_index];
            primitive.triangle.b.u = 0.5f * (env_vectors[b_index].x + 1.0f);
            primitive.triangle.b.v = 0.5f * (env_vectors[b_index].y + 1.0f);
            primitive.triangle.b.shade =
                clamp_unit((vertex_depths[b_index] - kNearPlane) / (kFarPlane - kNearPlane));

            primitive.triangle.c.x = screen_x[c_index];
            primitive.triangle.c.y = screen_y[c_index];
            primitive.triangle.c.depth = vertex_depths[c_index];
            primitive.triangle.c.u = 0.5f * (env_vectors[c_index].x + 1.0f);
            primitive.triangle.c.v = 0.5f * (env_vectors[c_index].y + 1.0f);
            primitive.triangle.c.shade =
                clamp_unit((vertex_depths[c_index] - kNearPlane) / (kFarPlane - kNearPlane));

            primitive.depth =
                (vertex_depths[a_index] + vertex_depths[b_index] + vertex_depths[c_index]) / 3.0f;
            primitive.triangle.depth =
                (vertex_depths[a_index] + vertex_depths[b_index] + vertex_depths[c_index]) / 3.0f;
            primitives.push_back(primitive);
        }
    }

    const Scene3dVec3 particle_origin = make_vec3(-5.0f, 35.0f, 5.501f);
    for (std::size_t index = 0; index < particles_.size(); ++index) {
        const Scene3dVec3 world_position = add(particle_origin, particles_[index].local_position);
        float screen_x_value = 0.0f;
        float screen_y_value = 0.0f;
        float depth = 0.0f;
        if (!project_point(camera, world_position, &screen_x_value, &screen_y_value, &depth)) {
            continue;
        }
        if (depth < kParticleMinimumDepth || depth > kFarPlane) {
            continue;
        }

        KukotRenderPrimitive primitive;
        primitive.type = kKukotPrimitiveSprite;
        primitive.depth = depth;
        primitive.sprite.center_x = screen_x_value;
        primitive.sprite.center_y = screen_y_value;
        primitive.sprite.depth = depth;
        primitive.sprite.size = kParticleSizeScale / depth;
        primitives.push_back(primitive);
    }

    std::sort(primitives.begin(),
              primitives.end(),
              [](const KukotRenderPrimitive& left, const KukotRenderPrimitive& right) {
                  return left.depth > right.depth;
              });

    for (std::size_t index = 0; index < primitives.size(); ++index) {
        if (primitives[index].type == kKukotPrimitiveTriangle) {
            rasterize_triangle(surface,
                               env_surface_,
                               env_indexed_asset_,
                               primitives[index].triangle);
        } else {
            draw_additive_sprite(surface,
                                 flare_asset_,
                                 primitives[index].sprite.center_x,
                                 primitives[index].sprite.center_y,
                                 primitives[index].sprite.size);
        }
    }

    apply_horizontal_smear(surface, kHorizontalSmearFactor);
    if (flash_amount_ > 0.0f) {
        flash_amount_ -= flash_decay_ * delta_seconds;
        if (flash_amount_ < 0.0f) {
            flash_amount_ = 0.0f;
        }

        const int line_count = clamp_int(static_cast<int>(flash_amount_), 0, surface.height() - 1);
        const int row_offset = static_cast<int>(flash_frame_random_.next_float() * 1000.0f);
        for (int line = 0; line < line_count; ++line) {
            const int row = flash_rows_[static_cast<std::size_t>((line + row_offset) % flash_rows_.size())];
            const int pattern_offset =
                static_cast<int>(flash_frame_random_.next_float() *
                                 static_cast<float>(kFlashPatternLength - 1 - surface.width()));
            std::size_t pixel_index =
                static_cast<std::size_t>(row) * static_cast<std::size_t>(surface.width());
            for (int x = 0; x < surface.width(); ++x) {
                pixels[pixel_index] =
                    add_rgb_saturate(pixels[pixel_index], flash_pattern_[static_cast<std::size_t>(pattern_offset + x)]);
                ++pixel_index;
            }
        }
    }

    apply_temporal_feedback(surface, frame_history_);
    frame_history_ = surface.pixels();
}

void KukotScene::handle_message(const std::string& message, float scene_time_seconds) {
    (void)scene_time_seconds;
    if (message == "suh") {
        flash_amount_ = 50.0f;
        flash_decay_ = 200.0f;
    } else if (message == "suh0") {
        flash_amount_ = 100.0f;
        flash_decay_ = 150.0f;
    } else if (message == "suh1") {
        flash_amount_ = 128.0f;
        flash_decay_ = 50.0f;
    } else if (message == "suh2") {
        flash_amount_ = 256.0f;
        flash_decay_ = 70.0f;
    }
}

bool KukotScene::is_ready() const {
    return ready_;
}

const std::string& KukotScene::error_message() const {
    return error_message_;
}

bool KukotScene::load_assets() {
    error_message_.clear();
    actors_.clear();
    camera_track_.clear();
    camera_target_track_.clear();

    if (!load_original_gif_indexed(image_asset_path("envplane.gif"), &env_indexed_asset_, &error_message_)) {
        return false;
    }
    build_environment_surface(env_indexed_asset_);

    if (!load_original_jpeg_packed_rgb(image_asset_path("flare1.jpg"), &flare_asset_, &error_message_)) {
        return false;
    }
    convert_original_packed_rgb_asset(&flare_asset_);

    build_background_noise();
    build_flash_tables();
    build_particle_cloud();
    return load_ase_scene();
}

bool KukotScene::load_ase_scene() {
    const std::string path = ase_asset_path("under1.ase");
    std::ifstream stream(path.c_str(), std::ios::in | std::ios::binary);
    if (!stream) {
        error_message_ = "unable to open kukot ase scene: " + path;
        return false;
    }

    std::ostringstream builder;
    builder << stream.rdbuf();
    const std::string content = builder.str();

    const std::vector<std::string> camera_blocks =
        forward_offline::extract_braced_blocks(content, "*CAMERAOBJECT");
    if (camera_blocks.empty()) {
        error_message_ = "missing camera block in kukot ase scene: " + path;
        return false;
    }

    forward_offline::parse_position_track(camera_blocks.front(), "Camera01", &camera_track_);
    forward_offline::parse_position_track(camera_blocks.front(), "Camera01.Target", &camera_target_track_);
    if (camera_track_.empty() || camera_target_track_.empty()) {
        error_message_ = "missing camera track data in kukot ase scene: " + path;
        return false;
    }

    const std::vector<std::string> geom_blocks =
        forward_offline::extract_braced_blocks(content, "*GEOMOBJECT");
    for (std::size_t index = 0; index < geom_blocks.size(); ++index) {
        KukotMeshActor actor;
        if (!parse_actor_name(geom_blocks[index], &actor.name)) {
            continue;
        }
        if (actor.name.empty() || actor.name[0] == '_') {
            continue;
        }
        if (!forward_offline::parse_mesh_vertices_and_faces(geom_blocks[index], &actor.mesh)) {
            continue;
        }

        forward_offline::parse_position_track(geom_blocks[index], actor.name, &actor.position_track);
        std::vector<Scene3dRotationSample> rotation_deltas;
        forward_offline::parse_rotation_track(geom_blocks[index], actor.name, &rotation_deltas);
        forward_offline::build_orientation_track(rotation_deltas, &actor.orientation_track);
        actors_.push_back(actor);
    }

    if (actors_.empty()) {
        error_message_ = "missing geometry in kukot ase scene: " + path;
        return false;
    }

    return true;
}

void KukotScene::build_environment_surface(const IndexedAsset& env_palette_asset) {
    env_surface_.width = kTextureSize;
    env_surface_.height = kTextureSize;
    env_surface_.packed_pixels.assign(static_cast<std::size_t>(kTextureSize) * static_cast<std::size_t>(kTextureSize), 0U);

    for (int y = 0; y < kTextureSize; ++y) {
        const float source_weight = 1.0f - static_cast<float>(y) / 255.0f;
        const float target_weight = 1.0f - source_weight;
        for (int x = 0; x < kTextureSize; ++x) {
            const int red =
                static_cast<int>(std::min(255.0f,
                                          static_cast<float>(env_palette_asset.palette_red[static_cast<std::size_t>(x)]) * source_weight +
                                              48.0f * target_weight));
            const int green =
                static_cast<int>(std::min(255.0f,
                                          static_cast<float>(env_palette_asset.palette_green[static_cast<std::size_t>(x)]) * source_weight +
                                              192.0f * target_weight));
            const int blue =
                static_cast<int>(std::min(255.0f,
                                          static_cast<float>(env_palette_asset.palette_blue[static_cast<std::size_t>(x)]) * source_weight +
                                              80.0f * target_weight));
            env_surface_.packed_pixels[static_cast<std::size_t>(y) * 256U +
                                       static_cast<std::size_t>(x)] = pack_rgb(red, green, blue);
        }
    }
}

void KukotScene::build_background_noise() {
    background_noise_.width = 256;
    background_noise_.height = 256;
    background_noise_.packed_pixels.assign(256U * 256U, 0U);

    background_init_random_ = JavaRandom(0x4B554B4FULL);
    for (int y = 0; y < 256; ++y) {
        for (int x = 0; x < 256; ++x) {
            const float a = background_init_random_.next_float();
            const float b = background_init_random_.next_float();
            const float c = background_init_random_.next_float();
            const float d = background_init_random_.next_float();
            const int red = static_cast<int>(20.0f + a * b * c * d * 200.0f);
            const int green = static_cast<int>(26.0f + background_init_random_.next_float() * 50.0f);
            const int blue = static_cast<int>(22.0f + background_init_random_.next_float() * 26.0f);
            background_noise_.packed_pixels[static_cast<std::size_t>(y) * 256U +
                                            static_cast<std::size_t>(x)] = pack_rgb(red, green, blue);
        }
    }
}

void KukotScene::build_flash_tables() {
    flash_init_random_ = JavaRandom(0x46534C48ULL);
    for (std::size_t index = 0; index < flash_pattern_.size(); ++index) {
        const int red = static_cast<int>(flash_init_random_.next_float() * 38.0f);
        const int green = static_cast<int>(flash_init_random_.next_float() * 16.0f);
        const int blue = static_cast<int>(flash_init_random_.next_float() * 87.0f);
        flash_pattern_[index] = pack_rgb(red, green, blue);
    }

    for (std::size_t index = 0; index < flash_rows_.size(); ++index) {
        flash_rows_[index] = static_cast<int>(index);
    }

    for (int iteration = 0; iteration < 3000; ++iteration) {
        const int first = iteration % static_cast<int>(flash_rows_.size());
        const int second =
            static_cast<int>(flash_init_random_.next_float() * static_cast<float>(flash_rows_.size() - 2));
        std::swap(flash_rows_[static_cast<std::size_t>(first)], flash_rows_[static_cast<std::size_t>(second)]);
    }
}

void KukotScene::build_particle_cloud() {
    particles_.clear();
    particles_.resize(180U);
    particle_random_ = JavaRandom(0x50415254ULL);

    for (std::size_t index = 0; index < particles_.size(); ++index) {
        particles_[index].local_position = make_vec3((particle_random_.next_float() - 0.5f) * 110.0f,
                                                     (particle_random_.next_float() - 0.5f) * 110.0f,
                                                     (particle_random_.next_float() - 0.5f) * 110.0f);
    }
}

std::string KukotScene::ase_asset_path(const std::string& file_name) const {
    return std::string("original/forward/asses/") + file_name;
}

std::string KukotScene::image_asset_path(const std::string& file_name) const {
    return std::string("original/forward/images/") + file_name;
}

}  // namespace forward_offline
