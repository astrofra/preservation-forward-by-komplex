/*
 * Decompiled with CFR 0.152.
 */
import java.applet.Applet;
import java.applet.AppletContext;
import java.applet.AudioClip;
import java.awt.Image;
import java.awt.Toolkit;
import java.net.URL;
import java.util.Enumeration;

class kmjakka
implements AppletContext {
    public static URL KAMAjaK() {
        return kmjakka.kaMAjaK();
    }

    public static URL kaMAjaK() {
        String string = System.getProperty("user.dir");
        char c = (string = string.replace('\\', '/')).charAt(string.length() - 1);
        if (c != '/') {
            string = String.valueOf(string) + "/";
        }
        try {
            URL uRL = new URL("file:///" + string);
            return uRL;
        }
        catch (Exception exception) {
            exception.printStackTrace();
            return null;
        }
    }

    public Applet getApplet(String string) {
        return null;
    }

    public Enumeration getApplets() {
        return null;
    }

    public AudioClip getAudioClip(URL uRL) {
        return null;
    }

    public Image getImage(URL uRL) {
        return Toolkit.getDefaultToolkit().getImage(uRL);
    }

    public void showDocument(URL uRL) {
    }

    public void showDocument(URL uRL, String string) {
    }

    public void showStatus(String string) {
    }

    kmjakka() {
    }
}
