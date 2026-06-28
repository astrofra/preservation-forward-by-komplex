import java.awt.Component;
import java.awt.Window;

final class ForwardExitSupport {
    private ForwardExitSupport() {
    }

    static void requestExit(Component component) {
        Component component2 = component;
        while (component2 != null) {
            if (component2 instanceof Window) {
                ((Window)component2).dispose();
                break;
            }
            component2 = component2.getParent();
        }
        System.exit(0);
    }
}
