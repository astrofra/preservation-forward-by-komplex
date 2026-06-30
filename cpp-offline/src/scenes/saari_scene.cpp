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
const float kNearPlane = 1.0f;
const float kDepthRampNear = 15.0f;
const float kDepthRampFar = 250.0f;
const float kTerrainHeightScale = 0.16f;

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
    v = clamp_unit(v);

    const int x = clamp_int(static_cast<int>(u * static_cast<float>(asset.width)), 0, asset.width - 1);
    const int y = clamp_int(static_cast<int>(v * static_cast<float>(asset.height)), 0, asset.height - 1);
    const std::size_t index = static_cast<std::size_t>(y) * static_cast<std::size_t>(asset.width) +
                              static_cast<std::size_t>(x);
    const int palette_index = asset.pixels[index];
    return pack_rgb(asset.palette_red[static_cast<std::size_t>(palette_index)],
                    asset.palette_green[static_cast<std::size_t>(palette_index)],
                    asset.palette_blue[static_cast<std::size_t>(palette_index)]);
}

float sample_height_value(const IndexedAsset& asset, int x, int y) {
    const int clamped_x = clamp_int(x, 0, asset.width - 1);
    const int clamped_y = clamp_int(y, 0, asset.height - 1);
    const std::size_t index = static_cast<std::size_t>(clamped_y) * static_cast<std::size_t>(asset.width) +
                              static_cast<std::size_t>(clamped_x);
    const int palette_index = asset.pixels[index];
    return static_cast<float>(asset.palette_red[static_cast<std::size_t>(palette_index)]) - 16.0f;
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

void draw_background(RgbSurface& surface,
                     const CameraState& camera,
                     const PackedRgbAsset& sky_asset,
                     const IndexedAsset& water_asset,
                     float scene_time_seconds) {
    std::vector<std::uint32_t>& pixels = surface.pixels();
    const SaariVec3 fog_color = make_vec3(234.0f, 239.0f, 255.0f);

    for (int y = 0; y < surface.height(); ++y) {
        for (int x = 0; x < surface.width(); ++x) {
            const SaariVec3 ray = camera_ray_direction(camera,
                                                       static_cast<float>(x) + 0.5f,
                                                       static_cast<float>(y) + 0.5f);
            std::uint32_t color = 0;

            if (camera.position.z > 0.0f && ray.z < -1.0e-4f) {
                const float t = camera.position.z / -ray.z;
                const SaariVec3 hit = add(camera.position, scale(ray, t));
                const float wave = std::sin(hit.x * 0.021f + scene_time_seconds * 0.65f) * 0.010f +
                                   std::cos(hit.y * 0.016f - scene_time_seconds * 0.42f) * 0.010f;
                const float tex_u = hit.x * 0.0042f + scene_time_seconds * 0.010f + wave;
                const float tex_v = hit.y * 0.0048f - scene_time_seconds * 0.008f - wave * 1.5f;
                color = sample_indexed_asset(water_asset, tex_u, tex_v);
                const float horizon_mix = clamp_unit(1.0f - std::fabs(ray.z) * 28.0f);
                if (horizon_mix > 0.0f) {
                    const float sky_u = 0.10f + std::atan2(ray.y, ray.x) / (2.0f * 3.14159265f);
                    const float sky_v = std::acos(clamp_unit(ray.z * 0.5f + 0.5f)) / 3.14159265f;
                    const std::uint32_t sky_color = sample_packed_rgb_asset(sky_asset, sky_u, sky_v);
                    color = blend_rgb(color, sky_color, horizon_mix * 0.35f);
                }
            } else {
                const float sky_u = 0.10f + std::atan2(ray.y, ray.x) / (2.0f * 3.14159265f);
                const float sky_v = std::acos(clamp_unit(ray.z * 0.5f + 0.5f)) / 3.14159265f;
                color = sample_packed_rgb_asset(sky_asset, sky_u, sky_v);
                const float zenith_boost = clamp_unit(ray.z * 0.75f + 0.25f);
                color = blend_rgb(color, pack_rgb(static_cast<int>(fog_color.x),
                                                  static_cast<int>(fog_color.y),
                                                  static_cast<int>(fog_color.z)),
                                  zenith_boost * 0.12f);
            }

            pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(surface.width()) +
                   static_cast<std::size_t>(x)] = color;
        }
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

void render_terrain(RgbSurface& surface,
                    std::vector<float>* depth_buffer,
                    const CameraState& camera,
                    const IndexedAsset& height_asset,
                    const IndexedAsset& terrain_asset,
                    bool reflection_pass,
                    float reflection_alpha) {
    if (height_asset.width <= 1 || height_asset.height <= 1) {
        return;
    }

    const int terrain_width = height_asset.width;
    const int terrain_height = height_asset.height;
    const float cell_size = 200.0f / static_cast<float>(terrain_width);

    std::vector<SaariVec3> world_vertices(static_cast<std::size_t>(terrain_width * terrain_height));
    std::vector<float> screen_x(static_cast<std::size_t>(terrain_width * terrain_height), 0.0f);
    std::vector<float> screen_y(static_cast<std::size_t>(terrain_width * terrain_height), 0.0f);
    std::vector<float> screen_depth(static_cast<std::size_t>(terrain_width * terrain_height), 0.0f);
    std::vector<bool> visible(static_cast<std::size_t>(terrain_width * terrain_height), false);

    for (int row = 0; row < terrain_height; ++row) {
        for (int column = 0; column < terrain_width; ++column) {
            const std::size_t index = static_cast<std::size_t>(row) * static_cast<std::size_t>(terrain_width) +
                                      static_cast<std::size_t>(column);
            const float height = sample_height_value(height_asset, column, row) * kTerrainHeightScale;
            const float x = (static_cast<float>(column) - static_cast<float>(terrain_width - 1) * 0.5f) * cell_size;
            const float y = (static_cast<float>(terrain_height - 1 - row) - static_cast<float>(terrain_height - 1) * 0.5f) *
                            cell_size;
            const float z = reflection_pass ? -height : height;
            world_vertices[index] = make_vec3(x, y, z);

            const SaariVec3 position = world_vertices[index];
            visible[index] = project_point(camera, position, &screen_x[index], &screen_y[index], &screen_depth[index]);

        }
    }

    for (int row = 0; row < terrain_height - 1; ++row) {
        for (int column = 0; column < terrain_width - 1; ++column) {
            const int a = row * terrain_width + column;
            const int b = row * terrain_width + column + 1;
            const int c = (row + 1) * terrain_width + column;
            const int d = (row + 1) * terrain_width + column + 1;
            const int triangle_indices[2][3] = {
                {a, d, b},
                {d, a, c}
            };

            for (int triangle_index = 0; triangle_index < 2; ++triangle_index) {
                const int ia = triangle_indices[triangle_index][0];
                const int ib = triangle_indices[triangle_index][1];
                const int ic = triangle_indices[triangle_index][2];
                if (!visible[static_cast<std::size_t>(ia)] ||
                    !visible[static_cast<std::size_t>(ib)] ||
                    !visible[static_cast<std::size_t>(ic)]) {
                    continue;
                }

                const SaariVec3& world_a = world_vertices[static_cast<std::size_t>(ia)];
                const SaariVec3& world_b = world_vertices[static_cast<std::size_t>(ib)];
                const SaariVec3& world_c = world_vertices[static_cast<std::size_t>(ic)];
                const SaariVec3 face_normal =
                    normalize(cross(subtract(world_b, world_a), subtract(world_c, world_a)));
                const SaariVec3 face_center = scale(add(add(world_a, world_b), world_c), 1.0f / 3.0f);
                if (dot(face_normal, subtract(camera.position, face_center)) <= 0.0f) {
                    continue;
                }

                ScreenVertex vertices[3];
                const int indices[3] = {ia, ib, ic};
                for (int vertex_index = 0; vertex_index < 3; ++vertex_index) {
                    const int grid_index = indices[vertex_index];
                    const int grid_x = grid_index % terrain_width;
                    const int grid_y = grid_index / terrain_width;
                    vertices[vertex_index].x = screen_x[static_cast<std::size_t>(grid_index)];
                    vertices[vertex_index].y = screen_y[static_cast<std::size_t>(grid_index)];
                    vertices[vertex_index].depth = screen_depth[static_cast<std::size_t>(grid_index)];
                    vertices[vertex_index].inv_depth = 1.0f / vertices[vertex_index].depth;
                    vertices[vertex_index].u =
                        static_cast<float>(grid_x) / static_cast<float>(terrain_width - 1);
                    vertices[vertex_index].v =
                        static_cast<float>(grid_y) / static_cast<float>(terrain_height - 1);
                    vertices[vertex_index].shade = 1.0f;
                }

                rasterize_textured_triangle(surface,
                                            depth_buffer,
                                            vertices[0],
                                            vertices[1],
                                            vertices[2],
                                            terrain_asset,
                                            reflection_pass ? reflection_alpha : 1.0f,
                                            !reflection_pass,
                                            reflection_pass ? kDepthFadeToBlack : kDepthFadeNone,
                                            reflection_pass);
            }
        }
    }
}

void render_env_mesh(RgbSurface& surface,
                     std::vector<float>* depth_buffer,
                     const CameraState& camera,
                     const SaariStaticMesh& mesh,
                     const IndexedAsset& env_asset,
                     const SaariVec3& translation,
                     float rotation_x,
                     float rotation_y,
                     float rotation_z,
                     bool reflection_pass,
                     float alpha) {
    if (mesh.vertices.empty() || mesh.normals.empty() || mesh.triangles.empty()) {
        return;
    }

    std::vector<SaariVec3> world_vertices(mesh.vertices.size());
    std::vector<SaariVec3> world_normals(mesh.normals.size());
    std::vector<float> screen_x(mesh.vertices.size(), 0.0f);
    std::vector<float> screen_y(mesh.vertices.size(), 0.0f);
    std::vector<float> screen_depth(mesh.vertices.size(), 0.0f);
    std::vector<bool> visible(mesh.vertices.size(), false);

    for (std::size_t index = 0; index < mesh.vertices.size(); ++index) {
        SaariVec3 world_position = rotate_xyz(mesh.vertices[index], rotation_x, rotation_y, rotation_z);
        world_position = add(world_position, translation);
        SaariVec3 world_normal = normalize(rotate_xyz(mesh.normals[index], rotation_x, rotation_y, rotation_z));

        if (reflection_pass) {
            world_position.z = -world_position.z;
            world_normal.z = -world_normal.z;
        }

        world_vertices[index] = world_position;
        world_normals[index] = world_normal;
        visible[index] = project_point(camera, world_position, &screen_x[index], &screen_y[index], &screen_depth[index]);
    }

    for (std::size_t triangle_index = 0; triangle_index < mesh.triangles.size(); ++triangle_index) {
        const SaariTriangle& triangle = mesh.triangles[triangle_index];
        if (!visible[static_cast<std::size_t>(triangle.a)] ||
            !visible[static_cast<std::size_t>(triangle.b)] ||
            !visible[static_cast<std::size_t>(triangle.c)]) {
            continue;
        }

        const SaariVec3& world_a = world_vertices[static_cast<std::size_t>(triangle.a)];
        const SaariVec3& world_b = world_vertices[static_cast<std::size_t>(triangle.b)];
        const SaariVec3& world_c = world_vertices[static_cast<std::size_t>(triangle.c)];
        const SaariVec3 face_normal =
            normalize(cross(subtract(world_b, world_a), subtract(world_c, world_a)));
        const SaariVec3 face_center = scale(add(add(world_a, world_b), world_c), 1.0f / 3.0f);
        if (dot(face_normal, subtract(camera.position, face_center)) <= 0.0f) {
            continue;
        }

        ScreenVertex vertices[3];
        const int indices[3] = {triangle.a, triangle.b, triangle.c};
        for (int vertex_index = 0; vertex_index < 3; ++vertex_index) {
            const int mesh_index = indices[vertex_index];
            const SaariVec3& world_normal = world_normals[static_cast<std::size_t>(mesh_index)];
            const float env_u = 0.5f + dot(world_normal, camera.right) * 0.5f;
            const float env_v = 0.5f + dot(world_normal, camera.up) * 0.5f;

            vertices[vertex_index].x = screen_x[static_cast<std::size_t>(mesh_index)];
            vertices[vertex_index].y = screen_y[static_cast<std::size_t>(mesh_index)];
            vertices[vertex_index].depth = screen_depth[static_cast<std::size_t>(mesh_index)];
            vertices[vertex_index].inv_depth = 1.0f / vertices[vertex_index].depth;
            vertices[vertex_index].u = env_u;
            vertices[vertex_index].v = env_v;
            vertices[vertex_index].shade = 1.0f;
        }

        rasterize_textured_triangle(surface,
                                    depth_buffer,
                                    vertices[0],
                                    vertices[1],
                                    vertices[2],
                                    env_asset,
                                    alpha,
                                    !reflection_pass,
                                    reflection_pass ? kDepthFadeToBlack : kDepthFadeNone,
                                    reflection_pass);
    }
}

}  // namespace

SaariScene::SaariScene()
    : sky_asset_(),
      saari_asset_(),
      terrain_asset_(),
      water_asset_(),
      env_asset_(),
      height_asset_(),
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
    const SaariVec3 camera_position = sample_track(camera_track_, track_tick);
    const SaariVec3 camera_target = sample_track(camera_target_track_, track_tick);
    const CameraState camera = make_camera_state(camera_position, camera_target);

    draw_background(surface, camera, sky_asset_, water_asset_, scene_time_seconds);

    std::vector<float> depth_buffer(static_cast<std::size_t>(surface.width()) * static_cast<std::size_t>(surface.height()),
                                    std::numeric_limits<float>::infinity());

    render_terrain(surface, NULL, camera, height_asset_, terrain_asset_, true, 0.34f);
    render_terrain(surface, &depth_buffer, camera, height_asset_, terrain_asset_, false, 1.0f);

    const SaariVec3 klunssi_position = sample_track(klunssi_track_, track_tick);
    render_env_mesh(surface,
                    NULL,
                    camera,
                    klunssi_mesh_,
                    env_asset_,
                    klunssi_position,
                    scene_time_seconds / 3.0f,
                    scene_time_seconds * 2.0f / 3.0f,
                    scene_time_seconds,
                    true,
                    0.28f);
    render_env_mesh(surface,
                    &depth_buffer,
                    camera,
                    klunssi_mesh_,
                    env_asset_,
                    klunssi_position,
                    scene_time_seconds / 3.0f,
                    scene_time_seconds * 2.0f / 3.0f,
                    scene_time_seconds,
                    false,
                    1.0f);

    render_env_mesh(surface,
                    NULL,
                    camera,
                    meditate_mesh_,
                    env_asset_,
                    meditate_position_,
                    0.0f,
                    0.0f,
                    3.14159265f,
                    true,
                    0.18f);
    render_env_mesh(surface,
                    &depth_buffer,
                    camera,
                    meditate_mesh_,
                    env_asset_,
                    meditate_position_,
                    0.0f,
                    0.0f,
                    3.14159265f,
                    false,
                    0.95f);

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

    const std::vector<std::string> camera_blocks = extract_braced_blocks(content, "*CAMERAOBJECT");
    if (camera_blocks.empty()) {
        error_message_ = "missing camera block in saari ase scene: " + path;
        return false;
    }

    parse_position_track(camera_blocks.front(), "Camera01", &camera_track_);
    parse_position_track(camera_blocks.front(), "Camera01.Target", &camera_target_track_);
    parse_rotation_track(camera_blocks.front(), "Camera01", &camera_rotation_track_);
    if (camera_track_.empty() || camera_target_track_.empty() || camera_rotation_track_.empty()) {
        error_message_ = "missing camera track data in saari ase scene: " + path;
        return false;
    }

    const std::vector<std::string> geom_blocks = extract_braced_blocks(content, "*GEOMOBJECT");
    bool found_meditate = false;
    bool found_klunssi = false;
    for (std::size_t index = 0; index < geom_blocks.size(); ++index) {
        const std::string& block = geom_blocks[index];
        if (!found_meditate && block.find("*NODE_NAME \"meditate\"") != std::string::npos) {
            if (!parse_mesh_vertices_and_faces(block, &meditate_mesh_)) {
                error_message_ = "failed to parse meditate mesh from saari ase scene: " + path;
                return false;
            }
            meditate_position_ = meditate_mesh_.pivot;
            found_meditate = true;
        } else if (!found_klunssi && block.find("*NODE_NAME \"klunssi\"") != std::string::npos) {
            if (!parse_mesh_vertices_and_faces(block, &klunssi_mesh_)) {
                error_message_ = "failed to parse klunssi mesh from saari ase scene: " + path;
                return false;
            }
            klunssi_initial_position_ = klunssi_mesh_.pivot;
            parse_position_track(block, "klunssi", &klunssi_track_);
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
