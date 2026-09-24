#include "app/maku_capture.h"

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

struct Segment {
    int start_sample;
    unsigned int song_position;
    float offset, speed;
};

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
    int end_sample = 0;
    const std::vector<Segment> segments = camera_segments(&end_sample);
    const double duration = double(end_sample) / kTimelineRate;
    make_directory(join_path(config.output_dir, "images"));
    std::ofstream path = output_file(join_path(config.output_dir, "camera_path.csv"));
    std::ofstream manifest = output_file(join_path(config.output_dir, "manifest.csv"));
    path << "px,py,pz,tx,ty,tz,hfov_degrees,group,scene_time_seconds,track_time_seconds,roll_radians\n";
    manifest << "image_id,file,split,group,scene_time_seconds,camera_id\n";
    std::vector<View> views;
    std::vector<Point> points;
    MakuCaptureGeometry geometry;
    RgbSurface surface(config.width, config.height);
    std::vector<int> owners;
    std::size_t segment_index = 0, validation_count = 0;
    std::cout << "Maku gsplat: " << config.frame_count << " views over " << duration
              << " s, original scripted camera path, horizontal FOV " << config.gsplat_fov << " degrees\n";
    for (int i = 0; i < config.frame_count; ++i) {
        const int sample = static_cast<int>(static_cast<std::int64_t>(i) * end_sample / config.frame_count);
        const double time = double(sample) / kTimelineRate;
        while (segment_index + 1 < segments.size() && segments[segment_index + 1].start_sample <= sample)
            ++segment_index;
        const Segment& segment = segments[segment_index];
        const float reference = static_cast<float>(double(segment.start_sample) / kTimelineRate);
        const float track_time = (static_cast<float>(time) - reference) * segment.speed + segment.offset;
        View view;
        view.fov = config.gsplat_fov;
        std::ostringstream group;
        group << "maku_" << std::hex << segment.song_position;
        view.group = group.str();
        view.camera = scene.capture_camera(track_time, config.width, config.height,
                                           static_cast<float>(view.fov * kPi / 180.0));
        view.camera_id = 1;
        view.validation = config.gsplat_validation_every > 0 && (i+1) % config.gsplat_validation_every == 0;
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
             << ',' << view.fov << ',' << view.group << ',' << time << ',' << track_time << ",0\n";
        manifest << i+1 << ',' << subdir << '/' << view.filename << ','
                 << (view.validation ? "validation" : "train") << ',' << view.group << ','
                 << time << ',' << view.camera_id << '\n';
        views.push_back(view);
        if ((i+1) % 10 == 0 || i+1 == config.frame_count)
            std::cout << "\rRendered " << i+1 << '/' << config.frame_count << std::flush;
    }
    finish(path); finish(manifest);
    const std::size_t count = write_model(join_path(config.output_dir, "sparse"), config, views, points, false);
    if (count == 0) throw std::runtime_error("no points seen by two training views; increase --frames");
    if (validation_count != 0)
        write_model(join_path(config.output_dir, "validation/sparse"), config, views, points, true);
    std::ofstream readme = output_file(join_path(config.output_dir, "IMPORT.txt"));
    readme << "Maku capture (C++ offline exporter)\n\n"
           << "Postshot: import ONLY images/ together with the three files in sparse/.\n"
           << "Keep validation/ separate from training. Do not import the entire dataset root.\n"
           << "Original ASE path and XM-scripted cuts/speeds, sampled over the whole scene.\n"
           << "Original fog and affine terrain textures retained; feedback and shocks disabled.\n"
           << "PNG resolution is rendered directly; no upscaling. No hemisphere pass.\n"
           << "camera_path.csv records native positions, targets, FOV and timeline (not a replay input).\n"
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
            << "  \"duration_seconds\": " << duration << ", \"timeline_sample_rate\": " << kTimelineRate << ",\n"
            << "  \"view_count\": " << views.size() << ", \"validation_views\": " << validation_count << ",\n"
            << "  \"validation_every\": " << config.gsplat_validation_every << ", \"points\": " << count << ",\n"
            << "  \"world_conversion\": \"COLMAP = diag(-1,1,1) * native; world Z up\",\n"
            << "  \"effects\": \"original fog and affine textures; feedback and shocks disabled\",\n"
            << "  \"path\": \"original ASE and XM script, 0x0D00 inclusive to 0x1000 exclusive\",\n"
            << "  \"segments\": [\n";
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
