import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

// Regenerates cpp-offline/src/scenes/uppol_text_data.h from the same
// Java-side font selection and rasterization path used by UppolRoutine.
public final class GenerateUppolTextData {
    private static final String[] RAW_LINES = new String[]{
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
        "mailto:komplex@jyu.fi",
        "http://www.jyu.fi/komplex",
        "",
        ""
    };

    private GenerateUppolTextData() {
    }

    public static void main(String[] args) {
        Font font = ForwardFontSupport.monospaceBold(16);
        BufferedImage probeImage = new BufferedImage(1, 1, BufferedImage.TYPE_INT_ARGB);
        Graphics2D probeGraphics = probeImage.createGraphics();
        ForwardFontSupport.prepare(probeGraphics, font);
        FontMetrics metrics = probeGraphics.getFontMetrics(font);

        Set<String> uniqueLines = new LinkedHashSet<String>();
        for (String line : RAW_LINES) {
            if (line.length() > 0) {
                uniqueLines.add(line);
            }
        }

        List<String> outputs = new ArrayList<String>();
        outputs.add("#ifndef FORWARD_OFFLINE_SCENES_UPPOL_TEXT_DATA_H");
        outputs.add("#define FORWARD_OFFLINE_SCENES_UPPOL_TEXT_DATA_H");
        outputs.add("");
        outputs.add("#include <cstddef>");
        outputs.add("#include <cstdint>");
        outputs.add("");
        outputs.add("namespace forward_offline {");
        outputs.add("");
        outputs.add("struct UppolTextBitmapData {");
        outputs.add("    const char* text;");
        outputs.add("    int advance_width;");
        outputs.add("    int bitmap_width;");
        outputs.add("    int bitmap_height;");
        outputs.add("    int anchor_dx;");
        outputs.add("    int anchor_dy;");
        outputs.add("    const std::uint8_t* bitmap;");
        outputs.add("    std::size_t bitmap_size;");
        outputs.add("};");
        outputs.add("");
        outputs.add("// Generated from java-desktop ForwardFontSupport.monospaceBold(16) with");
        outputs.add("// text antialiasing disabled to keep the C++ uppol credits aligned with");
        outputs.add("// the Java desktop preservation baseline.");
        outputs.add("");

        int glyphIndex = 0;
        List<String> records = new ArrayList<String>();
        for (String line : uniqueLines) {
            GlyphData data = buildGlyphData(line, font, metrics);
            String symbol = "kUppolTextBitmap" + glyphIndex;
            outputs.add("static const std::uint8_t " + symbol + "[] = {");
            outputs.add(formatBytes(data.bitmapBytes));
            outputs.add("};");
            outputs.add("");
            records.add("    {\"" + escape(line) + "\", " +
                        data.advanceWidth + ", " +
                        data.bitmapWidth + ", " +
                        data.bitmapHeight + ", " +
                        data.anchorDx + ", " +
                        data.anchorDy + ", " +
                        symbol + ", sizeof(" + symbol + ")}");
            ++glyphIndex;
        }

        outputs.add("static const UppolTextBitmapData kUppolTextBitmaps[] = {");
        for (int index = 0; index < records.size(); ++index) {
            String suffix = index + 1 < records.size() ? "," : "";
            outputs.add(records.get(index) + suffix);
        }
        outputs.add("};");
        outputs.add("");
        outputs.add("static const std::size_t kUppolTextBitmapCount =");
        outputs.add("    sizeof(kUppolTextBitmaps) / sizeof(kUppolTextBitmaps[0]);");
        outputs.add("");
        outputs.add("}  // namespace forward_offline");
        outputs.add("");
        outputs.add("#endif  // FORWARD_OFFLINE_SCENES_UPPOL_TEXT_DATA_H");

        for (String line : outputs) {
            System.out.println(line);
        }
    }

    private static GlyphData buildGlyphData(String line, Font font, FontMetrics metrics) {
        int advanceWidth = metrics.stringWidth(line);
        int ascent = metrics.getAscent();
        int height = metrics.getHeight();
        int canvasWidth = Math.max(advanceWidth + 8, 8);
        int canvasHeight = Math.max(height + 8, 8);
        int originX = 4;
        int baselineY = 4 + ascent;

        BufferedImage image = new BufferedImage(canvasWidth, canvasHeight, BufferedImage.TYPE_INT_ARGB);
        Graphics2D graphics = image.createGraphics();
        ForwardFontSupport.prepare(graphics, font);
        graphics.drawString(line, originX, baselineY);
        graphics.dispose();

        int minX = canvasWidth;
        int minY = canvasHeight;
        int maxX = -1;
        int maxY = -1;
        for (int y = 0; y < canvasHeight; ++y) {
            for (int x = 0; x < canvasWidth; ++x) {
                if ((image.getRGB(x, y) >>> 24) != 0) {
                    if (x < minX) {
                        minX = x;
                    }
                    if (y < minY) {
                        minY = y;
                    }
                    if (x > maxX) {
                        maxX = x;
                    }
                    if (y > maxY) {
                        maxY = y;
                    }
                }
            }
        }

        GlyphData data = new GlyphData();
        data.advanceWidth = advanceWidth;
        if (maxX < minX || maxY < minY) {
            data.bitmapWidth = 0;
            data.bitmapHeight = 0;
            data.anchorDx = 0;
            data.anchorDy = -ascent;
            data.bitmapBytes = new byte[0];
            return data;
        }

        data.bitmapWidth = maxX - minX + 1;
        data.bitmapHeight = maxY - minY + 1;
        data.anchorDx = minX - originX;
        data.anchorDy = minY - baselineY;

        int rowStride = (data.bitmapWidth + 7) / 8;
        data.bitmapBytes = new byte[rowStride * data.bitmapHeight];
        for (int y = 0; y < data.bitmapHeight; ++y) {
            for (int x = 0; x < data.bitmapWidth; ++x) {
                if ((image.getRGB(minX + x, minY + y) >>> 24) != 0) {
                    int byteIndex = y * rowStride + (x >> 3);
                    int bit = 7 - (x & 7);
                    data.bitmapBytes[byteIndex] = (byte)(data.bitmapBytes[byteIndex] | (1 << bit));
                }
            }
        }

        return data;
    }

    private static String formatBytes(byte[] values) {
        if (values.length == 0) {
            return "    0x00";
        }

        StringBuilder builder = new StringBuilder();
        for (int index = 0; index < values.length; ++index) {
            if (index % 12 == 0) {
                if (index != 0) {
                    builder.append('\n');
                }
                builder.append("    ");
            }
            int value = values[index] & 0xFF;
            builder.append(String.format("0x%02X", value));
            if (index + 1 < values.length) {
                builder.append(", ");
            }
        }
        return builder.toString();
    }

    private static String escape(String value) {
        return value.replace("\\", "\\\\").replace("\"", "\\\"");
    }

    private static final class GlyphData {
        int advanceWidth;
        int bitmapWidth;
        int bitmapHeight;
        int anchorDx;
        int anchorDy;
        byte[] bitmapBytes;
    }
}
