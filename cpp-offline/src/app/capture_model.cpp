#include "app/capture_model.h"

#include <cmath>
#include <iomanip>
#include <locale>
#include <map>
#include <stdexcept>
#include "platform/file_utils.h"

namespace forward_offline {
namespace capture {
std::ofstream output_file(const std::string& path) {
    std::ofstream stream(path.c_str());
    stream.imbue(std::locale::classic());
    stream << std::setprecision(17);
    if (!stream) throw std::runtime_error("unable to write " + path);
    return stream;
}

void finish(std::ofstream& stream) {
    stream.close();
    if (!stream) throw std::runtime_error("unable to finish capture file (check free disk space)");
}

void make_directory(const std::string& path) {
    std::string error;
    if (!create_directories(path, &error)) throw std::runtime_error(error);
}

void make_pose(View* view) {
    const CaptureCamera& c = view->camera;
    // Native image axes are [right, -up, forward], an improper rotation.
    // Reflect the entire exported world in X: M = A * diag(-1, 1, 1).
    const double m[9] = {-c.right.x, c.right.y, c.right.z,
                         c.up.x, -c.up.y, -c.up.z,
                        -c.forward.x, c.forward.y, c.forward.z};
    double w, x, y, z;
    const double trace = m[0] + m[4] + m[8];
    if (trace > 0) {
        const double s = std::sqrt(trace + 1.0) * 2.0;
        w = s / 4.0; x = (m[7]-m[5])/s; y = (m[2]-m[6])/s; z = (m[3]-m[1])/s;
    } else if (m[0] > m[4] && m[0] > m[8]) {
        const double s = std::sqrt(1.0+m[0]-m[4]-m[8]) * 2.0;
        w = (m[7]-m[5])/s; x = s/4.0; y = (m[1]+m[3])/s; z = (m[2]+m[6])/s;
    } else if (m[4] > m[8]) {
        const double s = std::sqrt(1.0+m[4]-m[0]-m[8]) * 2.0;
        w = (m[2]-m[6])/s; x = (m[1]+m[3])/s; y = s/4.0; z = (m[5]+m[7])/s;
    } else {
        const double s = std::sqrt(1.0+m[8]-m[0]-m[4]) * 2.0;
        w = (m[3]-m[1])/s; x = (m[2]+m[6])/s; y = (m[5]+m[7])/s; z = s/4.0;
    }
    double length = std::sqrt(w*w+x*x+y*y+z*z);
    if (w < 0) length = -length;
    w /= length; x /= length; y /= length; z /= length;
    view->q = {{w,x,y,z}};
    view->rotation = {{1-2*(y*y+z*z), 2*(x*y-z*w), 2*(x*z+y*w),
                       2*(x*y+z*w), 1-2*(x*x+z*z), 2*(y*z-x*w),
                       2*(x*z-y*w), 2*(y*z+x*w), 1-2*(x*x+y*y)}};
    const double center[3] = {-c.position.x, c.position.y, c.position.z};
    for (int row = 0; row < 3; ++row) {
        view->translation[row] = 0;
        for (int column = 0; column < 3; ++column)
            view->translation[row] -= view->rotation[row*3+column] * center[column];
    }
}

void collect_observations(View* view, const RgbSurface& image, const std::vector<int>& owners,
                           std::vector<Point>* points, float minimum_z, double maximum_depth) {
    for (std::size_t i = 0; i < points->size(); ++i) {
        Point& point = (*points)[i];
        const Scene3dVec3& p = point.sample.position;
        const double world[3] = {-p.x,p.y,p.z};
        double camera[3];
        for (int row = 0; row < 3; ++row) {
            camera[row] = view->translation[row];
            for (int c = 0; c < 3; ++c) camera[row] += view->rotation[row*3+c] * world[c];
        }
        if (camera[2] <= 0.1 || camera[2] >= maximum_depth || p.z <= minimum_z) continue;
        const double x = view->camera.half_width + view->camera.focal_length*camera[0]/camera[2];
        const double y = view->camera.half_height + view->camera.focal_length*camera[1]/camera[2];
        if (x < 0 || y < 0 || x >= image.width() || y >= image.height()) continue;
        const std::size_t pixel = static_cast<std::size_t>(y)*image.width() + static_cast<std::size_t>(x);
        if (owners[pixel] != point.sample.surface_id) continue;
        const Observation observation = {i,x,y};
        view->observations.push_back(observation);
        if (!view->validation) {
            const std::uint32_t color = image.pixels()[pixel];
            ++point.count;
            point.red += (color >> 16) & 255;
            point.green += (color >> 8) & 255;
            point.blue += color & 255;
        }
    }
}

std::size_t write_model(const std::string& directory, const ExportConfig& config,
                         const std::vector<View>& views, const std::vector<Point>& points,
                         bool validation) {
    make_directory(directory);
    std::ofstream cameras = output_file(join_path(directory, "cameras.txt"));
    std::ofstream images = output_file(join_path(directory, "images.txt"));
    std::ofstream cloud = output_file(join_path(directory, "points3D.txt"));
    cameras << "# CAMERA_ID MODEL WIDTH HEIGHT fx fy cx cy\n";
    images << "# IMAGE_ID QW QX QY QZ TX TY TZ CAMERA_ID NAME\n# Next line: X Y POINT3D_ID ...\n";
    cloud << "# POINT3D_ID X Y Z R G B ERROR TRACK[] (IMAGE_ID POINT2D_IDX)\n";
    std::map<int, const View*> models;
    std::vector<std::vector<std::pair<int, int> > > tracks(points.size());
    for (std::size_t i = 0; i < views.size(); ++i) {
        const View& view = views[i];
        if (view.validation != validation) continue;
        models[view.camera_id] = &view;
        images << i+1;
        for (int j = 0; j < 4; ++j) images << ' ' << view.q[j];
        for (int j = 0; j < 3; ++j) images << ' ' << view.translation[j];
        images << ' ' << view.camera_id << ' ' << view.filename << '\n';
        int observation_index = 0;
        for (std::size_t j = 0; j < view.observations.size(); ++j) {
            const Observation& observation = view.observations[j];
            if (points[observation.point].count < 2) continue;
            if (observation_index != 0) images << ' ';
            images << observation.x << ' ' << observation.y << ' ' << observation.point+1;
            tracks[observation.point].push_back(std::make_pair(static_cast<int>(i+1), observation_index++));
        }
        images << '\n';
    }
    for (std::map<int, const View*>::const_iterator it = models.begin(); it != models.end(); ++it) {
        const CaptureCamera& c = it->second->camera;
        cameras << it->first << " PINHOLE " << config.width << ' ' << config.height << ' '
                << c.focal_length << ' ' << c.focal_length << ' ' << c.half_width << ' ' << c.half_height << '\n';
    }
    std::size_t count = 0;
    for (std::size_t i = 0; i < points.size(); ++i) {
        if (tracks[i].empty()) continue;
        const Point& p = points[i];
        cloud << i+1 << ' ' << -p.sample.position.x << ' ' << p.sample.position.y << ' ' << p.sample.position.z
              << ' ' << p.red/p.count << ' ' << p.green/p.count << ' ' << p.blue/p.count << " 0";
        for (std::size_t j = 0; j < tracks[i].size(); ++j)
            cloud << ' ' << tracks[i][j].first << ' ' << tracks[i][j].second;
        cloud << '\n';
        ++count;
    }
    finish(cameras); finish(images); finish(cloud);
    return count;
}

}  // namespace capture
}  // namespace forward_offline
