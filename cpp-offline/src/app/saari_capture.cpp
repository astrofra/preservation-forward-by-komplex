#include "app/saari_capture.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>
#include <vector>

#include "platform/file_utils.h"
#include "render/png_writer.h"
#include "scenes/saari_scene.h"

#ifndef FORWARD_SOURCE_REVISION
#define FORWARD_SOURCE_REVISION "unknown"
#endif

namespace forward_offline {
namespace {
const double kPi = 3.14159265358979323846;
const double kSea = -0.001;
const double kSeaMargin = 2.0;
const char* const kPathHeader = "px,py,pz,tx,ty,tz,hfov_degrees,group";

struct Observation {
    std::size_t point;
    double x, y;
};

struct View {
    SaariCaptureCamera camera;
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
    SaariCapturePoint sample;
    unsigned int count;
    std::uint64_t red, green, blue;
    Point() : count(0), red(0), green(0), blue(0) {}
};

double distance(const SaariVec3& a, const SaariVec3& b) {
    const double x = double(a.x) - b.x, y = double(a.y) - b.y, z = double(a.z) - b.z;
    return std::sqrt(x*x + y*y + z*z);
}

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
    const SaariCaptureCamera& c = view->camera;
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

View make_view(const ExportConfig& config, const SaariVec3& position,
               const SaariVec3& target, double fov, const std::string& group) {
    View view;
    view.fov = fov;
    view.group = group;
    view.validation = false;
    view.camera_id = 0;
    view.camera = make_saari_capture_camera(position, target, config.width, config.height,
                                             static_cast<float>(fov * kPi / 180.0));
    make_pose(&view);
    return view;
}

SaariVec3 sphere_position(const SaariVec3& center, double radius, double azimuth, double elevation) {
    const SaariVec3 p = {
        static_cast<float>(center.x + radius * std::cos(elevation) * std::cos(azimuth)),
        static_cast<float>(center.y + radius * std::cos(elevation) * std::sin(azimuth)),
        static_cast<float>(center.z + radius * std::sin(elevation))};
    return p;
}

std::vector<View> create_views(const ExportConfig& config, const SaariVec3& center,
                               const SaariVec3& focus, double* radius, double minimum_radius) {
    std::vector<View> views;
    if (!config.gsplat_camera_path.empty()) {
        std::ifstream input(config.gsplat_camera_path.c_str());
        std::string line;
        if (!input || !std::getline(input, line)) throw std::runtime_error("unable to read camera CSV");
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line != kPathHeader) throw std::runtime_error(std::string("expected CSV header: ") + kPathHeader);
        int line_number = 1;
        while (std::getline(input, line)) {
            ++line_number;
            if (line.find_first_not_of(" \t\r") == std::string::npos) continue;
            const bool columns_ok = std::count(line.begin(), line.end(), ',') == 7;
            std::replace(line.begin(), line.end(), ',', ' ');
            std::istringstream row(line);
            row.imbue(std::locale::classic());
            SaariVec3 p = {}, t = {};
            double fov = 0;
            std::string group;
            bool valid = columns_ok && bool(row >> p.x >> p.y >> p.z >> t.x >> t.y >> t.z >> fov >> group);
            row >> std::ws;
            valid = valid && row.eof();
            const double values[] = {p.x,p.y,p.z,t.x,t.y,t.z,fov};
            for (int i = 0; i < 7; ++i)
                valid = valid && std::isfinite(values[i]) && std::fabs(values[i]) < 100000.0;
            valid = valid && fov >= 10 && fov <= 120 && p.z >= kSea+kSeaMargin &&
                std::hypot(double(t.x)-p.x, double(t.y)-p.y) > 0.001 &&
                (group == "island" || group == "bridge" || group == "meditate" || group == "custom");
            const double actual_radius = distance(p, center);
            if (views.empty() && config.gsplat_radius == 0) *radius = actual_radius;
            valid = valid && *radius >= minimum_radius && *radius <= 2000.0 &&
                std::fabs(actual_radius-*radius) <= std::max(0.001, *radius*1e-5);
            if (!valid) {
                std::ostringstream error;
                error << "invalid camera CSV line " << line_number
                      << ": expected finite camera on the enclosing hemisphere, Z >= " << kSea+kSeaMargin
                      << ", FOV 10..120, nonvertical look direction and known group";
                throw std::runtime_error(error.str());
            }
            views.push_back(make_view(config, p, t, fov, group));
            if (views.size() > 5000) throw std::runtime_error("camera CSV exceeds 5000 views");
        }
        if (input.bad() || views.size() < 12) throw std::runtime_error("camera CSV requires 12..5000 readable views");
        return views;
    }

