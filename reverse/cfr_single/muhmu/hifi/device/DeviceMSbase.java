// 
// Decompiled by Procyon v0.6.0
// 

package muhmu.hifi.device;

import java.awt.Component;

public abstract class DeviceMSbase extends MAD implements Runnable
{
    Mixable mixable;
    int len;
    Thread mixer;
    int splitAm;
    int[] buffer;
    int[] bufferEmpty;
    byte[] buffer8;
    boolean bit16;
    int bytesPerSample;
    int format;
    int lag;
    
    public boolean init(final Mixable mixable, final int n, final int format, final int frequency, final Component component) {
        this.mixable = mixable;
        MAD.component = component;
        this.splitAm = 10;
        this.len = n / this.splitAm;
        this.setFormat(format);
        this.setFrequency(frequency);
        this.initMixingBuffers();
        try {
            this.initDS();
            return true;
        }
        catch (final Exception ex) {
            ex.printStackTrace();
            return false;
        }
    }
    
    void initMixingBuffers() {
        this.buffer = new int[this.len];
        this.bufferEmpty = new int[this.len];
        this.buffer8 = new byte[this.len * this.bytesPerSample * this.splitAm];
    }
    
    void setFormat(final int format) {
        switch (this.format = format) {
            case 1: {
                super.stereo = false;
                this.bit16 = false;
                this.bytesPerSample = 1;
                return;
            }
            case 2: {
                super.stereo = false;
                this.bit16 = true;
                this.bytesPerSample = 2;
                return;
            }
            case 3: {
                super.stereo = true;
                this.bit16 = false;
                this.bytesPerSample = 2;
                return;
            }
            case 4: {
                super.stereo = true;
                this.bit16 = true;
                this.bytesPerSample = 4;
            }
            default: {}
        }
    }
    
    void setFrequency(final int frequency) {
        super.frequency = frequency;
    }
    
    public void start() {
        if (this.mixer == null) {
            (this.mixer = new Thread(this, "Muhmu DirectSound device")).start();
            this.mixer.setPriority(10);
        }
    }
    
    public void stop() {
        if (this.mixer != null) {
            this.mixer.stop();
            this.mixer = null;
        }
    }
    
    void clearBuffer() {
        System.arraycopy(this.bufferEmpty, 0, this.buffer, 0, this.buffer.length);
    }
    
    void hoxBuff(final int[] array, int i, final byte[] array2, int n, final int n2) {
        final int n3 = i + n2;
        switch (this.format) {
            case 1: {
                while (i < n3) {
                    array2[n++] = (byte)((array[i] >> 8) + 128);
                    ++i;
                }
                return;
            }
            case 2: {
                while (i < n3) {
                    final int n4 = array[i];
                    array2[n++] = (byte)n4;
                    array2[n++] = (byte)(n4 >> 8);
                    ++i;
                }
                return;
            }
            case 3: {
                while (i < n3) {
                    final int n5 = array[i];
                    array2[n++] = (byte)((n5 >> 8) + 128);
                    array2[n++] = (byte)((n5 >> 24) + 128);
                    ++i;
                }
                return;
            }
            case 4: {
                while (i < n3) {
                    final int n6 = array[i];
                    array2[n++] = (byte)n6;
                    array2[n++] = (byte)(n6 >>> 8);
                    array2[n++] = (byte)(n6 >>> 16);
                    array2[n++] = (byte)(n6 >>> 24);
                    ++i;
                }
            }
            default: {}
        }
    }
    
    public void run() {
        try {
            this.play();
            while (this.mixer != null) {
                for (int i = 0; i < this.splitAm; ++i) {
                    final int n = i * this.len;
                    final int n2 = (i + 1) * this.len;
                    super.bufferStartTime = (this.buffer8.length + n * this.bytesPerSample - this.playPos()) / this.bytesPerSample * 1000 / super.frequency + this.lag + System.currentTimeMillis();
                    this.clearBuffer();
                    this.mixable.mix(this, this.buffer, 0, this.buffer.length);
                    this.hoxBuff(this.buffer, 0, this.buffer8, i * this.buffer.length * this.bytesPerSample, this.buffer.length);
                    while (true) {
                        final int playPos = this.playPos();
                        if (playPos > n2 * this.bytesPerSample || playPos < n * this.bytesPerSample) {
                            break;
                        }
                        int n3 = (n2 - playPos / this.bytesPerSample) * 1000 / super.frequency;
                        n3 += 20;
                        if (n3 < 20) {
                            n3 = 20;
                        }
                        Thread.sleep(n3);
                    }
                    this.write(i);
                }
            }
        }
        catch (final Exception ex) {
            ex.printStackTrace();
        }
    }
    
    public abstract void initDS() throws Exception;
    
    public abstract void write(final int p0);
    
    public abstract int playPos();
    
    public abstract void play();
    
    public DeviceMSbase() {
        this.lag = 40;
    }
}
