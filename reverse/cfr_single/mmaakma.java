/*
 * Decompiled with CFR 0.152.
 */
import java.applet.Applet;
import java.awt.Image;
import java.awt.Toolkit;
import java.awt.image.ImageProducer;
import java.io.DataInputStream;
import java.io.InputStream;
import java.net.URL;

public final class mmaakma {
    static public Toolkit majaKkA;
    static public Applet MAjaKkA;
    static final public byte[] mAjaKkA;

    public static final int MajaKkA(double d, double d2, double d3, double[] dArray) {
        if (d == 0.0) {
            if (d2 == 0.0) {
                if (d3 == 0.0) {
                    return -1;
                }
                return 0;
            }
            dArray[0] = -d3 / d2;
            return 1;
        }
        double d4 = d2 * d2 - 4.0 * d * d3;
        if (d4 < 0.0) {
            return 0;
        }
        if (d4 == 0.0) {
            dArray[0] = -d2 / (2.0 * d);
            return 1;
        }
        d4 = Math.sqrt(d4);
        dArray[0] = (-d2 - d4) / (2.0 * d);
        dArray[1] = (-d2 + d4) / (2.0 * d);
        return 2;
    }

    public static final int MaJAKkA(double d, double d2, double d3, double d4, double d5, double[] dArray) {
        double d6 = d * d + d3 / d4;
        double d7 = 2.0 * d * d2;
        double d8 = d2 * d2 - d5 * d5 / d4;
        return mmaakma.MajaKkA(d6, d7, d8, dArray);
    }

    public static final synchronized mmajkka majaKkA(URL uRL) {
        Image image = MAjaKkA == null ? Toolkit.getDefaultToolkit().getImage(uRL) : MAjaKkA.getImage(uRL);
        if (image == null) {
            System.err.println("couldn't fetch image " + uRL);
            return null;
        }
        mmjamka mmjamka2 = new mmjamka();
        ImageProducer imageProducer = image.getSource();
        if (imageProducer == null) {
            System.err.println("couldn't fetch image " + uRL);
            return null;
        }
        imageProducer.startProduction(mmjamka2);
        mmjamka2.KaMAJak();
        image = null;
        if (!mmjamka2.kAmaJak()) {
            System.err.println("error fetching image " + uRL);
            return null;
        }
        return mmjamka2.KamaJAk;
    }

    public static final mmajkka MAJaKkA(URL uRL) {
        try {
            InputStream inputStream = uRL.openConnection().getInputStream();
            DataInputStream dataInputStream = new DataInputStream(inputStream);
            dataInputStream.skip(8L);
            int n = dataInputStream.read();
            int n2 = dataInputStream.read();
            int n3 = dataInputStream.read();
            int n4 = dataInputStream.read();
            int n5 = dataInputStream.read();
            int n6 = dataInputStream.read();
            int n7 = (n << 8) + n2;
            int n8 = (n3 << 8) + n4;
            int n9 = (n5 << 8) + n6;
            System.out.println("w=" + n7 + " h=" + n8 + " c=" + n9);
            dataInputStream.skip(18L);
            if (n9 == 0) {
                byte[] byArray = new byte[n7 * 3];
                int[] nArray = new int[n7 * n8];
                int n10 = 0;
                int n11 = 0;
                while (n11 < n8) {
                    dataInputStream.readFully(byArray);
                    int n12 = 0;
                    int n13 = 0;
                    while (n13 < n7) {
                        nArray[n10++] = 0xFF000000 | (byArray[n12++] << 16 & 0xFF0000 | byArray[n12++] << 8 & 0xFF00 | byArray[n12++] & 0xFF);
                        ++n13;
                    }
                    ++n11;
                }
                mmaamma mmaamma2 = new mmaamma(n7, n8, nArray);
                nArray = null;
                return mmaamma2;
            }
            byte[] byArray = new byte[n9 * 3];
            byte[] byArray2 = new byte[n7 * n8];
            dataInputStream.readFully(byArray);
            dataInputStream.readFully(byArray2);
            byte[] byArray3 = new byte[256];
            byte[] byArray4 = new byte[256];
            byte[] byArray5 = new byte[256];
            int n14 = 0;
            while (n14 < n9) {
                byArray3[n14] = byArray[n14 * 3];
                byArray4[n14] = byArray[n14 * 3 + 1];
                byArray5[n14] = byArray[n14 * 3 + 2];
                ++n14;
            }
            kmajkka kmajkka2 = new kmajkka(n7, n8, byArray2, byArray3, byArray4, byArray5);
            byArray5 = null;
            byArray4 = null;
            byArray3 = null;
            byArray2 = null;
            return kmajkka2;
        }
        catch (Exception exception) {
            exception.printStackTrace();
            return null;
        }
    }

    public static final int maJaKkA(int n, int n2, int n3, int n4, int n5, int n6, float f) {
        float f2 = n & 0xFF;
        float f3 = n2 & 0xFF;
        float f4 = n3 & 0xFF;
        float f5 = n4 & 0xFF;
        float f6 = n5 & 0xFF;
        float f7 = n6 & 0xFF;
        return mmaakma.MAjaKkA(f2, f3, f4, f5, f6, f7, f);
    }

    public static final int MAjaKkA(float f, float f2, float f3, float f4, float f5, float f6, float f7) {
        float f8 = 1.0f - f7;
        int n = (int)(f7 * f + f8 * f4) & 0xFF;
        int n2 = (int)(f7 * f2 + f8 * f5) & 0xFF;
        int n3 = (int)(f7 * f3 + f8 * f6) & 0xFF;
        return n << 16 | n2 << 8 | n3;
    }

    public static final int mAJaKkA(double d) {
        return (int)d;
    }

    public static final int mAjaKkA(float f) {
        return (int)f;
    }

    static {
        byte[] byArray = new byte[8];
        byArray[0] = 109;
        byArray[1] = 104;
        byArray[2] = 119;
        byArray[3] = 97;
        byArray[4] = 110;
        byArray[5] = 104;
        byArray[7] = 4;
        mAjaKkA = byArray;
    }
}
