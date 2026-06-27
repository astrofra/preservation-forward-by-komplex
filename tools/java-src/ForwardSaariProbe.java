import java.applet.Applet;
import java.applet.AppletStub;
import java.awt.image.BufferedImage;
import java.io.BufferedWriter;
import java.io.IOException;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.security.CodeSource;
import java.util.Arrays;
import java.util.IdentityHashMap;
import java.util.Locale;
import java.util.Vector;
import javax.imageio.ImageIO;

public final class ForwardSaariProbe {
    private static final String OUTPUT_DIR_PROPERTY = "forward.probe.outputDir";
    private static final String SCENE_TIME_MS_PROPERTY = "forward.probe.sceneTimeMs";
    private static final String LABEL_PROPERTY = "forward.probe.label";
    private static final String UV_MODE_PROPERTY = "forward.saariBackdropUvMode";
    private static final long DEFAULT_SCENE_TIME_MS = 144000L;
    private static final int DEFAULT_FRAME_WIDTH = 512;
    private static final int DEFAULT_FRAME_HEIGHT = 256;

    private ForwardSaariProbe() {
    }

    public static void main(String[] args) throws Exception {
        Config config = Config.from(args);
        Files.createDirectories(config.outputDir);

        ProbeForwardHost host = new ProbeForwardHost();
        prepareHost(host);

        forward.kkamAJA = host;
        forward.KAmajAk = DEFAULT_FRAME_WIDTH;
        forward.kAmajAk = DEFAULT_FRAME_HEIGHT;

        maajmka scene = new maajmka();
        scene.MajakkA(host);

        kaaakma backdrop = findBackdrop(scene.kaMAJak);
        applySaariState(scene, config.sceneTimeSeconds);

        ProjectionSnapshot projection = projectBackdrop(scene, backdrop);

        writeSummary(config, scene, backdrop, projection);
        writeCameraState(config.outputDir, scene, projection);
        writeScenegraph(config.outputDir, scene.kaMAJak, backdrop);
        writeBackdropVertices(config.outputDir, backdrop);
        writeBackdropFaces(config.outputDir, backdrop);
        writeProjectedVertices(config.outputDir, backdrop);
        writeVisibleTriangles(config.outputDir, backdrop, projection.visibleTriangles);
        writeBackdropRasterPreview(config.outputDir, host, projection.visibleTriangles);

        System.out.println("ForwardSaariProbe wrote probe files to " + config.outputDir.toAbsolutePath());
    }

    @SuppressWarnings("removal")
    private static void prepareHost(forward host) {
        Object rawHost = host;
        if (!(rawHost instanceof Applet)) {
            return;
        }
        kaajkmk stub = new kaajkmk();
        stub.KKAmajA("base", System.getProperty("user.dir"));
        ((Applet)rawHost).setStub((AppletStub)stub);
    }

    private static kaaakma findBackdrop(kaajkka scenegraph) {
        Vector objects = scenegraph.MaJAKkA;
        int index = 0;
        while (index < objects.size()) {
            Object candidate = objects.elementAt(index);
            if (candidate instanceof kaaakma) {
                return (kaaakma)candidate;
            }
            ++index;
        }
        throw new IllegalStateException("Unable to find kaaakma backdrop in saari scenegraph");
    }

    private static void applySaariState(maajmka scene, float sceneTimeSeconds) {
        scene.kaMAJak.kAMAJaK(sceneTimeSeconds * 1.16f, scene.KAmaJak);
        scene.KAmAJak.JakKaMA.AMaJAKk((float)Math.PI);
        scene.KaMAJak.JakKaMA.aMAjaKk();
        scene.KaMAJak.JakKaMA.amAJAKk(sceneTimeSeconds / 3.0f);
        scene.KaMAJak.JakKaMA.aMaJaKk(sceneTimeSeconds / 3.0f * 2.0f);
        scene.KaMAJak.JakKaMA.AMaJAKk(sceneTimeSeconds);
        if (scene.KAmaJak.JAKkaMa.mAJakKA < 0.3f) {
            scene.KAmaJak.JAKkaMa.mAJakKA = 0.3f;
        }
    }

