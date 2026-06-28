import java.awt.Dimension;
import java.awt.Toolkit;
import java.util.Locale;

final class ForwardLaunchConfiguration {
    static final String APPLICATION_TITLE = "forward::komplex";
    static final int NATIVE_WIDTH = 512;
    static final int NATIVE_HEIGHT = 256;
    static final String PARAM_DISPLAY_MODE = "displaymode";
    static final String PARAM_DISPLAY_SCALE = "displayscale";
    static final String PARAM_DISPLAY_WIDTH = "displaywidth";
    static final String PARAM_DISPLAY_HEIGHT = "displayheight";
    static final String PARAM_LAUNCHER = "launcher";
    static final String PARAM_FULLSCREEN = "fullscreen";
    static final String MODE_WINDOWED = "windowed";
    static final String MODE_FULLSCREEN = "fullscreen";
    final boolean fullscreen;
    final int displayWidth;
    final int displayHeight;

    private ForwardLaunchConfiguration(boolean bl, int n, int n2) {
        this.fullscreen = bl;
        this.displayWidth = n;
        this.displayHeight = n2;
    }

    static ForwardLaunchConfiguration fromParameters(mmjamma mmjamma2) {
        boolean bl = ForwardLaunchConfiguration.isTruthy(mmjamma2.getParameter(PARAM_FULLSCREEN));
        String string = ForwardLaunchConfiguration.normalizeMode(mmjamma2.getParameter(PARAM_DISPLAY_MODE));
        if (MODE_FULLSCREEN.equals(string)) {
            bl = true;
        } else if (MODE_WINDOWED.equals(string)) {
            bl = false;
        }
        int n = ForwardLaunchConfiguration.parseDimension(mmjamma2.getParameter(PARAM_DISPLAY_WIDTH));
        int n2 = ForwardLaunchConfiguration.parseDimension(mmjamma2.getParameter(PARAM_DISPLAY_HEIGHT));
        if (n > 0 && n2 > 0) {
            return ForwardLaunchConfiguration.create(bl, n, n2);
        }
        int n3 = ForwardLaunchConfiguration.parseScale(mmjamma2.getParameter(PARAM_DISPLAY_SCALE));
        return ForwardLaunchConfiguration.create(bl, n3);
    }

    static ForwardLaunchConfiguration create(boolean bl, int n) {
        int n2 = Math.max(1, n);
        return ForwardLaunchConfiguration.create(bl, NATIVE_WIDTH * n2, NATIVE_HEIGHT * n2);
    }

    static ForwardLaunchConfiguration create(boolean bl, int n, int n2) {
        int[] nArray = ForwardLaunchConfiguration.clampSizeToScreen(n, n2);
        return new ForwardLaunchConfiguration(bl, nArray[0], nArray[1]);
    }

    static int parseScale(String string) {
        try {
            return Math.max(1, Integer.parseInt(string));
        }
        catch (Exception exception) {
            return 1;
        }
    }

    static int parseDimension(String string) {
        try {
            return Math.max(1, Integer.parseInt(string));
        }
        catch (Exception exception) {
            return -1;
        }
    }

    static int[] clampSizeToScreen(int n, int n2) {
        int n3 = Math.max(1, n);
        int n4 = Math.max(1, n2);
        try {
            Dimension dimension = Toolkit.getDefaultToolkit().getScreenSize();
            double d = Math.min(1.0, Math.min((double)dimension.width / (double)n3, (double)dimension.height / (double)n4));
            if (d >= 1.0) {
                return new int[]{n3, n4};
            }
            return new int[]{Math.max(1, (int)Math.round((double)n3 * d)), Math.max(1, (int)Math.round((double)n4 * d))};
        }
        catch (Exception exception) {
            return new int[]{n3, n4};
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
