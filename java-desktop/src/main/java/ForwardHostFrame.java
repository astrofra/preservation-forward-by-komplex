import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Event;
import java.awt.Frame;
import java.awt.GraphicsDevice;
import java.awt.GraphicsEnvironment;
import java.awt.Insets;
import java.awt.Panel;
import java.awt.Rectangle;

final class ForwardHostFrame extends Frame {
    private final ForwardLaunchConfiguration launchConfiguration;
    private final Panel stagePanel;
    private GraphicsDevice fullScreenDevice;

    ForwardHostFrame(String string, ForwardLaunchConfiguration forwardLaunchConfiguration) {
        super(string);
        this.launchConfiguration = forwardLaunchConfiguration;
        this.stagePanel = new Panel(null);
        this.stagePanel.setBackground(Color.black);
        this.setBackground(Color.black);
        if (forwardLaunchConfiguration.fullscreen) {
            this.setUndecorated(true);
            this.setLayout(null);
            this.add(this.stagePanel);
            this.layoutFullscreen();
        } else {
            this.setLayout(null);
            this.add(this.stagePanel);
            this.layoutWindowed();
        }
    }

    void attach(Component component) {
        component.setFocusable(true);
        component.setBounds(0, 0, this.launchConfiguration.displayWidth, this.launchConfiguration.displayHeight);
        this.stagePanel.add(component);
    }

    public void show() {
        super.show();
        if (this.launchConfiguration.fullscreen) {
            this.enterFullscreen();
        }
        this.toFront();
        this.requestFocus();
        if (this.stagePanel.getComponentCount() > 0) {
            this.stagePanel.getComponent(0).requestFocus();
        }
    }

    public void dispose() {
        this.leaveFullscreen();
        this.removeAll();
        super.dispose();
    }

    public boolean handleEvent(Event event) {
        switch (event.id) {
            case 201: {
                this.dispose();
                System.exit(0);
                return true;
            }
            case 401:
            case 403: {
                if (event.key == 27) {
                    ForwardExitSupport.requestExit(this);
                    return true;
                }
                break;
            }
        }
        return super.handleEvent(event);
    }

    private void layoutWindowed() {
        this.setVisible(true);
        this.setVisible(false);
        Insets insets = this.getInsets();
        int n = insets.left + insets.right + this.launchConfiguration.displayWidth;
        int n2 = insets.top + insets.bottom + this.launchConfiguration.displayHeight;
        Dimension dimension = this.getToolkit().getScreenSize();
        int n3 = Math.max(0, (dimension.width - n) / 2);
        int n4 = Math.max(0, (dimension.height - n2) / 2);
        this.stagePanel.setBounds(insets.left, insets.top, this.launchConfiguration.displayWidth, this.launchConfiguration.displayHeight);
        this.setBounds(n3, n4, n, n2);
        this.setResizable(false);
    }

    private void layoutFullscreen() {
        Rectangle rectangle = GraphicsEnvironment.getLocalGraphicsEnvironment().getMaximumWindowBounds();
        if (rectangle.width <= 0 || rectangle.height <= 0) {
            Dimension dimension = this.getToolkit().getScreenSize();
            rectangle = new Rectangle(0, 0, dimension.width, dimension.height);
        }
        this.setBounds(rectangle);
        int n = rectangle.x + (rectangle.width - this.launchConfiguration.displayWidth) / 2;
        int n2 = rectangle.y + (rectangle.height - this.launchConfiguration.displayHeight) / 2;
        this.stagePanel.setBounds(n, n2, this.launchConfiguration.displayWidth, this.launchConfiguration.displayHeight);
    }

    private void enterFullscreen() {
        try {
            this.fullScreenDevice = GraphicsEnvironment.getLocalGraphicsEnvironment().getDefaultScreenDevice();
            if (this.fullScreenDevice != null) {
                this.fullScreenDevice.setFullScreenWindow(this);
                this.layoutFullscreen();
            }
        }
        catch (Exception exception) {
            this.fullScreenDevice = null;
            this.layoutFullscreen();
        }
    }

    private void leaveFullscreen() {
        if (this.fullScreenDevice != null) {
            try {
                if (this.fullScreenDevice.getFullScreenWindow() == this) {
                    this.fullScreenDevice.setFullScreenWindow(null);
                }
            }
            catch (Exception exception) {
            }
            this.fullScreenDevice = null;
        }
    }
}
