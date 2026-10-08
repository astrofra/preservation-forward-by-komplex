#include "app/export_config.h"

#include <cstdlib>
#include <cmath>
#include <cerrno>
#include <limits>
#include <ostream>
#include <string>

namespace forward_offline {

namespace {

bool parse_positive_int(const std::string& text, int* value) {
    if (text.empty()) {
        return false;
    }

    char* end = NULL;
    const long parsed = std::strtol(text.c_str(), &end, 10);
    if (end == NULL || *end != '\0') {
        return false;
    }
    if (parsed <= 0 || parsed > std::numeric_limits<int>::max()) {
        return false;
    }

    *value = static_cast<int>(parsed);
    return true;
}

bool parse_nonnegative_int(const std::string& text, int* value) {
    if (text.empty()) {
        return false;
    }

    char* end = NULL;
    const long parsed = std::strtol(text.c_str(), &end, 10);
    if (end == NULL || *end != '\0') {
        return false;
    }
    if (parsed < 0 || parsed > std::numeric_limits<int>::max()) {
        return false;
    }

    *value = static_cast<int>(parsed);
    return true;
}

bool parse_hex_u32(const std::string& text, unsigned int* value) {
    if (text.empty()) {
        return false;
    }

    char* end = NULL;
    const unsigned long parsed = std::strtoul(text.c_str(), &end, 0);
    if (end == NULL || *end != '\0' || parsed > std::numeric_limits<unsigned int>::max()) {
        return false;
    }

    *value = static_cast<unsigned int>(parsed);
    return true;
}

bool require_value(int argc, int index, const std::string& option, std::ostream& stream) {
    if (index + 1 < argc) {
        return true;
    }

    stream << "missing value for " << option << '\n';
    print_usage(stream);
    return false;
}

}  // namespace

ExportConfig::ExportConfig()
    : output_dir("output"),
      width(512),
      height(256),
      fps(50),
      sample_rate(22050),
      frame_count(250),
      intro_frames_per_row(6),
      intro_rows_per_order(64),
      has_end_song_position(false),
      end_song_position_hex(0U),
      post_roll_frames(0),
      write_log(true),
      sequence_name("intro"),
      gsplat_time(30.0f),
      gsplat_radius(0.0f),
      gsplat_end_radius(0.0f),
      gsplat_halo(true),
      gsplat_sampling("orbit"),
      gsplat_lateral_offset(0.0f),
      gsplat_target_offset(0.0f),
      gsplat_particle_size_scale(1.0f),
      gsplat_particle_cloud_scale(1.0f),
      gsplat_fov(80.0f),
      gsplat_height_fraction(0.25f),
      gsplat_grid_scale(1.5f),
      gsplat_grid_spacing(75.0f),
      gsplat_grid_clearance(0.0f),
      gsplat_validation_every(10),
      gsplat_camera_path() {
}

ParseStatus parse_export_config(int argc, char** argv, ExportConfig& config, std::ostream& stream) {
    bool gsplat_option = false, explicit_width = false, explicit_height = false, explicit_frames = false;
    bool path_option = false, static_option = false, fov_option = false, height_option = false;
    bool end_radius_option = false, halo_option = false;
    bool sampling_option = false, lateral_option = false;
    bool particle_option = false;
    bool grid_option = false;
    bool explicit_time = false;
    for (int index = 1; index < argc; ++index) {
        const std::string arg(argv[index]);

        if (arg == "--help" || arg == "-h") {
            print_usage(stream);
            return ParseStatus::help;
        }

        if (arg == "--no-log") {
            config.write_log = false;
            continue;
        }

        if (!require_value(argc, index, arg, stream)) {
            return ParseStatus::error;
        }

        const std::string value(argv[++index]);
        int parsed = 0;

        if (arg == "--output") {
            config.output_dir = value;
        } else if (arg == "--frames") {
            if (!parse_positive_int(value, &parsed)) {
                stream << "invalid frame count: " << value << '\n';
                return ParseStatus::error;
            }
            config.frame_count = parsed;
            explicit_frames = true;
        } else if (arg == "--width") {
            if (!parse_positive_int(value, &parsed)) {
                stream << "invalid width: " << value << '\n';
                return ParseStatus::error;
            }
            config.width = parsed;
            explicit_width = true;
        } else if (arg == "--height") {
            if (!parse_positive_int(value, &parsed)) {
                stream << "invalid height: " << value << '\n';
                return ParseStatus::error;
            }
            config.height = parsed;
            explicit_height = true;
        } else if (arg == "--fps") {
            if (!parse_positive_int(value, &parsed)) {
                stream << "invalid fps: " << value << '\n';
                return ParseStatus::error;
            }
            config.fps = parsed;
        } else if (arg == "--sample-rate") {
            if (!parse_positive_int(value, &parsed)) {
                stream << "invalid sample rate: " << value << '\n';
                return ParseStatus::error;
            }
            config.sample_rate = parsed;
        } else if (arg == "--sequence") {
            config.sequence_name = value;
        } else if (arg == "--gsplat-time" || arg == "--gsplat-radius" || arg == "--gsplat-fov" ||
                   arg == "--gsplat-height-fraction" || arg == "--gsplat-end-radius" ||
                   arg == "--gsplat-grid-scale" || arg == "--gsplat-grid-spacing" || arg == "--gsplat-grid-clearance") {
            char* end = NULL;
            errno = 0;
            const double number = std::strtod(value.c_str(), &end);
            double maximum = 2000.0;
            if (arg == "--gsplat-time") maximum = 86400.0;
            else if (arg == "--gsplat-fov") maximum = 120.0;
            else if (arg == "--gsplat-height-fraction") maximum = 1.0;
            else if (arg == "--gsplat-grid-scale") maximum = 3.0;
            else if (arg == "--gsplat-grid-spacing" || arg == "--gsplat-grid-clearance") maximum = 1000.0;
            const double minimum = (arg == "--gsplat-fov" || arg == "--gsplat-grid-spacing") ? 10.0 : 0.0;
            if (value.empty() || end == value.c_str() || *end != '\0' || errno == ERANGE ||
                !std::isfinite(number) || number < minimum || number > maximum ||
                (arg == "--gsplat-grid-scale" && number > 0 && number < 1)) {
                stream << "invalid value for " << arg << ": " << value << '\n';
                return ParseStatus::error;
            }
            if (arg == "--gsplat-time") {
                config.gsplat_time = static_cast<float>(number);
                explicit_time = true;
            }
            else if (arg == "--gsplat-radius") config.gsplat_radius = static_cast<float>(number);
            else if (arg == "--gsplat-end-radius") config.gsplat_end_radius = static_cast<float>(number);
            else if (arg == "--gsplat-fov") config.gsplat_fov = static_cast<float>(number);
            else if (arg == "--gsplat-grid-scale") config.gsplat_grid_scale = static_cast<float>(number);
            else if (arg == "--gsplat-grid-spacing") config.gsplat_grid_spacing = static_cast<float>(number);
            else if (arg == "--gsplat-grid-clearance") config.gsplat_grid_clearance = static_cast<float>(number);
            else config.gsplat_height_fraction = static_cast<float>(number);
            if (arg == "--gsplat-fov") fov_option = true;
            else if (arg == "--gsplat-height-fraction") height_option = true;
            else if (arg == "--gsplat-end-radius") end_radius_option = true;
            else if (arg.compare(0, 14, "--gsplat-grid-") == 0) grid_option = true;
            else static_option = true;
            gsplat_option = true;
        } else if (arg == "--gsplat-sampling") {
            if (value != "orbit" && value != "octahedral") {
                stream << "--gsplat-sampling expects orbit or octahedral\n";
                return ParseStatus::error;
            }
            config.gsplat_sampling = value;
            sampling_option = gsplat_option = true;
        } else if (arg == "--gsplat-lateral-offset" || arg == "--gsplat-target-offset") {
            char* end = NULL;
            errno = 0;
            const double number = std::strtod(value.c_str(), &end);
            if (value.empty() || end == value.c_str() || *end != '\0' || errno == ERANGE ||
                !std::isfinite(number) || number < 0 || number > 1) {
                stream << arg << " expects 0..1 world units\n";
                return ParseStatus::error;
            }
            if (arg == "--gsplat-target-offset") config.gsplat_target_offset = static_cast<float>(number);
            else config.gsplat_lateral_offset = static_cast<float>(number);
            lateral_option = gsplat_option = true;
        } else if (arg == "--gsplat-particle-size-scale" || arg == "--gsplat-particle-cloud-scale") {
            char* end = NULL;
            errno = 0;
            const double number = std::strtod(value.c_str(), &end);
            if (value.empty() || end == value.c_str() || *end != '\0' || errno == ERANGE ||
                !std::isfinite(number) || number < 0.1 || number > 10) {
                stream << arg << " expects a scale in 0.1..10\n";
                return ParseStatus::error;
            }
            if (arg == "--gsplat-particle-size-scale") config.gsplat_particle_size_scale = static_cast<float>(number);
            else config.gsplat_particle_cloud_scale = static_cast<float>(number);
            particle_option = gsplat_option = true;
        } else if (arg == "--gsplat-halo") {
            if (value != "on" && value != "off") {
                stream << "--gsplat-halo expects on or off\n";
                return ParseStatus::error;
            }
            config.gsplat_halo = value == "on";
            halo_option = gsplat_option = true;
        } else if (arg == "--gsplat-camera-path") {
            config.gsplat_camera_path = value;
            path_option = true;
            gsplat_option = true;
        } else if (arg == "--gsplat-validation-every") {
            if (!parse_nonnegative_int(value, &parsed) || parsed == 1) {
                stream << "validation interval must be 0 (disabled) or at least 2\n";
                return ParseStatus::error;
            }
            config.gsplat_validation_every = parsed;
            gsplat_option = true;
        } else if (arg == "--until-song-position") {
            unsigned int parsed_hex = 0U;
            if (!parse_hex_u32(value, &parsed_hex)) {
                stream << "invalid song position hex: " << value << '\n';
                return ParseStatus::error;
            }
            config.has_end_song_position = true;
            config.end_song_position_hex = parsed_hex;
        } else if (arg == "--post-roll-frames") {
            if (!parse_nonnegative_int(value, &parsed)) {
                stream << "invalid post-roll frame count: " << value << '\n';
                return ParseStatus::error;
            }
            config.post_roll_frames = parsed;
        } else if (arg == "--intro-frames-per-row") {
            if (!parse_positive_int(value, &parsed)) {
                stream << "invalid intro frames per row: " << value << '\n';
                return ParseStatus::error;
            }
            config.intro_frames_per_row = parsed;
        } else if (arg == "--intro-rows-per-order") {
            if (!parse_positive_int(value, &parsed)) {
                stream << "invalid intro rows per order: " << value << '\n';
                return ParseStatus::error;
            }
            config.intro_rows_per_order = parsed;
        } else {
            stream << "unknown option: " << arg << '\n';
            print_usage(stream);
            return ParseStatus::error;
        }
    }

    const bool saari_capture = config.sequence_name == "saari-gsplat";
    const bool maku_capture = config.sequence_name == "maku-gsplat";
    const bool feta_capture = config.sequence_name == "feta-gsplat";
    if ((gsplat_option && !saari_capture && !maku_capture && !feta_capture) ||
        ((static_option || path_option) && !saari_capture && !feta_capture) ||
        (fov_option && !maku_capture && !feta_capture) || (height_option && !maku_capture) ||
        ((end_radius_option || halo_option || sampling_option || lateral_option || particle_option) && !feta_capture) ||
        (grid_option && !maku_capture)) {
        stream << "GSplat options: time/radius/camera-path for Saari or Feta, fov for Maku or Feta, height-fraction/grid-* for Maku, end-radius/halo/sampling/lateral-offset/target-offset/particle-*-scale for Feta, validation-every for all capture modes\n";
        return ParseStatus::error;
    }
    if (saari_capture || maku_capture || feta_capture) {
        if (feta_capture && !explicit_time) config.gsplat_time = 0.0f;
        if (!explicit_width) config.width = 1024;
        if (!explicit_height) config.height = maku_capture ? 512 : 768;
        if (!explicit_frames) config.frame_count = 300;
        if (feta_capture && config.gsplat_sampling == "octahedral" && !explicit_frames)
            config.frame_count = 324;
        if (config.width < 16 || config.height < 16 || config.width > 4096 || config.height > 4096 ||
            config.frame_count < 12 || config.frame_count > 5000 ||
            config.has_end_song_position || config.post_roll_frames != 0) {
            stream << "GSplat capture requires dimensions 16..4096, frames 12..5000 and no song timeline override\n";
            return ParseStatus::error;
        }
    } else if (config.sample_rate % config.fps != 0) {
        stream << "sample rate must be divisible by fps for exact sync: "
               << config.sample_rate << " / " << config.fps << '\n';
        return ParseStatus::error;
    }

    return ParseStatus::ok;
}

void print_usage(std::ostream& stream) {
    stream
        << "Usage: forward-export [options]\n"
        << "  --output <dir>        Output directory (default: output)\n"
        << "  --frames <count>      Number of frames to export (default: 250)\n"
        << "  --width <pixels>      Frame width (default: 512)\n"
        << "  --height <pixels>     Frame height (default: 256)\n"
        << "  --fps <rate>          Video frame rate (default: 50)\n"
        << "  --sample-rate <hz>    Audio sample rate (default: 22050)\n"
        << "  --sequence <name>     Export sequence: intro|saari|saari-gsplat|kukot|maku|maku-gsplat|watercube|feta|feta-gsplat|uppol|bootstrap\n"
        << "                        (default: intro)\n"
        << "  saari-gsplat defaults: 300 PNG views at 1024x768, hemisphere + meditate focus\n"
        << "  maku-gsplat defaults: 300 views per path at 1024x512, original + raised H/4 + overhead grid\n"
        << "  feta-gsplat defaults: 300 views at 1024x768, constant-radius sphere, frozen time 0, static yellow halo\n"
        << "  --gsplat-fov <deg>    Maku/Feta horizontal FOV, 10..120 degrees (default: 80)\n"
        << "  --gsplat-height-fraction <f>  Maku second path: vertical offset f * terrain height, 0..1 (default: 0.25; 0 disables)\n"
        << "  --gsplat-grid-scale <f>  Maku grid XY extent / nominal camera bounds, 1..3 (default: 1.5; 0 disables)\n"
        << "  --gsplat-grid-spacing <r>  Maku maximum grid spacing, 10..1000 world units (default: 75)\n"
        << "  --gsplat-grid-clearance <r>  Maku grid height above terrain maximum, 0..1000 (default: 0 = H/8)\n"
        << "  --gsplat-time <s>     Frozen scene time (Saari: 30, Feta: 0)\n"
        << "  --gsplat-radius <r>   Saari hemisphere/Feta starting radius, 0 = automatic (default: 0)\n"
        << "  --gsplat-end-radius <r>  Feta optional inward orbit: final radius, 0 = constant starting radius (default: 0)\n"
        << "  --gsplat-halo <on|off>  Feta stationary native halo per view (default: on)\n"
        << "  --gsplat-sampling <orbit|octahedral>  Feta path (default: orbit); octahedral needs square frame count, default 324\n"
        << "  --gsplat-lateral-offset <d>  Feta octahedral tangent shift, reprojected onto sphere; 0..1 world units (default: 0)\n"
        << "  --gsplat-target-offset <d>  Feta octahedral look-target disk radius; camera positions unchanged; 0..1 (default: 0)\n"
        << "  --gsplat-particle-size-scale <s>  Feta capture sprite world-size multiplier, 0.1..10 (default: 1)\n"
        << "  --gsplat-particle-cloud-scale <s>  Feta capture particle-center spread about native origin, 0.1..10 (default: 1); camera framing unchanged\n"
        << "  --gsplat-camera-path <csv>  Replay/edit exported camera_path.csv (overrides view count)\n"
        << "  --gsplat-validation-every <n>  Hold out every nth view; 0 disables (default: 10)\n"
        << "  --until-song-position <hex>\n"
        << "                        Resolve frame count from the native XM timeline\n"
        << "  --post-roll-frames <n>\n"
        << "                        Extra frames after --until-song-position (default: 0)\n"
        << "  --intro-frames-per-row <n>\n"
        << "                        Legacy wrapper pacing hint kept for compatibility (default: 6)\n"
        << "  --intro-rows-per-order <n>\n"
        << "                        Legacy wrapper order length hint (default: 64)\n"
        << "  --no-log              Skip output/log.txt\n"
        << "  --help                Show this message\n";
}
}  // namespace forward_offline
