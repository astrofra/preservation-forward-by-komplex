package muhmu.hifi.device;

import java.awt.Component;

public final class DeviceNoSound
extends MAD
implements Runnable {
    Mixable mixable;
    Thread mixer;
    int len = 1000;
    int[] buf;

    public boolean init(Mixable mixable, int n, int n2, int n3, Component component) {
        this.mixable = mixable;
        this.frequency = 1000;
        this.stereo = false;
        this.buf = new int[this.len];
        return true;
    }

    public synchronized void start() {
        this.stop();
        this.mixer = new Thread(this, "Muhmu NoSound device");
        this.mixer.start();
        this.mixer.setPriority(10);
    }

    public synchronized void stop() {
        Thread thread = this.mixer;
        this.mixer = null;
        if (thread != null) {
            thread.interrupt();
        }
    }

    public synchronized void run() {
        while (this.mixer != null) {
            this.bufferStartTime = System.currentTimeMillis() + 1000L;
            try {
                Thread.sleep(this.len * 1000 / this.frequency);
            }
            catch (InterruptedException interruptedException) {
                Thread.currentThread().interrupt();
                return;
            }
            this.mixable.mix(this, this.buf, 0, this.len);
        }
    }
}
