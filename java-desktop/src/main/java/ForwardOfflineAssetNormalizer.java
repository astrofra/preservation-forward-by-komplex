import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.net.URL;

public final class ForwardOfflineAssetNormalizer {
    private static final String[] MUTE95_JPEGS = new String[]{
        "images/kosmos/sav1.jpg",
        "images/kosmos/sav2.jpg",
        "images/kosmos/jmag1.jpg",
        "images/kosmos/jmag2.jpg",
        "images/kosmos/jugi1.jpg",
        "images/kosmos/jugi2.jpg",
        "images/kosmos/car1.jpg",
        "images/kosmos/car2.jpg",
        "images/kosmos/anis1.jpg",
        "images/kosmos/anis2.jpg"
    };

    private static final String MUTE95_PALETTE_GIF = "images/kosmos/krad3.gif";

    public static void main(String[] args) throws Exception {
        if (args.length != 3 || !"mute95".equals(args[0])) {
            System.err.println("usage: ForwardOfflineAssetNormalizer mute95 <asset-root> <output-dir>");
            System.exit(1);
            return;
        }

        File assetRoot = new File(args[1]);
        File outputDir = new File(args[2]);
        if (!outputDir.exists() && !outputDir.mkdirs()) {
            throw new IllegalStateException("unable to create output dir: " + outputDir.getAbsolutePath());
        }

        for (int index = 0; index < MUTE95_JPEGS.length; ++index) {
            String relativePath = MUTE95_JPEGS[index];
            RgbSurface surface = (RgbSurface)ImageSupport.majaKkA(resolveAsset(assetRoot, relativePath));
            if (surface == null) {
                throw new IllegalStateException("failed to decode " + relativePath);
            }
            writePackedRgbSurface(surface, new File(outputDir, outputName(relativePath, ".frgb")));
        }

        IndexedSurface paletteSurface = (IndexedSurface)ImageSupport.majaKkA(resolveAsset(assetRoot, MUTE95_PALETTE_GIF));
        if (paletteSurface == null) {
            throw new IllegalStateException("failed to decode " + MUTE95_PALETTE_GIF);
        }
        writeIndexedSurface(paletteSurface, new File(outputDir, outputName(MUTE95_PALETTE_GIF, ".fidx")));

        System.out.println("normalized mute95 assets -> " + outputDir.getAbsolutePath());
    }

    private static URL resolveAsset(File assetRoot, String relativePath) throws Exception {
        File file = new File(assetRoot, relativePath.replace('/', File.separatorChar));
        return file.toURI().toURL();
    }

    private static String outputName(String relativePath, String extension) {
        String name = relativePath.replace('\\', '/');
        int slash = name.lastIndexOf('/');
        if (slash >= 0) {
            name = name.substring(slash + 1);
        }
        int dot = name.lastIndexOf('.');
        if (dot >= 0) {
            name = name.substring(0, dot);
        }
        return name + extension;
    }

    private static void writePackedRgbSurface(RgbSurface surface, File outputFile) throws Exception {
        DataOutputStream stream = new DataOutputStream(new FileOutputStream(outputFile));
        try {
            stream.writeBytes("FRGB");
            stream.writeShort(Short.reverseBytes((short)surface.amAjakK));
            stream.writeShort(Short.reverseBytes((short)surface.AMAjakK));
            for (int index = 0; index < surface.MAJakKa.length; ++index) {
                stream.writeInt(Integer.reverseBytes(surface.MAJakKa[index]));
            }
        }
        finally {
            stream.close();
        }
    }

    private static void writeIndexedSurface(IndexedSurface surface, File outputFile) throws Exception {
        DataOutputStream stream = new DataOutputStream(new FileOutputStream(outputFile));
        try {
            stream.writeBytes("FIDX");
            stream.writeShort(Short.reverseBytes((short)surface.amAjakK));
            stream.writeShort(Short.reverseBytes((short)surface.AMAjakK));
            for (int index = 0; index < 256; ++index) {
                stream.writeByte(surface.aMajakK[index] & 0xFF);
            }
            for (int index = 0; index < 256; ++index) {
                stream.writeByte(surface.AmAJakK[index] & 0xFF);
            }
            for (int index = 0; index < 256; ++index) {
                stream.writeByte(surface.amAJakK[index] & 0xFF);
            }
            stream.write(surface.aMAjakK);
        }
        finally {
            stream.close();
        }
    }
}
