/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Desktop;
import java.awt.Image;
import java.awt.Toolkit;
import java.net.URL;
import java.nio.file.Path;
import java.nio.file.Paths;

class kmjakka {
    public static URL KAMAjaK() {
        return kmjakka.kaMAjaK();
    }

    public static URL kaMAjaK() {
        return kmjakka.aMAJaK(System.getProperty("user.dir"));
    }

    public static URL aMAJaK(String string) {
        try {
            Path path = Paths.get(string);
            if (!path.isAbsolute()) {
                path = Paths.get(System.getProperty("user.dir")).resolve(path);
            }
            return path.normalize().toUri().toURL();
        }
        catch (Exception exception) {
            exception.printStackTrace();
            return null;
        }
    }

    public Image getImage(URL uRL) {
        return Toolkit.getDefaultToolkit().getImage(uRL);
    }

    public void showDocument(URL uRL) {
        if (uRL == null || !Desktop.isDesktopSupported()) {
            return;
        }
        try {
            Desktop.getDesktop().browse(uRL.toURI());
        }
        catch (Exception exception) {
            exception.printStackTrace();
        }
    }

    public void showDocument(URL uRL, String string) {
        this.showDocument(uRL);
    }

    public void showStatus(String string) {
    }

    kmjakka() {
    }
}
