/*
 * Decompiled with CFR 0.152.
 */
import java.awt.image.IndexColorModel;

public final class kmajkka
extends mmajkka {
    public byte[] aMAjakK;
    public byte[] AmajakK;
    public byte[] amajakK;
    public byte[] AMajakK;
    public final byte[] aMajakK = new byte[256];
    public final byte[] AmAJakK = new byte[256];
    public final byte[] amAJakK = new byte[256];
    public final int[] AMAJakK = new int[256];
    int aMAJakK;
    final byte[] AmaJakK = new byte[256];
    final static byte[] amaJakK = new byte[768];

    public kmajkka(int n, int n2, byte[] byArray, byte[] byArray2, byte[] byArray3, byte[] byArray4) {
        this(n, n2, 1, false);
        this.amaJAkk(byArray, byArray2, byArray3, byArray4);
    }

    public kmajkka(int n, int n2, int n3, boolean bl) {
        super(n, n2);
        this.aMAJakK = n3;
        if (this.aMAJakK == 1) {
            this.AMajakK = new byte[this.amAjakK * this.AMAjakK];
            this.amajakK = this.AMajakK;
            this.aMAjakK = this.amajakK;
            this.AmajakK = this.AMajakK;
        }
        if (this.aMAJakK == 2) {
            this.amajakK = new byte[this.amAjakK * this.AMAjakK];
            this.AMajakK = new byte[this.amAjakK * this.AMAjakK];
            this.aMAjakK = this.amajakK;
            this.AmajakK = this.AMajakK;
        }
        int n4 = 0;
        while (n4 < this.amAjakK * this.AMAjakK) {
            this.aMAjakK[n4] = (byte)n4;
            ++n4;
        }
        int n5 = 0;
        while (n5 < 256) {
            this.aMajakK[n5] = (byte)Math.min(255.0, (double)(n5 * 2));
            this.AmAJakK[n5] = (byte)Math.min(255.0, (double)(n5 * 3));
            this.amAJakK[n5] = (byte)Math.min(255.0, (double)n5);
            ++n5;
        }
        this.AmAJakk(this.aMajakK, this.AmAJakK, this.amAJakK);
        this.AMajAkk(bl);
    }

    public final void AMaJakk(kmajkka kmajkka2) {
        this.amAJakk((IndexColorModel)kmajkka2.AmAjakK);
    }

    public final void amAJakk(IndexColorModel indexColorModel) {
        int n = 0;
        while (n < 256) {
            this.aMajakK[n] = 0;
            this.AmAJakK[n] = 0;
            this.amAJakK[n] = 0;
            ++n;
        }
        indexColorModel.getReds(this.aMajakK);
        indexColorModel.getGreens(this.AmAJakK);
        indexColorModel.getBlues(this.amAJakK);
        this.aMaJAkk();
    }

    public final void AmAJakk(byte[] byArray, byte[] byArray2, byte[] byArray3) {
        System.arraycopy(byArray, 0, this.aMajakK, 0, 256);
        System.arraycopy(byArray2, 0, this.AmAJakK, 0, 256);
        System.arraycopy(byArray3, 0, this.amAJakK, 0, 256);
        this.aMaJAkk();
    }

    public final void aMaJAkk() {
        this.AmAjakK = new IndexColorModel(8, 256, this.aMajakK, this.AmAJakK, this.amAJakK);
        int n = 0;
        while (n < 256) {
            this.AMAJakK[n] = ((this.aMajakK[n] & 0xFF) << 20) + ((this.AmAJakK[n] & 0xFF) << 10) + (this.amAJakK[n] & 0xFF);
            ++n;
        }
    }

    public final void amaJAkk(byte[] byArray, byte[] byArray2, byte[] byArray3, byte[] byArray4) {
        System.arraycopy(byArray, 0, this.aMAjakK, 0, Math.min(byArray.length, this.aMAjakK.length));
        System.arraycopy(byArray2, 0, this.aMajakK, 0, Math.min(byArray2.length, this.aMajakK.length));
        System.arraycopy(byArray3, 0, this.AmAJakK, 0, Math.min(byArray3.length, this.AmAJakK.length));
        System.arraycopy(byArray4, 0, this.amAJakK, 0, Math.min(byArray4.length, this.amAJakK.length));
        this.aMaJAkk();
    }

    public final void Amajakk() {
        if (this.aMAjakK == this.amajakK) {
            this.aMAjakK = this.AMajakK;
            this.AmajakK = this.amajakK;
            return;
        }
        this.aMAjakK = this.amajakK;
        this.AmajakK = this.AMajakK;
    }

    public final void AmajAkk(int n, int n2, int n3, int n4) {
        if (this.AMaJAkK != null) {
            this.AMaJAkK.setPixels(n, n2, n3, n4, this.AmAjakK, this.aMAjakK, n2 * this.amAjakK + n, this.amAjakK);
            this.AMaJAkK.imageComplete(3);
        }
    }

    public final void amAjakk(int n) {
        byte by = (byte)n;
        int n2 = this.amAjakK * this.AMAjakK;
        byte[] byArray = this.amajakK;
        byte[] byArray2 = this.AMajakK;
        int n3 = 0;
        while (n3 < n2) {
            byArray[n3] = by;
            byArray2[n3++] = by;
        }
    }

    public final void AmaJAkk() {
        int n = this.amAjakK - 1 - 3;
        int n2 = this.AMAjakK - 1;
        byte[] byArray = this.AmajakK;
        int n3 = 1;
        while (n3 < n2) {
            int n4 = (n3 - 1) * this.amAjakK;
            int n5 = n3 * this.amAjakK;
            int n6 = (n3 + 1) * this.amAjakK;
            int n7 = this.aMAjakK[n4++] & 0xFF;
            int n8 = this.aMAjakK[n4++] & 0xFF;
            int n9 = this.aMAjakK[n4++] & 0xFF;
            int n10 = this.aMAjakK[n5++] & 0xFF;
            int n11 = this.aMAjakK[n5++] & 0xFF;
            int n12 = this.aMAjakK[n5++] & 0xFF;
            int n13 = this.aMAjakK[n6++] & 0xFF;
            int n14 = this.aMAjakK[n6++] & 0xFF;
            int n15 = this.aMAjakK[n6++] & 0xFF;
            int n16 = n7 + n8 + n9 + n10 + n11 + n12 + n13 + n14 + n15;
            int n17 = n3 * this.amAjakK + 1;
            int n18 = 1;
            while (n18 < n) {
                byArray[n17++] = (byte)((n16 + 1) / 9);
                n16 -= n7 + n10 + n13;
                n7 = this.aMAjakK[n4++] & 0xFF;
                n10 = this.aMAjakK[n5++] & 0xFF;
                n13 = this.aMAjakK[n6++] & 0xFF;
                byArray[n17++] = (byte)((n16 += n7 + n10 + n13) / 9);
                n16 -= n8 + n11 + n14;
                n8 = this.aMAjakK[n4++] & 0xFF;
                n11 = this.aMAjakK[n5++] & 0xFF;
                n14 = this.aMAjakK[n6++] & 0xFF;
                byArray[n17++] = (byte)((n16 += n8 + n11 + n14) / 9);
                n16 -= n9 + n12 + n15;
                n9 = this.aMAjakK[n4++] & 0xFF;
                n12 = this.aMAjakK[n5++] & 0xFF;
                n15 = this.aMAjakK[n6++] & 0xFF;
                n16 += n9 + n12 + n15;
                n18 += 3;
            }
            ++n3;
        }
        this.Amajakk();
    }

    public final void AMAjakk() {
        int n = this.amAjakK - 1;
        int n2 = this.AMAjakK - 1;
        byte[] byArray = this.AmajakK;
        int n3 = 1;
        while (n3 < n2) {
            int n4 = (n3 - 1) * this.amAjakK + 1;
            int n5 = n3 * this.amAjakK - n4;
            int n6 = n3 * this.amAjakK + 2 - n4;
            int n7 = (n3 + 1) * this.amAjakK + 1 - n4;
            int n8 = n3 * this.amAjakK + 1;
            int n9 = 1;
            while (n9 < n) {
                int n10 = this.aMAjakK[n4] & 0xFF;
                n10 += this.aMAjakK[n4 + n5] & 0xFF;
                n10 += this.aMAjakK[n4 + n6] & 0xFF;
                byArray[n8] = (byte)((n10 += this.aMAjakK[n4 + n7] & 0xFF) >>> 2);
                ++n4;
                ++n8;
                ++n9;
            }
            ++n3;
        }
        this.Amajakk();
    }

    public final void AMAJakk(int n, int n2, int n3, int n4, int n5, int n6) {
        if (n < 0 || n2 < 0) {
            return;
        }
        if (n + n5 > this.amAjakK || n2 + n6 > this.AMAjakK) {
            return;
        }
        byte[] byArray = this.AmajakK;
        byte[] byArray2 = this.aMAjakK;
        int n7 = n2 * this.amAjakK + n;
        int n8 = n4 * this.amAjakK + n3;
        int n9 = this.amAjakK - n5;
        int n10 = 0;
        while (n10 < n6) {
            int n11 = 0;
            while (n11 < n5) {
                byArray2[n8++] = byArray[n7++];
                ++n11;
            }
            n8 += n9;
            n7 += n9;
            ++n10;
        }
    }

    public final void amAJAkk() {
        byte[] byArray = this.AmajakK;
        byte[] byArray2 = this.aMAjakK;
        int n = this.amAjakK * this.AMAjakK;
        int n2 = 0;
        while (n2 < n) {
            byArray2[n2] = (byte)((byArray[n2] & 0xFF) + (byArray2[n2] & 0xFF) >> 1);
            ++n2;
        }
    }

    public final void amajakk(double d) {
        double d2 = this.amAjakK / 2;
        double d3 = this.AMAjakK / 2;
        this.aMAjakk(d, d2, d3);
    }

    public final void aMAjakk(double d, double d2, double d3) {
        double d4 = 1.0 / d;
        this.AMAJAkk(this.aMAjakK, d4, 0.0, 0.0, d4, d2, d3);
    }

    public final void aMAJAkk(double d, double d2) {
        double d3 = this.amAjakK / 2;
        double d4 = this.AMAjakK / 2;
        this.amaJakk(d, d2, d3, d4);
    }

    public final void amaJakk(double d, double d2, double d3, double d4) {
        double d5 = 1.0 / d;
        this.AMAJAkk(this.aMAjakK, d5 * Math.cos(d2), d5 * Math.sin(d2), d5 * Math.cos(d2 + 1.5707963267948966), d5 * Math.sin(d2 + 1.5707963267948966), d3, d4);
    }

    public final void AmAjakk(double d, double d2, double d3, double d4, double d5, double d6) {
        this.AMAJAkk(this.aMAjakK, d, d2, d3, d4, d5, d6);
    }

    public final void AMAJAkk(byte[] byArray, double d, double d2, double d3, double d4, double d5, double d6) {
        byte[] byArray2 = this.AmajakK;
        int n = (int)(d * 65536.0);
        int n2 = (int)(d2 * 65536.0);
        int n3 = (int)(d3 * 65536.0);
        int n4 = (int)(d4 * 65536.0);
        int n5 = (int)(-(d5 * d + d6 * d3) * 65536.0);
        int n6 = (int)(-(d5 * d2 + d6 * d4) * 65536.0);
        n5 += (int)(d5 * 65536.0);
        n6 += (int)(d6 * 65536.0);
        int n7 = 0;
        if (this.amAjakK == 256 && this.AMAjakK == 128) {
            int n8 = 0;
            while (n8 < this.AMAjakK) {
                int n9 = n5;
                int n10 = n6;
                int n11 = n7 + this.amAjakK;
                while (n7 < n11) {
                    byArray2[n7] = byArray[n9 << 8 >>> 24 | n10 << 9 >>> 25 << 8];
                    ++n7;
                    n9 += n;
                    n10 += n2;
                }
                n5 += n3;
                n6 += n4;
                ++n8;
            }
        } else if (this.amAjakK == 512 && this.AMAjakK == 256) {
            int n12 = 0;
            while (n12 < this.AMAjakK) {
                int n13 = n5;
                int n14 = n6;
                int n15 = n7 + this.amAjakK;
                while (n7 < n15) {
                    byArray2[n7] = byArray[n13 << 7 >>> 23 | n14 << 8 >>> 24 << 9];
                    ++n7;
                    n13 += n;
                    n14 += n2;
                }
                n5 += n3;
                n6 += n4;
                ++n12;
            }
        } else if (this.amAjakK == 256 && this.AMAjakK == 256) {
            int n16 = 0;
            while (n16 < this.AMAjakK) {
                int n17 = n5;
                int n18 = n6;
                int n19 = n7 + this.amAjakK;
                while (n7 < n19) {
                    byArray2[n7] = byArray[n17 << 8 >>> 24 | n18 << 8 >>> 24 << 8];
                    ++n7;
                    n17 += n;
                    n18 += n2;
                }
                n5 += n3;
                n6 += n4;
                ++n16;
            }
        }
        this.Amajakk();
    }

    /*
     * Unable to fully structure code
     */
    public final void AMajakk(int var1_1) {
        var2_2 = this.aMAjakK;
        var3_3 = this.amAjakK * this.AMAjakK;
        var4_4 = 0;
        switch (var3_3 & 7) {
            case 7: {
                var2_2[var4_4] = (byte)((var2_2[var4_4] & 255) >>> var1_1);
                ++var4_4;
            }
            case 6: {
                var2_2[var4_4] = (byte)((var2_2[var4_4] & 255) >>> var1_1);
                ++var4_4;
            }
            case 5: {
                var2_2[var4_4] = (byte)((var2_2[var4_4] & 255) >>> var1_1);
                ++var4_4;
            }
            case 4: {
                var2_2[var4_4] = (byte)((var2_2[var4_4] & 255) >>> var1_1);
                ++var4_4;
            }
            case 3: {
                var2_2[var4_4] = (byte)((var2_2[var4_4] & 255) >>> var1_1);
                ++var4_4;
            }
            case 2: {
                var2_2[var4_4] = (byte)((var2_2[var4_4] & 255) >>> var1_1);
                ++var4_4;
            }
            case 1: {
                var2_2[var4_4] = (byte)((var2_2[var4_4] & 255) >>> var1_1);
                ++var4_4;
                ** GOTO lbl39
            }
            default: {
                if (var4_4 < var3_3) ** continue;
            }
        }
        while (true) lbl-1000:
        // 2 sources

        {
            var2_2[var4_4] = (byte)((var2_2[var4_4] & 255) >>> var1_1);
            var2_2[++var4_4] = (byte)((var2_2[var4_4] & 255) >>> var1_1);
            var2_2[++var4_4] = (byte)((var2_2[var4_4] & 255) >>> var1_1);
            var2_2[++var4_4] = (byte)((var2_2[var4_4] & 255) >>> var1_1);
            var2_2[++var4_4] = (byte)((var2_2[var4_4] & 255) >>> var1_1);
            var2_2[++var4_4] = (byte)((var2_2[var4_4] & 255) >>> var1_1);
            var2_2[++var4_4] = (byte)((var2_2[var4_4] & 255) >>> var1_1);
            var2_2[++var4_4] = (byte)((var2_2[var4_4] & 255) >>> var1_1);
            ++var4_4;
lbl39:
            // 2 sources

            if (var4_4 < var3_3) ** GOTO lbl-1000
            break;
        }
    }

    /*
     * Unable to fully structure code
     */
    public final void aMajakk(int var1_1) {
        var2_2 = this.AmaJakK;
        var3_3 = this.aMAjakK;
        var4_4 = var1_1 < -256 ? -256 : (var1_1 < 256 ? var1_1 : 256);
        System.arraycopy(kmajkka.amaJakK, var4_4 + 256, var2_2, 128, 128);
        System.arraycopy(kmajkka.amaJakK, var4_4 + 384, var2_2, 0, 128);
        var5_5 = this.amAjakK * this.AMAjakK;
        var6_6 = 0;
        switch (var5_5 & 7) {
            case 7: {
                var3_3[var6_6] = var2_2[var3_3[var6_6] + 128];
                ++var6_6;
            }
            case 6: {
                var3_3[var6_6] = var2_2[var3_3[var6_6] + 128];
                ++var6_6;
            }
            case 5: {
                var3_3[var6_6] = var2_2[var3_3[var6_6] + 128];
                ++var6_6;
            }
            case 4: {
                var3_3[var6_6] = var2_2[var3_3[var6_6] + 128];
                ++var6_6;
            }
            case 3: {
                var3_3[var6_6] = var2_2[var3_3[var6_6] + 128];
                ++var6_6;
            }
            case 2: {
                var3_3[var6_6] = var2_2[var3_3[var6_6] + 128];
                ++var6_6;
            }
            case 1: {
                var3_3[var6_6] = var2_2[var3_3[var6_6] + 128];
                ++var6_6;
                ** GOTO lbl43
            }
            default: {
                if (var6_6 < var5_5) ** continue;
            }
        }
        while (true) lbl-1000:
        // 2 sources

        {
            var3_3[var6_6] = var2_2[var3_3[var6_6] + 128];
            var3_3[++var6_6] = var2_2[var3_3[var6_6] + 128];
            var3_3[++var6_6] = var2_2[var3_3[var6_6] + 128];
            var3_3[++var6_6] = var2_2[var3_3[var6_6] + 128];
            var3_3[++var6_6] = var2_2[var3_3[var6_6] + 128];
            var3_3[++var6_6] = var2_2[var3_3[var6_6] + 128];
            var3_3[++var6_6] = var2_2[var3_3[var6_6] + 128];
            var3_3[++var6_6] = var2_2[var3_3[var6_6] + 128];
            ++var6_6;
lbl43:
            // 2 sources

            if (var6_6 < var5_5) ** GOTO lbl-1000
            break;
        }
    }

    public void AMaJAkk(kmajkka kmajkka2, int n, int n2) {
        this.aMAJakk(kmajkka2, n, n2, 0, 0, kmajkka2.amAjakK, kmajkka2.AMAjakK);
    }

    public void aMAJakk(kmajkka kmajkka2, int n, int n2, int n3, int n4, int n5, int n6) {
        if (n < 0) {
            n5 += n;
            n3 -= n;
            n = 0;
        }
        if (n5 <= 0) {
            return;
        }
        if (n + n5 > this.amAjakK) {
            n5 = this.amAjakK - n;
        }
        if (n5 <= 0) {
            return;
        }
        if (n2 < 0) {
            n6 += n2;
            n4 -= n2;
            n2 = 0;
        }
        if (n6 <= 0) {
            return;
        }
        if (n2 + n6 > this.AMAjakK) {
            n6 = this.AMAjakK - n2;
        }
        if (n6 <= 0) {
            return;
        }
        int n7 = n4 * kmajkka2.amAjakK + n3;
        int n8 = n2 * this.amAjakK + n;
        byte[] byArray = kmajkka2.aMAjakK;
        byte[] byArray2 = this.aMAjakK;
        if (kmajkka2.amAjakK == this.amAjakK && this.amAjakK == n5) {
            System.arraycopy(byArray, n7, byArray2, n8, n5 * n6);
            return;
        }
        int n9 = 0;
        while (n9 < n6) {
            System.arraycopy(byArray, n7, byArray2, n8, n5);
            n7 += kmajkka2.amAjakK;
            n8 += this.amAjakK;
            ++n9;
        }
    }

    public final void AmaJakk(mmaamma mmaamma2, byte by) {
        int[] nArray = mmaamma2.MAJakKa;
        byte[] byArray = this.aMAjakK;
        int n = this.amAjakK * this.AMAjakK;
        int n2 = 0;
        while (n2 < n) {
            if (nArray[n2] < 0) {
                byArray[n2] = by;
            }
            ++n2;
        }
    }

    static {
        int n = 0;
        while (n < 768) {
            int n2 = n - 256;
            int n3 = n2 < 0 ? 0 : (n2 < 256 ? n2 : 255);
            kmajkka.amaJakK[n] = (byte)n3;
            ++n;
        }
    }
}
