// 
// Decompiled by Procyon v0.6.0
// 

package muhmu.hifi.device;

import java.awt.Component;

public abstract class MAD
{
    public static final int MONO8 = 1;
    public static final int MONO16 = 2;
    public static final int STEREO8 = 3;
    public static final int STEREO16 = 4;
    public static final int DEVICE_IE4 = 1;
    public static final int DEVICE_IE3 = 2;
    public static final int DEVICE_SUN = 4;
    public static final int DEVICE_NOS = 8;
    public int frequency;
    public boolean stereo;
    public int boost;
    public static Component component;
    public long bufferStartTime;
    static String nameSun;
    static String nameIE3;
    static String nameIE4;
    
    public abstract boolean init(final Mixable p0, final int p1, final int p2, final int p3, final Component p4);
    
    public abstract void start();
    
    public abstract void stop();
    
    public static MAD getBestAudioDevice(final Mixable mixable) {
        return getBestAudioDevice(mixable, -1);
    }
    
    public static MAD getBestAudioDevice(final Mixable mixable, final int n) {
        if ((n & 0x1) != 0x0) {
            final MAD device_IE4 = getDevice_IE4();
            if (device_IE4 != null) {
                device_IE4.init(mixable, 22000, 4, 44100, MAD.component);
                return device_IE4;
            }
        }
        if ((n & 0x2) != 0x0) {
            final MAD device_IE5 = getDevice_IE3();
            if (device_IE5 != null) {
                device_IE5.init(mixable, 22000, 4, 44100, MAD.component);
                return device_IE5;
            }
        }
        if ((n & 0x4) != 0x0) {
            final MAD device_Sun = getDevice_Sun();
            if (device_Sun != null) {
                device_Sun.init(mixable, 8000, 1, 8000, MAD.component);
                return device_Sun;
            }
        }
        if ((n & 0x8) != 0x0) {
            final MAD device_NOS = getDevice_NOS();
            if (device_NOS != null) {
                device_NOS.init(mixable, 1000, 4, 1000, MAD.component);
                return device_NOS;
            }
        }
        return null;
    }
    
    public static MAD getDevice_IE4() {
        if (MAD.component == null) {
            System.err.println("Component not set!");
        }
        try {
            System.out.println("loading directsound IE4");
            return (MAD)Class.forName(MAD.nameIE4).newInstance();
        }
        catch (final Throwable t) {
            System.out.println("loading directsound IE4 failed");
            t.printStackTrace();
            return null;
        }
    }
    
    public static MAD getDevice_IE3() {
        if (MAD.component == null) {
            System.err.println("Component not set!");
        }
        try {
            System.out.println("loading directsound IE3");
            return (MAD)Class.forName(MAD.nameIE3).newInstance();
        }
        catch (final Throwable t) {
            System.out.println("loading directsound IE3 failed");
            t.printStackTrace();
            return null;
        }
    }
    
    public static MAD getDevice_Sun() {
        MAD mad = null;
        try {
            System.out.println("loading sun.audio");
            mad = (MAD)Class.forName(MAD.nameSun).newInstance();
            return mad;
        }
        catch (final Throwable t) {
            t.printStackTrace();
            System.out.println("loading sun.audio failed");
            return mad;
        }
    }
    
    public static MAD getDevice_NOS() {
        return new DeviceNoSound();
    }
    
    public MAD() {
        this.boost = 128;
    }
    
    static {
        MAD.nameSun = "muhmu.hifi.device.DeviceSun";
        MAD.nameIE3 = "muhmu.hifi.device.DeviceMS_IE3";
        MAD.nameIE4 = "muhmu.hifi.device.DeviceMS_IE4";
    }
}