    private static ProjectionSnapshot projectBackdrop(maajmka scene, kaaakma backdrop) {
        kaajmka frustum = new kaajmka();
        frustum.kamaJak(scene.KAmaJak, null);
        int cullMask = frustum.KAmaJak(backdrop);
        if (cullMask == -1) {
            throw new IllegalStateException("Backdrop is outside the view frustum at " + scene.KAmaJak.JAKkaMa);
        }

        int bufferSize = Math.max(4096, backdrop.JAkkaMA.length * 4);
        kmjamma.majaKKa = new kmaamma[bufferSize];
        kmjamma.MAjaKKa = 0;
        backdrop.kkaMAJa(scene.KAmaJak, cullMask);

        return new ProjectionSnapshot(cullMask, Arrays.copyOf(kmjamma.majaKKa, kmjamma.MAjaKKa));
    }

    private static void writeSummary(Config config, maajmka scene, kaaakma backdrop, ProjectionSnapshot projection) throws IOException {
        Path output = config.outputDir.resolve("summary.txt");
        try (BufferedWriter writer = Files.newBufferedWriter(output)) {
            writer.write("probe_label=" + config.label);
            writer.newLine();
            writer.write("scene_name=saari");
            writer.newLine();
            writer.write("scene_time_ms=" + config.sceneTimeMs);
            writer.newLine();
            writer.write("scene_time_seconds=" + config.sceneTimeSeconds);
            writer.newLine();
            writer.write("user_dir=" + System.getProperty("user.dir"));
            writer.newLine();
            writer.write("forward_class_location=" + classLocation(forward.class));
            writer.newLine();
            writer.write("maajmka_class_location=" + classLocation(maajmka.class));
            writer.newLine();
            writer.write("uv_mode=" + System.getProperty(UV_MODE_PROPERTY, "procedural"));
            writer.newLine();
            writer.write("screen_coordinate_format=fixed_16_16_and_pixels");
            writer.newLine();
            writer.write("scenegraph_object_count=" + scene.kaMAJak.MaJAKkA.size());
            writer.newLine();
            writer.write("backdrop_vertex_count=" + backdrop.jAkkaMA.length);
            writer.newLine();
            writer.write("backdrop_face_count=" + backdrop.JAkkaMA.length);
            writer.newLine();
            writer.write("backdrop_visible_triangle_count=" + projection.visibleTriangles.length);
            writer.newLine();
            writer.write("backdrop_cull_mask=" + projection.cullMask);
            writer.newLine();
        }
    }

    private static void writeCameraState(Path outputDir, maajmka scene, ProjectionSnapshot projection) throws IOException {
        Path output = outputDir.resolve("camera_state.csv");
        try (BufferedWriter writer = Files.newBufferedWriter(output)) {
            writer.write("camera_x,camera_y,camera_z,near_z,far_z,fov_rad,viewport_width,viewport_height,backdrop_cull_mask");
            writer.newLine();
            writer.write(csvFloat(scene.KAmaJak.JAKkaMa.maJakKA));
            writer.write(",");
            writer.write(csvFloat(scene.KAmaJak.JAKkaMa.MAJakKA));
            writer.write(",");
            writer.write(csvFloat(scene.KAmaJak.JAKkaMa.mAJakKA));
            writer.write(",");
            writer.write(csvFloat(scene.KAmaJak.JAkkaMa));
            writer.write(",");
            writer.write(csvFloat(scene.KAmaJak.jAkkaMa));
            writer.write(",");
            writer.write(csvFloat(scene.KAmaJak.jaKkaMa));
            writer.write(",");
            writer.write(Integer.toString(scene.KAmaJak.JAkKAMa));
            writer.write(",");
            writer.write(Integer.toString(scene.KAmaJak.jAkKAMa));
            writer.write(",");
            writer.write(Integer.toString(projection.cullMask));
            writer.newLine();
        }
    }

