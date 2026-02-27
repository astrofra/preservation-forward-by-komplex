/*
 * Decompiled with CFR 0.152.
 */
package muhmu.hifi.device;

import java.awt.Component;
import muhmu.hifi.device.DeviceNoSound;
import muhmu.hifi.device.Mixable;

public abstract class MAD {
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
    public int boost = 128;
    public static Component component;
    public long bufferStartTime;
    static String nameSun;
    static String nameIE3;
    static String nameIE4;

    public abstract boolean init(Mixable var1, int var2, int var3, int var4, Component var5);

    public abstract void start();

    public abstract void stop();

    public static MAD getBestAudioDevice(Mixable mixable) {
        return MAD.getBestAudioDevice(mixable, -1);
    }

    public static MAD getBestAudioDevice(Mixable mixable, int n) {
        MAD mAD;
        if ((n & 1) != 0 && (mAD = MAD.getDevice_IE4()) != null) {
            mAD.init(mixable, 22000, 4, 44100, component);
            return mAD;
        }
        if ((n & 2) != 0 && (mAD = MAD.getDevice_IE3()) != null) {
            mAD.init(mixable, 22000, 4, 44100, component);
            return mAD;
        }
        if ((n & 4) != 0 && (mAD = MAD.getDevice_Sun()) != null) {
            mAD.init(mixable, 8000, 1, 8000, component);
            return mAD;
        }
        if ((n & 8) != 0 && (mAD = MAD.getDevice_NOS()) != null) {
            mAD.init(mixable, 1000, 4, 1000, component);
            return mAD;
        }
        return null;
    }

    public static MAD getDevice_IE4() {
        if (component == null) {
            System.err.println("Component not set!");
        }
        MAD mAD = null;
        try {
            System.out.println("loading directsound IE4");
            Class<?> clazz = Class.forName(nameIE4);
            mAD = (MAD)clazz.newInstance();
            return mAD;
        }
        catch (Throwable throwable) {
            System.out.println("loading directsound IE4 failed");
            throwable.printStackTrace();
            return null;
        }
    }

    public static MAD getDevice_IE3() {
        if (component == null) {
            System.err.println("Component not set!");
        }
        MAD mAD = null;
        try {
            System.out.println("loading directsound IE3");
            Class<?> clazz = Class.forName(nameIE3);
            mAD = (MAD)clazz.newInstance();
            return mAD;
        }
        catch (Throwable throwable) {
            System.out.println("loading directsound IE3 failed");
            throwable.printStackTrace();
            return null;
        }
    }

    public static MAD getDevice_Sun() {
        MAD mAD = null;
        try {
            System.out.println("loading sun.audio");
            Class<?> clazz = Class.forName(nameSun);
            mAD = (MAD)clazz.newInstance();
            return mAD;
        }
        catch (Throwable throwable) {
            throwable.printStackTrace();
            System.out.println("loading sun.audio failed");
            return mAD;
        }
    }

    public static MAD getDevice_NOS() {
        DeviceNoSound deviceNoSound = null;
        deviceNoSound = new DeviceNoSound();
        return deviceNoSound;
    }

    static {
        nameSun = "muhmu.hifi.device.DeviceSun";
        nameIE3 = "muhmu.hifi.device.DeviceMS_IE3";
        nameIE4 = "muhmu.hifi.device.DeviceMS_IE4";
    }
}
