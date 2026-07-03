#include "scenes/scene3d_shared.h"

#include <cmath>
#include <cstdio>
#include <sstream>

namespace forward_offline {

namespace {

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

std::size_t find_matching_brace(const std::string& content,
                                std::size_t open_brace_position) {
    int depth = 0;
    for (std::size_t index = open_brace_position; index < content.size(); ++index) {
        if (content[index] == '{') {
            ++depth;
        } else if (content[index] == '}') {
            --depth;
            if (depth == 0) {
                return index;
            }
        }
    }

    return std::string::npos;
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

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

Scene3dVec3 catmull_rom_vec3(const Scene3dVec3& p0,
                             const Scene3dVec3& p1,
                             const Scene3dVec3& p2,
                             const Scene3dVec3& p3,
                             float t) {
    const float t2 = t * t;
    const float t3 = t2 * t;
    const Scene3dVec3 m1 = scale(subtract(p2, p0), 0.5f);
    const Scene3dVec3 m2 = scale(subtract(p3, p1), 0.5f);
    const float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
    const float h10 = t3 - 2.0f * t2 + t;
    const float h01 = -2.0f * t3 + 3.0f * t2;
    const float h11 = t3 - t2;
    return add(add(scale(p1, h00), scale(m1, h10)),
               add(scale(p2, h01), scale(m2, h11)));
}

float wrap_tick(float tick, float duration) {
    if (duration <= 0.0f) {
        return 0.0f;
    }

    tick = std::fmod(tick, duration);
    if (tick < 0.0f) {
        tick += duration;
    }
    return tick;
}

Scene3dTrackSample get_loop_track_sample(const std::vector<Scene3dTrackSample>& track, int index) {
    if (index < 0) {
        return track[track.size() > 1U ? track.size() - 2U : 0U];
    }
    if (index >= static_cast<int>(track.size())) {
        return track[track.size() > 1U ? 1U : 0U];
    }
    return track[static_cast<std::size_t>(index)];
}

Scene3dQuaternion make_quaternion(float x, float y, float z, float w) {
    Scene3dQuaternion value;
    value.x = x;
    value.y = y;
    value.z = z;
    value.w = w;
    return value;
}

Scene3dQuaternion normalize_quaternion(const Scene3dQuaternion& value) {
    const float length =
        std::sqrt(value.x * value.x + value.y * value.y + value.z * value.z + value.w * value.w);
    if (length <= 1.0e-12f) {
        return make_quaternion(0.0f, 0.0f, 0.0f, 1.0f);
    }

    const float inverse_length = 1.0f / length;
    return make_quaternion(value.x * inverse_length,
                           value.y * inverse_length,
                           value.z * inverse_length,
                           value.w * inverse_length);
}

Scene3dQuaternion axis_angle_to_quaternion(const Scene3dVec3& axis, float angle) {
    const Scene3dVec3 normalized_axis = normalize(axis);
    const float half_angle = angle * 0.5f;
    const float sine = std::sin(half_angle);
    return normalize_quaternion(make_quaternion(normalized_axis.x * sine,
                                                normalized_axis.y * sine,
                                                normalized_axis.z * sine,
                                                std::cos(half_angle)));
}

Scene3dQuaternion multiply_quaternions(const Scene3dQuaternion& left,
                                       const Scene3dQuaternion& right) {
    return make_quaternion(left.w * right.x + left.x * right.w + left.y * right.z - left.z * right.y,
                           left.w * right.y + left.y * right.w + left.z * right.x - left.x * right.z,
                           left.w * right.z + left.z * right.w + left.x * right.y - left.y * right.x,
                           left.w * right.w - left.x * right.x - left.y * right.y - left.z * right.z);
}

float quaternion_dot(const Scene3dQuaternion& a, const Scene3dQuaternion& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

Scene3dQuaternion slerp_quaternion(const Scene3dQuaternion& a,
                                   const Scene3dQuaternion& b,
                                   float t) {
    Scene3dQuaternion end = b;
    float cosine = quaternion_dot(a, b);
    if (cosine < 0.0f) {
        cosine = -cosine;
        end = make_quaternion(-b.x, -b.y, -b.z, -b.w);
    }

    if (1.0f - cosine <= 1.0e-6f) {
        return normalize_quaternion(make_quaternion(lerp(a.x, end.x, t),
                                                    lerp(a.y, end.y, t),
                                                    lerp(a.z, end.z, t),
                                                    lerp(a.w, end.w, t)));
    }

    const float angle = std::acos(cosine);
    const float sine = std::sin(angle);
    const float weight_a = std::sin((1.0f - t) * angle) / sine;
    const float weight_b = std::sin(t * angle) / sine;
    return normalize_quaternion(make_quaternion(weight_a * a.x + weight_b * end.x,
                                                weight_a * a.y + weight_b * end.y,
                                                weight_a * a.z + weight_b * end.z,
                                                weight_a * a.w + weight_b * end.w));
}

void build_mesh_normals(Scene3dStaticMesh* mesh) {
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

std::vector<std::string> extract_braced_blocks(const std::string& content,
                                               const std::string& header) {
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

        blocks.push_back(content.substr(header_position, end_position - header_position + 1U));
        search_position = end_position + 1U;
    }

    return blocks;
}

bool parse_mesh_vertices_and_faces(const std::string& block,
                                   Scene3dStaticMesh* mesh) {
    if (mesh == NULL) {
        return false;
    }

    mesh->vertices.clear();
    mesh->normals.clear();
    mesh->triangles.clear();
    mesh->pivot = make_vec3(0.0f, 0.0f, 0.0f);

    std::vector<Scene3dVec3> world_vertices;
    std::istringstream stream(block);
    std::string line;

    while (std::getline(stream, line)) {
        const std::string trimmed = trim(line);
        if (starts_with(trimmed, "*MESH_VERTEX")) {
            int index = 0;
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            if (sscanf_s(trimmed.c_str(), "*MESH_VERTEX %d %f %f %f",
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
            if (sscanf_s(trimmed.c_str(), "*MESH_FACE %d: A: %d B: %d C: %d",
                         &face_index, &a, &b, &c) == 4) {
                (void)face_index;
                Scene3dTriangle triangle;
                triangle.a = a;
                triangle.b = b;
                triangle.c = c;
                mesh->triangles.push_back(triangle);
            }
        } else if (starts_with(trimmed, "*TM_POS")) {
            sscanf_s(trimmed.c_str(), "*TM_POS %f %f %f",
                     &mesh->pivot.x, &mesh->pivot.y, &mesh->pivot.z);
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
                          std::vector<Scene3dTrackSample>* track) {
    if (track == NULL) {
        return;
    }

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
            Scene3dTrackSample sample;
            if (sscanf_s(trimmed.c_str(), "*CONTROL_POS_SAMPLE %d %f %f %f",
                         &sample.tick,
                         &sample.value.x,
                         &sample.value.y,
                         &sample.value.z) == 4) {
                track->push_back(sample);
            }
        }
    }
}

void parse_rotation_track(const std::string& block,
                          const std::string& node_name,
                          std::vector<Scene3dRotationSample>* track) {
    if (track == NULL) {
        return;
    }

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
            Scene3dRotationSample sample;
            if (sscanf_s(trimmed.c_str(), "*CONTROL_ROT_SAMPLE %d %f %f %f %f",
                         &sample.tick,
                         &sample.axis.x,
                         &sample.axis.y,
                         &sample.axis.z,
                         &sample.angle) == 5) {
                track->push_back(sample);
            }
        }
    }
}

void build_orientation_track(const std::vector<Scene3dRotationSample>& rotation_track,
                             std::vector<Scene3dOrientationSample>* orientation_track) {
    if (orientation_track == NULL) {
        return;
    }

    orientation_track->clear();
    if (rotation_track.empty()) {
        return;
    }

    Scene3dQuaternion previous = make_quaternion(0.0f, 0.0f, 0.0f, 1.0f);
    for (std::size_t index = 0; index < rotation_track.size(); ++index) {
        const Scene3dRotationSample& sample = rotation_track[index];
        const Scene3dQuaternion delta = axis_angle_to_quaternion(sample.axis, sample.angle);

        Scene3dOrientationSample orientation_sample;
        orientation_sample.tick = sample.tick;
        orientation_sample.value =
            index == 0U ? delta : normalize_quaternion(multiply_quaternions(delta, previous));
        orientation_track->push_back(orientation_sample);
        previous = orientation_sample.value;
    }
}

Scene3dVec3 sample_track(const std::vector<Scene3dTrackSample>& track,
                         float tick) {
    if (track.empty()) {
        return make_vec3(0.0f, 0.0f, 0.0f);
    }
    if (track.size() == 1U) {
        return track.front().value;
    }

    tick = wrap_tick(tick, static_cast<float>(track.back().tick));

    for (std::size_t index = 1; index < track.size(); ++index) {
        if (tick <= static_cast<float>(track[index].tick)) {
            const std::size_t previous_index = index - 1U;
            const Scene3dTrackSample previous = get_loop_track_sample(track, static_cast<int>(previous_index));
            const Scene3dTrackSample next = get_loop_track_sample(track, static_cast<int>(index));
            const Scene3dTrackSample before_previous =
                get_loop_track_sample(track, static_cast<int>(previous_index) - 1);
            const Scene3dTrackSample after_next =
                get_loop_track_sample(track, static_cast<int>(index) + 1);
            const float range = static_cast<float>(next.tick - previous.tick);
            const float t = range <= 0.0f ? 0.0f : (tick - static_cast<float>(previous.tick)) / range;
            return catmull_rom_vec3(before_previous.value,
                                    previous.value,
                                    next.value,
                                    after_next.value,
                                    t);
        }
    }

    return track.front().value;
}

Scene3dQuaternion sample_orientation_track(const std::vector<Scene3dOrientationSample>& track,
                                           float tick) {
    const Scene3dQuaternion identity = make_quaternion(0.0f, 0.0f, 0.0f, 1.0f);
    if (track.empty()) {
        return identity;
    }
    if (track.size() == 1U) {
        return track.front().value;
    }

    tick = wrap_tick(tick, static_cast<float>(track.back().tick));

    for (std::size_t index = 1; index < track.size(); ++index) {
        if (tick <= static_cast<float>(track[index].tick)) {
            const Scene3dOrientationSample& previous = track[index - 1U];
            const Scene3dOrientationSample& next = track[index];
            const float range = static_cast<float>(next.tick - previous.tick);
            const float t = range <= 0.0f ? 0.0f : (tick - static_cast<float>(previous.tick)) / range;
            return slerp_quaternion(previous.value, next.value, t);
        }
    }

    return track.front().value;
}

}  // namespace forward_offline