    private static void writeScenegraph(Path outputDir, kaajkka scenegraph, kaaakma backdrop) throws IOException {
        Path output = outputDir.resolve("scenegraph.csv");
        try (BufferedWriter writer = Files.newBufferedWriter(output)) {
            writer.write("object_index,class_name,object_name,vertex_count,triangle_count,has_reflection_clone,is_backdrop");
            writer.newLine();
            Vector objects = scenegraph.MaJAKkA;
            int index = 0;
            while (index < objects.size()) {
                mmajmmk object = (mmajmmk)objects.elementAt(index);
                writer.write(Integer.toString(index));
                writer.write(",");
                writer.write(csvString(object.getClass().getName()));
                writer.write(",");
                writer.write(csvString(object.kkAMaja()));
                writer.write(",");
                writer.write(Integer.toString(object.jAkkaMA != null ? object.jAkkaMA.length : 0));
                writer.write(",");
                writer.write(Integer.toString(object.JAkkaMA != null ? object.JAkkaMA.length : 0));
                writer.write(",");
                writer.write(object.JaKkAma ? "1" : "0");
                writer.write(",");
                writer.write(object == backdrop ? "1" : "0");
                writer.newLine();
                ++index;
            }
        }
    }

    private static void writeBackdropVertices(Path outputDir, kaaakma backdrop) throws IOException {
        Path output = outputDir.resolve("backdrop_vertices.csv");
        try (BufferedWriter writer = Files.newBufferedWriter(output)) {
            writer.write("vertex_index,object_x,object_y,object_z,uv_u,uv_v");
            writer.newLine();
            int index = 0;
            while (index < backdrop.jAkkaMA.length) {
                mmjakka vertex = backdrop.jAkkaMA[index];
                kmajkmk uv = backdrop.JaKKaMA != null && index < backdrop.JaKKaMA.length ? backdrop.JaKKaMA[index] : null;
                writer.write(Integer.toString(index));
                writer.write(",");
                writer.write(csvFloat(vertex.maJakKA));
                writer.write(",");
                writer.write(csvFloat(vertex.MAJakKA));
                writer.write(",");
                writer.write(csvFloat(vertex.mAJakKA));
                writer.write(",");
                writer.write(csvFloat(uv != null ? uv.akKAMAJ : Float.NaN));
                writer.write(",");
                writer.write(csvFloat(uv != null ? uv.AKKAMAJ : Float.NaN));
                writer.newLine();
                ++index;
            }
        }
    }

    private static void writeBackdropFaces(Path outputDir, kaaakma backdrop) throws IOException {
        IdentityHashMap<mmjakka, Integer> vertexIndices = buildVertexIndex(backdrop.jAkkaMA);
        IdentityHashMap<kmajkmk, Integer> uvIndices = buildUvIndex(backdrop.JaKKaMA);

        Path output = outputDir.resolve("backdrop_faces.csv");
        try (BufferedWriter writer = Files.newBufferedWriter(output)) {
            writer.write("face_index,material_id,vertex0_index,vertex1_index,vertex2_index,uv0_index,uv1_index,uv2_index");
            writer.newLine();
            int index = 0;
            while (index < backdrop.JAkkaMA.length) {
                kmaamma face = backdrop.JAkkaMA[index];
                writer.write(Integer.toString(index));
                writer.write(",");
                writer.write(Integer.toString(face.mAjAkka));
                writer.write(",");
                writer.write(Integer.toString(indexOf(vertexIndices, face.mAjakKa)));
                writer.write(",");
                writer.write(Integer.toString(indexOf(vertexIndices, face.MaJAkKa)));
                writer.write(",");
                writer.write(Integer.toString(indexOf(vertexIndices, face.maJAkKa)));
                writer.write(",");
                writer.write(Integer.toString(indexOf(uvIndices, face.MAJAkKa)));
                writer.write(",");
                writer.write(Integer.toString(indexOf(uvIndices, face.mAJAkKa)));
                writer.write(",");
                writer.write(Integer.toString(indexOf(uvIndices, face.MajAkKa)));
                writer.newLine();
                ++index;
            }
        }
    }

