// Behavioral check against the unmodified playback renderer, held at one pose.
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include "scenes/feta_scene.h"

using namespace forward_offline;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

int main() {
    try {
        FetaScene capture;
        capture.init();
        require(capture.is_ready(), capture.error_message().c_str());
        const float times[] = {0.0f, 7.0f, 19.0f};
        for (int pose = 0; pose < 3; ++pose) {
            const float time = times[pose];
            FetaScene playback;
            playback.init();
            require(playback.is_ready(), playback.error_message().c_str());
            RgbSurface reference(512, 256), halo(512, 256), bare(512, 256), repeated(512, 256);
            // Native temporal averaging and mask history have converged by here.
            for (int i = 0; i < 32; ++i) playback.render(reference, time, 0.0f);
            const float elevation = std::sin(time / 10.0f);
            const float y = 5.0f * std::cos(elevation);
            const Scene3dVec3 position = {-y * std::sin(time / 4.0f),
                                          y * std::cos(time / 4.0f), 5.0f * std::sin(elevation)};
            const Scene3dVec3 target = {0, 0, 0};
            const CaptureCamera camera = make_feta_capture_camera(position, target, 512, 256, 1.9f);
            std::vector<int> owners, bare_owners;
            capture.render_capture(halo, camera, time, &owners);
            capture.render_capture(bare, camera, time, &bare_owners, false);
            require(owners == bare_owners, "halo changed surface ownership");
            std::size_t changed = 0, background_halo = 0;
            int maximum_error = 0;
            for (std::size_t i = 0; i < halo.pixels().size(); ++i) {
                if (halo.pixels()[i] != bare.pixels()[i]) {
                    ++changed;
                    if (owners[i] == 0) ++background_halo;
                }
                if (owners[i] > 0 && owners[i] < 1000000)
                    require(halo.pixels()[i] == bare.pixels()[i], "halo tinted the fetus surface");
                for (int shift = 0; shift <= 16; shift += 8) {
                    const int a = (halo.pixels()[i] >> shift) & 255;
                    const int b = (bare.pixels()[i] >> shift) & 255;
                    const int c = (reference.pixels()[i] >> shift) & 255;
                    require(a >= b, "halo must be additive");
                    maximum_error = std::max(maximum_error, std::abs(a-c));
                }
            }
            require(changed > 100 && background_halo > 100, "no visible halo outside the mesh");
            // The playback running integer average converges one level below a
            // constant input; capture intentionally excludes this motion blur.
            require(maximum_error <= 1, "capture differs from stationary native halo");
            capture.render_capture(repeated, camera, time, &owners);
            require(halo.pixels() == repeated.pixels(), "capture depends on prior views");
            std::cout << "pose " << time << ": " << changed << " halo pixels, native max error "
                      << maximum_error << "/255\n";
        }
        // Arbitrary dimensions and an off-center target must also work. This is
        // outside the original Java feedback's two hard-coded framebuffer sizes.
        const Scene3dVec3 position = {0, 16, 0}, target = {0.5f, 0, 0.2f};
        const int sizes[][2] = {{321,199}, {16,16}, {1024,768}};
        for (int size = 0; size < 3; ++size) {
            RgbSurface image(sizes[size][0], sizes[size][1]), again(sizes[size][0], sizes[size][1]);
            std::vector<int> owners;
            const CaptureCamera camera = make_feta_capture_camera(position, target,
                image.width(), image.height(), 1.4f);
            capture.render_capture(image, camera, 0, &owners);
            capture.render_capture(again, camera, 0, &owners);
            require(image.pixels() == again.pixels(), "non-native halo is not deterministic");
        }
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
