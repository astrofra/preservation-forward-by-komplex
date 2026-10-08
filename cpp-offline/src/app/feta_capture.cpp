#include "app/feta_capture.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <locale>
#include <map>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>
#include "app/capture_model.h"
#include "platform/file_utils.h"
#include "render/png_writer.h"
#include "scenes/feta_scene.h"

#ifndef FORWARD_SOURCE_REVISION
#define FORWARD_SOURCE_REVISION "unknown"
#endif

namespace forward_offline {
namespace {
using namespace capture;
const double kPi = 3.14159265358979323846;
const char* const kPathHeader = "px,py,pz,tx,ty,tz,hfov_degrees,group";

double radius_of(const Scene3dVec3& p) {
    return std::sqrt(double(p.x)*p.x + double(p.y)*p.y + double(p.z)*p.z);
}

double distance_to(const Scene3dVec3& p, const Scene3dVec3& center) {
    const double x = double(p.x)-center.x, y = double(p.y)-center.y, z = double(p.z)-center.z;
    return std::sqrt(x*x + y*y + z*z);
}

View make_view(const ExportConfig& config, const Scene3dVec3& position,
               const Scene3dVec3& target, double fov, const std::string& group) {
    View view;
    view.fov = fov;
    view.group = group;
    view.validation = false;
    view.camera_id = 0;
    view.camera = make_feta_capture_camera(position, target, config.width, config.height,
                                           static_cast<float>(fov*kPi/180));
    make_pose(&view);
    return view;
}

std::array<double, 3> octahedral_direction(int index, int side) {
    // Cell centers on [-1,1]^2, decoded as in the supplied GLSL octDecode.
    // The lower hemisphere folds XY simultaneously, retaining the original Z.
    const double u = 2.0 * ((index % side) + 0.5) / side - 1.0;
    const double v = 2.0 * ((index / side) + 0.5) / side - 1.0;
    const double z = 1.0 - std::fabs(u) - std::fabs(v);
    const double x = z < 0 ? (1.0-std::fabs(v)) * (u < 0 ? -1.0 : 1.0) : u;
    const double y = z < 0 ? (1.0-std::fabs(u)) * (v < 0 ? -1.0 : 1.0) : v;
    const double length = std::sqrt(x*x + y*y + z*z);
    return {{x/length, y/length, z/length}};
}

std::vector<View> create_views(const ExportConfig& config, const FetaCaptureBounds& bounds) {
    std::vector<View> views;
    if (!config.gsplat_camera_path.empty()) {
        std::ifstream input(config.gsplat_camera_path.c_str());
        std::string line;
        if (!input || !std::getline(input, line)) throw std::runtime_error("unable to read camera CSV");
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line != kPathHeader) throw std::runtime_error(std::string("expected CSV header: ") + kPathHeader);
        while (std::getline(input, line)) {
            if (line.find_first_not_of(" \t\r") == std::string::npos) continue;
            const bool columns_ok = std::count(line.begin(), line.end(), ',') == 7;
            std::replace(line.begin(), line.end(), ',', ' ');
            std::istringstream row(line);
            row.imbue(std::locale::classic());
            Scene3dVec3 p = {}, t = {};
            double fov = 0;
            std::string group;
            bool valid = columns_ok && bool(row >> p.x >> p.y >> p.z >> t.x >> t.y >> t.z >> fov >> group);
            row >> std::ws;
            valid = valid && row.eof();
            const double values[] = {p.x,p.y,p.z,t.x,t.y,t.z,fov};
            for (int i = 0; i < 7; ++i) valid = valid && std::isfinite(values[i]) && std::fabs(values[i]) < 100000;
            const Scene3dVec3 direction = {t.x-p.x, t.y-p.y, t.z-p.z};
            valid = valid && radius_of(direction) > 0.001 && fov >= 10 && fov <= 120 &&
                (group == "progressive" || group == "sphere" || group == "custom" ||
                 group == "octahedral" || group == "octahedral_offset" || group == "octahedral_target");
            const double actual_radius = distance_to(p, bounds.center);
            // Replay includes variable radii, reordered paths and legacy spheres.
            // Keep cameras outside the fetus, but allow entry into the particle cloud.
            valid = valid && actual_radius >= bounds.fetus_radius+0.5-0.0001 && actual_radius <= 2000.001;
            if (!valid) throw std::runtime_error("invalid Feta CSV: expected finite camera outside fetus bounds with 0.5-unit margin, valid target, FOV 10..120 and group progressive/sphere/custom/octahedral/octahedral_offset/octahedral_target");
            views.push_back(make_view(config, p, t, fov, group));
            if (views.size() > 5000) throw std::runtime_error("camera CSV exceeds 5000 views");
        }
        if (input.bad() || views.size() < 12) throw std::runtime_error("camera CSV requires 12..5000 readable views");
        return views;
    }
    const double half_hfov = config.gsplat_fov*kPi/360;
    const double half_vfov = std::atan(std::tan(half_hfov)*config.height/config.width);
    const double framing_factor = 1.05/std::sin(std::min(half_hfov, half_vfov));
    const double start_radius = config.gsplat_radius > 0 ? config.gsplat_radius :
        std::max(bounds.enclosing_radius+0.5, bounds.enclosing_radius*framing_factor);
    const double end_radius = config.gsplat_end_radius > 0 ? config.gsplat_end_radius :
        start_radius;
    if (start_radius < bounds.enclosing_radius+0.5 || start_radius > 2000 ||
        end_radius < bounds.fetus_radius+0.5 || end_radius > start_radius)
        throw std::runtime_error("Feta start radius must enclose all geometry; final radius must enclose fetus; keep a 0.5-unit margin and final <= start <= 2000 (0 = auto)");
    if (config.gsplat_sampling == "octahedral") {
        const int side = static_cast<int>(std::sqrt(double(config.frame_count)) + 0.5);
        if (side*side != config.frame_count)
            throw std::runtime_error("octahedral sampling requires a square --frames count (e.g. 256, 324, 400); default is 324");
        if (end_radius != start_radius)
            throw std::runtime_error("octahedral sampling keeps a constant radius; omit --gsplat-end-radius or match --gsplat-radius");
        for (int i = 0; i < config.frame_count; ++i) {
            std::array<double, 3> direction = octahedral_direction(i, side);
            if (config.gsplat_lateral_offset > 0) {
                // Shift along the unperturbed camera's right vector. Alternate
                // signs by grid cell, then normalize to preserve the exact radius.
                const double horizontal = std::hypot(direction[0], direction[1]);
                const double right_x = horizontal > 1e-12 ? direction[1]/horizontal : 1.0;
                const double right_y = horizontal > 1e-12 ? -direction[0]/horizontal : 0.0;
                const double shift = config.gsplat_lateral_offset / start_radius *
                    (((i % side + i / side) % 2) == 0 ? 1.0 : -1.0);
                direction[0] += shift*right_x;
                direction[1] += shift*right_y;
                const double length = std::sqrt(1.0+shift*shift);
                for (int axis = 0; axis < 3; ++axis) direction[axis] /= length;
            }
            const Scene3dVec3 position = {
                static_cast<float>(bounds.center.x + start_radius*direction[0]),
                static_cast<float>(bounds.center.y + start_radius*direction[1]),
                static_cast<float>(bounds.center.z + start_radius*direction[2])};
            Scene3dVec3 target = bounds.center;
            if (config.gsplat_target_offset > 0) {
                // Distribute look targets on a disk facing the unperturbed
                // camera, centered on the mesh. Only orientation changes.
                const double horizontal = std::hypot(direction[0], direction[1]);
                const double rx = horizontal > 1e-12 ? direction[1]/horizontal : 1.0;
                const double ry = horizontal > 1e-12 ? -direction[0]/horizontal : 0.0;
                const double up[3] = {direction[2]*ry, -direction[2]*rx,
                                      -direction[0]*ry + direction[1]*rx};
                const double angle = i*kPi*(3.0-std::sqrt(5.0));
                const double radius = config.gsplat_target_offset *
                    std::sqrt((i+0.5)/config.frame_count);
                const double x = radius*std::cos(angle), y = radius*std::sin(angle);
                target.x += static_cast<float>(x*rx + y*up[0]);
                target.y += static_cast<float>(x*ry + y*up[1]);
                target.z += static_cast<float>(y*up[2]);
            }
            views.push_back(make_view(config, position, target, config.gsplat_fov,
                config.gsplat_target_offset > 0 ? "octahedral_target" :
                (config.gsplat_lateral_offset > 0 ? "octahedral_offset" : "octahedral")));
        }
        return views;
    }
    if (config.gsplat_lateral_offset != 0 || config.gsplat_target_offset != 0)
        throw std::runtime_error("--gsplat-lateral-offset/--gsplat-target-offset require --gsplat-sampling octahedral (or CSV replay)");
    // Default to a sphere. An explicit smaller final radius opts into approach.
    // Each third revisits both hemispheres over three complete elevation cycles.
    // Nonintegral turns per elevation cycle avoid repeating the same directions.
    for (int i = 0; i < config.frame_count; ++i) {
        const double u = double(i)/(config.frame_count-1);
        const double approach = u*u*(3-2*u);
        const double radius = start_radius + (end_radius-start_radius)*approach;
        const double z = 0.995*std::cos(6*kPi*u);
        const double xy = std::sqrt(std::max(0.0, 1-z*z));
        const double azimuth = -kPi/2 + 21*kPi*u;
        const Scene3dVec3 position = {static_cast<float>(bounds.center.x + radius*xy*std::cos(azimuth)),
                                      static_cast<float>(bounds.center.y + radius*xy*std::sin(azimuth)),
                                      static_cast<float>(bounds.center.z + radius*z)};
        views.push_back(make_view(config, position, bounds.center, config.gsplat_fov,
                                  end_radius == start_radius ? "sphere" : "progressive"));
    }
    return views;
}

int run_capture(const ExportConfig& config) {
    struct stat info;
    if (stat(config.output_dir.c_str(), &info) == 0)
        throw std::runtime_error("capture output already exists; choose a new --output directory");
    FetaScene scene;
    scene.init();
    if (!scene.is_ready()) throw std::runtime_error(scene.error_message());
    std::vector<CapturePoint> samples;
    // Camera planning keeps the native enclosure, so expanding the cloud does
    // not pull back the camera and cancel the requested increase in coverage.
    const FetaCaptureBounds framing_bounds = scene.capture_geometry(config.gsplat_time, &samples);
    const FetaCaptureBounds bounds = scene.capture_geometry(config.gsplat_time, &samples,
        config.gsplat_particle_size_scale, config.gsplat_particle_cloud_scale);
    if (samples.empty()) throw std::runtime_error("Feta contains no capture geometry");
    std::vector<View> views = create_views(config, framing_bounds);
    const double radius_start = distance_to(views.front().camera.position, bounds.center);
    const double radius_end = distance_to(views.back().camera.position, bounds.center);
    double radius_min = radius_start, radius_max = radius_start;
    for (std::size_t i = 0; i < views.size(); ++i) {
        const double radius = distance_to(views[i].camera.position, bounds.center);
        radius_min = std::min(radius_min, radius);
        radius_max = std::max(radius_max, radius);
    }
    std::vector<Point> points(samples.size());
    for (std::size_t i = 0; i < samples.size(); ++i) points[i].sample = samples[i];
    make_directory(join_path(config.output_dir, "images"));
    std::ofstream path = output_file(join_path(config.output_dir, "camera_path.csv"));
    std::ofstream manifest = output_file(join_path(config.output_dir, "manifest.csv"));
    std::ofstream particles = output_file(join_path(config.output_dir, "particles.csv"));
    path << kPathHeader << '\n';
    manifest << "image_id,file,split,group,scene_time_seconds,camera_id\n";
    particles << "particle_id,point3D_id,x,y,z,world_size\n";
    std::size_t particle_count = 0;
    for (std::size_t i = 0; i < samples.size(); ++i) {
        const CapturePoint& p = samples[i];
        if (p.surface_id < 1000000) continue;
        particles << p.surface_id-1000000 << ',' << i+1 << ',' << p.position.x << ','
                  << p.position.y << ',' << p.position.z << ',' << bounds.particle_world_size << '\n';
        ++particle_count;
    }
    finish(particles);
    std::map<float,int> camera_models;
    RgbSurface surface(config.width, config.height);
    std::vector<int> owners;
    std::size_t validation_count = 0;
    std::cout << "Feta gsplat: " << views.size() << " views, radius " << radius_start << " -> " << radius_end
              << ", frozen scene time " << config.gsplat_time << " s\n";
    for (std::size_t i = 0; i < views.size(); ++i) {
        View& view = views[i];
        view.validation = config.gsplat_validation_every > 0 && (i+1) % config.gsplat_validation_every == 0;
        const float focal = view.camera.focal_length;
        if (camera_models.find(focal) == camera_models.end()) {
            const int id = static_cast<int>(camera_models.size())+1;
            camera_models.insert(std::make_pair(focal,id));
        }
        view.camera_id = camera_models[focal];
        std::ostringstream filename;
        filename << "frame_" << std::setfill('0') << std::setw(6) << i << ".png";
        view.filename = filename.str();
        const std::string subdir = view.validation ? "validation/images" : "images";
        if (view.validation && validation_count++ == 0) make_directory(join_path(config.output_dir, subdir));
        scene.render_capture(surface, view.camera, config.gsplat_time, &owners, config.gsplat_halo,
                             config.gsplat_particle_size_scale, config.gsplat_particle_cloud_scale);
        collect_observations(&view, surface, owners, &points, -std::numeric_limits<float>::infinity(),
                             std::numeric_limits<double>::infinity());
        std::string error;
        if (!write_png24(join_path(join_path(config.output_dir,subdir),view.filename),surface,&error))
            throw std::runtime_error(error);
        const Scene3dVec3& p = view.camera.position;
        const Scene3dVec3& t = view.camera.target;
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
    if (count == 0) throw std::runtime_error("no points seen by two training views; increase --frames or revise path");
    if (validation_count != 0)
        write_model(join_path(config.output_dir,"validation/sparse"),config,views,points,true);
    std::size_t fetus_points = 0, particle_points = 0;
    for (std::size_t i = 0; i < points.size(); ++i) {
        if (points[i].count < 2) continue;
        if (points[i].sample.surface_id >= 1000000) ++particle_points;
        else ++fetus_points;
    }
    std::ofstream readme = output_file(join_path(config.output_dir,"IMPORT.txt"));
    readme << "Feta static full-sphere capture (C++ offline exporter)\n\n"
           << "Generated path: " << (config.gsplat_camera_path.empty() ? views.front().group : "csv") << ".\n"
           << "Octahedral sampling uses a square grid of cell centers; lateral offsets are reprojected onto the same sphere.\n"
           << "Target offsets are distributed on a disk around the mesh center, changing orientation without adding camera positions.\n"
           << "CSV replay uses the supplied positions/targets/FOVs, including legacy sphere paths.\n"
           << "Postshot: import ONLY images/ together with the three files in sparse/.\n"
           << "Keep validation/ outside training. Do not import the whole dataset root.\n"
           << "Animations frozen; no inter-view feedback, temporal averaging or scripted fades.\n"
           << "Yellow halo: " << (config.gsplat_halo ? "stationary native mask feedback, evaluated independently per view" : "disabled") << ".\n"
           << "The halo is a screen-space appearance effect, with no additional finite geometry or point tracks.\n"
           << "Particles retain frozen world centers and face each capture camera as additive billboards.\n"
           << "Sprite size follows focal length/depth, with world-size multiplier " << config.gsplat_particle_size_scale << ".\n"
           << "Particle centers have spread multiplier " << config.gsplat_particle_cloud_scale << " about native origin; frozen rotation follows scaling.\n"
           << "Camera planning uses the original cloud enclosure; expanded particles may extend outside the frame or surround cameras.\n"
           << "The surrounding sphere is a directional panorama, not a finite mesh; no background seeds.\n"
           << "Seed points are visible fetus surface samples and particle centers, seen in >=2 training views.\n"
           << "particles.csv records all frozen centers in native coordinates; a point3D_id may be absent if not retained.\n"
           << "COLMAP world mirrors native X; Z remains up. Poses are world-to-camera.\n"
           << "Replay camera_path.csv with --gsplat-camera-path, same dimensions, --gsplat-time, --gsplat-halo and particle scale options.\n"
           << "Original environment mapping and additive composition remain view-dependent.\n"
           << "A complete export has capture.json with status=complete.\n";
    finish(readme);
    std::ofstream summary = output_file(join_path(config.output_dir,"capture.json"));
    summary << "{\n  \"status\": \"complete\",\n  \"sequence\": \"feta-gsplat\",\n"
            << "  \"source_revision\": \"" << FORWARD_SOURCE_REVISION << "\",\n"
            << "  \"width\": " << config.width << ", \"height\": " << config.height << ",\n"
            << "  \"scene_time_seconds\": " << config.gsplat_time << ",\n"
            << "  \"view_count\": " << views.size() << ", \"validation_views\": " << validation_count << ",\n"
            << "  \"validation_every\": " << config.gsplat_validation_every << ",\n"
            << "  \"path_kind\": \"" << (config.gsplat_camera_path.empty() ? views.front().group : "csv") << "\",\n"
            << "  \"sampling\": \"" << (config.gsplat_camera_path.empty() ? config.gsplat_sampling : "csv") << "\",\n"
            << "  \"octahedral_grid_side\": " << (config.gsplat_camera_path.empty() && config.gsplat_sampling == "octahedral" ? static_cast<int>(std::sqrt(double(views.size()))+0.5) : 0) << ",\n"
            << "  \"lateral_offset_requested\": " << config.gsplat_lateral_offset << ",\n"
            << "  \"lateral_offset_applied\": " << (config.gsplat_camera_path.empty() ? config.gsplat_lateral_offset : 0) << ",\n"
            << "  \"target_offset_requested\": " << config.gsplat_target_offset << ",\n"
            << "  \"target_offset_applied\": " << (config.gsplat_camera_path.empty() ? config.gsplat_target_offset : 0) << ",\n"
            << "  \"target_offset_pattern\": \"" << (!config.gsplat_camera_path.empty() ? "csv" :
                    (config.gsplat_target_offset > 0 ? "golden_angle_disk_camera_plane" : "none")) << "\",\n"
            << "  \"orbit_center_native\": [" << bounds.center.x << ',' << bounds.center.y << ',' << bounds.center.z << "],\n"
            << "  \"radius_start\": " << radius_start << ", \"radius_end\": " << radius_end << ",\n"
            << "  \"radius_min\": " << radius_min << ", \"radius_max\": " << radius_max << ",\n"
            << "  \"fetus_radius\": " << bounds.fetus_radius << ", \"enclosing_radius\": " << bounds.enclosing_radius << ",\n"
            << "  \"particle_count\": " << particle_count << ", \"particle_world_size\": " << bounds.particle_world_size << ",\n"
            << "  \"particle_size_scale\": " << config.gsplat_particle_size_scale << ",\n"
            << "  \"particle_cloud_scale\": " << config.gsplat_particle_cloud_scale << ",\n"
            << "  \"particle_scale_origin_native\": [0,0,0],\n"
            << "  \"camera_framing_enclosing_radius\": " << framing_bounds.enclosing_radius << ",\n"
            << "  \"points\": " << count << ", \"fetus_points\": " << fetus_points
            << ", \"particle_points\": " << particle_points << ",\n"
            << "  \"world_conversion\": \"COLMAP = diag(-1,1,1) * native; world Z up\",\n"
            << "  \"halo\": \"" << (config.gsplat_halo ? "stationary_native_feedback" : "off") << "\",\n"
            << "  \"effects\": \"frozen particles; camera-facing additive billboards; no inter-view feedback, temporal averaging or scripted fades\",\n"
            << "  \"background\": \"original directional panorama; no finite sphere geometry\"\n}\n";
    finish(summary);
    std::cout << "\nWrote " << views.size()-validation_count << " training PNGs, " << validation_count
              << " validation PNGs and " << count << " sparse points (fetus=" << fetus_points
              << ", particles=" << particle_points << ") to " << config.output_dir << '\n';
    return 0;
}
}  // namespace

int export_feta_capture(const ExportConfig& config) {
    try { return run_capture(config); }
    catch (const std::exception& error) {
        std::cerr << "Feta capture: " << error.what() << '\n';
        return 1;
    }
}
}  // namespace forward_offline