    private static void writeProjectedVertices(Path outputDir, kaaakma backdrop) throws IOException {
        Path output = outputDir.resolve("backdrop_projected_vertices.csv");
        try (BufferedWriter writer = Files.newBufferedWriter(output)) {
            writer.write("vertex_index,object_x,object_y,object_z,camera_x,camera_y,depth_z,screen_x_fp,screen_y_fp,screen_x_px,screen_y_px,clip_flags,uv_u,uv_v");
            writer.newLine();
            int index = 0;
            while (index < backdrop.jAkkaMA.length) {
                mmjakka vertex = backdrop.jAkkaMA[index];
                kmajkmk uv = backdrop.JaKKaMA != null && index < backdrop.JaKKaMA.length ? backdrop.JaKKaMA[index] : null;
                writer.write(Integer.toString(index));
                writer.write(",");
                writer.write(csvFloat(vertex.maJakKA));
                writer.write(",");
                writer.write(csvFloat(vertex.MAJakKA));
                writer.write(",");
                writer.write(csvFloat(vertex.mAJakKA));
                writer.write(",");
                writer.write(csvFloat(vertex.AMaJaKK));
                writer.write(",");
                writer.write(csvFloat(vertex.aMaJaKK));
                writer.write(",");
                writer.write(csvFloat(vertex.AmAjAkk));
                writer.write(",");
                writer.write(csvFloat(vertex.amAjAkk));
                writer.write(",");
                writer.write(csvFloat(vertex.AMAjAkk));
                writer.write(",");
                writer.write(csvFloat(vertex.amAjAkk / 65536.0f));
                writer.write(",");
                writer.write(csvFloat(vertex.AMAjAkk / 65536.0f));
                writer.write(",");
                writer.write(Integer.toString(vertex.aMAjAkk));
                writer.write(",");
                writer.write(csvFloat(uv != null ? uv.akKAMAJ : Float.NaN));
                writer.write(",");
                writer.write(csvFloat(uv != null ? uv.AKKAMAJ : Float.NaN));
                writer.newLine();
                ++index;
            }
        }
    }

