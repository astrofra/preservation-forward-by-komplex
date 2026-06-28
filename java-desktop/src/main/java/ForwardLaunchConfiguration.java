import java.awt.Dimension;
import java.awt.Toolkit;
import java.util.Locale;

final class ForwardLaunchConfiguration {
    static final int NATIVE_WIDTH = 512;
    static final int NATIVE_HEIGHT = 256;
    static final String PARAM_DISPLAY_MODE = "displaymode";
    static final String PARAM_DISPLAY_SCALE = "displayscale";
    static final String PARAM_LAUNCHER = "launcher";
    static final String PARAM_FULLSCREEN = "fullscreen";
    static final String MODE_WINDOWED = "windowed";
    static final String MODE_FULLSCREEN = "fullscreen";
    final boolean fullscreen;
    final int displayScale;
    final int displayWidth;
    final int displayHeight;

    private ForwardLaunchConfiguration(boolean bl, int n) {
        this.fullscreen = bl;
        this.displayScale = n;
        this.displayWidth = NATIVE_WIDTH * n;
        this.displayHeight = NATIVE_HEIGHT * n;
    }

    static ForwardLaunchConfiguration fromParameters(mmjamma mmjamma2) {
        boolean bl = ForwardLaunchConfiguration.isTruthy(mmjamma2.getParameter(PARAM_FULLSCREEN));
        String string = ForwardLaunchConfiguration.normalizeMode(mmjamma2.getParameter(PARAM_DISPLAY_MODE));
        if (MODE_FULLSCREEN.equals(string)) {
            bl = true;
        } else if (MODE_WINDOWED.equals(string)) {
            bl = false;
        }
        int n = ForwardLaunchConfiguration.parseScale(mmjamma2.getParameter(PARAM_DISPLAY_SCALE));
        return ForwardLaunchConfiguration.create(bl, n);
    }

    static ForwardLaunchConfiguration create(boolean bl, int n) {
        return new ForwardLaunchConfiguration(bl, ForwardLaunchConfiguration.clampScaleToScreen(n));
    }

    static int parseScale(String string) {
        try {
            return Math.max(1, Integer.parseInt(string));
        }
        catch (Exception exception) {
            return 1;
        }
    }

    static int clampScaleToScreen(int n) {
        int n2 = Math.max(1, n);
        try {
            Dimension dimension = Toolkit.getDefaultToolkit().getScreenSize();
            int n3 = Math.max(1, dimension.width / NATIVE_WIDTH);
            int n4 = Math.max(1, dimension.height / NATIVE_HEIGHT);
            return Math.max(1, Math.min(n2, Math.min(n3, n4)));
        }
        catch (Exception exception) {
            return n2;
        }
    }

    private static String normalizeMode(String string) {
        if (string == null) {
            return null;
        }
        String string2 = string.trim().toLowerCase(Locale.ROOT);
        if (string2.isEmpty()) {
            return null;
        }
        if ("full".equals(string2)) {
            return MODE_FULLSCREEN;
        }
        return string2;
    }

    static boolean isTruthy(String string) {
        if (string == null) {
            return false;
        }
        String string2 = string.trim().toLowerCase(Locale.ROOT);
        return string2.equals("1") || string2.equals("true") || string2.equals("yes") || string2.equals("on");
    }

    static boolean isFalsey(String string) {
        if (string == null) {
            return false;
        }
        String string2 = string.trim().toLowerCase(Locale.ROOT);
        return string2.equals("0") || string2.equals("false") || string2.equals("no") || string2.equals("off");
    }
}
