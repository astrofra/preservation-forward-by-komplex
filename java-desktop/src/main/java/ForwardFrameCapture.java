import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;
import javax.imageio.ImageIO;

final class ForwardFrameCapture {
    private final File rootDir;
    private final File framesDir;
    private final PrintWriter manifestWriter;
    private final BufferedImage frameImage;
    private final Graphics2D frameGraphics;
    private final long sessionStartMs;
    private final int width;
    private final int height;
    private final int captureEveryFrames;
    private final long captureIntervalMs;
    private final long captureStartMs;
    private final long captureStopMs;
    private final int captureLimit;
    private final boolean captureExit;
    private long nextCaptureAtMs;
    private int captureCount;
    private boolean ended;
    private boolean closed;

    private ForwardFrameCapture(File rootDir, int width, int height, int captureEveryFrames, long captureIntervalMs, long captureStartMs, long captureStopMs, int captureLimit, boolean captureExit) throws IOException {
        this.rootDir = rootDir;
        this.width = width;
        this.height = height;
        this.sessionStartMs = System.currentTimeMillis();
        this.captureEveryFrames = Math.max(1, captureEveryFrames);
        this.captureIntervalMs = captureIntervalMs;
        this.captureStartMs = Math.max(0L, captureStartMs);
        this.captureStopMs = captureStopMs;
        this.captureLimit = captureLimit;
        this.captureExit = captureExit;
        this.nextCaptureAtMs = this.captureStartMs;
        if (!this.rootDir.exists() && !this.rootDir.mkdirs()) {
            throw new IOException("Unable to create capture dir: " + this.rootDir.getAbsolutePath());
        }
        this.framesDir = new File(this.rootDir, "frames");
        if (!this.framesDir.exists() && !this.framesDir.mkdirs()) {
            throw new IOException("Unable to create frame dir: " + this.framesDir.getAbsolutePath());
        }
        this.manifestWriter = new PrintWriter(new OutputStreamWriter(new FileOutputStream(new File(this.rootDir, "manifest.csv")), StandardCharsets.UTF_8));
        this.manifestWriter.println("capture_index,render_frame,demo_time_ms,demo_time_seconds,scene_time_ms,scene_time_seconds,scene,next_script_time_hex,frame_path");
        this.manifestWriter.flush();
        this.frameImage = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
        this.frameGraphics = this.frameImage.createGraphics();
        this.frameGraphics.setBackground(Color.black);
    }

    public static ForwardFrameCapture create(mmjamma app, int width, int height) throws IOException {
        String captureDir = app.getParameter("capture");
        if (captureDir == null || captureDir.trim().isEmpty()) {
            return null;
        }
        long captureIntervalMs = app.AmAjAkK("captureintervalms", -1L);
        int captureEveryFrames = app.aMAJAkK("captureevery", 10);
        long captureStartMs = app.AmAjAkK("capturestartms", 0L);
        long captureStopMs = app.AmAjAkK("capturestopms", -1L);
        int captureLimit = app.aMAJAkK("capturelimit", -1);
        boolean captureExit = app.getParameter("captureexit") != null;
        File resolvedCaptureDir = new File(captureDir);
        if (!resolvedCaptureDir.isAbsolute()) {
            String repoRoot = System.getProperty("forward.repoRoot");
            if (repoRoot != null && !repoRoot.isEmpty()) {
                resolvedCaptureDir = new File(repoRoot, captureDir);
            }
        }
        ForwardFrameCapture capture = new ForwardFrameCapture(resolvedCaptureDir, width, height, captureEveryFrames, captureIntervalMs, captureStartMs, captureStopMs, captureLimit, captureExit);
        System.out.println("frame capture enabled: " + capture.rootDir.getAbsolutePath());
        return capture;
    }

    public Graphics beginFrame() {
        this.frameGraphics.setClip(0, 0, this.width, this.height);
        this.frameGraphics.clearRect(0, 0, this.width, this.height);
        return this.frameGraphics;
    }

    public void copyFrame(BufferedImage bufferedImage) {
        if (bufferedImage == null) {
            return;
        }
        this.frameGraphics.drawImage(bufferedImage, 0, 0, this.width, this.height, null);
    }

    public boolean captureFrame(int renderFrame, float demoSeconds, String sceneName, int nextScriptTimeHex) throws IOException {
        if (this.closed || this.ended) {
            return false;
        }
        long captureTimeMs = Math.max(0L, System.currentTimeMillis() - this.sessionStartMs);
        long sceneTimeMs = Math.max(0L, Math.round(demoSeconds * 1000.0f));
        if (captureTimeMs < this.captureStartMs) {
            return false;
        }
        boolean due = this.captureIntervalMs > 0L ? captureTimeMs >= this.nextCaptureAtMs : renderFrame % this.captureEveryFrames == 0;
        if (due) {
            this.writeFrame(renderFrame, captureTimeMs, sceneTimeMs, demoSeconds, sceneName, nextScriptTimeHex);
            if (this.captureIntervalMs > 0L) {
                do {
                    this.nextCaptureAtMs += this.captureIntervalMs;
                } while (this.nextCaptureAtMs <= captureTimeMs);
            }
        }
        if (this.captureLimit > 0 && this.captureCount >= this.captureLimit) {
            this.ended = true;
        }
        if (this.captureStopMs >= 0L && captureTimeMs >= this.captureStopMs) {
            this.ended = true;
        }
        if (this.ended) {
            this.close();
            return this.captureExit;
        }
        return false;
    }

    public boolean isEnded() {
        return this.ended;
    }

    public void close() {
        if (this.closed) {
            return;
        }
        this.closed = true;
        this.frameGraphics.dispose();
        this.manifestWriter.flush();
        this.manifestWriter.close();
    }

    private void writeFrame(int renderFrame, long captureTimeMs, long sceneTimeMs, float sceneSeconds, String sceneName, int nextScriptTimeHex) throws IOException {
        String frameFileName = this.buildFrameFileName(this.captureCount, captureTimeMs, renderFrame);
        File outputFile = new File(this.framesDir, frameFileName);
        ImageIO.write(this.frameImage, "png", outputFile);
        String normalizedScene = sceneName == null ? "" : sceneName;
        String nextScript = nextScriptTimeHex < 0 ? "" : "0x" + Integer.toHexString(nextScriptTimeHex);
        this.manifestWriter.print(this.captureCount);
        this.manifestWriter.print(',');
        this.manifestWriter.print(renderFrame);
        this.manifestWriter.print(',');
        this.manifestWriter.print(captureTimeMs);
        this.manifestWriter.print(',');
        this.manifestWriter.print((float)captureTimeMs / 1000.0f);
        this.manifestWriter.print(',');
        this.manifestWriter.print(sceneTimeMs);
        this.manifestWriter.print(',');
        this.manifestWriter.print(sceneSeconds);
        this.manifestWriter.print(',');
        this.manifestWriter.print(this.csvEscape(normalizedScene));
        this.manifestWriter.print(',');
        this.manifestWriter.print(this.csvEscape(nextScript));
        this.manifestWriter.print(',');
        this.manifestWriter.println(this.csvEscape("frames/" + frameFileName));
        this.manifestWriter.flush();
        ++this.captureCount;
    }

    private String buildFrameFileName(int captureIndex, long demoTimeMs, int renderFrame) {
        return String.format("frame_%06d_t%08d_f%06d.png", captureIndex, demoTimeMs, renderFrame);
    }

    private String csvEscape(String value) {
        if (value.indexOf(44) == -1 && value.indexOf(34) == -1) {
            return value;
        }
        return "\"" + value.replace("\"", "\"\"") + "\"";
    }
}
