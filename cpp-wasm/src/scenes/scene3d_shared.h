#ifndef FORWARD_OFFLINE_SCENES_SCENE3D_SHARED_H
#define FORWARD_OFFLINE_SCENES_SCENE3D_SHARED_H

#include <string>
#include <vector>

namespace forward_offline {

struct Scene3dVec3 {
    float x;
    float y;
    float z;
};

struct Scene3dTriangle {
    int a;
    int b;
    int c;
};

struct Scene3dTrackSample {
    int tick;
    Scene3dVec3 value;
};

struct Scene3dRotationSample {
    int tick;
    Scene3dVec3 axis;
    float angle;
};

struct Scene3dQuaternion {
    float x;
    float y;
    float z;
    float w;
};

struct Scene3dOrientationSample {
    int tick;
    Scene3dQuaternion value;
    Scene3dQuaternion tangent;
};

struct Scene3dStaticMesh {
    std::vector<Scene3dVec3> vertices;
    std::vector<Scene3dVec3> normals;
    std::vector<Scene3dTriangle> triangles;
    Scene3dVec3 pivot;
};

std::vector<std::string> extract_braced_blocks(const std::string& content,
                                               const std::string& header);

bool parse_mesh_vertices_and_faces(const std::string& block,
                                   Scene3dStaticMesh* mesh);

void parse_position_track(const std::string& block,
                          const std::string& node_name,
                          std::vector<Scene3dTrackSample>* track);

void parse_rotation_track(const std::string& block,
                          const std::string& node_name,
                          std::vector<Scene3dRotationSample>* track);

void build_orientation_track(const std::vector<Scene3dRotationSample>& rotation_track,
                             std::vector<Scene3dOrientationSample>* orientation_track);

Scene3dVec3 sample_track(const std::vector<Scene3dTrackSample>& track,
                         float tick);

Scene3dQuaternion sample_orientation_track(const std::vector<Scene3dOrientationSample>& track,
                                           float tick);

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_SCENES_SCENE3D_SHARED_H
