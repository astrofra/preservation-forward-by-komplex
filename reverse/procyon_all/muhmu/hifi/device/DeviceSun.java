// 
// Decompiled by Procyon v0.6.0
// 

package muhmu.hifi.device;

import java.io.IOException;
import sun.audio.AudioPlayer;
import sun.audio.AudioDataStream;
import sun.audio.AudioData;
import java.awt.Component;
import java.io.InputStream;
import sun.audio.AudioStreamSequence;
import java.util.Enumeration;

public final class DeviceSun extends MAD implements Enumeration, Runnable
{
    Mixable mixable;
    AudioStreamSequence ass;
    int whichBuffer;
    int len;
    byte[] bytebuf1;
    byte[] bytebuf2;
    InputStream buffer1;
    InputStream buffer2;
    int[] mixtemp;
    int[] mixtemp_Empty;
    Thread mixer;
    boolean isRunning;
    int mixTime;
    int mixMax;
    float mixCPU;
    long nextTime1;
    long nextTime2;
    static final long lag = 70L;
    
    public boolean init(final Mixable mixable, final int n, final int n2, final int n3, final Component component) {
        this.mixable = mixable;
        this.len = n * 8000 / n3;
        this.len = 1600;
        super.frequency = 8000;
        super.stereo = false;
        this.initMixingBuffers();
        this.initULAW();
        return true;
    }
    
    void initULAW() {
        ULaw.initEncodingTable();
    }
    
    void initMixingBuffers() {
        this.bytebuf1 = new byte[this.len];
        this.bytebuf2 = new byte[this.len];
        this.mixtemp = new int[this.len];
        System.arraycopy(this.mixtemp_Empty = new int[this.len], 0, this.mixtemp, 0, this.mixtemp.length);
        final AudioData audioData = new AudioData(this.bytebuf1);
        final AudioData audioData2 = new AudioData(this.bytebuf2);
        this.buffer1 = (InputStream)new AudioDataStream(audioData);
        this.buffer2 = (InputStream)new AudioDataStream(audioData2);
        this.clearBuffers();
    }
    
    void clearBuffers() {
        if (this.bytebuf1 != null) {
            for (int i = 0; i < this.len; ++i) {
                this.bytebuf1[i] = -1;
                this.bytebuf2[i] = -1;
                this.mixtemp[i] = 0;
            }
        }
    }
    
    public synchronized void start() {
        this.stop();
        (this.mixer = new Thread(this, "Muhmu SunAudio device")).start();
        this.mixer.setPriority(10);
        this.ass = new AudioStreamSequence((Enumeration)this);
        this.nextTime1 = System.currentTimeMillis();
        AudioPlayer.player.start((InputStream)this.ass);
        this.isRunning = true;
    }
    
    public synchronized void stop() {
        this.isRunning = false;
        if (this.ass != null) {
            AudioPlayer.player.stop((InputStream)this.ass);
        }
        if (this.mixer != null) {
            final Thread mixer = this.mixer;
            this.mixer = null;
            mixer.stop();
            this.notifyAll();
        }
    }
    
    public void finalize() {
        if (this.ass != null) {
            AudioPlayer.player.stop((InputStream)this.ass);
        }
    }
    
    public boolean hasMoreElements() {
        return true;
    }
    
    public synchronized Object nextElement() {
        this.whichBuffer = (this.whichBuffer + 1 & 0x1);
        InputStream inputStream;
        if (this.whichBuffer == 0) {
            inputStream = this.buffer2;
        }
        else {
            inputStream = this.buffer1;
        }
        try {
            inputStream.reset();
        }
        catch (final IOException ex) {}
        super.bufferStartTime = System.currentTimeMillis() + this.len * 1000 / super.frequency + 70L;
        this.notifyAll();
        return inputStream;
    }
    
    public synchronized void run() {
        while (true) {
            Label_0173: {
                try {
                    this.wait();
                    break Label_0173;
                }
                catch (final Exception ex) {
                    ex.printStackTrace();
                    return;
                }
                try {
                    Thread.sleep(5L);
                }
                catch (final Exception ex2) {}
                this.nextTime2 = System.currentTimeMillis();
                this.mixMax = (int)(this.nextTime2 - this.nextTime1);
                this.nextTime1 = this.nextTime2;
                final long nextTime2 = this.nextTime2;
                System.arraycopy(this.mixtemp_Empty, 0, this.mixtemp, 0, this.len);
                this.mixable.mix(this, this.mixtemp, 0, this.mixtemp.length);
                byte[] array;
                if (this.whichBuffer == 0) {
                    array = this.bytebuf1;
                }
                else {
                    array = this.bytebuf2;
                }
                ULaw.encodeBlock(this.mixtemp, 0, array, 0, this.len);
                this.mixTime = (int)(System.currentTimeMillis() - nextTime2);
                this.mixCPU = this.mixTime / (float)this.mixMax;
                try {
                    this.wait();
                }
                catch (final Exception ex3) {
                    ex3.printStackTrace();
                    return;
                }
            }
            if (this.mixer == null) {
                System.out.println("run exit");
                return;
            }
            continue;
        }
    }
}
