#include "scenes/uppol_routine.h"

#include <algorithm>
#include <cstring>
#include <vector>

#include "scenes/uppol_text_data.h"

namespace forward_offline {

namespace {

enum class UppolAlignment {
    center,
    left,
    right,
    link
};

struct UppolCreditLine {
    UppolAlignment alignment;
    const char* text;
    const UppolTextBitmapData* bitmap;
};

const char* const kUppolRawLines[] = {
    "",
    "forward",
    "komplex",
    "",
    "",
    "",
    "",
    "",
    "code",
    "",
    "saviour",
    "jmagic",
    "anis",
    "",
    "",
    "graphics",
    "",
    "jugi",
    "",
    "",
    "intro theme",
    "",
    "jugi",
    "",
    "",
    "main theme",
    "",
    "carebear/orange",
    "",
    "",
    "klunssi object",
    "",
    "reward",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "rebellion will not be televised",
    "",
    "",
    "",
    "__mailto:komplex@jyu.fi",
    "__http://www.jyu.fi/komplex",
    "",
    ""
};

const int kUppolWidth = 512;
const int kUppolHeight = 256;
const int kLineHeight = 26;
const int kRightMarginInset = 150;
const float kTextScrollSpeed = 25.0f;
const std::uint32_t kWhite = 0xFFFFFFU;

std::size_t uppol_line_count() {
    return sizeof(kUppolRawLines) / sizeof(kUppolRawLines[0]);
}

const UppolTextBitmapData* find_text_bitmap(const char* text) {
    if (text == NULL || text[0] == '\0') {
        return NULL;
    }

    for (std::size_t index = 0; index < kUppolTextBitmapCount; ++index) {
        if (std::strcmp(kUppolTextBitmaps[index].text, text) == 0) {
            return &kUppolTextBitmaps[index];
        }
    }

    return NULL;
}

UppolCreditLine resolve_credit_line(int index) {
    UppolCreditLine line;
    line.alignment = UppolAlignment::center;
    line.text = "";
    line.bitmap = NULL;

    if (index < 0 || index >= static_cast<int>(uppol_line_count())) {
        return line;
    }

    const char* text = kUppolRawLines[static_cast<std::size_t>(index)];
    if (text == NULL) {
        return line;
    }

    if (std::strncmp(text, "l_", 2) == 0) {
        line.alignment = UppolAlignment::left;
        text += 2;
    }
    if (std::strncmp(text, "r_", 2) == 0) {
        line.alignment = UppolAlignment::right;
        text += 2;
    }
    if (std::strncmp(text, "__", 2) == 0) {
        line.alignment = UppolAlignment::link;
        text += 2;
    }

    line.text = text;
    line.bitmap = find_text_bitmap(text);
    return line;
}

void draw_horizontal_line(RgbSurface& surface,
                          int start_x,
                          int end_x,
                          int y,
                          std::uint32_t color) {
    if (y < 0 || y >= surface.height()) {
        return;
    }

    if (start_x > end_x) {
        const int swap = start_x;
        start_x = end_x;
        end_x = swap;
    }

    if (end_x < 0 || start_x >= surface.width()) {
        return;
    }

    if (start_x < 0) {
        start_x = 0;
    }
    if (end_x >= surface.width()) {
        end_x = surface.width() - 1;
    }

    std::vector<std::uint32_t>& pixels = surface.pixels();
    const std::size_t row_offset = static_cast<std::size_t>(y) * static_cast<std::size_t>(surface.width());
    for (int x = start_x; x <= end_x; ++x) {
        pixels[row_offset + static_cast<std::size_t>(x)] = color;
    }
}

void draw_text_bitmap(RgbSurface& surface,
                      const UppolTextBitmapData& bitmap,
                      int pen_x,
                      int baseline_y,
                      std::uint32_t color) {
    if (bitmap.bitmap == NULL || bitmap.bitmap_width <= 0 || bitmap.bitmap_height <= 0) {
        return;
    }

    const int target_x = pen_x + bitmap.anchor_dx;
    const int target_y = baseline_y + bitmap.anchor_dy;
    const int row_stride = (bitmap.bitmap_width + 7) / 8;
    std::vector<std::uint32_t>& pixels = surface.pixels();

    for (int y = 0; y < bitmap.bitmap_height; ++y) {
        const int dst_y = target_y + y;
        if (dst_y < 0 || dst_y >= surface.height()) {
            continue;
        }

        const std::size_t src_row_offset = static_cast<std::size_t>(y) * static_cast<std::size_t>(row_stride);
        const std::size_t dst_row_offset =
            static_cast<std::size_t>(dst_y) * static_cast<std::size_t>(surface.width());

        for (int x = 0; x < bitmap.bitmap_width; ++x) {
            const int dst_x = target_x + x;
            if (dst_x < 0 || dst_x >= surface.width()) {
                continue;
            }

            const std::size_t byte_index =
                src_row_offset + static_cast<std::size_t>(x >> 3);
            const std::uint8_t bit_mask =
                static_cast<std::uint8_t>(0x80U >> (x & 7));
            if ((bitmap.bitmap[byte_index] & bit_mask) == 0U) {
                continue;
            }

            pixels[dst_row_offset + static_cast<std::size_t>(dst_x)] = color;
        }
    }
}

}  // namespace

UppolRoutine::UppolRoutine()
    : source_asset_(),
      frame_(kUppolWidth, kUppolHeight),
      frame_counter_(0),
      ready_(false),
      error_message_() {
}

const char* UppolRoutine::script_name() const {
    return "uppol";
}

void UppolRoutine::init() {
    frame_counter_ = 0;
    ready_ = load_assets();
}

void UppolRoutine::on_show() {
    frame_counter_ = 0;
}

void UppolRoutine::render(RgbSurface& surface, float scene_time_seconds, float delta_seconds) {
    (void)delta_seconds;
    surface.clear(0);
    if (!ready_) {
        return;
    }

    populate_frame(frame_counter_);
    frame_.render_to_rgb(surface);
    render_credits(surface, scene_time_seconds);
    ++frame_counter_;
}

bool UppolRoutine::is_ready() const {
    return ready_;
}

const std::string& UppolRoutine::error_message() const {
    return error_message_;
}

bool UppolRoutine::load_assets() {
    error_message_.clear();
    if (!load_original_gif_indexed(gif_asset_path("phorward.gif"), &source_asset_, &error_message_)) {
        return false;
    }

    if (source_asset_.width != kUppolWidth || source_asset_.height < kUppolHeight) {
        error_message_ = "unexpected uppol gif dimensions: " + gif_asset_path("phorward.gif");
        return false;
    }

    for (int index = 0; index < 256; ++index) {
        frame_.set_palette_entry(index,
                                 0U,
                                 0U,
                                 source_asset_.palette_blue[static_cast<std::size_t>(index)]);
    }

    return validate_credit_bitmaps();
}

bool UppolRoutine::validate_credit_bitmaps() {
    for (std::size_t index = 0; index < uppol_line_count(); ++index) {
        const UppolCreditLine line =
            resolve_credit_line(static_cast<int>(index));
        if (line.text[0] != '\0' && line.bitmap == NULL) {
            error_message_ = std::string("missing uppol text bitmap for line: ") + line.text;
            return false;
        }
    }

    return true;
}

void UppolRoutine::populate_frame(int frame_index) {
    std::vector<std::uint8_t>& frame_pixels = frame_.pixels();
    const std::vector<std::uint8_t>& source_pixels = source_asset_.pixels;
    const int source_width = source_asset_.width;
    const int source_height = source_asset_.height;

    const int source_y = (frame_index * kUppolHeight) % source_height;
    for (int row = 0; row < kUppolHeight; ++row) {
        int wrapped_row = source_y + row;
        if (wrapped_row >= source_height) {
            wrapped_row -= source_height;
        }

        const std::size_t src_offset =
            static_cast<std::size_t>(wrapped_row) * static_cast<std::size_t>(source_width);
        const std::size_t dst_offset =
            static_cast<std::size_t>(row) * static_cast<std::size_t>(kUppolWidth);
        std::memcpy(&frame_pixels[dst_offset],
                    &source_pixels[src_offset],
                    static_cast<std::size_t>(kUppolWidth));
    }
}

void UppolRoutine::render_credits(RgbSurface& surface, float scene_time_seconds) const {
    const int left_margin = 0;
    const int right_margin = kUppolWidth - kRightMarginInset;
    const int center_x = (left_margin + right_margin) / 2;
    const int visible_line_count = kUppolHeight / kLineHeight + 2;
    const int line_count = static_cast<int>(uppol_line_count());

    const double scroll_value = static_cast<double>(scene_time_seconds) * static_cast<double>(kTextScrollSpeed);
    int scroll_offset = static_cast<int>(scroll_value) - (kUppolHeight + kLineHeight);
    if (scroll_offset / kLineHeight + visible_line_count >= line_count) {
        scroll_offset = (line_count - visible_line_count) * kLineHeight;
    }

    int line_y = kLineHeight - scroll_offset % kLineHeight;
    int line_index = scroll_offset / kLineHeight;

    for (int visible_index = 0; visible_index < visible_line_count; ++visible_index) {
        const UppolCreditLine line = resolve_credit_line(line_index + visible_index);
        const int advance_width = line.bitmap != NULL ? line.bitmap->advance_width : 0;
        int pen_x = center_x - (advance_width >> 1);

        if (line.alignment == UppolAlignment::left) {
            pen_x = left_margin;
        } else if (line.alignment == UppolAlignment::right) {
            pen_x = right_margin - advance_width;
        }

        if (line.alignment == UppolAlignment::link) {
            draw_horizontal_line(surface,
                                 center_x - (advance_width >> 1),
                                 center_x + (advance_width >> 1),
                                 line_y - 4,
                                 kWhite);
        }

        if (line.bitmap != NULL) {
            draw_text_bitmap(surface, *line.bitmap, pen_x, line_y - 5, kWhite);
        }

        line_y += kLineHeight;
    }
}

std::string UppolRoutine::gif_asset_path(const std::string& file_name) const {
    return std::string("original/forward/images/") + file_name;
}

}  // namespace forward_offline