    private static void writeVisibleTriangles(Path outputDir, kaaakma backdrop, kmaamma[] visibleTriangles) throws IOException {
        IdentityHashMap<mmjakka, Integer> vertexIndices = buildVertexIndex(backdrop.jAkkaMA);
        IdentityHashMap<kmajkmk, Integer> uvIndices = buildUvIndex(backdrop.JaKKaMA);
        IdentityHashMap<kmaamma, Integer> faceIndices = buildFaceIndex(backdrop.JAkkaMA);

        Path output = outputDir.resolve("backdrop_visible_triangles.csv");
        try (BufferedWriter writer = Files.newBufferedWriter(output)) {
            writer.write("triangle_index,source_face_index,synthetic,material_id,depth_sum,"
                    + "vertex0_index,vertex1_index,vertex2_index,uv0_index,uv1_index,uv2_index,"
                    + "x0_fp,y0_fp,z0,x0_px,y0_px,flags0,u0,v0,"
                    + "x1_fp,y1_fp,z1,x1_px,y1_px,flags1,u1,v1,"
                    + "x2_fp,y2_fp,z2,x2_px,y2_px,flags2,u2,v2");
            writer.newLine();
            int index = 0;
            while (index < visibleTriangles.length) {
                kmaamma triangle = visibleTriangles[index];
                int sourceFaceIndex = indexOf(faceIndices, triangle);
                writer.write(Integer.toString(index));
                writer.write(",");
                writer.write(Integer.toString(sourceFaceIndex));
                writer.write(",");
                writer.write(sourceFaceIndex >= 0 ? "0" : "1");
                writer.write(",");
                writer.write(Integer.toString(triangle.mAjAkka));
                writer.write(",");
                writer.write(csvFloat(triangle.majAkKa));
                writer.write(",");
                writer.write(Integer.toString(indexOf(vertexIndices, triangle.mAjakKa)));
                writer.write(",");
                writer.write(Integer.toString(indexOf(vertexIndices, triangle.MaJAkKa)));
                writer.write(",");
                writer.write(Integer.toString(indexOf(vertexIndices, triangle.maJAkKa)));
                writer.write(",");
                writer.write(Integer.toString(indexOf(uvIndices, triangle.MAJAkKa)));
                writer.write(",");
                writer.write(Integer.toString(indexOf(uvIndices, triangle.mAJAkKa)));
                writer.write(",");
                writer.write(Integer.toString(indexOf(uvIndices, triangle.MajAkKa)));
                writeTriangleVertex(writer, triangle.mAjakKa, triangle.MAJAkKa);
                writeTriangleVertex(writer, triangle.MaJAkKa, triangle.mAJAkKa);
                writeTriangleVertex(writer, triangle.maJAkKa, triangle.MajAkKa);
                writer.newLine();
                ++index;
            }
        }
    }

    private static void writeBackdropRasterPreview(Path outputDir, mmjamma host, kmaamma[] visibleTriangles) throws IOException {
        kaaakka renderer = new kaaakka();
        renderer.KKamAjA(host, DEFAULT_FRAME_WIDTH, DEFAULT_FRAME_HEIGHT, 1);
        renderer.KAMaJAK(0);
        renderer.kKAMAjA(visibleTriangles, visibleTriangles.length);

        BufferedImage image = new BufferedImage(DEFAULT_FRAME_WIDTH, DEFAULT_FRAME_HEIGHT, BufferedImage.TYPE_INT_ARGB);
        int[] packedPixels = renderer.aMAjaKk.MAJakKa;
        int y = 0;
        while (y < DEFAULT_FRAME_HEIGHT) {
            int rowOffset = y * DEFAULT_FRAME_WIDTH;
            int x = 0;
            while (x < DEFAULT_FRAME_WIDTH) {
                image.setRGB(x, y, unpackColor(packedPixels[rowOffset + x]));
                ++x;
            }
            ++y;
        }
        ImageIO.write(image, "png", outputDir.resolve("backdrop_raster_preview.png").toFile());
    }

