/*
 * Decompiled with CFR 0.152.
 */
package muhmu.hifi.device;

import java.awt.Component;
import muhmu.hifi.device.MAD;
import muhmu.hifi.device.Mixable;

public final class DeviceNoSound
extends MAD
implements Runnable {
    Mixable mixable;
    Thread mixer;
    int len = 1000;
    int[] buf;
    final static long lag = 1000L;

    public boolean init(Mixable mixable, int n, int n2, int n3, Component component) {
        this.mixable = mixable;
        this.frequency = 1000;
        this.stereo = false;
        this.buf = new int[this.len];
        return true;
    }

    public synchronized void start() {
        this.stop();
        this.mixer = new Thread((Runnable)this, "Muhmu SunAudio device");
        this.mixer.start();
        this.mixer.setPriority(10);
    }

    public synchronized void stop() {
        if (this.mixer != null) {
            Thread thread = this.mixer;
            this.mixer = null;
            thread.stop();
            thread = null;
        }
    }

    public synchronized void run() {
        while (this.mixer != null) {
            this.bufferStartTime = System.currentTimeMillis() + 1000L;
            try {
                Thread.sleep(this.len * 1000 / this.frequency);
            }
            catch (Exception exception) {}
            this.mixable.mix(this, this.buf, 0, this.len);
        }
    }
}
