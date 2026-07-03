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

Scene3dVec3 sample_track(const std::vector<Scene3dTrackSample>& track,
                         float tick) {
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
            const Scene3dTrackSample& previous = track[index - 1U];
            const Scene3dTrackSample& next = track[index];
            const float range = static_cast<float>(next.tick - previous.tick);
            const float t = range <= 0.0f ? 0.0f : (tick - static_cast<float>(previous.tick)) / range;
            return make_vec3(lerp(previous.value.x, next.value.x, t),
                             lerp(previous.value.y, next.value.y, t),
                             lerp(previous.value.z, next.value.z, t));
        }
    }

    return track.back().value;
}

Scene3dRotationSample sample_rotation_track(const std::vector<Scene3dRotationSample>& track,
                                            float tick) {
    Scene3dRotationSample identity;
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
            const Scene3dRotationSample& previous = track[index - 1U];
            const Scene3dRotationSample& next = track[index];
            const float range = static_cast<float>(next.tick - previous.tick);
            const float t = range <= 0.0f ? 0.0f : (tick - static_cast<float>(previous.tick)) / range;
            Scene3dRotationSample sample;
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

}  // namespace forward_offline
