#include "app/maku_capture.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>
#include "app/capture_model.h"
#include "app/intro_script.h"
#include "audio/module_player.h"
#include "platform/file_utils.h"
#include "render/png_writer.h"
#include "scenes/maku_scene.h"

#ifndef FORWARD_SOURCE_REVISION
#define FORWARD_SOURCE_REVISION "unknown"
#endif

namespace forward_offline {
namespace {
using namespace capture;
const double kPi = 3.14159265358979323846;
// Match the offline demo's default XM clock, independently of dataset density.
const int kTimelineRate = 22050;
const int kBoundsRate = 50;
const int kViewsPerStation = 17;

struct Segment {
    int start_sample;
    unsigned int song_position;
    float offset, speed;
};

struct Grid {
    bool enabled;
    Scene3dVec3 camera_min, camera_max;
    double min_x, max_x, min_y, max_y, z, clearance, spacing_x, spacing_y;
    int columns, rows;
};

float track_time_at(const std::vector<Segment>& segments, int sample, std::size_t* index) {
    *index = 0;
    while (*index+1 < segments.size() && segments[*index+1].start_sample <= sample) ++*index;
    const Segment& s = segments[*index];
    const float reference = static_cast<float>(double(s.start_sample)/kTimelineRate);
    return (static_cast<float>(double(sample)/kTimelineRate)-reference)*s.speed+s.offset;
}

Grid create_grid(const ExportConfig& config, const MakuScene& scene,
                 const std::vector<Segment>& segments, int end_sample,
                 const std::pair<float, float>& height_range) {
    Grid grid = {};
    grid.enabled = config.gsplat_grid_scale > 0;
    if (!grid.enabled) return grid;
    // Fixed 50 Hz plus both sides of every cut: independent of capture density,
    // height offset and the ASE track portions that the demo never visits.
    std::vector<int> samples;
    for (int sample = 0; sample < end_sample; sample += kTimelineRate/kBoundsRate) samples.push_back(sample);
    for (std::size_t i = 0; i < segments.size(); ++i) {
        samples.push_back(segments[i].start_sample);
        if (i > 0) samples.push_back(segments[i].start_sample-1);
    }
    samples.push_back(end_sample-1);
    for (std::size_t i = 0; i < samples.size(); ++i) {
        std::size_t segment_index = 0;
        const Scene3dVec3 p = scene.capture_camera(track_time_at(segments, samples[i], &segment_index),
            config.width, config.height, static_cast<float>(config.gsplat_fov*kPi/180)).position;
        if (i == 0) grid.camera_min = grid.camera_max = p;
        grid.camera_min.x = std::min(grid.camera_min.x, p.x); grid.camera_max.x = std::max(grid.camera_max.x, p.x);
        grid.camera_min.y = std::min(grid.camera_min.y, p.y); grid.camera_max.y = std::max(grid.camera_max.y, p.y);
        grid.camera_min.z = std::min(grid.camera_min.z, p.z); grid.camera_max.z = std::max(grid.camera_max.z, p.z);
    }
    const double cx = (double(grid.camera_min.x)+grid.camera_max.x)*0.5;
    const double cy = (double(grid.camera_min.y)+grid.camera_max.y)*0.5;
    const double half_x = (double(grid.camera_max.x)-grid.camera_min.x)*config.gsplat_grid_scale*0.5;
    const double half_y = (double(grid.camera_max.y)-grid.camera_min.y)*config.gsplat_grid_scale*0.5;
    grid.min_x = cx-half_x; grid.max_x = cx+half_x;
    grid.min_y = cy-half_y; grid.max_y = cy+half_y;
    grid.columns = std::max(2, static_cast<int>(std::ceil(2*half_x/config.gsplat_grid_spacing))+1);
    grid.rows = std::max(2, static_cast<int>(std::ceil(2*half_y/config.gsplat_grid_spacing))+1);
    if (grid.columns*grid.rows*kViewsPerStation > 10000)
        throw std::runtime_error("Maku grid exceeds 10000 views; increase --gsplat-grid-spacing or reduce --gsplat-grid-scale");
    grid.spacing_x = 2*half_x/(grid.columns-1);
    grid.spacing_y = 2*half_y/(grid.rows-1);
    grid.clearance = config.gsplat_grid_clearance > 0 ? config.gsplat_grid_clearance :
        std::max(1.0, (double(height_range.second)-height_range.first)/8);
    grid.z = height_range.second+grid.clearance;
    return grid;
}

int song_sample(unsigned int position) {
    int sample = 0;
    std::string error;
    // One frame per sample asks the existing resolver for exact sample indices,
    // without rendering or writing an audio track.
    if (!resolve_sequence_frame_count_for_song_position("maku", kTimelineRate, kTimelineRate,
                                                         position, 0, &sample, &error))
        throw std::runtime_error(error);
    return sample;
}

std::vector<Segment> camera_segments(int* end_sample) {
    const MakuScript script;
    const std::vector<ScriptCommand>& commands = script.commands();
    std::vector<Segment> segments;
    float speed = 3.0f;
    *end_sample = 0;
    for (std::size_t i = 0; i < commands.size(); ++i) {
        const ScriptCommand& command = commands[i];
        if (command.verb == "shutdown") {
            *end_sample = song_sample(command.song_position_hex);
            break;
        }
        if (command.verb != "msg" || command.target != "maku") continue;
        if (command.argument.compare(0, 3, "go ") == 0) {
            Segment segment;
            segment.start_sample = song_sample(command.song_position_hex);
            segment.song_position = command.song_position_hex;
            segment.offset = std::strtof(command.argument.c_str() + 3, NULL);
            segment.speed = speed;
            segments.push_back(segment);
        } else if (command.argument.compare(0, 6, "speed ") == 0) {
            // The original script pairs every speed change with a go command.
            if (segments.empty() || segments.back().song_position != command.song_position_hex)
                throw std::runtime_error("Maku script changed: unpaired speed command");
            speed = std::strtof(command.argument.c_str() + 6, NULL);
            segments.back().speed = speed;
        } else if (command.argument == "roll") {
            throw std::runtime_error("Maku script changed: capture needs roll support");
        }
    }
    if (segments.empty() || segments.front().start_sample != 0 ||
        *end_sample <= segments.back().start_sample)
        throw std::runtime_error("invalid Maku capture timeline");
    return segments;
}

int run_capture(const ExportConfig& config) {
    struct stat info;
    if (stat(config.output_dir.c_str(), &info) == 0)
        throw std::runtime_error("capture output already exists; choose a new --output directory");
    MakuScene scene;
    scene.init();
    if (!scene.is_ready()) throw std::runtime_error(scene.error_message());
    const std::pair<float, float> height_range = scene.capture_height_range();
    const float height_offset = (height_range.second - height_range.first) * config.gsplat_height_fraction;
    const int pass_count = height_offset > 0.0f ? 2 : 1;
    const int path_view_count = config.frame_count * pass_count;
    int end_sample = 0;
    const std::vector<Segment> segments = camera_segments(&end_sample);
    const double duration = double(end_sample) / kTimelineRate;
    const Grid grid = create_grid(config, scene, segments, end_sample, height_range);
    const int grid_view_count = grid.columns*grid.rows*kViewsPerStation;
    const int view_count = path_view_count+grid_view_count;
    make_directory(join_path(config.output_dir, "images"));
    std::ofstream path = output_file(join_path(config.output_dir, "camera_path.csv"));
    std::ofstream manifest = output_file(join_path(config.output_dir, "manifest.csv"));
    path << "px,py,pz,tx,ty,tz,hfov_degrees,group,scene_time_seconds,track_time_seconds,roll_radians,pass,height_offset,grid_x,grid_y,azimuth_degrees,downward_degrees\n";
    manifest << "image_id,file,split,group,scene_time_seconds,camera_id,pass,height_offset,grid_x,grid_y,azimuth_degrees,downward_degrees\n";
    std::vector<View> views;
    std::vector<Point> points;
    MakuCaptureGeometry geometry;
    RgbSurface surface(config.width, config.height);
    std::vector<int> owners;
    std::size_t validation_count = 0;
    std::cout << "Maku gsplat: " << view_count << " views (" << config.frame_count << " per path) over " << duration
              << " s, horizontal FOV " << config.gsplat_fov << " degrees, raised offset " << height_offset << "\n";
    if (grid.enabled)
        std::cout << "Grid: " << grid.columns << 'x' << grid.rows << " stations, " << grid_view_count
                  << " views, Z " << grid.z << ", maximum spacing " << config.gsplat_grid_spacing << "\n";
    for (int i = 0; i < view_count; ++i) {
        const bool grid_view = i >= path_view_count;
        const int path_index = grid_view ? i-path_view_count : i % config.frame_count;
        const bool raised = !grid_view && i >= config.frame_count;
        const char* const pass_name = grid_view ? "grid" : (raised ? "raised" : "original");
        const float offset = raised ? height_offset : 0.0f;
        const int sample = grid_view ? 0 : static_cast<int>(static_cast<std::int64_t>(path_index) * end_sample / config.frame_count);
        const double time = double(sample) / kTimelineRate;
        std::size_t segment_index = 0;
        const float track_time = track_time_at(segments, sample, &segment_index);
        const Segment& segment = segments[segment_index];
        View view;
        view.fov = config.gsplat_fov;
        int column = -1, row = -1, azimuth = 0, downward = 0;
        if (grid_view) {
            const int station = path_index/kViewsPerStation;
            const int direction = path_index%kViewsPerStation;
            row = station/grid.columns;
            column = station%grid.columns;
            if (row%2 != 0) column = grid.columns-1-column;
            azimuth = direction < 16 ? (direction%8)*45 : 0;
            downward = direction < 8 ? 30 : (direction < 16 ? 60 : 90);
            const double az = azimuth*kPi/180, pitch = downward*kPi/180;
            const Scene3dVec3 p = {static_cast<float>(grid.min_x+column*grid.spacing_x),
                                   static_cast<float>(grid.min_y+row*grid.spacing_y), static_cast<float>(grid.z)};
            // A 100-unit target offset avoids cancellation when building float axes.
            const Scene3dVec3 t = {static_cast<float>(p.x+100*std::cos(pitch)*std::cos(az)),
                                   static_cast<float>(p.y+100*std::cos(pitch)*std::sin(az)),
                                   static_cast<float>(p.z-100*std::sin(pitch))};
            view.camera = make_maku_capture_camera(p, t, config.width, config.height,
                                                   static_cast<float>(view.fov*kPi/180));
            view.group = "maku_grid";
        } else {
            std::ostringstream group;
            group << "maku_" << std::hex << segment.song_position;
            view.group = group.str();
            view.camera = scene.capture_camera(track_time, config.width, config.height,
                                               static_cast<float>(view.fov * kPi / 180.0));
            // Translate endpoints; retain the original orientation's float rounding.
            view.camera.position.z += offset;
            view.camera.target.z += offset;
        }
        view.camera_id = 1;
        view.validation = config.gsplat_validation_every > 0 && (path_index+1) % config.gsplat_validation_every == 0;
        make_pose(&view);
        std::ostringstream filename;
        filename << "frame_" << std::setfill('0') << std::setw(6) << i << ".png";
        view.filename = filename.str();
        const std::string subdir = view.validation ? "validation/images" : "images";
        if (view.validation && validation_count++ == 0) make_directory(join_path(config.output_dir, subdir));
        scene.render_capture(surface, view.camera, &owners, &geometry);
        const std::size_t old_count = points.size();
        points.resize(geometry.points.size());
        for (std::size_t j = old_count; j < points.size(); ++j) points[j].sample = geometry.points[j];
        collect_observations(&view, surface, owners, &points,
                             -std::numeric_limits<float>::infinity(), 200.0);
        std::string error;
        if (!write_png24(join_path(join_path(config.output_dir, subdir), view.filename), surface, &error))
            throw std::runtime_error(error);
        const Scene3dVec3& p = view.camera.position;
        const Scene3dVec3& t = view.camera.target;
        path << p.x << ',' << p.y << ',' << p.z << ',' << t.x << ',' << t.y << ',' << t.z
             << ',' << view.fov << ',' << view.group << ',' << time << ',';
        if (!grid_view) path << track_time;
        path << ",0," << pass_name << ',';
        if (!grid_view) path << offset;
        manifest << i+1 << ',' << subdir << '/' << view.filename << ','
                 << (view.validation ? "validation" : "train") << ',' << view.group << ','
                 << time << ',' << view.camera_id << ',' << pass_name << ',';
        if (!grid_view) manifest << offset;
        if (grid_view) {
            path << ',' << column << ',' << row << ',' << azimuth << ',' << downward;
            manifest << ',' << column << ',' << row << ',' << azimuth << ',' << downward;
        } else {
            path << ",,,,"; manifest << ",,,,";
        }
        path << '\n'; manifest << '\n';
        views.push_back(view);
        if ((i+1) % 10 == 0 || i+1 == view_count)
            std::cout << "\rRendered " << i+1 << '/' << view_count << std::flush;
    }
    finish(path); finish(manifest);
    const std::size_t count = write_model(join_path(config.output_dir, "sparse"), config, views, points, false);
    if (count == 0) throw std::runtime_error("no points seen by two training views; increase --frames");
    if (validation_count != 0)
        write_model(join_path(config.output_dir, "validation/sparse"), config, views, points, true);
    std::vector<unsigned char> observed_by(points.size(), 0);
    std::size_t grid_empty_views = 0, grid_points = 0, shared_grid_points = 0;
    for (std::size_t i = 0; i < views.size(); ++i) {
        if (views[i].validation) continue;
        bool observed = false;
        for (std::size_t j = 0; j < views[i].observations.size(); ++j) {
            const std::size_t point = views[i].observations[j].point;
            if (points[point].count < 2) continue;
            observed_by[point] |= i < static_cast<std::size_t>(path_view_count) ? 1 : 2;
            observed = true;
        }
        if (i >= static_cast<std::size_t>(path_view_count) && !observed) ++grid_empty_views;
    }
    for (std::size_t i = 0; i < observed_by.size(); ++i) {
        if ((observed_by[i]&2) != 0) ++grid_points;
        if (observed_by[i] == 3) ++shared_grid_points;
    }
    std::ofstream readme = output_file(join_path(config.output_dir, "IMPORT.txt"));
    readme << "Maku capture (C++ offline exporter)\n\n"
           << "Postshot: import ONLY images/ together with the three files in sparse/.\n"
           << "Keep validation/ separate from training. Do not import the entire dataset root.\n"
           << "Original ASE path and XM-scripted cuts/speeds, sampled over the whole scene.\n"
           << "Paths: " << pass_count << "; views per path: " << config.frame_count
           << "; raised offset: " << height_offset << " native Z units.\n"
           << "All paths and grid views share images/ and one sparse/ model; no separate alignment.\n"
           << "The raised path translates both camera and target; orientation and fog are preserved.\n"
           << "Grid: " << grid.columns << " x " << grid.rows << " stations, " << grid_view_count << " views.\n"
           << "Grid uses nominal camera XY bounds at 50 Hz plus cut endpoints, scaled around their center.\n"
           << "Each station has eight azimuths at 30 and 60 degrees downward, plus one nadir view.\n"
           << "Grid altitude is above the terrain maximum; original fog and depth cutoff remain active.\n"
           << "Original fog and affine terrain textures retained; feedback and shocks disabled.\n"
           << "PNG resolution is rendered directly; no upscaling. No hemisphere pass.\n"
           << "camera_path.csv records poses, FOV, timeline, pass, height_offset and grid coordinates/angles (not a replay input).\n"
           << "Grid scene time is 0 (static terrain); track_time and height_offset are empty.\n"
           << "COLMAP world mirrors native X; Z remains up. Poses are world-to-camera.\n"
           << "Points sample unwrapped terrain triangles, visible in at least two training views.\n"
           << "Visibility follows actual painter order; seed points beyond depth 200 are excluded.\n"
           << "Exact camera poses do not remove view-dependent fog or affine texture distortion.\n"
           << "A complete export has capture.json with status=complete.\n";
    finish(readme);
    std::ofstream summary = output_file(join_path(config.output_dir, "capture.json"));
    summary << "{\n  \"status\": \"complete\",\n  \"sequence\": \"maku-gsplat\",\n"
            << "  \"source_revision\": \"" << FORWARD_SOURCE_REVISION << "\",\n"
            << "  \"width\": " << config.width << ", \"height\": " << config.height << ",\n"
            << "  \"hfov_degrees\": " << config.gsplat_fov << ",\n"
            << "  \"terrain_z_min\": " << height_range.first << ", \"terrain_z_max\": " << height_range.second << ",\n"
            << "  \"height_offset_fraction\": " << config.gsplat_height_fraction
            << ", \"height_offset\": " << height_offset << ",\n"
            << "  \"views_per_pass\": " << config.frame_count << ",\n"
            << "  \"duration_seconds\": " << duration << ", \"timeline_sample_rate\": " << kTimelineRate << ",\n"
            << "  \"view_count\": " << views.size() << ", \"validation_views\": " << validation_count << ",\n"
            << "  \"validation_every\": " << config.gsplat_validation_every << ", \"points\": " << count << ",\n"
            << "  \"world_conversion\": \"COLMAP = diag(-1,1,1) * native; world Z up\",\n"
            << "  \"effects\": \"original fog and affine textures; feedback and shocks disabled\",\n"
            << "  \"path\": \"original ASE and XM script, 0x0D00 inclusive to 0x1000 exclusive\",\n"
            << "  \"passes\": [{\"name\": \"original\", \"height_offset\": 0}";
    if (pass_count == 2)
        summary << ", {\"name\": \"raised\", \"height_offset\": " << height_offset << "}";
    if (grid.enabled) summary << ", {\"name\": \"grid\", \"views\": " << grid_view_count << ", \"height\": " << grid.z << "}";
    summary << "],\n  \"grid\": {\"enabled\": " << (grid.enabled ? "true" : "false");
    if (grid.enabled) {
        summary << ", \"scale\": " << config.gsplat_grid_scale << ", \"bounds_sample_rate\": " << kBoundsRate
                << ",\n    \"camera_min_native\": [" << grid.camera_min.x << ',' << grid.camera_min.y << ',' << grid.camera_min.z << ']'
                << ", \"camera_max_native\": [" << grid.camera_max.x << ',' << grid.camera_max.y << ',' << grid.camera_max.z << ']'
                << ",\n    \"min_xy\": [" << grid.min_x << ',' << grid.min_y << "], \"max_xy\": [" << grid.max_x << ',' << grid.max_y << ']'
                << ", \"z\": " << grid.z << ", \"clearance\": " << grid.clearance
                << ",\n    \"columns\": " << grid.columns << ", \"rows\": " << grid.rows
                << ", \"maximum_spacing\": " << config.gsplat_grid_spacing
                << ", \"spacing_x\": " << grid.spacing_x << ", \"spacing_y\": " << grid.spacing_y
                << ",\n    \"views_per_station\": " << kViewsPerStation << ", \"view_count\": " << grid_view_count
                << ", \"azimuth_step_degrees\": 45, \"downward_degrees\": [30,60,90]"
                << ",\n    \"points\": " << grid_points << ", \"points_shared_with_paths\": " << shared_grid_points
                << ", \"training_views_without_points\": " << grid_empty_views;
    }
    summary << "},\n  \"segments\": [\n";
    for (std::size_t i = 0; i < segments.size(); ++i) {
        const Segment& s = segments[i];
        summary << "    {\"song_position\": " << s.song_position << ", \"start_sample\": " << s.start_sample
                << ", \"track_offset_seconds\": " << s.offset << ", \"speed\": " << s.speed << "}"
                << (i+1 < segments.size() ? ",\n" : "\n");
    }
    summary << "  ]\n}\n";
    finish(summary);
    std::cout << "\nWrote " << views.size()-validation_count << " training PNGs, " << validation_count
              << " validation PNGs and " << count << " sparse points to " << config.output_dir << '\n';
    return 0;
}
}  // namespace

int export_maku_capture(const ExportConfig& config) {
    try { return run_capture(config); }
    catch (const std::exception& error) {
        std::cerr << "Maku capture: " << error.what() << '\n';
        return 1;
    }
}
}  // namespace forward_offline
