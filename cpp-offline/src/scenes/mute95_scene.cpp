#include "scenes/mute95_scene.h"

#include <algorithm>
#include <cctype>
#include <cmath>

namespace forward_offline {

namespace {

struct Glyph {
    char character;
    const char* rows[7];
};

const Glyph kGlyphs[] = {
    {'A', {"01110", "10001", "10001", "11111", "10001", "10001", "10001"}},
    {'B', {"11110", "10001", "10001", "11110", "10001", "10001", "11110"}},
    {'C', {"01110", "10001", "10000", "10000", "10000", "10001", "01110"}},
    {'D', {"11110", "10001", "10001", "10001", "10001", "10001", "11110"}},
    {'E', {"11111", "10000", "10000", "11110", "10000", "10000", "11111"}},
    {'F', {"11111", "10000", "10000", "11110", "10000", "10000", "10000"}},
    {'G', {"01110", "10001", "10000", "10111", "10001", "10001", "01110"}},
    {'I', {"11111", "00100", "00100", "00100", "00100", "00100", "11111"}},
    {'J', {"00111", "00010", "00010", "00010", "10010", "10010", "01100"}},
    {'K', {"10001", "10010", "10100", "11000", "10100", "10010", "10001"}},
    {'M', {"10001", "11011", "10101", "10101", "10001", "10001", "10001"}},
    {'N', {"10001", "11001", "10101", "10011", "10001", "10001", "10001"}},
    {'O', {"01110", "10001", "10001", "10001", "10001", "10001", "01110"}},
    {'R', {"11110", "10001", "10001", "11110", "10100", "10010", "10001"}},
    {'S', {"01111", "10000", "10000", "01110", "00001", "00001", "11110"}},
    {'T', {"11111", "00100", "00100", "00100", "00100", "00100", "00100"}},
    {'U', {"10001", "10001", "10001", "10001", "10001", "10001", "01110"}},
    {'V', {"10001", "10001", "10001", "10001", "10001", "01010", "00100"}},
    {'Y', {"10001", "10001", "01010", "00100", "00100", "00100", "00100"}}
};

const Glyph* find_glyph(char character) {
    for (std::size_t index = 0; index < sizeof(kGlyphs) / sizeof(kGlyphs[0]); ++index) {
        if (kGlyphs[index].character == character) {
            return &kGlyphs[index];
        }
    }
    return NULL;
}

}  // namespace

Mute95Scene::CreditPair::CreditPair(const std::string& label_value,
                                    std::uint32_t primary_a,
                                    std::uint32_t secondary_a,
                                    std::uint32_t primary_b,
                                    std::uint32_t secondary_b)
    : label(label_value),
      card_a(256, 50),
      card_b(256, 50) {
    draw_card(card_a, label, primary_a, secondary_a);
    draw_card(card_b, label, primary_b, secondary_b);
}

Mute95Scene::Mute95Scene()
    : background_(512, 256),
      credits_(),
      active_credit_(-1),
      message_start_seconds_(-1.0f),
      phase_ticks_(0.0f) {
}

const char* Mute95Scene::script_name() const {
    return "mute95";
}

void Mute95Scene::init() {
    credits_.clear();
    credits_.push_back(CreditPair("SAVIOUR",
                                  pack_rgb(224, 124, 72),
                                  pack_rgb(54, 18, 12),
                                  pack_rgb(255, 202, 124),
                                  pack_rgb(76, 22, 12)));
    credits_.push_back(CreditPair("JMAGIC",
                                  pack_rgb(82, 182, 230),
                                  pack_rgb(12, 22, 42),
                                  pack_rgb(156, 228, 255),
                                  pack_rgb(22, 38, 58)));
    credits_.push_back(CreditPair("JUGI",
                                  pack_rgb(80, 220, 162),
                                  pack_rgb(12, 44, 28),
                                  pack_rgb(164, 255, 212),
                                  pack_rgb(20, 58, 36)));
    credits_.push_back(CreditPair("ANIS",
                                  pack_rgb(210, 92, 196),
                                  pack_rgb(54, 12, 48),
                                  pack_rgb(255, 168, 238),
                                  pack_rgb(72, 18, 62)));
    credits_.push_back(CreditPair("CAREBEAR",
                                  pack_rgb(236, 176, 70),
                                  pack_rgb(56, 34, 10),
                                  pack_rgb(255, 230, 148),
                                  pack_rgb(82, 52, 16)));
    build_palette();
    active_credit_ = -1;
    message_start_seconds_ = -1.0f;
    phase_ticks_ = 0.0f;
}

void Mute95Scene::on_show() {
    active_credit_ = -1;
    message_start_seconds_ = -1.0f;
    phase_ticks_ = 0.0f;
}

void Mute95Scene::render(RgbSurface& surface, float scene_time_seconds, float delta_seconds) {
    phase_ticks_ += delta_seconds * 50.0f;
    build_background(scene_time_seconds);
    background_.render_to_rgb(surface);
    render_credit_overlay(surface, scene_time_seconds);
}

void Mute95Scene::handle_message(const std::string& message, float scene_time_seconds) {
    select_credit(message, scene_time_seconds);
}

std::uint32_t Mute95Scene::pack_rgb(int red, int green, int blue) {
    const int r = std::max(0, std::min(255, red));
    const int g = std::max(0, std::min(255, green));
    const int b = std::max(0, std::min(255, blue));
    return static_cast<std::uint32_t>((r << 16) | (g << 8) | b);
}

float Mute95Scene::clamp_unit(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }
    return value;
}

