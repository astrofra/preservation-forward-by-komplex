#include "scenes/feta_scene.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>

namespace forward_offline {

namespace {

const int kSurfaceWidth = 512;
const int kSurfaceHeight = 256;
const float kFieldOfView = 1.9f;
const float kNearPlane = 0.1f;
const float kFarPlane = 26.0f;
const float kFetusScale = 0.09f;
const float kParticleSizeScale = 20.0f;
const float kPi = 3.14159265358979323846f;
const std::uint32_t kPackedCarryMask = 0x10040100U;
const std::uint32_t kPackedAverageMask = 0x0FF3FCFFU;
const std::uint32_t kSignedPixelMask = 0x80000000U;
const std::uint32_t kPackedGrayUnit = 0x00100401U;

struct FetaCameraState {
    Scene3dVec3 position;
    Scene3dVec3 target;
    Scene3dVec3 forward;
    Scene3dVec3 right;
    Scene3dVec3 up;
    float focal_length;
    float half_width;
    float half_height;
};

struct FetaProjectedVertex {
    float x;
    float y;
    float depth;
    float u;
    float v;
};

struct FetaTrianglePrimitive {
    FetaProjectedVertex a;
    FetaProjectedVertex b;
    FetaProjectedVertex c;
    float depth;
};

struct FetaSpritePrimitive {
    float center_x;
    float center_y;
    float depth;
    float size;
};

enum FetaPrimitiveType {
    kFetaPrimitiveTriangle = 0,
    kFetaPrimitiveSprite = 1
};

struct FetaRenderPrimitive {
    FetaPrimitiveType type;
    float depth;
    FetaTrianglePrimitive triangle;
    FetaSpritePrimitive sprite;
};

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

bool starts_with(const std::string& text, const std::string& prefix) {
    return text.size() >= prefix.size() && text.compare(0, prefix.size(), prefix) == 0;
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

Scene3dVec3 rotate_x(const Scene3dVec3& value, float angle) {
    const float cosine = std::cos(angle);
    const float sine = std::sin(angle);
    return make_vec3(value.x,
                     value.y * cosine - value.z * sine,
                     value.z * cosine + value.y * sine);
}

Scene3dVec3 rotate_z(const Scene3dVec3& value, float angle) {
    const float cosine = std::cos(angle);
    const float sine = std::sin(angle);
    return make_vec3(value.x * cosine - value.y * sine,
                     value.y * cosine + value.x * sine,
                     value.z);
}

FetaCameraState make_camera_state(const Scene3dVec3& position,
                                  const Scene3dVec3& target) {
    const Scene3dVec3 forward = normalize(subtract(target, position));
    const Scene3dVec3 world_up = make_vec3(0.0f, 0.0f, 1.0f);
    Scene3dVec3 right = normalize(cross(world_up, forward));
    if (length_sq(right) <= 1.0e-12f) {
        right = make_vec3(1.0f, 0.0f, 0.0f);
    }
    const Scene3dVec3 up = normalize(cross(forward, right));

    FetaCameraState camera;
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

bool project_point(const FetaCameraState& camera,
                   const Scene3dVec3& world_position,
                   float* screen_x,
                   float* screen_y,
                   float* depth) {
    const Scene3dVec3 relative = subtract(world_position, camera.position);
    const float view_x = dot(relative, camera.right);
    const float view_y = dot(relative, camera.up);
    const float view_z = dot(relative, camera.forward);
    if (view_z <= kNearPlane || view_z >= kFarPlane) {
        return false;
    }

    *screen_x = camera.half_width + view_x * camera.focal_length / view_z;
    *screen_y = camera.half_height - view_y * camera.focal_length / view_z;
    *depth = view_z;
    return true;
}

std::uint32_t add_packed_saturate(std::uint32_t left, std::uint32_t right) {
    const std::uint32_t sum = left + right;
    const std::uint32_t carry = sum & kPackedCarryMask;
    return (sum - carry) | (carry - (carry >> 8));
}

std::uint32_t subtract_packed_floor(std::uint32_t left, std::uint32_t right) {
    const std::uint32_t diff = left + kPackedCarryMask - right;
    const std::uint32_t borrow = diff & kPackedCarryMask;
    return diff & (borrow - (borrow >> 8));
}

std::uint32_t sample_packed_clamped(const PackedRgbAsset& asset, float u, float v) {
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

std::uint32_t sample_packed_wrapped(const PackedRgbAsset& asset, float u, float v) {
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

void render_backdrop(const FetaCameraState& camera,
                     const PackedRgbAsset& texture,
                     std::vector<std::uint32_t>* destination) {
    if (destination == NULL || static_cast<int>(destination->size()) != kSurfaceWidth * kSurfaceHeight) {
        return;
    }

    for (int y = 0; y < kSurfaceHeight; ++y) {
        for (int x = 0; x < kSurfaceWidth; ++x) {
            const float ndc_x =
                (static_cast<float>(x) + 0.5f - camera.half_width) / camera.focal_length;
            const float ndc_y =
                (camera.half_height - static_cast<float>(y) - 0.5f) / camera.focal_length;
            const Scene3dVec3 ray =
                normalize(add(camera.forward,
                              add(scale(camera.right, ndc_x), scale(camera.up, ndc_y))));
            const float rotated_x =
                ray.x * std::cos(0.7853982f) - ray.y * std::sin(0.7853982f);
            const float rotated_y =
                ray.y * std::cos(0.7853982f) + ray.x * std::sin(0.7853982f);
            const float denominator = std::max(std::fabs(rotated_x), std::fabs(rotated_y));
            const float radius =
                std::acos(std::max(0.0f, std::min(1.0f, std::fabs(ray.z)))) / (kPi * 0.5f);
            float projected_x = 0.0f;
            float projected_y = 0.0f;
            if (denominator > 1.0e-6f) {
                const float scale_factor = radius / denominator;
                projected_x = rotated_x * scale_factor;
                projected_y = rotated_y * scale_factor;
            }

            const float base_u = 0.5f * (1.0f - projected_x);
            const float base_v = 0.5f * (projected_y + 1.0f);
            const float u = (ray.z < 0.0f) ? (base_u * 0.5f) : (0.5f + base_u * 0.5f);
            const float v = base_v;
            (*destination)[static_cast<std::size_t>(y) * static_cast<std::size_t>(kSurfaceWidth) +
                           static_cast<std::size_t>(x)] =
                sample_packed_clamped(texture, u, v);
        }
    }
}

void rasterize_triangle(std::vector<std::uint32_t>* surface,
                        const FetaTrianglePrimitive& primitive,
                        const PackedRgbAsset& texture) {
    if (surface == NULL) {
        return;
    }

    const float min_x = std::floor(std::min(primitive.a.x, std::min(primitive.b.x, primitive.c.x)));
    const float max_x = std::ceil(std::max(primitive.a.x, std::max(primitive.b.x, primitive.c.x)));
    const float min_y = std::floor(std::min(primitive.a.y, std::min(primitive.b.y, primitive.c.y)));
    const float max_y = std::ceil(std::max(primitive.a.y, std::max(primitive.b.y, primitive.c.y)));
    const int start_x = clamp_int(static_cast<int>(min_x), 0, kSurfaceWidth - 1);
    const int end_x = clamp_int(static_cast<int>(max_x), 0, kSurfaceWidth - 1);
    const int start_y = clamp_int(static_cast<int>(min_y), 0, kSurfaceHeight - 1);
    const int end_y = clamp_int(static_cast<int>(max_y), 0, kSurfaceHeight - 1);

    const float denominator =
        ((primitive.b.y - primitive.c.y) * (primitive.a.x - primitive.c.x)) +
        ((primitive.c.x - primitive.b.x) * (primitive.a.y - primitive.c.y));
    if (std::fabs(denominator) <= 1.0e-6f) {
        return;
    }

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

            const float u = w0 * primitive.a.u + w1 * primitive.b.u + w2 * primitive.c.u;
            const float v = w0 * primitive.a.v + w1 * primitive.b.v + w2 * primitive.c.v;
            (*surface)[static_cast<std::size_t>(y) * static_cast<std::size_t>(kSurfaceWidth) +
                       static_cast<std::size_t>(x)] =
                sample_packed_clamped(texture, u, v);
        }
    }
}

void draw_additive_sprite(std::vector<std::uint32_t>* surface,
                          const PackedRgbAsset& asset,
                          float center_x,
                          float center_y,
                          float size) {
    if (surface == NULL || asset.width <= 0 || asset.height <= 0) {
        return;
    }

    const int draw_width = static_cast<int>(size);
    const int draw_height = static_cast<int>(size);
    if (draw_width <= 0 || draw_height <= 0) {
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
    if (start_y < 0) {
        clipped_height += start_y;
        source_offset_y = -start_y;
        start_y = 0;
    }
    if (start_x + clipped_width > kSurfaceWidth) {
        clipped_width = kSurfaceWidth - start_x;
    }
    if (start_y + clipped_height > kSurfaceHeight) {
        clipped_height = kSurfaceHeight - start_y;
    }
    if (clipped_width <= 0 || clipped_height <= 0) {
        return;
    }

    const int step_x = static_cast<int>((1024.0f * static_cast<float>(asset.width)) / size);
    const int step_y = static_cast<int>((1024.0f * static_cast<float>(asset.height)) / size);
    const int source_x_fp_origin = step_x * source_offset_x;
    int source_y_fp = step_y * source_offset_y;

    for (int row = 0; row < clipped_height; ++row) {
        std::size_t destination_index =
            static_cast<std::size_t>(start_y + row) * static_cast<std::size_t>(kSurfaceWidth) +
            static_cast<std::size_t>(start_x);
        int source_index_fp =
            source_x_fp_origin + ((source_y_fp & ~1023) * asset.width);
        for (int column = 0; column < clipped_width; ++column) {
            const std::uint32_t sample =
                asset.packed_pixels[static_cast<std::size_t>(source_index_fp >> 10)];
            (*surface)[destination_index] =
                add_packed_saturate((*surface)[destination_index], sample);
            ++destination_index;
            source_index_fp += step_x;
        }
        source_y_fp += step_y;
    }
}

void apply_temporal_average(std::vector<std::uint32_t>* packed_surface,
                            const std::vector<std::uint32_t>& history) {
    if (packed_surface == NULL || packed_surface->size() != history.size()) {
        return;
    }

    for (std::size_t index = 0; index < packed_surface->size(); ++index) {
        const std::int32_t current = static_cast<std::int32_t>((*packed_surface)[index]);
        const std::int32_t previous = static_cast<std::int32_t>(history[index]);
        (*packed_surface)[index] =
            static_cast<std::uint32_t>((current + previous) >> 1) & kPackedAverageMask;
    }
}

void convert_to_rgb_surface(const std::vector<std::uint32_t>& packed_surface,
                            RgbSurface& surface) {
    std::vector<std::uint32_t>& pixels = surface.pixels();
    if (pixels.size() != packed_surface.size()) {
        return;
    }

    for (std::size_t index = 0; index < packed_surface.size(); ++index) {
        pixels[index] = unpack_original_packed_rgb(packed_surface[index]);
    }
}

void build_igu_normals(FetaIguMesh* mesh) {
    if (mesh == NULL) {
        return;
    }

    mesh->normals.assign(mesh->vertices.size(), make_vec3(0.0f, 0.0f, 0.0f));
    for (std::size_t index = 0; index < mesh->triangles.size(); ++index) {
        const Scene3dTriangle& triangle = mesh->triangles[index];
        const Scene3dVec3& a = mesh->vertices[static_cast<std::size_t>(triangle.a)];
        const Scene3dVec3& b = mesh->vertices[static_cast<std::size_t>(triangle.b)];
        const Scene3dVec3& c = mesh->vertices[static_cast<std::size_t>(triangle.c)];
        const Scene3dVec3 normal = normalize(cross(subtract(b, a), subtract(c, a)));
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

}  // namespace

FetaScene::FetaScene()
    : backdrop_texture_(),
      env_texture_(),
      flare_texture_(),
      fetus_mesh_(),
      particles_(),
      feedback_current_(),
      feedback_next_(),
      feedback_palette_(256U, 0U),
      frame_history_(static_cast<std::size_t>(kSurfaceWidth) * static_cast<std::size_t>(kSurfaceHeight), 0U),
      particle_random_(0x46455441ULL),
      black_feta_time_(0.0f),
      black_muna_time_(0.0f),
      ready_(false),
      error_message_() {
}

const char* FetaScene::script_name() const {
    return "feta";
}

void FetaScene::init() {
    ready_ = load_assets();
    if (!ready_) {
        return;
    }

    initialize_feedback_buffers();
    update_feedback_palette(true);
    black_feta_time_ = 0.0f;
    black_muna_time_ = 0.0f;
    std::fill(frame_history_.begin(), frame_history_.end(), 0U);
}

void FetaScene::on_show() {
}

void FetaScene::dispose() {
    particles_.clear();
    feedback_current_.clear();
    feedback_next_.clear();
    std::fill(frame_history_.begin(), frame_history_.end(), 0U);
}

void FetaScene::render(RgbSurface& surface, float scene_time_seconds, float delta_seconds) {
    (void)delta_seconds;

    surface.clear(0U);
    if (!ready_) {
        return;
    }

    Scene3dVec3 camera_position = make_vec3(0.0f, 5.0f, 0.0f);
    camera_position = rotate_x(camera_position, std::sin(scene_time_seconds / 10.0f));
    camera_position = rotate_z(camera_position, scene_time_seconds / 4.0f);
    const FetaCameraState camera = make_camera_state(camera_position, make_vec3(0.0f, 0.0f, 0.0f));

    std::vector<std::uint32_t> frame_packed(
        static_cast<std::size_t>(kSurfaceWidth) * static_cast<std::size_t>(kSurfaceHeight),
        0U);
    render_backdrop(camera, backdrop_texture_, &frame_packed);

    std::vector<FetaRenderPrimitive> primitives;
    primitives.reserve(fetus_mesh_.triangles.size() + particles_.size());

    for (std::size_t triangle_index = 0; triangle_index < fetus_mesh_.triangles.size(); ++triangle_index) {
        const Scene3dTriangle& triangle = fetus_mesh_.triangles[triangle_index];
        const std::size_t a_index = static_cast<std::size_t>(triangle.a);
        const std::size_t b_index = static_cast<std::size_t>(triangle.b);
        const std::size_t c_index = static_cast<std::size_t>(triangle.c);

        const Scene3dVec3 world_a = scale(fetus_mesh_.vertices[a_index], kFetusScale);
        const Scene3dVec3 world_b = scale(fetus_mesh_.vertices[b_index], kFetusScale);
        const Scene3dVec3 world_c = scale(fetus_mesh_.vertices[c_index], kFetusScale);
        const Scene3dVec3 face_normal =
            normalize(cross(subtract(world_b, world_a), subtract(world_c, world_a)));
        const Scene3dVec3 centroid =
            scale(add(add(world_a, world_b), world_c), 1.0f / 3.0f);
        if (dot(face_normal, subtract(camera.position, centroid)) <= 0.0f) {
            continue;
        }

        FetaTrianglePrimitive primitive;
        if (!project_point(camera, world_a, &primitive.a.x, &primitive.a.y, &primitive.a.depth) ||
            !project_point(camera, world_b, &primitive.b.x, &primitive.b.y, &primitive.b.depth) ||
            !project_point(camera, world_c, &primitive.c.x, &primitive.c.y, &primitive.c.depth)) {
            continue;
        }

        const Scene3dVec3 env_a = fetus_mesh_.normals[a_index];
        const Scene3dVec3 env_b = fetus_mesh_.normals[b_index];
        const Scene3dVec3 env_c = fetus_mesh_.normals[c_index];
        primitive.a.u = 0.5f * (dot(env_a, camera.right) + 1.0f);
        primitive.a.v = 0.5f * (dot(env_a, camera.up) + 1.0f);
        primitive.b.u = 0.5f * (dot(env_b, camera.right) + 1.0f);
        primitive.b.v = 0.5f * (dot(env_b, camera.up) + 1.0f);
        primitive.c.u = 0.5f * (dot(env_c, camera.right) + 1.0f);
        primitive.c.v = 0.5f * (dot(env_c, camera.up) + 1.0f);
        primitive.depth = (primitive.a.depth + primitive.b.depth + primitive.c.depth) / 3.0f;

        FetaRenderPrimitive render_primitive;
        render_primitive.type = kFetaPrimitiveTriangle;
        render_primitive.depth = primitive.depth;
        render_primitive.triangle = primitive;
        primitives.push_back(render_primitive);
    }

    for (std::size_t index = 0; index < particles_.size(); ++index) {
        const Scene3dVec3 world_position =
            rotate_z(particles_[index].local_position, -scene_time_seconds / 2.0f);
        float screen_x = 0.0f;
        float screen_y = 0.0f;
        float depth = 0.0f;
        if (!project_point(camera, world_position, &screen_x, &screen_y, &depth)) {
            continue;
        }

        FetaRenderPrimitive primitive;
        primitive.type = kFetaPrimitiveSprite;
        primitive.depth = depth;
        primitive.sprite.center_x = screen_x;
        primitive.sprite.center_y = screen_y;
        primitive.sprite.depth = depth;
        primitive.sprite.size = kParticleSizeScale / depth;
        primitives.push_back(primitive);
    }

    std::sort(primitives.begin(),
              primitives.end(),
              [](const FetaRenderPrimitive& left, const FetaRenderPrimitive& right) {
                  return left.depth > right.depth;
              });

    for (std::size_t index = 0; index < primitives.size(); ++index) {
        if (primitives[index].type == kFetaPrimitiveTriangle) {
            rasterize_triangle(&frame_packed, primitives[index].triangle, env_texture_);
        } else {
            draw_additive_sprite(&frame_packed,
                                 flare_texture_,
                                 primitives[index].sprite.center_x,
                                 primitives[index].sprite.center_y,
                                 primitives[index].sprite.size);
        }
    }

    apply_feedback_composite(&frame_packed, scene_time_seconds);
    apply_temporal_average(&frame_packed, frame_history_);
    frame_history_ = frame_packed;
    convert_to_rgb_surface(frame_packed, surface);
}

void FetaScene::handle_message(const std::string& message, float scene_time_seconds) {
    if (message == "1") {
        update_feedback_palette(true);
    } else if (message == "2") {
        update_feedback_palette(false);
    } else if (message == "blackfeta") {
        black_feta_time_ = scene_time_seconds;
    } else if (message == "blackmuna") {
        black_muna_time_ = scene_time_seconds;
    }
}

bool FetaScene::is_ready() const {
    return ready_;
}

const std::string& FetaScene::error_message() const {
    return error_message_;
}

bool FetaScene::load_assets() {
    error_message_.clear();
    particles_.clear();
    feedback_current_.clear();
    feedback_next_.clear();

    if (!load_original_jpeg_packed_rgb(image_asset_path("verax/kosmusp.jpg"),
                                       &backdrop_texture_,
                                       &error_message_)) {
        return false;
    }
    if (!load_original_jpeg_packed_rgb(image_asset_path("babyenv.jpg"),
                                       &env_texture_,
                                       &error_message_)) {
        return false;
    }
    for (std::size_t index = 0; index < env_texture_.packed_pixels.size(); ++index) {
        env_texture_.packed_pixels[index] |= kSignedPixelMask;
    }
    if (!load_original_jpeg_packed_rgb(image_asset_path("flare1.jpg"),
                                       &flare_texture_,
                                       &error_message_)) {
        return false;
    }
    if (!load_igu_mesh(mesh_asset_path("fetus.igu"), &fetus_mesh_, &error_message_)) {
        return false;
    }

    build_particle_cloud();
    return true;
}

bool FetaScene::load_igu_mesh(const std::string& path,
                              FetaIguMesh* mesh,
                              std::string* error_message) const {
    if (mesh == NULL) {
        return false;
    }

    std::ifstream stream(path.c_str(), std::ios::in);
    if (!stream) {
        if (error_message != NULL) {
            *error_message = "unable to open igu mesh: " + path;
        }
        return false;
    }

    mesh->vertices.clear();
    mesh->normals.clear();
    mesh->triangles.clear();

    int remaining_vertices = 0;
    int remaining_faces = 0;
    bool vertex_block_seen = false;
    std::string line;
    while (std::getline(stream, line)) {
        const std::string trimmed = trim(line);
        if (remaining_vertices > 0) {
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            if (std::sscanf(trimmed.c_str(), "X: %f, Y: %f, Z: %f", &x, &y, &z) == 3) {
                mesh->vertices.push_back(make_vec3(x, y, z));
                --remaining_vertices;
            }
            continue;
        }
        if (remaining_faces > 0) {
            int a = 0;
            int b = 0;
            int c = 0;
            if (std::sscanf(trimmed.c_str(), "A %d, B %d, C %d", &a, &b, &c) == 3) {
                Scene3dTriangle triangle;
                triangle.a = a;
                triangle.b = b;
                triangle.c = c;
                mesh->triangles.push_back(triangle);
                --remaining_faces;
            }
            continue;
        }

        if (starts_with(trimmed, "Vertices:")) {
            std::istringstream count_stream(trimmed.substr(9));
            int count = 0;
            count_stream >> count;
            if (!vertex_block_seen) {
                remaining_vertices = count;
                vertex_block_seen = true;
            }
            continue;
        }

        if (starts_with(trimmed, "Faces:")) {
            std::istringstream count_stream(trimmed.substr(6));
            count_stream >> remaining_faces;
        }
    }

    if (mesh->vertices.empty() || mesh->triangles.empty()) {
        if (error_message != NULL) {
            *error_message = "unable to parse igu mesh data: " + path;
        }
        return false;
    }

    build_igu_normals(mesh);
    return true;
}

void FetaScene::build_particle_cloud() {
    particles_.clear();
    particles_.resize(300U);
    particle_random_ = JavaRandom(0x46455441ULL);
    for (std::size_t index = 0; index < particles_.size(); ++index) {
        particles_[index].local_position =
            make_vec3((particle_random_.next_float() - 0.5f) * 10.0f,
                      (particle_random_.next_float() - 0.5f) * 10.0f,
                      (particle_random_.next_float() - 0.5f) * 10.0f);
    }
}

void FetaScene::initialize_feedback_buffers() {
    const std::size_t pixel_count =
        static_cast<std::size_t>(kSurfaceWidth) * static_cast<std::size_t>(kSurfaceHeight);
    feedback_current_.assign(pixel_count, 0U);
    feedback_next_.assign(pixel_count, 0U);

    for (std::size_t index = 0; index < pixel_count; ++index) {
        feedback_current_[index] = static_cast<std::uint8_t>(index & 0xFFU);
    }
}

void FetaScene::update_feedback_palette(bool black_index_255) {
    for (int index = 0; index < 256; ++index) {
        int red = std::min(255, index * 2);
        int green = std::min(255, index * 3);
        int blue = std::min(255, index);
        if (black_index_255 && index == 255) {
            red = 0;
            green = 0;
            blue = 0;
        }
        feedback_palette_[static_cast<std::size_t>(index)] =
            ((static_cast<std::uint32_t>(red) & 0xffU) << 20) |
            ((static_cast<std::uint32_t>(green) & 0xffU) << 10) |
            (static_cast<std::uint32_t>(blue) & 0xffU);
    }
}

void FetaScene::apply_feedback_composite(std::vector<std::uint32_t>* packed_surface,
                                         float scene_time_seconds) {
    if (packed_surface == NULL || packed_surface->size() != feedback_current_.size()) {
        return;
    }

    const int n = kSurfaceWidth;
    const int n2 = kSurfaceHeight;
    const double scale_inverse = 1.0 / 1.100000023841858;
    const int step_x = static_cast<int>(scale_inverse * 65536.0);
    const int step_y = step_x;
    const double half_width = static_cast<double>(n) / 2.0;
    const double half_height = static_cast<double>(n2) / 2.0;
    int start_x =
        static_cast<int>(-(half_width * scale_inverse) * 65536.0) +
        static_cast<int>(half_width * 65536.0);
    int start_y =
        static_cast<int>(-(half_height * scale_inverse) * 65536.0) +
        static_cast<int>(half_height * 65536.0);

    const bool mask_signed_pixels = black_feta_time_ == 0.0f;
    std::size_t pixel_index = 0U;
    for (int row = 0; row < n2; ++row) {
        int sample_x_fp = start_x;
        int sample_y_fp = start_y;
        for (int column = 0; column < n; ++column) {
            if (mask_signed_pixels &&
                (((*packed_surface)[pixel_index] & kSignedPixelMask) != 0U)) {
                feedback_next_[pixel_index] = 255U;
            } else {
                const std::size_t source_index =
                    static_cast<std::size_t>(((sample_x_fp >> 16) & 511) |
                                             (((sample_y_fp >> 16) & 255) << 9));
                const std::uint8_t value =
                    static_cast<std::uint8_t>((feedback_current_[source_index] & 0xffU) >> 1U);
                feedback_next_[pixel_index] = value;
                if (value != 0U) {
                    (*packed_surface)[pixel_index] =
                        add_packed_saturate((*packed_surface)[pixel_index],
                                            feedback_palette_[value]);
                }
            }
            ++pixel_index;
            sample_x_fp += step_x;
        }
        start_y += step_y;
    }

    feedback_current_.swap(feedback_next_);

    if (black_feta_time_ == 0.0f) {
        return;
    }

    int subtract_signed = static_cast<int>(
        std::min(255.0f, std::max(0.0f, (scene_time_seconds - black_feta_time_) * 0.7f * 255.0f)));
    int subtract_unsigned = 0;
    if (black_muna_time_ != 0.0f) {
        subtract_unsigned = static_cast<int>(
            std::min(255.0f, std::max(0.0f, (scene_time_seconds - black_muna_time_) * 0.4f * 255.0f)));
    }

    const std::uint32_t packed_signed =
        static_cast<std::uint32_t>(subtract_signed) * kPackedGrayUnit;
    const std::uint32_t packed_unsigned =
        static_cast<std::uint32_t>(subtract_unsigned) * kPackedGrayUnit;
    for (std::size_t index = 0; index < packed_surface->size(); ++index) {
        if (((*packed_surface)[index] & kSignedPixelMask) != 0U) {
            (*packed_surface)[index] =
                subtract_packed_floor((*packed_surface)[index] & ~kSignedPixelMask,
                                      packed_signed);
        } else {
            (*packed_surface)[index] =
                subtract_packed_floor((*packed_surface)[index], packed_unsigned);
        }
    }
}

std::string FetaScene::image_asset_path(const std::string& file_name) const {
    return std::string("original/forward/images/") + file_name;
}

std::string FetaScene::mesh_asset_path(const std::string& file_name) const {
    return std::string("original/forward/meshes/") + file_name;
}

}  // namespace forward_offline
