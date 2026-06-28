import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.image.BufferedImage;

final class ForwardFrameBuffer {
    private final BufferedImage frameImage;
    private final Graphics2D frameGraphics;
    private final int width;
    private final int height;

    ForwardFrameBuffer(int n, int n2) {
        this.width = n;
        this.height = n2;
        this.frameImage = new BufferedImage(n, n2, BufferedImage.TYPE_INT_RGB);
        this.frameGraphics = this.frameImage.createGraphics();
        this.frameGraphics.setBackground(Color.black);
    }

    Graphics beginFrame() {
        this.frameGraphics.setClip(0, 0, this.width, this.height);
        this.frameGraphics.clearRect(0, 0, this.width, this.height);
        return this.frameGraphics;
    }

    BufferedImage image() {
        return this.frameImage;
    }

    void present(Graphics graphics, int n, int n2, int n3, int n4) {
        if (graphics == null) {
            return;
        }
        if (graphics instanceof Graphics2D) {
            Graphics2D graphics2D = (Graphics2D)graphics;
            graphics2D.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_NEAREST_NEIGHBOR);
            graphics2D.setRenderingHint(RenderingHints.KEY_RENDERING, RenderingHints.VALUE_RENDER_SPEED);
            graphics2D.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_OFF);
        }
        graphics.drawImage(this.frameImage, n, n2, n3, n4, null);
    }
}