    private static void writeTriangleVertex(BufferedWriter writer, mmjakka vertex, kmajkmk uv) throws IOException {
        writer.write(",");
        writer.write(csvFloat(vertex.amAjAkk));
        writer.write(",");
        writer.write(csvFloat(vertex.AMAjAkk));
        writer.write(",");
        writer.write(csvFloat(vertex.AmAjAkk));
        writer.write(",");
        writer.write(csvFloat(vertex.amAjAkk / 65536.0f));
        writer.write(",");
        writer.write(csvFloat(vertex.AMAjAkk / 65536.0f));
        writer.write(",");
        writer.write(Integer.toString(vertex.aMAjAkk));
        writer.write(",");
        writer.write(csvFloat(uv != null ? uv.akKAMAJ : Float.NaN));
        writer.write(",");
        writer.write(csvFloat(uv != null ? uv.AKKAMAJ : Float.NaN));
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

    private static IdentityHashMap<mmjakka, Integer> buildVertexIndex(mmjakka[] vertices) {
        IdentityHashMap<mmjakka, Integer> indices = new IdentityHashMap<mmjakka, Integer>();
        if (vertices == null) {
            return indices;
        }
        int index = 0;
        while (index < vertices.length) {
            indices.put(vertices[index], Integer.valueOf(index));
            ++index;
        }
        return indices;
    }

    private static IdentityHashMap<kmajkmk, Integer> buildUvIndex(kmajkmk[] uvs) {
        IdentityHashMap<kmajkmk, Integer> indices = new IdentityHashMap<kmajkmk, Integer>();
        if (uvs == null) {
            return indices;
        }
        int index = 0;
        while (index < uvs.length) {
            indices.put(uvs[index], Integer.valueOf(index));
            ++index;
        }
        return indices;
    }

    private static IdentityHashMap<kmaamma, Integer> buildFaceIndex(kmaamma[] faces) {
        IdentityHashMap<kmaamma, Integer> indices = new IdentityHashMap<kmaamma, Integer>();
        if (faces == null) {
            return indices;
        }
        int index = 0;
        while (index < faces.length) {
            indices.put(faces[index], Integer.valueOf(index));
            ++index;
        }
        return indices;
    }

    private static <T> int indexOf(IdentityHashMap<T, Integer> indices, T value) {
        Integer index = indices.get(value);
        return index != null ? index.intValue() : -1;
    }

    private static String csvFloat(float value) {
        if (Float.isNaN(value)) {
            return "NaN";
        }
        if (Float.isInfinite(value)) {
            return value > 0.0f ? "Infinity" : "-Infinity";
        }
        return String.format(Locale.ROOT, "%.9f", value);
    }

    private static String csvString(String value) {
        String safe = value == null ? "" : value.replace("\"", "\"\"");
        return "\"" + safe + "\"";
    }

    private static int unpackColor(int packed) {
        int red = packed >> 20 & 0xFF;
        int green = packed >> 10 & 0xFF;
        int blue = packed & 0xFF;
        return 0xFF000000 | red << 16 | green << 8 | blue;
    }

    private static final class ProjectionSnapshot {
        final int cullMask;
        final kmaamma[] visibleTriangles;

        ProjectionSnapshot(int cullMask, kmaamma[] visibleTriangles) {
            this.cullMask = cullMask;
            this.visibleTriangles = visibleTriangles;
        }
    }

    private static final class Config {
        final Path outputDir;
        final long sceneTimeMs;
        final float sceneTimeSeconds;
        final String label;

        Config(Path outputDir, long sceneTimeMs, String label) {
            this.outputDir = outputDir;
            this.sceneTimeMs = sceneTimeMs;
            this.sceneTimeSeconds = sceneTimeMs / 1000.0f;
            this.label = label;
        }

        static Config from(String[] args) {
            String outputDirValue = System.getProperty(OUTPUT_DIR_PROPERTY, "documentation/reference-capture/saari-probe/default");
            String sceneTimeMsValue = System.getProperty(SCENE_TIME_MS_PROPERTY, Long.toString(DEFAULT_SCENE_TIME_MS));
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
                } else if ("scenetimems".equals(key)) {
                    sceneTimeMsValue = value;
                } else if ("label".equals(key)) {
                    labelValue = value;
                } else {
                    throw new IllegalArgumentException("Unknown probe argument: " + args[index]);
                }
                index += 2;
            }

            long sceneTimeMs = Long.parseLong(sceneTimeMsValue);
            Path outputDir = Paths.get(outputDirValue).toAbsolutePath().normalize();
            return new Config(outputDir, sceneTimeMs, labelValue);
        }

        private static String normalizeKey(String key) {
            String normalized = key == null ? "" : key.trim();
            while (normalized.startsWith("-")) {
                normalized = normalized.substring(1);
            }
            return normalized.toLowerCase(Locale.ROOT);
        }
    }

    private static final class ProbeForwardHost extends forward {
        private ProbeForwardHost() {
        }

        @Override
        public void kAMajak() {
        }
    }
}
