import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Locale;

final class ForwardDisplayOptions {
    static final String CONFIG_FILE_NAME = "forward-launcher.ini";

    private ForwardDisplayOptions() {
    }

    static List<ResolutionOption> loadResolutions() {
        Path path = ForwardDisplayOptions.findConfigFile();
        if (path != null) {
            List<ResolutionOption> list = ForwardDisplayOptions.parseConfig(path);
            if (!list.isEmpty()) {
                return list;
            }
        }
        return ForwardDisplayOptions.defaultResolutions();
    }

    static List<ResolutionOption> defaultResolutions() {
        ArrayList<ResolutionOption> arrayList = new ArrayList<ResolutionOption>();
        arrayList.add(new ResolutionOption("Native 512x256", 512, 256));
        arrayList.add(new ResolutionOption("X2 1024x512", 1024, 512));
        return arrayList;
    }

    private static Path findConfigFile() {
        String string = System.getProperty("forward.launcherConfig");
        LinkedHashSet<Path> linkedHashSet = new LinkedHashSet<Path>();
        if (string != null && !string.isBlank()) {
            linkedHashSet.add(Paths.get(string).toAbsolutePath().normalize());
        }
        Path path2 = ForwardDesktopLauncher.resolveCodeHome();
        if (path2 != null) {
            linkedHashSet.add(path2.resolve(CONFIG_FILE_NAME));
            Path path3 = path2.getParent();
            if (path3 != null) {
                linkedHashSet.add(path3.resolve(CONFIG_FILE_NAME));
            }
        }
        Path path = Paths.get(System.getProperty("user.dir")).toAbsolutePath().normalize();
        linkedHashSet.add(path.resolve(CONFIG_FILE_NAME));
        String string2 = System.getProperty("forward.repoRoot");
        if (string2 != null && !string2.isBlank()) {
            linkedHashSet.add(Paths.get(string2).toAbsolutePath().normalize().resolve(CONFIG_FILE_NAME));
        }
        for (Path path4 : linkedHashSet) {
            if (!Files.isRegularFile(path4)) continue;
            return path4;
        }
        return null;
    }

    private static List<ResolutionOption> parseConfig(Path path) {
        ArrayList<ResolutionOption> arrayList = new ArrayList<ResolutionOption>();
        List<String> list = null;
        try {
            list = Files.readAllLines(path, StandardCharsets.UTF_8);
        }
        catch (IOException iOException) {
            System.err.println("forward launcher: unable to read " + path + ", using built-in display sizes");
            return arrayList;
        }
        for (String string : list) {
            ResolutionOption resolutionOption;
            String string2 = string.trim();
            if (string2.isEmpty() || string2.startsWith("#") || string2.startsWith(";") || string2.startsWith("[")) continue;
            int n = string2.indexOf(61);
            if (n < 0) continue;
            String string3 = string2.substring(0, n).trim().toLowerCase(Locale.ROOT);
            if (!string3.startsWith("resolution")) continue;
            if ((resolutionOption = ForwardDisplayOptions.parseResolution(string2.substring(n + 1).trim())) == null) {
                System.err.println("forward launcher: ignoring invalid resolution entry in " + path.getFileName() + ": " + string2);
                continue;
            }
            arrayList.add(resolutionOption);
        }
        return arrayList;
    }

    private static ResolutionOption parseResolution(String string) {
        String string2 = string;
        String string3 = string;
        int n = string.indexOf(124);
        if (n >= 0) {
            string2 = string.substring(0, n).trim();
            string3 = string.substring(n + 1).trim();
        }
        int n2 = string3.toLowerCase(Locale.ROOT).indexOf(120);
        if (n2 <= 0 || n2 >= string3.length() - 1) {
            return null;
        }
        try {
            int n3 = Math.max(1, Integer.parseInt(string3.substring(0, n2).trim()));
            int n4 = Math.max(1, Integer.parseInt(string3.substring(n2 + 1).trim()));
            if (string2.isEmpty()) {
                string2 = n3 + "x" + n4;
            }
            return new ResolutionOption(string2, n3, n4);
        }
        catch (NumberFormatException numberFormatException) {
            return null;
        }
    }

    static final class ResolutionOption {
        final String label;
        final int width;
        final int height;

        ResolutionOption(String string, int n, int n2) {
            this.label = string;
            this.width = n;
            this.height = n2;
        }

        public String toString() {
            return this.label;
        }
    }
}