    const int focus_count = std::max(2, config.frame_count / 5);
    const int bridge_count = std::max(1, config.frame_count / 20);
    const int island_count = config.frame_count - focus_count - bridge_count;
    const SaariVec3 target = {center.x, center.y, static_cast<float>(focus.z * 0.35)};
    const double low = std::max(10.0*kPi/180.0, std::asin(kSeaMargin / *radius));
    const double high = 80.0*kPi/180.0;
    double end_azimuth = 0;
    for (int i = 0; i < island_count; ++i) {
        const double u = double(i) / (island_count - 1);
        const double elevation = std::asin(std::sin(low) + u * (std::sin(high)-std::sin(low)));
        end_azimuth = -kPi/2 + u * 8*kPi;
        views.push_back(make_view(config, sphere_position(center, *radius, end_azimuth, elevation),
                                  target, 1.4*180.0/kPi, "island"));
    }
    // A spherical transition connects the island pass to the summit pass.
    for (int i = 0; i < bridge_count; ++i) {
        const double u = double(i+1) / (bridge_count+1);
        const SaariVec3 t = {static_cast<float>(target.x + u*(focus.x-target.x)),
                            static_cast<float>(target.y + u*(focus.y-target.y)),
                            static_cast<float>(target.z + u*(focus.z-target.z))};
        views.push_back(make_view(config, sphere_position(center, *radius, end_azimuth+u*kPi/2,
                                   high+u*(25*kPi/180.0-high)), t, 1.4*180.0/kPi, "bridge"));
    }
    for (int i = 0; i < focus_count; ++i) {
        const double u = double(i) / (focus_count-1);
        views.push_back(make_view(config, sphere_position(center, *radius, end_azimuth+kPi/2+u*6*kPi,
                                   (25.0+50.0*u)*kPi/180.0), focus, 35.0, "meditate"));
    }
    return views;
}

void collect_observations(View* view, const RgbSurface& image, const std::vector<int>& owners,
                           std::vector<Point>* points) {
    for (std::size_t i = 0; i < points->size(); ++i) {
        Point& point = (*points)[i];
        const SaariVec3& p = point.sample.position;
        const double world[3] = {-p.x,p.y,p.z};
        double camera[3];
        for (int row = 0; row < 3; ++row) {
            camera[row] = view->translation[row];
            for (int c = 0; c < 3; ++c) camera[row] += view->rotation[row*3+c] * world[c];
        }
        if (camera[2] <= 0.1 || p.z <= 0.001f) continue;
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
        const SaariCaptureCamera& c = it->second->camera;
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

int run_capture(const ExportConfig& config) {
    // A fresh directory prevents stale images/poses from contaminating a dataset.
    struct stat info;
    if (stat(config.output_dir.c_str(), &info) == 0)
        throw std::runtime_error("capture output already exists; choose a new --output directory");
    SaariScene scene;
    scene.init();
    if (!scene.is_ready()) throw std::runtime_error(scene.error_message());
    std::vector<SaariCapturePoint> samples;
    SaariVec3 bounds_min, bounds_max, focus;
    scene.capture_geometry(config.gsplat_time, &samples, &bounds_min, &bounds_max, &focus);
    if (samples.empty()) throw std::runtime_error("Saari contains no capture geometry");
    const SaariVec3 center = {(bounds_min.x+bounds_max.x)*0.5f,
                              (bounds_min.y+bounds_max.y)*0.5f, static_cast<float>(kSea)};
    // Bounding-box corners conservatively enclose every geometry vertex, not just samples.
    double enclosing_radius = 0;
    for (int corner = 0; corner < 8; ++corner) {
        const SaariVec3 p = {corner&1 ? bounds_max.x : bounds_min.x,
                            corner&2 ? bounds_max.y : bounds_min.y,
                            corner&4 ? bounds_max.z : bounds_min.z};
        enclosing_radius = std::max(enclosing_radius, distance(center,p));
    }
    const double minimum_radius = enclosing_radius + 5.0;
    double radius = config.gsplat_radius > 0 ? config.gsplat_radius : enclosing_radius*1.15+5.0;
    if (radius < minimum_radius || radius > 2000)
        throw std::runtime_error("radius does not enclose the frozen scene with a 5-unit margin (use 0 for auto)");
    std::vector<View> views = create_views(config, center, focus, &radius, minimum_radius);
    std::vector<Point> points(samples.size());
    for (std::size_t i = 0; i < samples.size(); ++i) points[i].sample = samples[i];
    make_directory(join_path(config.output_dir, "images"));
    std::ofstream path = output_file(join_path(config.output_dir, "camera_path.csv"));
    std::ofstream manifest = output_file(join_path(config.output_dir, "manifest.csv"));
    path << kPathHeader << '\n';
    manifest << "image_id,file,split,group,scene_time_seconds,camera_id\n";
    std::map<float,int> camera_models;
    RgbSurface surface(config.width, config.height);
    std::vector<int> owners;
    std::size_t validation_count = 0;
    std::cout << "Saari gsplat: " << views.size() << " views, radius " << radius
              << ", frozen scene time " << config.gsplat_time << " s\n";
    for (std::size_t i = 0; i < views.size(); ++i) {
        View& view = views[i];
        view.validation = config.gsplat_validation_every > 0 &&
            (i+1) % static_cast<std::size_t>(config.gsplat_validation_every) == 0;
        const float focal = view.camera.focal_length;
        if (camera_models.find(focal) == camera_models.end()) {
            const int next_camera_id = static_cast<int>(camera_models.size()+1);
            camera_models.insert(std::make_pair(focal, next_camera_id));
        }
        view.camera_id = camera_models[focal];
        std::ostringstream filename;
        filename << "frame_" << std::setfill('0') << std::setw(6) << i << ".png";
        view.filename = filename.str();
        const std::string subdir = view.validation ? "validation/images" : "images";
        if (view.validation && validation_count++ == 0) make_directory(join_path(config.output_dir, subdir));
        scene.render_capture(surface, view.camera, config.gsplat_time, &owners);
        collect_observations(&view, surface, owners, &points);
        std::string error;
        if (!write_png24(join_path(join_path(config.output_dir,subdir),view.filename),surface,&error))
            throw std::runtime_error(error);
        const SaariVec3& p = view.camera.position;
        const SaariVec3& t = view.camera.target;
        path << p.x << ',' << p.y << ',' << p.z << ',' << t.x << ',' << t.y << ',' << t.z
             << ',' << view.fov << ',' << view.group << '\n';
        manifest << i+1 << ',' << subdir << '/' << view.filename << ','
                 << (view.validation ? "validation" : "train") << ',' << view.group << ','
                 << config.gsplat_time << ',' << view.camera_id << '\n';
        if ((i+1)%10 == 0 || i+1 == views.size())
            std::cout << "\rRendered " << i+1 << '/' << views.size() << std::flush;
    }
    finish(path); finish(manifest);
    const std::size_t count = write_model(join_path(config.output_dir,"sparse"),config,views,points,false);
    if (count == 0) throw std::runtime_error("no points seen by two training views; increase view count or revise camera path");
    if (validation_count != 0)
        write_model(join_path(config.output_dir,"validation/sparse"),config,views,points,true);
    std::size_t meditate_points = 0, klunssi_points = 0;
    for (std::size_t i = 0; i < points.size(); ++i) {
        if (points[i].count < 2) continue;
        if (points[i].sample.surface_id >= 20000) ++meditate_points;
        else if (points[i].sample.surface_id >= 10000) ++klunssi_points;
    }
    std::ofstream summary = output_file(join_path(config.output_dir,"capture.json"));
    summary << "{\n  \"status\": \"complete\",\n  \"sequence\": \"saari-gsplat\",\n"
            << "  \"source_revision\": \"" << FORWARD_SOURCE_REVISION << "\",\n"
            << "  \"width\": " << config.width << ", \"height\": " << config.height << ",\n"
            << "  \"scene_time_seconds\": " << config.gsplat_time << ",\n"
            << "  \"view_count\": " << views.size() << ", \"validation_views\": " << validation_count << ",\n"
            << "  \"validation_every\": " << config.gsplat_validation_every << ",\n"
            << "  \"hemisphere_center_native\": [" << center.x << ',' << center.y << ',' << center.z << "],\n"
            << "  \"hemisphere_radius\": " << radius << ", \"sea_z\": " << kSea
            << ", \"sea_margin\": " << kSeaMargin << ",\n"
            << "  \"focus_native\": [" << focus.x << ',' << focus.y << ',' << focus.z << "],\n"
            << "  \"points\": " << count << ", \"meditate_points\": " << meditate_points
            << ", \"klunssi_points\": " << klunssi_points << ",\n"
            << "  \"world_conversion\": \"COLMAP = diag(-1,1,1) * native; world Z up\",\n"
            << "  \"effects\": \"klunssi and reflection frozen; shocks disabled; original materials retained\"\n}\n";
    finish(summary);
    std::ofstream readme = output_file(join_path(config.output_dir,"IMPORT.txt"));
    readme << "Saari static capture (C++ offline exporter)\n\n"
           << "Postshot: import ONLY images/ together with the three files in sparse/.\n"
           << "Keep validation/ separate from training. Do not import the entire dataset root.\n"
           << "PNG images are native renders, not resized screenshots.\n"
           << "camera_path.csv is editable in native coordinates; replay with --gsplat-camera-path.\n"
           << "Use the same --gsplat-time and dimensions when replaying.\n"
           << "Points are geometric surface samples seen in at least two training views.\n"
           << "Visibility follows the final native painter/compositing order; no sky/reflection seeds.\n"
           << "COLMAP world is mirrored in X relative to the native world (see capture.json).\n"
           << "Projection/pose data are exact; affine textures, fog and environment mapping remain view-dependent.\n"
           << "A complete export has capture.json with status=complete.\n";
    finish(readme);
    std::cout << "\nWrote " << views.size()-validation_count << " training PNGs, " << validation_count
              << " validation PNGs and " << count << " sparse points (meditate=" << meditate_points
              << ", klunssi=" << klunssi_points << ") to " << config.output_dir << '\n';
    return 0;
}
}  // namespace

int export_saari_capture(const ExportConfig& config) {
    try { return run_capture(config); }
    catch (const std::exception& error) {
        std::cerr << "Saari capture: " << error.what() << '\n';
        return 1;
    }
}
}  // namespace forward_offline
