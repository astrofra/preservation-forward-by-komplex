/*
 * Decompiled with CFR 0.152.
 * 
 * Could not load the following classes:
 *  sun.audio.AudioData
 *  sun.audio.AudioDataStream
 *  sun.audio.AudioPlayer
 *  sun.audio.AudioStreamSequence
 */
package muhmu.hifi.device;

import java.awt.Component;
import java.io.IOException;
import java.io.InputStream;
import java.util.Enumeration;
import muhmu.hifi.device.MAD;
import muhmu.hifi.device.Mixable;
import muhmu.hifi.device.ULaw;
import sun.audio.AudioData;
import sun.audio.AudioDataStream;
import sun.audio.AudioPlayer;
import sun.audio.AudioStreamSequence;

public final class DeviceSun
extends MAD
implements Enumeration,
Runnable {
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
    final static long lag = 70L;

    public boolean init(Mixable mixable, int n, int n2, int n3, Component component) {
        this.mixable = mixable;
        this.len = n * 8000 / n3;
        this.len = 1600;
        this.frequency = 8000;
        this.stereo = false;
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
        this.mixtemp_Empty = new int[this.len];
        System.arraycopy(this.mixtemp_Empty, 0, this.mixtemp, 0, this.mixtemp.length);
        AudioData audioData = new AudioData(this.bytebuf1);
        AudioData audioData2 = new AudioData(this.bytebuf2);
        this.buffer1 = new AudioDataStream(audioData);
        this.buffer2 = new AudioDataStream(audioData2);
        this.clearBuffers();
    }

    void clearBuffers() {
        if (this.bytebuf1 != null) {
            int n = 0;
            while (n < this.len) {
                this.bytebuf1[n] = -1;
                this.bytebuf2[n] = -1;
                this.mixtemp[n] = 0;
                ++n;
            }
        }
    }

    public synchronized void start() {
        this.stop();
        this.mixer = new Thread((Runnable)this, "Muhmu SunAudio device");
        this.mixer.start();
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
            Thread thread = this.mixer;
            this.mixer = null;
            thread.stop();
            thread = null;
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
        this.whichBuffer = this.whichBuffer + 1 & 1;
        InputStream inputStream = this.whichBuffer == 0 ? this.buffer2 : this.buffer1;
        try {
            inputStream.reset();
        }
        catch (IOException iOException) {}
        this.bufferStartTime = System.currentTimeMillis() + (long)(this.len * 1000 / this.frequency) + 70L;
        this.notifyAll();
        return inputStream;
    }

    /*
     * Unable to fully structure code
     */
    public synchronized void run() {
        try {
            this.wait();
            if (true) ** GOTO lbl30
        }
        catch (Exception var1_1) {
            var1_1.printStackTrace();
            return;
        }
        do {
            try {
                Thread.sleep(5L);
            }
            catch (Exception v0) {}
            this.nextTime2 = System.currentTimeMillis();
            this.mixMax = (int)(this.nextTime2 - this.nextTime1);
            this.nextTime1 = this.nextTime2;
            var1_2 = this.nextTime2;
            System.arraycopy(this.mixtemp_Empty, 0, this.mixtemp, 0, this.len);
            this.mixable.mix(this, this.mixtemp, 0, this.mixtemp.length);
            var3_3 = this.whichBuffer == 0 ? this.bytebuf1 : this.bytebuf2;
            ULaw.encodeBlock(this.mixtemp, 0, var3_3, 0, this.len);
            var4_4 = System.currentTimeMillis();
            this.mixTime = (int)(var4_4 - var1_2);
            this.mixCPU = (float)this.mixTime / (float)this.mixMax;
            try {
                this.wait();
            }
            catch (Exception var6_5) {
                var6_5.printStackTrace();
                return;
            }
lbl30:
            // 2 sources

        } while (this.mixer != null);
        System.out.println("run exit");
    }
}
