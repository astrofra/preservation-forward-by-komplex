#include "scenes/watercube_scene.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>

namespace forward_offline {

namespace {

const int kSurfaceWidth = 512;
const int kSurfaceHeight = 256;
const int kRippleSize = 256;
const int kFlashPatternLength = 1000;
const float kFieldOfView = 1.49f;
const float kNearPlane = 0.1f;
const float kPi = 3.14159265358979323846f;
const std::uint32_t kPackedCarryMask = 0x10040100U;
const std::uint32_t kPackedColorMask = 0x0FF3FCFFU;

struct WatercubeCameraState {
    Scene3dVec3 position;
    Scene3dVec3 target;
    Scene3dVec3 forward;
    Scene3dVec3 right;
    Scene3dVec3 up;
    float focal_length;
    float half_width;
    float half_height;
};

struct WatercubeProjectedVertex {
    float x;
    float y;
    float depth;
    float u;
    float v;
};

struct WatercubePrimitive {
    WatercubeProjectedVertex a;
    WatercubeProjectedVertex b;
    WatercubeProjectedVertex c;
    const PackedRgbAsset* texture;
    float depth;
    bool additive;
};

struct WatercubeMatrix3 {
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

Scene3dVec3 rotate_euler(const Scene3dVec3& value, float rotate_x, float rotate_y, float rotate_z) {
    Scene3dVec3 result = value;

    float cosine = std::cos(rotate_x);
    float sine = std::sin(rotate_x);
    float y = result.y * cosine - result.z * sine;
    float z = result.z * cosine + result.y * sine;
    result.y = y;
    result.z = z;

    cosine = std::cos(rotate_y);
    sine = std::sin(rotate_y);
    float x = result.x * cosine + result.z * sine;
    z = result.z * cosine - result.x * sine;
    result.x = x;
    result.z = z;

    cosine = std::cos(rotate_z);
    sine = std::sin(rotate_z);
    x = result.x * cosine - result.y * sine;
    y = result.y * cosine + result.x * sine;
    result.x = x;
    result.y = y;

    return result;
}

WatercubeMatrix3 identity_matrix3() {
    WatercubeMatrix3 matrix;
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

void matrix_rotate_x_in_place(WatercubeMatrix3* matrix, float angle) {
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

void matrix_rotate_y_in_place(WatercubeMatrix3* matrix, float angle) {
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

void matrix_rotate_z_in_place(WatercubeMatrix3* matrix, float angle) {
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

WatercubeMatrix3 build_rotation_matrix(float angle_x, float angle_y, float angle_z) {
    WatercubeMatrix3 matrix = identity_matrix3();
    matrix_rotate_x_in_place(&matrix, angle_x);
    matrix_rotate_y_in_place(&matrix, angle_y);
    matrix_rotate_z_in_place(&matrix, angle_z);
    return matrix;
}

Scene3dVec3 transform_matrix3(const WatercubeMatrix3& matrix, const Scene3dVec3& value) {
    return make_vec3(matrix.m00 * value.x + matrix.m10 * value.y + matrix.m20 * value.z,
                     matrix.m01 * value.x + matrix.m11 * value.y + matrix.m21 * value.z,
                     matrix.m02 * value.x + matrix.m12 * value.y + matrix.m22 * value.z);
}

void build_textured_mesh_normals(WatercubeTexturedMesh* mesh) {
    if (mesh == NULL) {
        return;
    }

    mesh->normals.assign(mesh->vertices.size(), make_vec3(0.0f, 0.0f, 0.0f));
    for (std::size_t index = 0; index < mesh->triangles.size(); ++index) {
        const WatercubeTexturedTriangle& triangle = mesh->triangles[index];
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

void build_igu_normals(WatercubeIguMesh* mesh) {
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

bool parse_textured_ase_mesh(const std::string& block, WatercubeTexturedMesh* mesh) {
    if (mesh == NULL) {
        return false;
    }

    mesh->vertices.clear();
    mesh->normals.clear();
    mesh->texcoords.clear();
    mesh->triangles.clear();
    mesh->pivot = make_vec3(0.0f, 0.0f, 0.0f);

    std::vector<Scene3dVec3> world_vertices;
    std::vector<WatercubeUvCoord> texcoords;
    std::vector<WatercubeTexturedTriangle> triangles;
    std::istringstream stream(block);
    std::string line;

    while (std::getline(stream, line)) {
        const std::string trimmed = trim(line);
        if (starts_with(trimmed, "*MESH_VERTEX")) {
            int index = 0;
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            if (std::sscanf(trimmed.c_str(), "*MESH_VERTEX %d %f %f %f",
                            &index, &x, &y, &z) == 4) {
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
            if (std::sscanf(trimmed.c_str(), "*MESH_FACE %d: A: %d B: %d C: %d",
                            &face_index, &a, &b, &c) == 4) {
                if (face_index >= static_cast<int>(triangles.size())) {
                    triangles.resize(static_cast<std::size_t>(face_index + 1));
                }
                triangles[static_cast<std::size_t>(face_index)].a = a;
                triangles[static_cast<std::size_t>(face_index)].b = b;
                triangles[static_cast<std::size_t>(face_index)].c = c;
                triangles[static_cast<std::size_t>(face_index)].ta = a;
                triangles[static_cast<std::size_t>(face_index)].tb = b;
                triangles[static_cast<std::size_t>(face_index)].tc = c;
            }
        } else if (starts_with(trimmed, "*MESH_TVERT")) {
            int index = 0;
            float u = 0.0f;
            float v = 0.0f;
            float w = 0.0f;
            if (std::sscanf(trimmed.c_str(), "*MESH_TVERT %d %f %f %f",
                            &index, &u, &v, &w) == 4) {
                (void)w;
                if (index >= static_cast<int>(texcoords.size())) {
                    texcoords.resize(static_cast<std::size_t>(index + 1));
                }
                texcoords[static_cast<std::size_t>(index)].u = u;
                texcoords[static_cast<std::size_t>(index)].v = v;
            }
        } else if (starts_with(trimmed, "*MESH_TFACE")) {
            int face_index = 0;
            int ta = 0;
            int tb = 0;
            int tc = 0;
            if (std::sscanf(trimmed.c_str(), "*MESH_TFACE %d %d %d %d",
                            &face_index, &ta, &tb, &tc) == 4) {
                if (face_index >= static_cast<int>(triangles.size())) {
                    triangles.resize(static_cast<std::size_t>(face_index + 1));
                }
                triangles[static_cast<std::size_t>(face_index)].ta = ta;
                triangles[static_cast<std::size_t>(face_index)].tb = tb;
                triangles[static_cast<std::size_t>(face_index)].tc = tc;
            }
        } else if (starts_with(trimmed, "*TM_POS")) {
            std::sscanf(trimmed.c_str(), "*TM_POS %f %f %f",
                        &mesh->pivot.x, &mesh->pivot.y, &mesh->pivot.z);
        }
    }

    if (world_vertices.empty() || triangles.empty()) {
        return false;
    }

    mesh->vertices.resize(world_vertices.size());
    for (std::size_t index = 0; index < world_vertices.size(); ++index) {
        mesh->vertices[index] = subtract(world_vertices[index], mesh->pivot);
    }
    mesh->texcoords = texcoords;
    mesh->triangles = triangles;
    build_textured_mesh_normals(mesh);
    return true;
}

std::uint32_t pack_original_rgb(int red, int green, int blue) {
    return ((static_cast<std::uint32_t>(clamp_int(red, 0, 255)) & 0xffU) << 20) |
           ((static_cast<std::uint32_t>(clamp_int(green, 0, 255)) & 0xffU) << 10) |
           (static_cast<std::uint32_t>(clamp_int(blue, 0, 255)) & 0xffU);
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

void clear_packed_surface(PackedRgbAsset* surface, std::uint32_t packed_color) {
    if (surface == NULL) {
        return;
    }
    std::fill(surface->packed_pixels.begin(), surface->packed_pixels.end(), packed_color);
}

void packed_copy_blit(PackedRgbAsset* destination,
                      const PackedRgbAsset& source,
                      int x,
                      int y) {
    if (destination == NULL) {
        return;
    }

    int source_x = 0;
    int source_y = 0;
    int width = source.width;
    int height = source.height;

    if (x < 0) {
        width += x;
        source_x = -x;
        x = 0;
    }
    if (y < 0) {
        height += y;
        source_y = -y;
        y = 0;
    }
    if (x + width > destination->width) {
        width = destination->width - x;
    }
    if (y + height > destination->height) {
        height = destination->height - y;
    }
    if (width <= 0 || height <= 0) {
        return;
    }

    for (int row = 0; row < height; ++row) {
        const std::size_t source_index =
            static_cast<std::size_t>(source_y + row) * static_cast<std::size_t>(source.width) +
            static_cast<std::size_t>(source_x);
        const std::size_t destination_index =
            static_cast<std::size_t>(y + row) * static_cast<std::size_t>(destination->width) +
            static_cast<std::size_t>(x);
        std::copy(source.packed_pixels.begin() + static_cast<std::ptrdiff_t>(source_index),
                  source.packed_pixels.begin() + static_cast<std::ptrdiff_t>(source_index + width),
                  destination->packed_pixels.begin() + static_cast<std::ptrdiff_t>(destination_index));
    }
}

void packed_add_blit(PackedRgbAsset* destination,
                     const PackedRgbAsset& source,
                     int x,
                     int y) {
    if (destination == NULL) {
        return;
    }

    int source_x = 0;
    int source_y = 0;
    int width = source.width;
    int height = source.height;

    if (x < 0) {
        width += x;
        source_x = -x;
        x = 0;
    }
    if (y < 0) {
        height += y;
        source_y = -y;
        y = 0;
    }
    if (x + width > destination->width) {
        width = destination->width - x;
    }
    if (y + height > destination->height) {
        height = destination->height - y;
    }
    if (width <= 0 || height <= 0) {
        return;
    }

    for (int row = 0; row < height; ++row) {
        std::size_t source_index =
            static_cast<std::size_t>(source_y + row) * static_cast<std::size_t>(source.width) +
            static_cast<std::size_t>(source_x);
        std::size_t destination_index =
            static_cast<std::size_t>(y + row) * static_cast<std::size_t>(destination->width) +
            static_cast<std::size_t>(x);
        for (int column = 0; column < width; ++column) {
            destination->packed_pixels[destination_index] =
                add_packed_saturate(destination->packed_pixels[destination_index],
                                    source.packed_pixels[source_index]);
            ++source_index;
            ++destination_index;
        }
    }
}

void packed_shift_fade(PackedRgbAsset* surface, int shift) {
    if (surface == NULL || shift <= 0) {
        return;
    }

    const int fade_mask = 255 - ((1 << shift) - 1);
    const std::uint32_t mask =
        static_cast<std::uint32_t>(fade_mask | (fade_mask << 10) | (fade_mask << 20));
    for (std::size_t index = 0; index < surface->packed_pixels.size(); ++index) {
        surface->packed_pixels[index] = (surface->packed_pixels[index] & mask) >> shift;
    }
}

void packed_scaled_add_blit(PackedRgbAsset* destination,
                            const PackedRgbAsset& source,
                            float x,
                            float y,
                            float width,
                            float height) {
    if (destination == NULL) {
        return;
    }

    int draw_x = static_cast<int>(x);
    int draw_y = static_cast<int>(y);
    int draw_width = static_cast<int>(width);
    int draw_height = static_cast<int>(height);
    int source_offset_x = 0;
    int source_offset_y = 0;

    if (draw_x < 0) {
        draw_width += draw_x;
        source_offset_x = -draw_x;
        draw_x = 0;
    }
    if (draw_y < 0) {
        draw_height += draw_y;
        source_offset_y = -draw_y;
        draw_y = 0;
    }
    if (draw_width <= 0 || draw_height <= 0) {
        return;
    }
    if (draw_x + draw_width > destination->width) {
        draw_width = destination->width - draw_x;
    }
    if (draw_y + draw_height > destination->height) {
        draw_height = destination->height - draw_y;
    }
    if (draw_width <= 0 || draw_height <= 0) {
        return;
    }

    const int step_x = static_cast<int>((1024.0f * static_cast<float>(source.width)) / width);
    const int step_y = static_cast<int>((1024.0f * static_cast<float>(source.height)) / height);
    const int source_x_fp_origin = step_x * source_offset_x;
    int source_y_fp = step_y * source_offset_y;

    for (int row = 0; row < draw_height; ++row) {
        std::size_t destination_index =
            static_cast<std::size_t>(draw_y + row) * static_cast<std::size_t>(destination->width) +
            static_cast<std::size_t>(draw_x);
        int source_index_fp =
            source_x_fp_origin + ((source_y_fp & ~1023) * source.width);
        for (int column = 0; column < draw_width; ++column) {
            destination->packed_pixels[destination_index] =
                add_packed_saturate(destination->packed_pixels[destination_index],
                                    source.packed_pixels[static_cast<std::size_t>(source_index_fp >> 10)]);
            ++destination_index;
            source_index_fp += step_x;
        }
        source_y_fp += step_y;
    }
}

WatercubeCameraState make_camera_state(const Scene3dVec3& position,
                                       const Scene3dVec3& target,
                                       float roll_angle) {
    const Scene3dVec3 forward = normalize(subtract(target, position));
    const Scene3dVec3 world_up = make_vec3(0.0f, 0.0f, 1.0f);
    Scene3dVec3 right = normalize(cross(world_up, forward));
    if (length_sq(right) <= 1.0e-12f) {
        right = make_vec3(1.0f, 0.0f, 0.0f);
    }
    Scene3dVec3 up = normalize(cross(forward, right));

    if (std::fabs(roll_angle) > 1.0e-6f) {
        const float cosine = std::cos(roll_angle);
        const float sine = std::sin(roll_angle);
        const Scene3dVec3 rolled_right = add(scale(right, cosine), scale(up, sine));
        const Scene3dVec3 rolled_up = add(scale(right, -sine), scale(up, cosine));
        right = normalize(rolled_right);
        up = normalize(rolled_up);
    }

    WatercubeCameraState camera;
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

bool project_point(const WatercubeCameraState& camera,
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

    const int x =
        clamp_int(static_cast<int>(u * static_cast<float>(asset.width)), 0, asset.width - 1);
    const int y =
        clamp_int(static_cast<int>(v * static_cast<float>(asset.height)), 0, asset.height - 1);
    return asset.packed_pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(asset.width) +
                               static_cast<std::size_t>(x)];
}

std::uint32_t sample_packed_clamped(const PackedRgbAsset& asset, float u, float v) {
    if (asset.width <= 0 || asset.height <= 0 || asset.packed_pixels.empty()) {
        return 0U;
    }

    const int x =
        clamp_int(static_cast<int>(clamp_unit(u) * static_cast<float>(asset.width)), 0, asset.width - 1);
    const int y =
        clamp_int(static_cast<int>(clamp_unit(v) * static_cast<float>(asset.height)), 0, asset.height - 1);
    return asset.packed_pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(asset.width) +
                               static_cast<std::size_t>(x)];
}

void rasterize_triangle(PackedRgbAsset* surface, const WatercubePrimitive& primitive) {
    if (surface == NULL || primitive.texture == NULL) {
        return;
    }

    const float min_x = std::floor(std::min(primitive.a.x, std::min(primitive.b.x, primitive.c.x)));
    const float max_x = std::ceil(std::max(primitive.a.x, std::max(primitive.b.x, primitive.c.x)));
    const float min_y = std::floor(std::min(primitive.a.y, std::min(primitive.b.y, primitive.c.y)));
    const float max_y = std::ceil(std::max(primitive.a.y, std::max(primitive.b.y, primitive.c.y)));
    const int start_x = clamp_int(static_cast<int>(min_x), 0, surface->width - 1);
    const int end_x = clamp_int(static_cast<int>(max_x), 0, surface->width - 1);
    const int start_y = clamp_int(static_cast<int>(min_y), 0, surface->height - 1);
    const int end_y = clamp_int(static_cast<int>(max_y), 0, surface->height - 1);

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
            const std::uint32_t sample = sample_packed_clamped(*primitive.texture, u, v);
            const std::size_t index =
                static_cast<std::size_t>(y) * static_cast<std::size_t>(surface->width) +
                static_cast<std::size_t>(x);
            if (primitive.additive) {
                surface->packed_pixels[index] =
                    add_packed_saturate(surface->packed_pixels[index], sample);
            } else {
                surface->packed_pixels[index] = sample;
            }
        }
    }
}

void apply_flash_noise(PackedRgbAsset* surface,
                       const std::vector<std::uint32_t>& pattern,
                       const std::vector<int>& rows,
                       JavaRandom* random,
                       int amount) {
    if (surface == NULL || random == NULL || pattern.empty() || rows.empty()) {
        return;
    }

    int absolute_amount = std::abs(amount);
    if (absolute_amount > surface->height) {
        absolute_amount = surface->height - 1;
    }
    const int pattern_offset = static_cast<int>(random->next_float() * 1000.0f);

    for (int index = 0; index < absolute_amount; ++index) {
        const int row = rows[static_cast<std::size_t>((index + pattern_offset) % rows.size())];
        int pattern_start =
            static_cast<int>(random->next_float() * static_cast<float>(pattern.size() - 1 - surface->width));
        if (pattern_start < 0) {
            pattern_start = 0;
        }
        std::size_t pixel_index =
            static_cast<std::size_t>(row) * static_cast<std::size_t>(surface->width);
        for (int x = 0; x < surface->width; ++x) {
            if (amount > 0) {
                surface->packed_pixels[pixel_index] =
                    add_packed_saturate(surface->packed_pixels[pixel_index],
                                        pattern[static_cast<std::size_t>(pattern_start + x)]);
            } else {
                surface->packed_pixels[pixel_index] =
                    subtract_packed_floor(surface->packed_pixels[pixel_index],
                                          pattern[static_cast<std::size_t>(pattern_start + x)]);
            }
            ++pixel_index;
        }
    }
}

void convert_to_rgb_surface(const PackedRgbAsset& packed_surface, RgbSurface& surface) {
    std::vector<std::uint32_t>& pixels = surface.pixels();
    const std::size_t count = std::min(pixels.size(), packed_surface.packed_pixels.size());
    for (std::size_t index = 0; index < count; ++index) {
        pixels[index] = unpack_original_packed_rgb(packed_surface.packed_pixels[index]);
    }
}

}  // namespace

WatercubeScene::WatercubeScene()
    : overlay_panel_texture_(),
      overlay_scroll_texture_(),
      box_texture_(),
      env_texture_(),
      ring_texture_(),
      ripple_texture_(),
      wave_a_(),
      wave_b_(),
      water_texture_(),
      panel_surface_(),
      water_mesh_(),
      box_mesh_(),
      kluns1_mesh_(),
      kluns2_mesh_(),
      camera_track_(),
      camera_target_track_(),
      flash_pattern_(kFlashPatternLength, 0U),
      flash_rows_(static_cast<std::size_t>(kSurfaceHeight), 0),
      frame_packed_(static_cast<std::size_t>(kSurfaceWidth) * static_cast<std::size_t>(kSurfaceHeight), 0U),
      flash_init_random_(0x57415446ULL),
      render_random_(0x57415452ULL),
      second_kluns_enabled_(true),
      wave_toggle_(true),
      mode_scale_(kSurfaceHeight / 128),
      ripple_phase_(0),
      text_strip_offset_(0),
      last_tick_index_(-1),
      simulation_tick_count_(0),
      roll_impulse_(0.0f),
      flash_amount_(0.0f),
      flash_decay_(0.0f),
      shock_amount_(0.0f),
      shock_decay_(0.0f),
      ready_(false),
      error_message_() {
}

const char* WatercubeScene::script_name() const {
    return "watercube";
}

void WatercubeScene::init() {
    ready_ = load_assets();
    on_show();
}

void WatercubeScene::on_show() {
    wave_toggle_ = true;
    ripple_phase_ = 0;
    text_strip_offset_ = 0;
    last_tick_index_ = -1;
    simulation_tick_count_ = 0;
    roll_impulse_ = 0.0f;
    flash_amount_ = 0.0f;
    flash_decay_ = 0.0f;
    shock_amount_ = 0.0f;
    shock_decay_ = 0.0f;
    render_random_ = JavaRandom(0x57415452ULL);
    clear_packed_surface(&wave_a_, 0U);
    clear_packed_surface(&wave_b_, 0U);
    clear_packed_surface(&water_texture_, 0U);
    clear_packed_surface(&panel_surface_, 0U);
    std::fill(frame_packed_.begin(), frame_packed_.end(), 0U);
}

void WatercubeScene::dispose() {
    wave_a_.packed_pixels.clear();
    wave_b_.packed_pixels.clear();
    water_texture_.packed_pixels.clear();
    panel_surface_.packed_pixels.clear();
    frame_packed_.clear();
}

void WatercubeScene::render(RgbSurface& surface, float scene_time_seconds, float delta_seconds) {
    surface.clear(0U);
    if (!ready_) {
        return;
    }

    const std::int64_t scene_tick = static_cast<std::int64_t>(scene_time_seconds * 50.0f);
    std::int64_t tick_delta = last_tick_index_ < 0 ? scene_tick + 1 : scene_tick - last_tick_index_;
    if (tick_delta < 0) {
        tick_delta = 0;
    }
    last_tick_index_ = scene_tick;

    if (tick_delta > 1) {
        roll_impulse_ *= static_cast<float>(std::pow(0.917, static_cast<double>(tick_delta - 1)));
    }

    for (std::int64_t index = 0; index < tick_delta; ++index) {
        step_simulation();
    }

    PackedRgbAsset frame_surface;
    frame_surface.width = kSurfaceWidth;
    frame_surface.height = kSurfaceHeight;
    frame_surface.packed_pixels = frame_packed_;
    clear_packed_surface(&frame_surface, 0U);

    const float track_time_seconds = scene_time_seconds * 1.8f + 2.0f;
    const float track_tick = track_time_seconds * 1000.0f;
    const Scene3dVec3 camera_position = forward_offline::sample_track(camera_track_, track_tick);
    const Scene3dVec3 camera_target = forward_offline::sample_track(camera_target_track_, track_tick);
    const WatercubeCameraState camera =
        make_camera_state(camera_position, camera_target, roll_impulse_ * 2.0f * kPi);

    std::vector<WatercubePrimitive> primitives;
    primitives.reserve(box_mesh_.triangles.size() + water_mesh_.triangles.size() +
                       kluns1_mesh_.triangles.size() + kluns2_mesh_.triangles.size());

    auto enqueue_mesh = [&](const WatercubeTexturedMesh& mesh,
                            const PackedRgbAsset& texture,
                            bool additive) {
        for (std::size_t triangle_index = 0; triangle_index < mesh.triangles.size(); ++triangle_index) {
            const WatercubeTexturedTriangle& triangle = mesh.triangles[triangle_index];
            const Scene3dVec3 world_a = add(mesh.vertices[static_cast<std::size_t>(triangle.a)], mesh.pivot);
            const Scene3dVec3 world_b = add(mesh.vertices[static_cast<std::size_t>(triangle.b)], mesh.pivot);
            const Scene3dVec3 world_c = add(mesh.vertices[static_cast<std::size_t>(triangle.c)], mesh.pivot);
            const Scene3dVec3 centroid = scale(add(add(world_a, world_b), world_c), 1.0f / 3.0f);
            const Scene3dVec3 normal = normalize(cross(subtract(world_b, world_a), subtract(world_c, world_a)));
            if (dot(normal, subtract(camera.position, centroid)) <= 0.0f) {
                continue;
            }

            WatercubePrimitive primitive;
            float depth_a = 0.0f;
            float depth_b = 0.0f;
            float depth_c = 0.0f;
            if (!project_point(camera, world_a, &primitive.a.x, &primitive.a.y, &depth_a) ||
                !project_point(camera, world_b, &primitive.b.x, &primitive.b.y, &depth_b) ||
                !project_point(camera, world_c, &primitive.c.x, &primitive.c.y, &depth_c)) {
                continue;
            }

            primitive.a.depth = depth_a;
            primitive.b.depth = depth_b;
            primitive.c.depth = depth_c;
            primitive.a.u = mesh.texcoords[static_cast<std::size_t>(triangle.ta)].u;
            primitive.a.v = mesh.texcoords[static_cast<std::size_t>(triangle.ta)].v;
            primitive.b.u = mesh.texcoords[static_cast<std::size_t>(triangle.tb)].u;
            primitive.b.v = mesh.texcoords[static_cast<std::size_t>(triangle.tb)].v;
            primitive.c.u = mesh.texcoords[static_cast<std::size_t>(triangle.tc)].u;
            primitive.c.v = mesh.texcoords[static_cast<std::size_t>(triangle.tc)].v;
            primitive.texture = &texture;
            primitive.depth = (depth_a + depth_b + depth_c) / 3.0f;
            primitive.additive = additive;
            primitives.push_back(primitive);
        }
    };

    auto enqueue_env_mesh = [&](const WatercubeIguMesh& mesh,
                                float translate_z,
                                float rotate_x_base) {
        const float rotate_x = rotate_x_base + static_cast<float>(simulation_tick_count_) * 0.02f;
        const float rotate_z = static_cast<float>(simulation_tick_count_) * 0.07f;
        const Scene3dVec3 translation = make_vec3(0.0f, 0.0f, translate_z);
        const float object_scale = 0.45f;
        const WatercubeMatrix3 rotation_matrix = build_rotation_matrix(rotate_x, 0.0f, rotate_z);

        for (std::size_t triangle_index = 0; triangle_index < mesh.triangles.size(); ++triangle_index) {
            const Scene3dTriangle& triangle = mesh.triangles[triangle_index];
            const Scene3dVec3 local_a =
                scale(transform_matrix3(rotation_matrix,
                                        mesh.vertices[static_cast<std::size_t>(triangle.a)]),
                      object_scale);
            const Scene3dVec3 local_b =
                scale(transform_matrix3(rotation_matrix,
                                        mesh.vertices[static_cast<std::size_t>(triangle.b)]),
                      object_scale);
            const Scene3dVec3 local_c =
                scale(transform_matrix3(rotation_matrix,
                                        mesh.vertices[static_cast<std::size_t>(triangle.c)]),
                      object_scale);
            const Scene3dVec3 world_a = add(local_a, translation);
            const Scene3dVec3 world_b = add(local_b, translation);
            const Scene3dVec3 world_c = add(local_c, translation);
            const Scene3dVec3 centroid = scale(add(add(world_a, world_b), world_c), 1.0f / 3.0f);
            const Scene3dVec3 normal = normalize(cross(subtract(world_b, world_a), subtract(world_c, world_a)));
            if (dot(normal, subtract(camera.position, centroid)) <= 0.0f) {
                continue;
            }

            WatercubePrimitive primitive;
            float depth_a = 0.0f;
            float depth_b = 0.0f;
            float depth_c = 0.0f;
            if (!project_point(camera, world_a, &primitive.a.x, &primitive.a.y, &depth_a) ||
                !project_point(camera, world_b, &primitive.b.x, &primitive.b.y, &depth_b) ||
                !project_point(camera, world_c, &primitive.c.x, &primitive.c.y, &depth_c)) {
                continue;
            }

            // Match MeshObject.KKAMaja: transform the smoothed vertex normal by the object's
            // local rotation matrix, then project its X/Y components into [0,1] UV space.
            const Scene3dVec3 env_a =
                normalize(transform_matrix3(rotation_matrix,
                                            mesh.normals[static_cast<std::size_t>(triangle.a)]));
            const Scene3dVec3 env_b =
                normalize(transform_matrix3(rotation_matrix,
                                            mesh.normals[static_cast<std::size_t>(triangle.b)]));
            const Scene3dVec3 env_c =
                normalize(transform_matrix3(rotation_matrix,
                                            mesh.normals[static_cast<std::size_t>(triangle.c)]));

            primitive.a.depth = depth_a;
            primitive.b.depth = depth_b;
            primitive.c.depth = depth_c;
            primitive.a.u = 0.5f * (env_a.x + 1.0f);
            primitive.a.v = 0.5f * (env_a.y + 1.0f);
            primitive.b.u = 0.5f * (env_b.x + 1.0f);
            primitive.b.v = 0.5f * (env_b.y + 1.0f);
            primitive.c.u = 0.5f * (env_c.x + 1.0f);
            primitive.c.v = 0.5f * (env_c.y + 1.0f);
            primitive.texture = &env_texture_;
            primitive.depth = (depth_a + depth_b + depth_c) / 3.0f;
            primitive.additive = false;
            primitives.push_back(primitive);
        }
    };

    enqueue_mesh(box_mesh_, box_texture_, false);
    enqueue_mesh(water_mesh_, water_texture_, false);
    enqueue_env_mesh(kluns1_mesh_, 20.0f, 0.7f);
    if (second_kluns_enabled_) {
        enqueue_env_mesh(kluns2_mesh_, -20.0f, -0.7f);
    }

    std::sort(primitives.begin(),
              primitives.end(),
              [](const WatercubePrimitive& left, const WatercubePrimitive& right) {
                  return left.depth > right.depth;
              });
    for (std::size_t index = 0; index < primitives.size(); ++index) {
        rasterize_triangle(&frame_surface, primitives[index]);
    }

    packed_add_blit(&panel_surface_,
                    overlay_panel_texture_,
                    -292 + static_cast<int>(render_random_.next_float() * 20.0f) - 20,
                    -80 + static_cast<int>(render_random_.next_float() * 40.0f) - 20);
    packed_shift_fade(&panel_surface_, 1);
    packed_scaled_add_blit(&frame_surface,
                           panel_surface_,
                           static_cast<float>(126 * mode_scale_),
                           0.0f,
                           static_cast<float>(128 * mode_scale_),
                           static_cast<float>(128 * mode_scale_));
    packed_scaled_add_blit(&frame_surface,
                           overlay_scroll_texture_,
                           -scene_time_seconds * 135.0f,
                           -260.0f,
                           1280.0f,
                           960.0f);
    if (text_strip_offset_ != 0) {
        packed_add_blit(&frame_surface, overlay_scroll_texture_, -200, text_strip_offset_);
    }

    if (tick_delta > 0) {
        roll_impulse_ *= 0.917f;
    }
    if (flash_amount_ > 0.0f) {
        apply_flash_noise(&frame_surface,
                          flash_pattern_,
                          flash_rows_,
                          &render_random_,
                          static_cast<int>(flash_amount_));
        flash_amount_ -= flash_decay_ * delta_seconds;
    }
    if (shock_amount_ > 0.0f) {
        const int offset_x = static_cast<int>(-(render_random_.next_float() * 384.0f));
        const int offset_y = static_cast<int>(-(render_random_.next_float() * 352.0f));
        packed_add_blit(&frame_surface, overlay_scroll_texture_, offset_x, offset_y);
        packed_add_blit(&frame_surface, overlay_scroll_texture_, offset_x + 640, offset_y);
        packed_add_blit(&frame_surface, overlay_scroll_texture_, offset_x + 640, offset_y + 480);
        packed_add_blit(&frame_surface, overlay_scroll_texture_, offset_x, offset_y + 480);
        shock_amount_ -= shock_decay_ * delta_seconds;
    }

    frame_packed_ = frame_surface.packed_pixels;
    convert_to_rgb_surface(frame_surface, surface);
}

void WatercubeScene::handle_message(const std::string& message, float scene_time_seconds) {
    (void)scene_time_seconds;

    if (message == "suh") {
        flash_amount_ = 50.0f;
        flash_decay_ = 200.0f;
    } else if (message == "suh0") {
        flash_amount_ = 100.0f;
        flash_decay_ = 150.0f;
    } else if (message == "suh1") {
        flash_amount_ = 128.0f;
        flash_decay_ = 120.0f;
    } else if (message == "suh2") {
        flash_amount_ = 256.0f;
        flash_decay_ = 90.0f;
    } else if (message == "rok") {
        roll_impulse_ = 1.0f;
    } else if (message == "pum") {
        shock_amount_ = 100.0f;
        shock_decay_ = 130.0f;
    } else if (message == "tex0") {
        text_strip_offset_ = -80;
    } else if (message == "tex1") {
        text_strip_offset_ = -160;
    } else if (message == "tex2") {
        text_strip_offset_ = -240;
    } else if (message == "tex3") {
        text_strip_offset_ = -320;
    }
}

bool WatercubeScene::is_ready() const {
    return ready_;
}

const std::string& WatercubeScene::error_message() const {
    return error_message_;
}

bool WatercubeScene::load_assets() {
    std::string error;
    if (!load_original_jpeg_packed_rgb(image_asset_path("1.jpg"),
                                       &overlay_panel_texture_,
                                       &error)) {
        error_message_ = error;
        return false;
    }
    if (!load_original_jpeg_packed_rgb(image_asset_path("txt1.jpg"),
                                       &overlay_scroll_texture_,
                                       &error)) {
        error_message_ = error;
        return false;
    }
    if (!load_original_jpeg_packed_rgb(image_asset_path("reunus2.jpg"),
                                       &box_texture_,
                                       &error)) {
        error_message_ = error;
        return false;
    }
    if (!load_original_jpeg_packed_rgb(image_asset_path("env3.jpg"),
                                       &env_texture_,
                                       &error)) {
        error_message_ = error;
        return false;
    }
    if (!load_original_jpeg_packed_rgb(image_asset_path("rinku2.jpg"),
                                       &ring_texture_,
                                       &error)) {
        error_message_ = error;
        return false;
    }
    if (!load_original_jpeg_packed_rgb(image_asset_path("riple2.jpg"),
                                       &ripple_texture_,
                                       &error)) {
        error_message_ = error;
        return false;
    }
    if (!load_ase_scene()) {
        return false;
    }
    if (!load_igu_mesh(mesh_asset_path("kluns1.igu"), &kluns1_mesh_, &error)) {
        error_message_ = error;
        return false;
    }
    if (second_kluns_enabled_ &&
        !load_igu_mesh(mesh_asset_path("kluns2.igu"), &kluns2_mesh_, &error)) {
        error_message_ = error;
        return false;
    }

    wave_a_.width = kRippleSize;
    wave_a_.height = kRippleSize;
    wave_a_.packed_pixels.assign(static_cast<std::size_t>(kRippleSize * kRippleSize), 0U);
    wave_b_ = wave_a_;
    water_texture_.width = kRippleSize;
    water_texture_.height = kRippleSize;
    water_texture_.packed_pixels.assign(static_cast<std::size_t>(kRippleSize * kRippleSize), 0U);
    panel_surface_.width = 128;
    panel_surface_.height = 128;
    panel_surface_.packed_pixels.assign(128U * 128U, 0U);

    build_flash_tables();
    return true;
}

bool WatercubeScene::load_ase_scene() {
    std::ifstream stream(ase_asset_path("nosto3.ase").c_str(), std::ios::in | std::ios::binary);
    if (!stream) {
        error_message_ = "unable to open ase asset: " + ase_asset_path("nosto3.ase");
        return false;
    }

    std::ostringstream buffer;
    buffer << stream.rdbuf();
    const std::string content = buffer.str();

    const std::vector<std::string> camera_blocks =
        forward_offline::extract_braced_blocks(content, "*CAMERAOBJECT");
    if (camera_blocks.empty()) {
        error_message_ = "missing camera block in ase asset: " + ase_asset_path("nosto3.ase");
        return false;
    }
    forward_offline::parse_position_track(camera_blocks.front(), "Camera01", &camera_track_);
    forward_offline::parse_position_track(camera_blocks.front(), "Camera01.target", &camera_target_track_);

    const std::vector<std::string> geom_blocks =
        forward_offline::extract_braced_blocks(content, "*GEOMOBJECT");
    for (std::size_t index = 0; index < geom_blocks.size(); ++index) {
        std::string name;
        if (!parse_actor_name(geom_blocks[index], &name)) {
            continue;
        }

        if (name == "TriPatch01") {
            if (!parse_textured_ase_mesh(geom_blocks[index], &water_mesh_)) {
                error_message_ = "unable to parse watercube water mesh from ase asset";
                return false;
            }
        } else if (name == "Box01") {
            if (!parse_textured_ase_mesh(geom_blocks[index], &box_mesh_)) {
                error_message_ = "unable to parse watercube box mesh from ase asset";
                return false;
            }
        }
    }

    if (water_mesh_.vertices.empty() || box_mesh_.vertices.empty()) {
        error_message_ = "missing Box01 or TriPatch01 mesh in ase asset: " + ase_asset_path("nosto3.ase");
        return false;
    }
    return true;
}

bool WatercubeScene::load_igu_mesh(const std::string& path,
                                   WatercubeIguMesh* mesh,
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

void WatercubeScene::build_flash_tables() {
    flash_init_random_ = JavaRandom(0x57415446ULL);
    for (std::size_t index = 0; index < flash_pattern_.size(); ++index) {
        const int red = static_cast<int>(flash_init_random_.next_float() * 68.0f);
        const int green = static_cast<int>(flash_init_random_.next_float() * 56.0f);
        const int blue = static_cast<int>(flash_init_random_.next_float() * 37.0f);
        flash_pattern_[index] = pack_original_rgb(red, green, blue);
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

void WatercubeScene::step_simulation() {
    ++simulation_tick_count_;
    ++ripple_phase_;
    inject_ring_stamp();
    advance_wave_buffers();
}

void WatercubeScene::inject_ring_stamp() {
    const int x = 106 + static_cast<int>(10.0 * -std::sin(static_cast<double>(ripple_phase_) / 6.24));
    const int y = 106 + static_cast<int>(15.0 * std::cos(static_cast<double>(2 * ripple_phase_) / 6.24));
    packed_add_blit(&wave_b_, ring_texture_, x, y);
}

void WatercubeScene::advance_wave_buffers() {
    const std::vector<std::uint32_t>& source = wave_toggle_ ? wave_b_.packed_pixels : wave_a_.packed_pixels;
    std::vector<std::uint32_t>& target = wave_toggle_ ? wave_a_.packed_pixels : wave_b_.packed_pixels;

    int n3 = kRippleSize * 2;
    const int n4 = kRippleSize + kRippleSize;
    const int n6 = n4 - 2;
    const int n7 = n4 + 2;
    const int n8 = n4 + n4;
    for (int row = 2; row < kRippleSize - 2; row += 2) {
        int n10 = n3 - n4 + 1;
        int n11 = n3 + 1;
        for (int column = 1; column < kRippleSize - 1; column += 2) {
            const std::uint32_t sum =
                source[static_cast<std::size_t>(n10)] +
                source[static_cast<std::size_t>(n10 + n6)] +
                source[static_cast<std::size_t>(n10 + n7)] +
                source[static_cast<std::size_t>(n10 + n8)];
            const std::uint32_t previous = target[static_cast<std::size_t>(n11)];
            const std::uint32_t value = ((sum >> 1U) + kPackedCarryMask) - previous;
            const std::uint32_t carry = value & kPackedCarryMask;
            const std::uint32_t result = value & (carry - (carry >> 8));
            target[static_cast<std::size_t>(n11 - kRippleSize)] = result;
            target[static_cast<std::size_t>(n11 - kRippleSize + 1)] = result;
            target[static_cast<std::size_t>(n11)] = result;
            target[static_cast<std::size_t>(n11 + 1)] = result;
            n10 += 2;
            n11 += 2;
        }
        n3 += 2 * kRippleSize;
    }

    packed_copy_blit(&water_texture_, ripple_texture_, 0, 0);
    if (wave_toggle_) {
        packed_add_blit(&water_texture_, wave_a_, 0, 0);
    } else {
        packed_add_blit(&water_texture_, wave_b_, 0, 0);
    }
    wave_toggle_ = !wave_toggle_;
}

std::string WatercubeScene::ase_asset_path(const std::string& file_name) const {
    return std::string("original/forward/asses/") + file_name;
}

std::string WatercubeScene::image_asset_path(const std::string& file_name) const {
    return std::string("original/forward/images/") + file_name;
}

std::string WatercubeScene::mesh_asset_path(const std::string& file_name) const {
    return std::string("original/forward/meshes/") + file_name;
}

}  // namespace forward_offline
