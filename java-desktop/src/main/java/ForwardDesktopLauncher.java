import java.net.URISyntaxException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.security.CodeSource;
import java.util.ArrayList;
import java.util.List;

public final class ForwardDesktopLauncher {
    private static final String BUNDLED_ASSETS_DIR = "forward-assets";
    private static final int MAX_SEARCH_DEPTH = 8;

    private ForwardDesktopLauncher() {
    }

    public static void main(String[] stringArray) {
        stringArray = ForwardStartupDialog.maybeShow(stringArray);
        Path path = ForwardDesktopLauncher.resolveAssetRoot();
        if (path == null) {
            System.err.println("forward packaging warning: unable to locate bundled assets, using current working directory");
            forward.main(stringArray);
            return;
        }
        if (System.getProperty("forward.repoRoot") == null) {
            Path path2 = path.getParent();
            if (path2 != null) {
                System.setProperty("forward.repoRoot", path2.toString());
            }
        }
        forward.main(ForwardDesktopLauncher.injectBaseDir(stringArray, path));
    }

    static Path resolveAssetRoot() {
        String string = System.getProperty("forward.basedir");
        if (string != null && !string.isBlank()) {
            Path path = Paths.get(string).toAbsolutePath().normalize();
            if (ForwardDesktopLauncher.isAssetRoot(path)) {
                return path;
            }
        }
        List<Path> list = new ArrayList<Path>();
        Path path = ForwardDesktopLauncher.resolveCodeHome();
        if (path != null) {
            list.add(path);
        }
        list.add(Paths.get(System.getProperty("user.dir")).toAbsolutePath().normalize());
        for (Path path2 : list) {
            Path path3 = ForwardDesktopLauncher.findAssetRoot(path2);
            if (path3 != null) {
                return path3;
            }
        }
        return null;
    }

    static Path resolveCodeHome() {
        try {
            CodeSource codeSource = ForwardDesktopLauncher.class.getProtectionDomain().getCodeSource();
            if (codeSource == null || codeSource.getLocation() == null) {
                return null;
            }
            Path path = Paths.get(codeSource.getLocation().toURI()).toAbsolutePath().normalize();
            if (Files.isDirectory(path)) {
                return path;
            }
            return path.getParent();
        }
        catch (URISyntaxException uRISyntaxException) {
            uRISyntaxException.printStackTrace();
            return null;
        }
    }

    static Path findAssetRoot(Path path) {
        Path path2 = path;
        int n = 0;
        while (path2 != null && n < MAX_SEARCH_DEPTH) {
            Path[] pathArray = new Path[]{path2, path2.resolve(BUNDLED_ASSETS_DIR), path2.resolve("original").resolve("forward")};
            for (Path path3 : pathArray) {
                if (ForwardDesktopLauncher.isAssetRoot(path3)) {
                    return path3.toAbsolutePath().normalize();
                }
            }
            path2 = path2.getParent();
            ++n;
        }
        return null;
    }

    static boolean isAssetRoot(Path path) {
        if (path == null || !Files.isDirectory(path)) {
            return false;
        }
        return Files.isDirectory(path.resolve("asses")) && Files.isDirectory(path.resolve("images")) && Files.isDirectory(path.resolve("meshes")) && Files.isDirectory(path.resolve("mods"));
    }

    static String[] injectBaseDir(String[] stringArray, Path path) {
        if (stringArray != null) {
            int n = 0;
            while (n < stringArray.length - 1) {
                String string = stringArray[n];
                if ("base".equalsIgnoreCase(string) || "basedir".equalsIgnoreCase(string)) {
                    return stringArray;
                }
                n += 2;
            }
        }
        String[] stringArray2 = new String[(stringArray == null ? 0 : stringArray.length) + 2];
        stringArray2[0] = "basedir";
        stringArray2[1] = path.toString();
        if (stringArray != null && stringArray.length > 0) {
            System.arraycopy(stringArray, 0, stringArray2, 2, stringArray.length);
        }
        return stringArray2;
    }
}
