import java.awt.image.BufferedImage;
import java.io.BufferedWriter;
import java.io.IOException;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.security.CodeSource;
import java.util.Locale;
import javax.imageio.ImageIO;

public final class ForwardPaletteProbe {
    private static final String OUTPUT_DIR_PROPERTY = "forward.probe.outputDir";
    private static final String IMAGE_PATH_PROPERTY = "forward.probe.imagePath";
    private static final String LABEL_PROPERTY = "forward.probe.label";

    private ForwardPaletteProbe() {
    }

    public static void main(String[] args) throws Exception {
        Config config = Config.from(args);
        Files.createDirectories(config.outputDir);

        URL imageUrl = Paths.get(config.imagePath).toAbsolutePath().normalize().toUri().toURL();
        mmajkka loaded = mmaakma.majaKkA(imageUrl);
        if (!(loaded instanceof kmajkka)) {
            throw new IllegalStateException("Expected indexed image for " + config.imagePath + ", got " + loaded);
        }

        kmajkka indexed = (kmajkka)loaded;
        writeSummary(config, indexed);
        writePalette(config.outputDir, indexed);
        writeHistogram(config.outputDir, indexed);
        writePaletteBinary(config.outputDir, indexed);
        writeIndexBinary(config.outputDir, indexed);
        writePreview(config.outputDir, indexed);

        System.out.println("ForwardPaletteProbe wrote probe files to " + config.outputDir.toAbsolutePath());
    }

    private static void writeSummary(Config config, kmajkka indexed) throws IOException {
        Path output = config.outputDir.resolve("summary.txt");
        try (BufferedWriter writer = Files.newBufferedWriter(output)) {
            writer.write("probe_label=" + config.label);
            writer.newLine();
            writer.write("image_path=" + config.imagePath);
            writer.newLine();
            writer.write("image_width=" + indexed.amAjakK);
            writer.newLine();
            writer.write("image_height=" + indexed.AMAjakK);
            writer.newLine();
            writer.write("palette_size=256");
            writer.newLine();
            writer.write("palette_class_location=" + classLocation(kmajkka.class));
            writer.newLine();
            writer.write("loader_class_location=" + classLocation(mmaakma.class));
            writer.newLine();
        }
    }

    private static void writePalette(Path outputDir, kmajkka indexed) throws IOException {
        Path output = outputDir.resolve("runtime_palette.csv");
        try (BufferedWriter writer = Files.newBufferedWriter(output)) {
            writer.write("index,red,green,blue,packed_hex");
            writer.newLine();
            int index = 0;
            while (index < 256) {
                int red = indexed.aMajakK[index] & 0xFF;
                int green = indexed.AmAJakK[index] & 0xFF;
                int blue = indexed.amAJakK[index] & 0xFF;
                writer.write(Integer.toString(index));
                writer.write(",");
                writer.write(Integer.toString(red));
                writer.write(",");
                writer.write(Integer.toString(green));
                writer.write(",");
                writer.write(Integer.toString(blue));
                writer.write(",");
                writer.write(String.format(Locale.ROOT, "0x%08X", indexed.AMAJakK[index]));
                writer.newLine();
                ++index;
            }
        }
    }

    private static void writeHistogram(Path outputDir, kmajkka indexed) throws IOException {
        int[] histogram = new int[256];
        byte[] pixels = indexed.aMAjakK;
        int index = 0;
        while (index < pixels.length) {
            ++histogram[pixels[index] & 0xFF];
            ++index;
        }

        Path output = outputDir.resolve("runtime_histogram.csv");
        try (BufferedWriter writer = Files.newBufferedWriter(output)) {
            writer.write("index,count");
            writer.newLine();
            index = 0;
            while (index < histogram.length) {
                writer.write(Integer.toString(index));
                writer.write(",");
                writer.write(Integer.toString(histogram[index]));
                writer.newLine();
                ++index;
            }
        }
    }

    private static void writePaletteBinary(Path outputDir, kmajkka indexed) throws IOException {
        byte[] palette = new byte[256 * 3];
        int index = 0;
        while (index < 256) {
            int base = index * 3;
            palette[base] = indexed.aMajakK[index];
            palette[base + 1] = indexed.AmAJakK[index];
            palette[base + 2] = indexed.amAJakK[index];
            ++index;
        }
        Files.write(outputDir.resolve("runtime_palette.rgb"), palette);
    }

    private static void writeIndexBinary(Path outputDir, kmajkka indexed) throws IOException {
        Files.write(outputDir.resolve("runtime_indices.bin"), indexed.aMAjakK);
    }

    private static void writePreview(Path outputDir, kmajkka indexed) throws IOException {
        BufferedImage image = new BufferedImage(indexed.amAjakK, indexed.AMAjakK, BufferedImage.TYPE_INT_ARGB);
        byte[] pixels = indexed.aMAjakK;
        int offset = 0;
        int y = 0;
        while (y < indexed.AMAjakK) {
            int x = 0;
            while (x < indexed.amAjakK) {
                int paletteIndex = pixels[offset++] & 0xFF;
                int red = indexed.aMajakK[paletteIndex] & 0xFF;
                int green = indexed.AmAJakK[paletteIndex] & 0xFF;
                int blue = indexed.amAJakK[paletteIndex] & 0xFF;
                image.setRGB(x, y, 0xFF000000 | red << 16 | green << 8 | blue);
                ++x;
            }
            ++y;
        }
        ImageIO.write(image, "png", outputDir.resolve("runtime_preview.png").toFile());
    }

    private static String classLocation(Class<?> type) {
        try {
            CodeSource codeSource = type.getProtectionDomain().getCodeSource();
            URL location = codeSource != null ? codeSource.getLocation() : null;
            return location != null ? location.toString() : "unknown";
        }
        catch (SecurityException exception) {
            return "unknown";
        }
    }

    private static final class Config {
        final Path outputDir;
        final String imagePath;
        final String label;

        Config(Path outputDir, String imagePath, String label) {
            this.outputDir = outputDir;
            this.imagePath = imagePath;
            this.label = label;
        }

        static Config from(String[] args) {
            String outputDirValue = System.getProperty(OUTPUT_DIR_PROPERTY, "documentation/reference-capture/palette-probe/default");
            String imagePathValue = System.getProperty(IMAGE_PATH_PROPERTY, "images/kosmos/krad3.gif");
            String labelValue = System.getProperty(LABEL_PROPERTY, "default");

            if (args.length % 2 != 0) {
                throw new IllegalArgumentException("Arguments must be provided as key/value pairs");
            }

            int index = 0;
            while (index < args.length) {
                String key = normalizeKey(args[index]);
                String value = args[index + 1];
                if ("outputdir".equals(key)) {
                    outputDirValue = value;
                } else if ("imagepath".equals(key)) {
                    imagePathValue = value;
                } else if ("label".equals(key)) {
                    labelValue = value;
                } else {
                    throw new IllegalArgumentException("Unknown probe argument: " + args[index]);
                }
                index += 2;
            }

            Path outputDir = Paths.get(outputDirValue).toAbsolutePath().normalize();
            return new Config(outputDir, imagePathValue, labelValue);
        }

        private static String normalizeKey(String key) {
            String normalized = key == null ? "" : key.trim();
            while (normalized.startsWith("-")) {
                normalized = normalized.substring(1);
            }
            return normalized.toLowerCase(Locale.ROOT);
        }
    }
}
