// 
// Decompiled by Procyon v0.6.0
// 

package muhmu.hifi.device;

import java.awt.Component;

public final class DeviceNoSound extends MAD implements Runnable
{
    Mixable mixable;
    Thread mixer;
    int len;
    int[] buf;
    static final long lag = 1000L;
    
    public boolean init(final Mixable mixable, final int n, final int n2, final int n3, final Component component) {
        this.mixable = mixable;
        super.frequency = 1000;
        super.stereo = false;
        this.buf = new int[this.len];
        return true;
    }
    
    public synchronized void start() {
        this.stop();
        (this.mixer = new Thread(this, "Muhmu SunAudio device")).start();
        this.mixer.setPriority(10);
    }
    
    public synchronized void stop() {
        if (this.mixer != null) {
            final Thread mixer = this.mixer;
            this.mixer = null;
            mixer.stop();
        }
    }
    
    public synchronized void run() {
        while (this.mixer != null) {
            super.bufferStartTime = System.currentTimeMillis() + 1000L;
            try {
                Thread.sleep(this.len * 1000 / super.frequency);
            }
            catch (final Exception ex) {}
            this.mixable.mix(this, this.buf, 0, this.len);
        }
    }
    
    public DeviceNoSound() {
        this.len = 1000;
    }
}
