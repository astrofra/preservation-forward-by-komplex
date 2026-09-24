#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "app/demo_runtime.h"
#include "render/tga_writer.h"

using namespace forward_player;
using namespace forward_offline;

void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

void check_stream(const char* name) {
    std::string error;
    SequenceAudioRender reference, block;
    const std::size_t frames = kSampleRate * 9 + 137;
    require(render_sequence_module_audio(name, kSampleRate, frames, &reference, &error), error);
    ModuleAudioStream stream;
    require(stream.open(name, kSampleRate, &error), error);
    const std::size_t lengths[] = {1, 127, 441, 513, 2048, 13, 16384};
    std::vector<SongPositionEvent> events;
    for (std::size_t offset = 0, index = 0; offset < frames; ++index) {
        const std::size_t count = std::min(lengths[index % 7], frames - offset);
        require(stream.render(count, &block, &error), error);
        require(std::equal(block.interleaved_samples.begin(), block.interleaved_samples.end(),
                           reference.interleaved_samples.begin() + offset * 2),
                std::string(name) + ": PCM changes with block size at " + std::to_string(offset));
        events.insert(events.end(), block.song_positions.begin(), block.song_positions.end());
        offset += count;
    }
    require(events.size() == reference.song_positions.size(), "row event count differs");
    for (std::size_t i = 0; i < events.size(); ++i)
        require(events[i].sample_index == reference.song_positions[i].sample_index &&
                events[i].song_position_hex == reference.song_positions[i].song_position_hex,
                "row timestamps change with block size");
    std::cout << name << ": irregular-block PCM and row timestamps match batch renderer\n";
}

int main(int argc, char** argv) {
    try {
        check_stream("intro"); check_stream("saari");
        std::string error;
        ScoreAudio score;
        require(score.initialize(&error), error);
        std::uint64_t expected_start = 0;
        while (!score.ready()) {
            require(score.prepare_block(16384, &error), error);
            require(score.block_start() == expected_start, "gap or overlap in PCM preparation");
            expected_start += score.block().interleaved_samples.size() / 2;
        }
        require(expected_start == score.total_samples(), "prepared length mismatch");
        const char* expected[] = {"mute95", "domina", "saari", "kukot", "maku", "watercube", "feta", "uppol"};
        unsigned shows = 0;
        std::uint64_t prep_feta = 0, show_feta = 0;
        for (std::size_t i = 0; i < score.actions().size(); ++i) {
            const Action& a = score.actions()[i];
            if (i) require(a.sample >= score.actions()[i - 1].sample, "unordered action time");
            if (a.command.verb == "show") {
                require(shows < 8 && a.command.target == expected[shows++], "scene order mismatch");
                if (a.command.target == "saari") require(a.sample == score.module_switch_sample(), "module join is rounded");
                if (a.command.target == "feta") show_feta = a.sample;
                if (a.command.target == "uppol") require(a.sample == score.credits_sample(), "credits timing mismatch");
                std::cout << a.command.target << " @ " << a.sample << " samples\n";
            }
            if (a.command.target == "feta" && a.command.verb == "msg" && a.command.argument == "1") prep_feta = a.sample;
        }
        require(shows == 8 && prep_feta > 0 && prep_feta < show_feta, "Feta preparation must precede visibility");
        DemoRuntime regular(score), irregular(score);
        require(regular.reset(&error), error); require(irregular.reset(&error), error);
        const std::uint64_t endpoint = kSampleRate * 3;
        for (std::uint64_t sample = 0; sample <= endpoint; sample += kTickSamples)
            regular.advance_to_sample(sample);
        while (!irregular.caught_up(endpoint)) irregular.advance_to_sample(endpoint, 7);
        require(regular.framebuffer().pixels() == irregular.framebuffer().pixels(), "catch-up changes feedback state");
        const std::vector<std::uint32_t> saved = regular.framebuffer().pixels();
        require(regular.advance_to_sample(endpoint) == 0, "presenting a duplicate frame advances state");
        require(regular.reset(&error), error);
        while (!regular.caught_up(endpoint)) regular.advance_to_sample(endpoint, 11);
        require(saved == regular.framebuffer().pixels(), "restart changes deterministic state");
        if (argc > 1) {
            // Optional whole-score visual run, writing compact scene checkpoints and timing evidence.
            const std::string output(argv[1]);
            require(write_tga24(output + "/intro-3s.tga", regular.framebuffer(), &error), error);
            require(regular.reset(&error), error);
            std::ofstream trace((output + "/trace.csv").c_str());
            require(static_cast<bool>(trace), "create the capture output directory first");
            trace << "sample,scene,tick_ms\n";
            std::string previous;
            double maximum_ms = 0;
            for (std::uint64_t sample = 0; sample < score.total_samples(); sample += kTickSamples) {
                const auto begin = std::chrono::steady_clock::now();
                regular.advance_to_sample(sample, 1);
                const double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - begin).count();
                maximum_ms = std::max(maximum_ms, ms);
                trace << sample << ',' << regular.scene_name() << ',' << ms << '\n';
                if (regular.scene_name() != previous || sample % (kSampleRate * 10) == 0) {
                    require(write_tga24(output + "/" + std::to_string(sample) + "-" + regular.scene_name() + ".tga",
                                        regular.framebuffer(), &error), error);
                    previous = regular.scene_name();
                }
            }
            require(regular.scene_name() == "uppol", "full run does not reach credits");
            std::cout << "Full runtime checked; maximum tick " << maximum_ms << " ms\n";
        }
        std::cout << "PASS: score, sample continuity, catch-up, duplicate presentation and restart\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