void Mute95Scene::blend_card(const RgbSurface& source,
                             RgbSurface& destination,
                             int dst_x,
                             int dst_y,
                             float amount) {
    if (amount <= 0.0f) {
        return;
    }

    const std::vector<std::uint32_t>& src_pixels = source.pixels();
    std::vector<std::uint32_t>& dst_pixels = destination.pixels();
    const int dst_width = destination.width();
    const int dst_height = destination.height();

    for (int y = 0; y < source.height(); ++y) {
        const int out_y = dst_y + y;
        if (out_y < 0 || out_y >= dst_height) {
            continue;
        }
        for (int x = 0; x < source.width(); ++x) {
            const int out_x = dst_x + x;
            if (out_x < 0 || out_x >= dst_width) {
                continue;
            }

            const std::uint32_t src = src_pixels[static_cast<std::size_t>(y) *
                                                 static_cast<std::size_t>(source.width()) +
                                                 static_cast<std::size_t>(x)];
            const std::uint32_t dst = dst_pixels[static_cast<std::size_t>(out_y) *
                                                 static_cast<std::size_t>(dst_width) +
                                                 static_cast<std::size_t>(out_x)];

            int red = static_cast<int>((dst >> 16) & 0xff) +
                      static_cast<int>(static_cast<float>((src >> 16) & 0xff) * amount);
            int green = static_cast<int>((dst >> 8) & 0xff) +
                        static_cast<int>(static_cast<float>((src >> 8) & 0xff) * amount);
            int blue = static_cast<int>(dst & 0xff) +
                       static_cast<int>(static_cast<float>(src & 0xff) * amount);

            dst_pixels[static_cast<std::size_t>(out_y) * static_cast<std::size_t>(dst_width) +
                       static_cast<std::size_t>(out_x)] = pack_rgb(red, green, blue);
        }
    }
}

void Mute95Scene::draw_card(RgbSurface& surface,
                            const std::string& label,
                            std::uint32_t primary,
                            std::uint32_t secondary) {
    const int width = surface.width();
    const int height = surface.height();
    std::vector<std::uint32_t>& pixels = surface.pixels();

    for (int y = 0; y < height; ++y) {
        const float fy = static_cast<float>(y) / static_cast<float>(height > 1 ? height - 1 : 1);
        for (int x = 0; x < width; ++x) {
            const float fx = static_cast<float>(x) / static_cast<float>(width > 1 ? width - 1 : 1);
            const int base_red = static_cast<int>(((primary >> 16) & 0xff) * (0.4f + 0.6f * fx));
            const int base_green = static_cast<int>(((primary >> 8) & 0xff) * (0.45f + 0.55f * fy));
            const int base_blue = static_cast<int>((primary & 0xff) * (0.35f + 0.65f * fx));
            const int shade = ((x / 8) ^ (y / 8)) & 1;
            pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(width) +
                   static_cast<std::size_t>(x)] = pack_rgb(base_red + shade * 12,
                                                           base_green + shade * 10,
                                                           base_blue + shade * 8);
        }
    }

    surface.fill_rect(0, 0, width, 4, secondary);
    surface.fill_rect(0, height - 4, width, 4, secondary);
    surface.fill_rect(0, 0, 4, height, secondary);
    surface.fill_rect(width - 4, 0, 4, height, secondary);
    surface.fill_rect(10, 10, width - 20, height - 20, pack_rgb(8, 8, 8));
    draw_text(surface, 20, 16, "FORWARD", secondary);
    draw_text(surface, 20, 30, label, primary);
}

