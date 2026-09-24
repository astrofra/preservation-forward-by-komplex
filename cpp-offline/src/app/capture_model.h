#ifndef FORWARD_OFFLINE_APP_CAPTURE_MODEL_H
#define FORWARD_OFFLINE_APP_CAPTURE_MODEL_H

#include <array>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include "app/export_config.h"
#include "render/rgb_surface.h"
#include "scenes/capture_types.h"

namespace forward_offline {
namespace capture {

struct Observation {
    std::size_t point;
    double x, y;
};

struct View {
    CaptureCamera camera;
    double fov;
    std::string group, filename;
    bool validation;
    int camera_id;
    std::array<double, 4> q;
    std::array<double, 9> rotation;
    std::array<double, 3> translation;
    std::vector<Observation> observations;
};

struct Point {
    CapturePoint sample;
    unsigned int count;
    std::uint64_t red, green, blue;
    Point() : count(0), red(0), green(0), blue(0) {}
};

std::ofstream output_file(const std::string& path);
void finish(std::ofstream& stream);
void make_directory(const std::string& path);
void make_pose(View* view);
void collect_observations(View* view, const RgbSurface& image, const std::vector<int>& owners,
                          std::vector<Point>* points, float minimum_z, double maximum_depth);
std::size_t write_model(const std::string& directory, const ExportConfig& config,
                        const std::vector<View>& views, const std::vector<Point>& points,
                        bool validation);

}  // namespace capture
}  // namespace forward_offline
#endif
