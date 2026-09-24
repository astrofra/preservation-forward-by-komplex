#include "scenes/maku_scene.h"

#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <sstream>

namespace forward_offline {

namespace {

const int kSurfaceWidth = 512;
const int kSurfaceHeight = 256;
const int kTerrainRampSize = 256;
const int kShockPatternLength = 1000;
const float kFieldOfView = 1.2f;
const float kNearPlane = 0.1f;
const float kFarPlane = 200.0f;
const float kFadeNear = 81.0f;
const float kDefaultTrackSpeed = 3.0f;
const float kTrackTickScale = 1000.0f;
const float kHeightScale = 1.94f;
const float kRollSpeed = 3.0f;
const float kKsorSmearFactor = 0.625f;
const std::uint32_t kPackedCarryMask = 0x10040100U;
const std::uint32_t kPackedColorMask = 0x0FF3FCFFU;
const std::uint32_t kPackedHalfMask = 0x07E1F87EU;
const std::uint32_t kPackedBlendMask = 0x01F07C1FU;
const std::uint64_t kShockSeed = 195ULL;
const std::uint64_t kShockFrameSeed = 1337ULL;

struct MakuVec3 {
    float x;
    float y;
    float z;
};

struct MakuCameraState {
    MakuVec3 position;
    MakuVec3 target;
    MakuVec3 forward;
    MakuVec3 right;
    MakuVec3 up;
    float focal_length;
    float half_width;
    float half_height;
};

struct MakuClipVertex {
    float view_x;
    float view_y;
    float view_z;
    float u;
    float v;
    float fade;
};

struct MakuScreenVertex {
    float x;
    float y;
    float u;
    float v;
    float fade;
};

struct MakuPrimitive {
    std::vector<MakuClipVertex> polygon;
    float sort_key;
};

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

int java_trunc_to_int(float value) {
    return static_cast<int>(value);
}

MakuVec3 make_vec3(float x, float y, float z) {
    MakuVec3 value;
    value.x = x;
    value.y = y;
    value.z = z;
    return value;
}

MakuVec3 scene_vec_to_maku(const Scene3dVec3& value) {
    return make_vec3(value.x, value.y, value.z);
}

MakuVec3 add(const MakuVec3& a, const MakuVec3& b) {
    return make_vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

MakuVec3 subtract(const MakuVec3& a, const MakuVec3& b) {
    return make_vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

MakuVec3 scale(const MakuVec3& value, float factor) {
    return make_vec3(value.x * factor, value.y * factor, value.z * factor);
}

float dot(const MakuVec3& a, const MakuVec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

MakuVec3 cross(const MakuVec3& a, const MakuVec3& b) {
    return make_vec3(a.y * b.z - a.z * b.y,
                     a.z * b.x - a.x * b.z,
                     a.x * b.y - a.y * b.x);
}

float length_sq(const MakuVec3& value) {
    return dot(value, value);
}

MakuVec3 normalize(const MakuVec3& value) {
    const float magnitude_sq = length_sq(value);
    if (magnitude_sq <= 1.0e-12f) {
        return make_vec3(0.0f, 0.0f, 0.0f);
    }

    const float inverse_magnitude = 1.0f / std::sqrt(magnitude_sq);
    return scale(value, inverse_magnitude);
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

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

int wrap_index(int value, int modulus) {
    if (modulus <= 0) {
        return 0;
    }

    value %= modulus;
    if (value < 0) {
        value += modulus;
    }
    return value;
}

MakuCameraState make_camera_state(const MakuVec3& position,
                                  const MakuVec3& target,
                                  float roll_angle) {
    const MakuVec3 forward = normalize(subtract(target, position));
    const MakuVec3 world_up = make_vec3(0.0f, 0.0f, 1.0f);
    MakuVec3 right = normalize(cross(world_up, forward));
    if (length_sq(right) <= 1.0e-12f) {
        right = make_vec3(1.0f, 0.0f, 0.0f);
    }
    MakuVec3 up = normalize(cross(forward, right));

    if (std::fabs(roll_angle) > 1.0e-6f) {
        const float cosine = std::cos(roll_angle);
        const float sine = std::sin(roll_angle);
        const MakuVec3 rolled_right = add(scale(right, cosine), scale(up, sine));
        const MakuVec3 rolled_up = add(scale(right, -sine), scale(up, cosine));
        right = normalize(rolled_right);
        up = normalize(rolled_up);
    }

    MakuCameraState camera;
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

void view_space_coordinates(const MakuCameraState& camera,
                            const MakuVec3& world_position,
                            float* view_x,
                            float* view_y,
                            float* view_z) {
    const MakuVec3 relative = subtract(world_position, camera.position);
    *view_x = dot(relative, camera.right);
    *view_y = dot(relative, camera.up);
    *view_z = dot(relative, camera.forward);
}

MakuVec3 camera_ray_direction(const MakuCameraState& camera, float screen_x, float screen_y) {
    const float ndc_x = (screen_x - camera.half_width) / camera.focal_length;
    const float ndc_y = (camera.half_height - screen_y) / camera.focal_length;
    MakuVec3 direction = add(camera.forward,
                             add(scale(camera.right, ndc_x), scale(camera.up, ndc_y)));
    return normalize(direction);
}

float fade_row_from_depth(float depth) {
    const float fade = (depth - kFadeNear) / (kFarPlane - kFadeNear);
    return std::min(255.0f / 256.0f, std::max(0.0f, fade));
}

MakuClipVertex make_clip_vertex(float view_x,
                                float view_y,
                                float view_z,
                                float u,
                                float v,
                                float fade) {
    MakuClipVertex vertex;
    vertex.view_x = view_x;
    vertex.view_y = view_y;
    vertex.view_z = view_z;
    vertex.u = u;
    vertex.v = v;
    vertex.fade = fade;
    return vertex;
}

MakuClipVertex interpolate_clip_vertex(const MakuClipVertex& a,
                                       const MakuClipVertex& b,
                                       float t) {
    MakuClipVertex value;
    value.view_x = lerp(a.view_x, b.view_x, t);
    value.view_y = lerp(a.view_y, b.view_y, t);
    value.view_z = lerp(a.view_z, b.view_z, t);
    value.u = lerp(a.u, b.u, t);
    value.v = lerp(a.v, b.v, t);
    value.fade = lerp(a.fade, b.fade, t);
    return value;
}

void clip_polygon_against_near_plane(std::vector<MakuClipVertex>* polygon) {
    if (polygon == NULL || polygon->empty()) {
        return;
    }

    std::vector<MakuClipVertex> input = *polygon;
    polygon->clear();

    for (std::size_t index = 0; index < input.size(); ++index) {
        const MakuClipVertex& current = input[index];
        const MakuClipVertex& previous = input[(index + input.size() - 1U) % input.size()];
        const bool current_inside = current.view_z >= kNearPlane;
        const bool previous_inside = previous.view_z >= kNearPlane;

        if (current_inside != previous_inside) {
            const float denominator = current.view_z - previous.view_z;
            if (std::fabs(denominator) > 1.0e-6f) {
                const float t = (kNearPlane - previous.view_z) / denominator;
                polygon->push_back(interpolate_clip_vertex(previous, current, t));
            }
        }
        if (current_inside) {
            polygon->push_back(current);
        }
    }
}

MakuScreenVertex project_clip_vertex(const MakuCameraState& camera,
                                     const MakuClipVertex& vertex) {
    MakuScreenVertex projected;
    projected.x = camera.half_width + vertex.view_x * camera.focal_length / vertex.view_z;
    projected.y = camera.half_height - vertex.view_y * camera.focal_length / vertex.view_z;
    projected.u = vertex.u;
    projected.v = vertex.v;
    projected.fade = vertex.fade;
    return projected;
}

std::uint32_t sample_maku_ramp(const PackedRgbAsset& ramp_surface,
                               const IndexedAsset& terrain_asset,
                               float u,
                               float v,
                               float fade) {
    if (ramp_surface.width <= 0 || ramp_surface.height <= 0 || ramp_surface.packed_pixels.empty()) {
        return 0U;
    }
    if (terrain_asset.width <= 0 || terrain_asset.height <= 0 || terrain_asset.pixels.empty()) {
        return 0U;
    }

    const int x =
        wrap_index(static_cast<int>(std::floor(u * static_cast<float>(terrain_asset.width))),
                   terrain_asset.width);
    const int y =
        wrap_index(static_cast<int>(std::floor(v * static_cast<float>(terrain_asset.height))),
                   terrain_asset.height);
    const int palette_index =
        terrain_asset.pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(terrain_asset.width) +
                             static_cast<std::size_t>(x)];
    const int fade_row =
        clamp_int(static_cast<int>(clamp_unit(fade) * 255.0f), 0, ramp_surface.height - 1);
    return ramp_surface.packed_pixels[static_cast<std::size_t>(fade_row) *
                                          static_cast<std::size_t>(ramp_surface.width) +
                                      static_cast<std::size_t>(palette_index)];
}

void rasterize_affine_triangle(RgbSurface& surface,
                               const PackedRgbAsset& ramp_surface,
                               const IndexedAsset& terrain_asset,
                               const MakuScreenVertex& a,
                               const MakuScreenVertex& b,
                               const MakuScreenVertex& c) {
    const float min_x = std::floor(std::min(a.x, std::min(b.x, c.x)));
    const float max_x = std::ceil(std::max(a.x, std::max(b.x, c.x)));
    const float min_y = std::floor(std::min(a.y, std::min(b.y, c.y)));
    const float max_y = std::ceil(std::max(a.y, std::max(b.y, c.y)));
    const int start_x = clamp_int(static_cast<int>(min_x), 0, surface.width() - 1);
    const int end_x = clamp_int(static_cast<int>(max_x), 0, surface.width() - 1);
    const int start_y = clamp_int(static_cast<int>(min_y), 0, surface.height() - 1);
    const int end_y = clamp_int(static_cast<int>(max_y), 0, surface.height() - 1);

    const float denominator =
        ((b.y - c.y) * (a.x - c.x)) + ((c.x - b.x) * (a.y - c.y));
    if (std::fabs(denominator) <= 1.0e-6f) {
        return;
    }

    std::vector<std::uint32_t>& pixels = surface.pixels();
    for (int y = start_y; y <= end_y; ++y) {
        for (int x = start_x; x <= end_x; ++x) {
            const float sample_x = static_cast<float>(x) + 0.5f;
            const float sample_y = static_cast<float>(y) + 0.5f;

            const float w0 =
                ((b.y - c.y) * (sample_x - c.x) +
                 (c.x - b.x) * (sample_y - c.y)) / denominator;
            const float w1 =
                ((c.y - a.y) * (sample_x - c.x) +
                 (a.x - c.x) * (sample_y - c.y)) / denominator;
            const float w2 = 1.0f - w0 - w1;
            if (w0 < 0.0f || w1 < 0.0f || w2 < 0.0f) {
                continue;
            }

            const float u = w0 * a.u + w1 * b.u + w2 * c.u;
            const float v = w0 * a.v + w1 * b.v + w2 * c.v;
            const float fade = w0 * a.fade + w1 * b.fade + w2 * c.fade;
            pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(surface.width()) +
                   static_cast<std::size_t>(x)] =
                sample_maku_ramp(ramp_surface, terrain_asset, u, v, fade);
        }
    }
}

void rasterize_polygon(RgbSurface& surface,
                       const PackedRgbAsset& ramp_surface,
                       const IndexedAsset& terrain_asset,
                       const MakuCameraState& camera,
                       const std::vector<MakuClipVertex>& polygon) {
    if (polygon.size() < 3U) {
        return;
    }

    std::vector<MakuScreenVertex> projected;
    projected.reserve(polygon.size());
    for (std::size_t index = 0; index < polygon.size(); ++index) {
        projected.push_back(project_clip_vertex(camera, polygon[index]));
    }

    const MakuScreenVertex& base = projected[0];
    for (std::size_t index = 1; index + 1U < projected.size(); ++index) {
        rasterize_affine_triangle(surface,
                                  ramp_surface,
                                  terrain_asset,
                                  base,
                                  projected[index],
                                  projected[index + 1U]);
    }
}

void render_maku_terrain(RgbSurface& surface,
                         const MakuCameraState& camera,
                         const IndexedAsset& height_asset,
                         const IndexedAsset& terrain_asset,
                         const PackedRgbAsset& ramp_surface) {
    if (height_asset.width <= 1 || height_asset.height <= 1) {
        return;
    }

    const int terrain_width = height_asset.width;
    const int terrain_height = height_asset.height;
    const int half_width = terrain_width / 2;
    const int half_height = terrain_height / 2;
    const float cell_size = 200.0f / static_cast<float>(terrain_width);
    const float tan_half_fov = std::tan(kFieldOfView * 0.5f);
    const float far_half_width = kFarPlane * tan_half_fov;
    const float far_half_height =
        far_half_width * static_cast<float>(kSurfaceHeight) / static_cast<float>(kSurfaceWidth);

    const MakuVec3 frustum_corners[4] = {
        add(scale(camera.forward, kFarPlane),
            add(scale(camera.right, -far_half_width), scale(camera.up, far_half_height))),
        add(scale(camera.forward, kFarPlane),
            add(scale(camera.right, far_half_width), scale(camera.up, far_half_height))),
        add(scale(camera.forward, kFarPlane),
            add(scale(camera.right, -far_half_width), scale(camera.up, -far_half_height))),
        add(scale(camera.forward, kFarPlane),
            add(scale(camera.right, far_half_width), scale(camera.up, -far_half_height)))
    };

    float min_world_x = camera.position.x;
    float max_world_x = camera.position.x;
    float min_world_y = camera.position.y;
    float max_world_y = camera.position.y;
    for (int corner_index = 0; corner_index < 4; ++corner_index) {
        const MakuVec3 corner = add(camera.position, frustum_corners[corner_index]);
        min_world_x = std::min(min_world_x, corner.x);
        max_world_x = std::max(max_world_x, corner.x);
        min_world_y = std::min(min_world_y, corner.y);
        max_world_y = std::max(max_world_y, corner.y);
    }

    const int start_grid_x = java_trunc_to_int(min_world_x / cell_size) - 1;
    const int end_grid_x = java_trunc_to_int(max_world_x / cell_size) + 2;
    const int start_grid_y = java_trunc_to_int(min_world_y / cell_size) - 1;
    const int end_grid_y = java_trunc_to_int(max_world_y / cell_size) + 2;
    const int patch_width = std::max(2, end_grid_x - start_grid_x);
    const int patch_height = std::max(2, end_grid_y - start_grid_y);

    std::vector<MakuVec3> world_vertices(static_cast<std::size_t>(patch_width * patch_height));
    std::vector<float> tex_u(static_cast<std::size_t>(patch_width * patch_height), 0.0f);
    std::vector<float> tex_v(static_cast<std::size_t>(patch_width * patch_height), 0.0f);
    std::vector<float> view_x(static_cast<std::size_t>(patch_width * patch_height), 0.0f);
    std::vector<float> view_y(static_cast<std::size_t>(patch_width * patch_height), 0.0f);
    std::vector<float> view_z(static_cast<std::size_t>(patch_width * patch_height), 0.0f);
    std::vector<float> fade_values(static_cast<std::size_t>(patch_width * patch_height), 0.0f);

    for (int row = 0; row < patch_height; ++row) {
        const int grid_y = start_grid_y + row;
        for (int column = 0; column < patch_width; ++column) {
            const int grid_x = start_grid_x + column;
            const std::size_t index = static_cast<std::size_t>(row) * static_cast<std::size_t>(patch_width) +
                                      static_cast<std::size_t>(column);
            const int sample_x = grid_x + half_width;
            const int sample_y = grid_y + half_height;
            const int wrapped_x = wrap_index(sample_x, terrain_width);
            const int wrapped_y = wrap_index(sample_y, terrain_height);
            const int height_row = terrain_height - 1 - wrapped_y;
            const int palette_index =
                height_asset.pixels[static_cast<std::size_t>(height_row) *
                                        static_cast<std::size_t>(terrain_width) +
                                    static_cast<std::size_t>(wrapped_x)];
            const float height_value =
                static_cast<float>(height_asset.palette_red[static_cast<std::size_t>(palette_index)]);
            const MakuVec3 position =
                make_vec3(static_cast<float>(grid_x) * cell_size,
                          static_cast<float>(grid_y) * cell_size,
                          height_value * kHeightScale);

            world_vertices[index] = position;
            tex_u[index] = static_cast<float>(sample_x) / static_cast<float>(terrain_width);
            tex_v[index] = static_cast<float>(sample_y) / static_cast<float>(terrain_height);
            view_space_coordinates(camera, position, &view_x[index], &view_y[index], &view_z[index]);
            fade_values[index] = fade_row_from_depth(view_z[index]);
        }
    }

    std::vector<MakuPrimitive> primitives;
    primitives.reserve(static_cast<std::size_t>((patch_width - 1) * (patch_height - 1) * 2));

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

                const MakuVec3& world_a = world_vertices[static_cast<std::size_t>(ia)];
                const MakuVec3& world_b = world_vertices[static_cast<std::size_t>(ib)];
                const MakuVec3& world_c = world_vertices[static_cast<std::size_t>(ic)];
                const float slope_x = (triangle_index == 0)
                    ? (world_c.z - world_a.z)
                    : (world_a.z - world_c.z);
                const float slope_y = (triangle_index == 0)
                    ? (world_b.z - world_c.z)
                    : (world_c.z - world_b.z);
                const float relative_x = world_a.x - camera.position.x;
                const float relative_y = world_a.y - camera.position.y;
                const float relative_z = world_a.z - camera.position.z;
                const float facing_value =
                    relative_x * slope_x + relative_y * slope_y + relative_z * -cell_size;
                if (facing_value <= 0.0f) {
                    continue;
                }

                MakuPrimitive primitive;
                primitive.sort_key =
                    (view_z[static_cast<std::size_t>(ia)] +
                     view_z[static_cast<std::size_t>(ib)] +
                     view_z[static_cast<std::size_t>(ic)]) / 3.0f;
                primitive.polygon.reserve(4U);
                primitive.polygon.push_back(make_clip_vertex(view_x[static_cast<std::size_t>(ia)],
                                                             view_y[static_cast<std::size_t>(ia)],
                                                             view_z[static_cast<std::size_t>(ia)],
                                                             tex_u[static_cast<std::size_t>(ia)],
                                                             tex_v[static_cast<std::size_t>(ia)],
                                                             fade_values[static_cast<std::size_t>(ia)]));
                primitive.polygon.push_back(make_clip_vertex(view_x[static_cast<std::size_t>(ib)],
                                                             view_y[static_cast<std::size_t>(ib)],
                                                             view_z[static_cast<std::size_t>(ib)],
                                                             tex_u[static_cast<std::size_t>(ib)],
                                                             tex_v[static_cast<std::size_t>(ib)],
                                                             fade_values[static_cast<std::size_t>(ib)]));
                primitive.polygon.push_back(make_clip_vertex(view_x[static_cast<std::size_t>(ic)],
                                                             view_y[static_cast<std::size_t>(ic)],
                                                             view_z[static_cast<std::size_t>(ic)],
                                                             tex_u[static_cast<std::size_t>(ic)],
                                                             tex_v[static_cast<std::size_t>(ic)],
                                                             fade_values[static_cast<std::size_t>(ic)]));
                clip_polygon_against_near_plane(&primitive.polygon);
                if (primitive.polygon.size() >= 3U) {
                    primitives.push_back(primitive);
                }
            }
        }
    }

    std::sort(primitives.begin(),
              primitives.end(),
              [](const MakuPrimitive& left, const MakuPrimitive& right) {
                  return left.sort_key > right.sort_key;
              });

    for (std::size_t index = 0; index < primitives.size(); ++index) {
        rasterize_polygon(surface,
                          ramp_surface,
                          terrain_asset,
                          camera,
                          primitives[index].polygon);
    }
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

void apply_average_feedback(RgbSurface& surface,
                            const std::vector<std::uint32_t>& previous_frame) {
    std::vector<std::uint32_t>& pixels = surface.pixels();
    if (previous_frame.size() != pixels.size()) {
        return;
    }

    for (std::size_t index = 0; index < pixels.size(); ++index) {
        const std::uint32_t current = pack_original_packed_rgb(pixels[index]);
        const std::uint32_t previous = pack_original_packed_rgb(previous_frame[index]);
        const std::uint32_t averaged = ((current + previous) >> 1U) & kPackedColorMask;
        pixels[index] = unpack_original_packed_rgb(averaged);
    }
}

void apply_ksor_feedback(RgbSurface& surface) {
    std::vector<std::uint32_t>& pixels = surface.pixels();
    for (std::size_t index = 0; index < pixels.size(); ++index) {
        pixels[index] ^= 0x00FFFFFFU;
    }

    apply_horizontal_smear(surface, kKsorSmearFactor);
    for (std::size_t index = 0; index < pixels.size(); ++index) {
        pixels[index] = add_rgb_saturate(pixels[index], pixels[index]);
    }
}

bool starts_with(const std::string& text, const std::string& prefix) {
    return text.size() >= prefix.size() && text.compare(0, prefix.size(), prefix) == 0;
}

}  // namespace

MakuScene::MakuScene()
    : height_asset_(),
      terrain_asset_(),
      terrain_surface_(),
      camera_track_(),
      camera_target_track_(),
      shock_pattern_(kShockPatternLength, 0),
      shock_rows_(kSurfaceHeight, 0),
      shock_init_random_(kShockSeed),
      shock_frame_random_(kShockFrameSeed),
      frame_history_(static_cast<std::size_t>(kSurfaceWidth) * static_cast<std::size_t>(kSurfaceHeight), 0U),
      ksor_enabled_(false),
      low_enabled_(false),
      roll_enabled_(false),
      roll_angle_(0.0f),
      track_speed_(kDefaultTrackSpeed),
      track_offset_seconds_(0.0f),
      track_reference_seconds_(0.0f),
      shock_amount_(0.0f),
      shock_decay_(0.0f),
      ready_(false),
      error_message_() {
}

const char* MakuScene::script_name() const {
    return "maku";
}

void MakuScene::init() {
    ready_ = load_assets();
    on_show();
}

void MakuScene::on_show() {
    ksor_enabled_ = false;
    low_enabled_ = false;
    roll_enabled_ = false;
    roll_angle_ = 0.0f;
    track_speed_ = kDefaultTrackSpeed;
    track_offset_seconds_ = 0.0f;
    track_reference_seconds_ = 0.0f;
    shock_amount_ = 0.0f;
    shock_decay_ = 0.0f;
    shock_frame_random_ = JavaRandom(kShockFrameSeed);
    std::fill(frame_history_.begin(), frame_history_.end(), 0U);
}

void MakuScene::dispose() {
    camera_track_.clear();
    camera_target_track_.clear();
    std::fill(frame_history_.begin(), frame_history_.end(), 0U);
}

void MakuScene::render(RgbSurface& surface, float scene_time_seconds, float delta_seconds) {
    surface.clear(0x00FFFFFFU);
    if (!ready_) {
        return;
    }

    if (roll_enabled_) {
        roll_angle_ += delta_seconds * kRollSpeed;
    }

    const float track_time_seconds =
        (scene_time_seconds - track_reference_seconds_) * track_speed_ + track_offset_seconds_;
    const float track_tick = track_time_seconds * kTrackTickScale;
    const Scene3dVec3 camera_position_sample =
        forward_offline::sample_track(camera_track_, track_tick);
    const Scene3dVec3 camera_target_sample =
        forward_offline::sample_track(camera_target_track_, track_tick);
    const MakuCameraState camera = make_camera_state(scene_vec_to_maku(camera_position_sample),
                                                     scene_vec_to_maku(camera_target_sample),
                                                     roll_angle_);

    render_maku_terrain(surface, camera, height_asset_, terrain_asset_, terrain_surface_);

    if (shock_amount_ > 0.0f) {
        if (shock_decay_ > 0.0f) {
            shock_amount_ -= shock_decay_ * delta_seconds;
            if (shock_amount_ < 0.0f) {
                shock_amount_ = 0.0f;
            }
        }
        apply_shock(surface, static_cast<int>(shock_amount_));
    }

    if (ksor_enabled_) {
        apply_ksor_feedback(surface);
    } else {
        apply_average_feedback(surface, frame_history_);
    }
    frame_history_ = surface.pixels();
}

void MakuScene::handle_message(const std::string& message, float scene_time_seconds) {
    if (message == "suh") {
        shock_amount_ = 120.0f;
        shock_decay_ = 200.0f;
        return;
    }
    if (message == "suh0") {
        shock_amount_ = 128.0f;
        shock_decay_ = 50.0f;
        return;
    }
    if (message == "ksor") {
        ksor_enabled_ = !ksor_enabled_;
        return;
    }
    if (message == "low") {
        low_enabled_ = !low_enabled_;
        return;
    }
    if (message == "roll") {
        roll_enabled_ = !roll_enabled_;
        return;
    }
    if (starts_with(message, "go ")) {
        track_offset_seconds_ = std::strtof(message.c_str() + 3, NULL);
        track_reference_seconds_ = scene_time_seconds;
        return;
    }
    if (starts_with(message, "speed ")) {
        track_speed_ = std::strtof(message.c_str() + 6, NULL);
    }
}

bool MakuScene::is_ready() const {
    return ready_;
}

const std::string& MakuScene::error_message() const {
    return error_message_;
}

bool MakuScene::load_assets() {
    error_message_.clear();
    camera_track_.clear();
    camera_target_track_.clear();

    if (!load_original_gif_indexed(image_asset_path("loopk40.gif"), &height_asset_, &error_message_)) {
        return false;
    }
    if (!load_original_gif_indexed(image_asset_path("loopa2.gif"), &terrain_asset_, &error_message_)) {
        return false;
    }
    if (height_asset_.width <= 1 || height_asset_.height <= 1) {
        error_message_ = "unexpected maku heightmap dimensions: " + image_asset_path("loopk40.gif");
        return false;
    }
    if (terrain_asset_.width != kTerrainRampSize || terrain_asset_.height != kTerrainRampSize) {
        error_message_ = "unexpected maku terrain texture dimensions: " + image_asset_path("loopa2.gif");
        return false;
    }

    build_terrain_surface();
    build_shock_tables(105);
    return load_ase_scene();
}

bool MakuScene::load_ase_scene() {
    const std::string path = ase_asset_path("vuori5.ase");
    std::ifstream stream(path.c_str(), std::ios::in | std::ios::binary);
    if (!stream) {
        error_message_ = "unable to open maku ase scene: " + path;
        return false;
    }

    std::ostringstream builder;
    builder << stream.rdbuf();
    const std::string content = builder.str();

    const std::vector<std::string> camera_blocks =
        forward_offline::extract_braced_blocks(content, "*CAMERAOBJECT");
    if (camera_blocks.empty()) {
        error_message_ = "missing camera block in maku ase scene: " + path;
        return false;
    }

    forward_offline::parse_position_track(camera_blocks.front(), "Camera01", &camera_track_);
    forward_offline::parse_position_track(camera_blocks.front(), "Camera01.Target", &camera_target_track_);
    if (camera_track_.empty() || camera_target_track_.empty()) {
        error_message_ = "missing camera track data in maku ase scene: " + path;
        return false;
    }

    return true;
}

void MakuScene::build_terrain_surface() {
    terrain_surface_.width = kTerrainRampSize;
    terrain_surface_.height = kTerrainRampSize;
    terrain_surface_.packed_pixels.assign(static_cast<std::size_t>(kTerrainRampSize) *
                                              static_cast<std::size_t>(kTerrainRampSize),
                                          0U);

    int gradient[256];
    for (int index = 0; index < 128; ++index) {
        const float factor = 1.0f - static_cast<float>(index) / 128.0f;
        const float inverse = 1.0f - factor;
        gradient[index] = pack_rgb(static_cast<int>(factor * 0.0f + inverse * 80.0f),
                                   static_cast<int>(factor * 0.0f + inverse * 140.0f),
                                   static_cast<int>(factor * 0.0f + inverse * 200.0f));
    }
    for (int index = 0; index < 128; ++index) {
        const float factor = 1.0f - static_cast<float>(index) / 128.0f;
        const float inverse = 1.0f - factor;
        gradient[index + 128] = pack_rgb(static_cast<int>(factor * 80.0f + inverse * 255.0f),
                                         static_cast<int>(factor * 140.0f + inverse * 255.0f),
                                         static_cast<int>(factor * 200.0f + inverse * 255.0f));
    }

    for (int row = 0; row < 256; ++row) {
        const float source_weight = 1.0f - static_cast<float>(row) / 255.0f;
        const float target_weight = 1.0f - source_weight;
        const std::uint32_t gradient_color = static_cast<std::uint32_t>(gradient[row]);
        const int gradient_red = static_cast<int>((gradient_color >> 16) & 0xffU);
        const int gradient_green = static_cast<int>((gradient_color >> 8) & 0xffU);
        const int gradient_blue = static_cast<int>(gradient_color & 0xffU);
        for (int palette_index = 0; palette_index < 256; ++palette_index) {
            const int red =
                static_cast<int>(std::min(255.0f,
                                          static_cast<float>(terrain_asset_.palette_red[static_cast<std::size_t>(palette_index)]) *
                                              source_weight +
                                              target_weight * static_cast<float>(gradient_red)));
            const int green =
                static_cast<int>(std::min(255.0f,
                                          static_cast<float>(terrain_asset_.palette_green[static_cast<std::size_t>(palette_index)]) *
                                              source_weight +
                                              target_weight * static_cast<float>(gradient_green)));
            const int blue =
                static_cast<int>(std::min(255.0f,
                                          static_cast<float>(terrain_asset_.palette_blue[static_cast<std::size_t>(palette_index)]) *
                                              source_weight +
                                              target_weight * static_cast<float>(gradient_blue)));
            terrain_surface_.packed_pixels[static_cast<std::size_t>(row) * 256U +
                                           static_cast<std::size_t>(palette_index)] =
                pack_rgb(red, green, blue);
        }
    }
}

void MakuScene::build_shock_tables(int max_gray_value) {
    shock_init_random_ = JavaRandom(kShockSeed);
    for (std::size_t index = 0; index < shock_pattern_.size(); ++index) {
        shock_pattern_[index] =
            static_cast<int>(shock_init_random_.next_float() * static_cast<float>(max_gray_value));
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

void MakuScene::apply_shock(RgbSurface& surface, int line_count) {
    if (line_count <= 0) {
        return;
    }

    if (line_count > surface.height()) {
        line_count = surface.height() - 1;
    }

    const int row_offset = static_cast<int>(shock_frame_random_.next_float() * 1000.0f);
    std::vector<std::uint32_t>& pixels = surface.pixels();
    for (int line = 0; line < line_count; ++line) {
        const int row = shock_rows_[static_cast<std::size_t>((line + row_offset) % shock_rows_.size())];
        const int pattern_offset =
            static_cast<int>(shock_frame_random_.next_float() *
                             static_cast<float>(shock_pattern_.size() - 1 - surface.width()));
        std::size_t pixel_index =
            static_cast<std::size_t>(row) * static_cast<std::size_t>(surface.width());
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

std::string MakuScene::ase_asset_path(const std::string& file_name) const {
    return std::string("original/forward/asses/") + file_name;
}

std::string MakuScene::image_asset_path(const std::string& file_name) const {
    return std::string("original/forward/images/scape/") + file_name;
}

}  // namespace forward_offline
