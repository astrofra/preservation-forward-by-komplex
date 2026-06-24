package muhmu.hifi.device;

import java.awt.Component;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.SourceDataLine;

class DeviceJavaSound extends MAD implements Runnable {
    private static final int SPLIT_AM = 10;
    protected Mixable mixable;
    protected Thread mixer;
    protected SourceDataLine line;
    protected int len;
    protected int[] buffer;
    protected int[] emptyBuffer;
    protected byte[] buffer8;
    protected boolean bit16;
    protected int bytesPerSample;
    protected int format;
    protected int lag = 40;

    @Override
    public boolean init(Mixable mixable, int n, int format, int frequency, Component component) {
        this.mixable = mixable;
        MAD.component = component;
        this.len = Math.max(256, n / SPLIT_AM);
        this.setFormat(format);
        this.frequency = frequency;
        this.buffer = new int[this.len];
        this.emptyBuffer = new int[this.len];
        this.buffer8 = new byte[this.len * this.bytesPerSample];
        try {
            AudioFormat audioFormat = new AudioFormat((float)frequency, this.bit16 ? 16 : 8, this.stereo ? 2 : 1, this.bit16, false);
            this.line = AudioSystem.getSourceDataLine(audioFormat);
            this.line.open(audioFormat, this.buffer8.length * SPLIT_AM);
            return true;
        }
        catch (LineUnavailableException lineUnavailableException) {
            lineUnavailableException.printStackTrace();
            this.line = null;
            return false;
        }
    }

    @Override
    public synchronized void start() {
        if (this.line == null) {
            return;
        }
        if (this.mixer != null) {
            return;
        }
        this.line.flush();
        this.line.start();
        this.mixer = new Thread(this, "Muhmu JavaSound device");
        this.mixer.setPriority(Thread.MAX_PRIORITY);
        this.mixer.start();
    }

    @Override
    public synchronized void stop() {
        Thread thread = this.mixer;
        this.mixer = null;
        if (thread != null) {
            thread.interrupt();
        }
        if (this.line != null) {
            this.line.stop();
            this.line.flush();
            this.line.close();
            this.line = null;
        }
    }

    protected void setFormat(int n) {
        this.format = n;
        switch (n) {
            case MONO8: {
                this.stereo = false;
                this.bit16 = false;
                this.bytesPerSample = 1;
                break;
            }
            case MONO16: {
                this.stereo = false;
                this.bit16 = true;
                this.bytesPerSample = 2;
                break;
            }
            case STEREO8: {
                this.stereo = true;
                this.bit16 = false;
                this.bytesPerSample = 2;
                break;
            }
            case STEREO16: {
                this.stereo = true;
                this.bit16 = true;
                this.bytesPerSample = 4;
                break;
            }
            default: {
                throw new IllegalArgumentException("Unsupported audio format: " + n);
            }
        }
    }

    protected void clearBuffer() {
        System.arraycopy(this.emptyBuffer, 0, this.buffer, 0, this.buffer.length);
    }

    protected void encodeBuffer() {
        int n = 0;
        for (int n2 = 0; n2 < this.buffer.length; ++n2) {
            int n3 = this.buffer[n2];
            switch (this.format) {
                case MONO8: {
                    this.buffer8[n++] = (byte)((n3 >> 8) + 128);
                    break;
                }
                case MONO16: {
                    this.buffer8[n++] = (byte)n3;
                    this.buffer8[n++] = (byte)(n3 >> 8);
                    break;
                }
                case STEREO8: {
                    this.buffer8[n++] = (byte)((n3 >> 8) + 128);
                    this.buffer8[n++] = (byte)((n3 >> 24) + 128);
                    break;
                }
                case STEREO16: {
                    this.buffer8[n++] = (byte)n3;
                    this.buffer8[n++] = (byte)(n3 >>> 8);
                    this.buffer8[n++] = (byte)(n3 >>> 16);
                    this.buffer8[n++] = (byte)(n3 >>> 24);
                }
            }
        }
    }

    @Override
    public void run() {
        try {
            while (Thread.currentThread() == this.mixer && this.line != null) {
                int n = this.line.getBufferSize() - this.line.available();
                int n2 = n / this.bytesPerSample;
                this.bufferStartTime = System.currentTimeMillis() + (long)(n2 * 1000 / Math.max(1, this.frequency)) + this.lag;
                this.clearBuffer();
                this.mixable.mix(this, this.buffer, 0, this.buffer.length);
                this.encodeBuffer();
                this.line.write(this.buffer8, 0, this.buffer8.length);
            }
        }
        catch (Exception exception) {
            exception.printStackTrace();
        }
    }
}
