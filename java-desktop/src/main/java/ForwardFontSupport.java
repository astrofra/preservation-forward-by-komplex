import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GraphicsEnvironment;
import java.awt.RenderingHints;
import java.util.HashSet;
import java.util.Locale;
import java.util.Set;

final class ForwardFontSupport {
    private static final String[] MONOSPACE_FAMILIES = new String[]{"Courier New", "Courier", "Lucida Console", Font.MONOSPACED, "DialogInput"};
    private static final String RESOLVED_MONOSPACE_FAMILY = ForwardFontSupport.resolveMonospaceFamily();

    private ForwardFontSupport() {
    }

    static Font monospaceBold(int n) {
        return new Font(RESOLVED_MONOSPACE_FAMILY, Font.BOLD, n);
    }

    static void prepare(Graphics graphics, Font font) {
        ForwardFontSupport.configureTextGraphics(graphics);
        if (font != null) {
            graphics.setFont(font);
        }
    }

    static void configureTextGraphics(Graphics graphics) {
        if (!(graphics instanceof Graphics2D)) {
            return;
        }
        Graphics2D graphics2D = (Graphics2D)graphics;
        graphics2D.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_OFF);
        graphics2D.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_OFF);
        graphics2D.setRenderingHint(RenderingHints.KEY_FRACTIONALMETRICS, RenderingHints.VALUE_FRACTIONALMETRICS_OFF);
        graphics2D.setRenderingHint(RenderingHints.KEY_RENDERING, RenderingHints.VALUE_RENDER_SPEED);
    }

    private static String resolveMonospaceFamily() {
        try {
            Set<String> set = new HashSet<String>();
            String[] stringArray = GraphicsEnvironment.getLocalGraphicsEnvironment().getAvailableFontFamilyNames();
            int n = 0;
            while (n < stringArray.length) {
                set.add(stringArray[n].toLowerCase(Locale.ROOT));
                ++n;
            }
            int n2 = 0;
            while (n2 < MONOSPACE_FAMILIES.length) {
                String string = MONOSPACE_FAMILIES[n2];
                if (set.contains(string.toLowerCase(Locale.ROOT))) {
                    return string;
                }
                ++n2;
            }
        }
        catch (Exception exception) {
            exception.printStackTrace();
        }
        return Font.MONOSPACED;
    }
}