void Mute95Scene::draw_text(RgbSurface& surface,
                            int x,
                            int y,
                            const std::string& text,
                            std::uint32_t color) {
    int cursor_x = x;
    for (std::size_t index = 0; index < text.size(); ++index) {
        const char character = text[index];
        if (character == ' ') {
            cursor_x += 6;
            continue;
        }

        const Glyph* glyph = find_glyph(character);
        if (glyph == NULL) {
            cursor_x += 6;
            continue;
        }

        for (int row = 0; row < 7; ++row) {
            for (int column = 0; column < 5; ++column) {
                if (glyph->rows[row][column] == '1') {
                    surface.fill_rect(cursor_x + column * 2,
                                      y + row * 2,
                                      2,
                                      2,
                                      color);
                }
            }
        }
        cursor_x += 12;
    }
}

void Mute95Scene::build_palette() {
    for (int index = 0; index < 256; ++index) {
        const float t = static_cast<float>(index) / 255.0f;
        const int red = static_cast<int>(16.0f + 220.0f * std::pow(t, 1.3f));
        const int green = static_cast<int>(8.0f + 180.0f * t);
        const int blue = static_cast<int>(32.0f + 100.0f * (1.0f - t));
        background_.set_palette_entry(index,
                                      static_cast<std::uint8_t>(red),
                                      static_cast<std::uint8_t>(green),
                                      static_cast<std::uint8_t>(blue));
    }
}

void Mute95Scene::build_background(float scene_time_seconds) {
    for (int y = 0; y < background_.height(); ++y) {
        for (int x = 0; x < background_.width(); ++x) {
            const float fx = static_cast<float>(x) * 0.023f;
            const float fy = static_cast<float>(y) * 0.037f;
            const float wave = std::sin(fx + scene_time_seconds * 1.2f) +
                               std::sin(fy - scene_time_seconds * 0.9f) +
                               std::sin((fx + fy) * 0.6f + scene_time_seconds * 0.7f);
            const float radial = std::sqrt((static_cast<float>(x - 256) * static_cast<float>(x - 256)) +
                                           (static_cast<float>(y - 128) * static_cast<float>(y - 128)));
            const float glow = 1.0f - std::min(1.0f, radial / 220.0f);
            const int value = std::max(0, std::min(255,
                                                   120 + static_cast<int>(wave * 28.0f) +
                                                       static_cast<int>(glow * 90.0f)));
            background_.set_pixel(x, y, static_cast<std::uint8_t>(value));
        }
    }

    const int noise_writes = static_cast<int>(220.0f);
    for (int index = 0; index < noise_writes; ++index) {
        const int x = static_cast<int>(std::fmod(phase_ticks_ * 19.0f + static_cast<float>(index * 17), 512.0f));
        const int y = static_cast<int>(std::fmod(phase_ticks_ * 11.0f + static_cast<float>(index * 7), 256.0f));
        const std::uint8_t current = background_.pixel_at(x, y);
        const int next = std::min(255, static_cast<int>(current) + 70);
        background_.set_pixel(x, y, static_cast<std::uint8_t>(next));
    }
}

void Mute95Scene::render_credit_overlay(RgbSurface& surface, float scene_time_seconds) {
    if (active_credit_ < 0 || active_credit_ >= static_cast<int>(credits_.size()) ||
        message_start_seconds_ < 0.0f) {
        return;
    }

    const float elapsed = scene_time_seconds - message_start_seconds_;
    if (elapsed < 0.0f || elapsed > 9.0f) {
        return;
    }

    float first_amount = 0.0f;
    float second_amount = 0.0f;
    if (elapsed < 1.5f) {
        first_amount = elapsed / 1.5f;
    } else if (elapsed < 4.0f) {
        first_amount = 1.0f;
        second_amount = (elapsed - 1.5f) / 2.5f;
    } else if (elapsed < 6.0f) {
        first_amount = 1.0f - (elapsed - 4.0f) / 2.0f;
        second_amount = 1.0f;
    } else {
        second_amount = 1.0f - (elapsed - 6.0f) / 3.0f;
    }

    const CreditPair& credit = credits_[static_cast<std::size_t>(active_credit_)];
    blend_card(credit.card_a, surface, 128, 103, clamp_unit(first_amount));
    blend_card(credit.card_b, surface, 128, 103, clamp_unit(second_amount));
}

void Mute95Scene::select_credit(const std::string& message, float scene_time_seconds) {
    for (std::size_t index = 0; index < credits_.size(); ++index) {
        std::string lowered = credits_[index].label;
        for (std::size_t char_index = 0; char_index < lowered.size(); ++char_index) {
            lowered[char_index] = static_cast<char>(std::tolower(static_cast<unsigned char>(lowered[char_index])));
        }
        if (lowered == message) {
            active_credit_ = static_cast<int>(index);
            message_start_seconds_ = scene_time_seconds;
            return;
        }
    }
}

}  // namespace forward_offline
