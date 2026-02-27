/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Rectangle;
import java.awt.image.DirectColorModel;

public final class mmaamma
extends mmajkka {
    public int[] MaJakKa;
    public int[] maJakKa;
    public int[] MAJakKa;
    public int[] mAJakKa;
    static final int MajakKa = 32537631;
    static final int majakKa = 260301048;
    static final int MAjakKa = 0xFF3FCFF;

    public mmaamma(int n, int n2, int[] nArray) {
        this(n, n2, 1, false);
        this.AmaJakK(nArray);
    }

    public mmaamma(int n, int n2, int n3, boolean bl) {
        super(n, n2);
        if (n3 == 1) {
            this.maJakKa = new int[this.amAjakK * this.AMAjakK];
            this.MaJakKa = this.maJakKa;
            this.MAJakKa = this.MaJakKa;
            this.mAJakKa = this.maJakKa;
        }
        if (n3 == 2) {
            this.MaJakKa = new int[this.amAjakK * this.AMAjakK];
            this.maJakKa = new int[this.amAjakK * this.AMAjakK];
            this.MAJakKa = this.MaJakKa;
            this.mAJakKa = this.maJakKa;
        }
        this.AmAjakK = new DirectColorModel(28, 0xFF00000, 261120, 255);
        this.AMajAkk(bl);
    }

    public final void AmajAkk(int n, int n2, int n3, int n4) {
        this.AMaJAkK.setPixels(n, n2, n3, n4, this.AmAjakK, this.MAJakKa, n2 * this.amAjakK + n, this.amAjakK);
        this.AMaJAkK.imageComplete(3);
    }

    public final void AMAjAKK() {
        this.AmaJakK(this.MAJakKa);
    }

    public final void AmaJakK(int[] nArray) {
        int[] nArray2 = this.MAJakKa;
        int n = Math.min(this.MAJakKa.length, nArray.length);
        int n2 = 0;
        while (n2 < n) {
            int n3 = nArray[n2];
            nArray2[n2] = n3 & 0xFF | (n3 & 0xFF00) << 2 | (n3 & 0xFF0000) << 4;
            ++n2;
        }
    }

    public static final int AMajAKK(int n) {
        return n & 0xFF | (n & 0xFF00) << 2 | (n & 0xFF0000) << 4;
    }

    public static final int aMAJAKK(int n, int n2, int n3) {
        return ((n & 0xFF) << 20) + ((n2 & 0xFF) << 10) + (n3 & 0xFF);
    }

    public final void amaJakK() {
        if (this.MAJakKa == this.MaJakKa) {
            this.MAJakKa = this.maJakKa;
            this.mAJakKa = this.MaJakKa;
            return;
        }
        this.MAJakKa = this.MaJakKa;
        this.mAJakKa = this.maJakKa;
    }

    /*
     * Unable to fully structure code
     */
    public final void amAjAKK(int var1_1) {
        var2_2 = this.amAjakK * this.AMAjakK;
        var3_3 = this.MAJakKa;
        var4_4 = 255 - ((1 << var1_1) - 1);
        var5_5 = var4_4 | var4_4 << 10 | var4_4 << 20;
        var6_6 = 0;
        switch (var2_2 & 7) {
            case 7: {
                var3_3[var6_6] = (var3_3[var6_6] & var5_5) >>> var1_1;
                ++var6_6;
            }
            case 6: {
                var3_3[var6_6] = (var3_3[var6_6] & var5_5) >>> var1_1;
                ++var6_6;
            }
            case 5: {
                var3_3[var6_6] = (var3_3[var6_6] & var5_5) >>> var1_1;
                ++var6_6;
            }
            case 4: {
                var3_3[var6_6] = (var3_3[var6_6] & var5_5) >>> var1_1;
                ++var6_6;
            }
            case 3: {
                var3_3[var6_6] = (var3_3[var6_6] & var5_5) >>> var1_1;
                ++var6_6;
            }
            case 2: {
                var3_3[var6_6] = (var3_3[var6_6] & var5_5) >>> var1_1;
                ++var6_6;
            }
            case 1: {
                var3_3[var6_6] = (var3_3[var6_6] & var5_5) >>> var1_1;
                ++var6_6;
                ** GOTO lbl41
            }
            default: {
                if (var6_6 < var2_2) ** continue;
            }
        }
        while (true) lbl-1000:
        // 2 sources

        {
            var3_3[var6_6] = (var3_3[var6_6] & var5_5) >>> var1_1;
            var3_3[++var6_6] = (var3_3[var6_6] & var5_5) >>> var1_1;
            var3_3[++var6_6] = (var3_3[var6_6] & var5_5) >>> var1_1;
            var3_3[++var6_6] = (var3_3[var6_6] & var5_5) >>> var1_1;
            var3_3[++var6_6] = (var3_3[var6_6] & var5_5) >>> var1_1;
            var3_3[++var6_6] = (var3_3[var6_6] & var5_5) >>> var1_1;
            var3_3[++var6_6] = (var3_3[var6_6] & var5_5) >>> var1_1;
            var3_3[++var6_6] = (var3_3[var6_6] & var5_5) >>> var1_1;
            ++var6_6;
lbl41:
            // 2 sources

            if (var6_6 < var2_2) ** GOTO lbl-1000
            break;
        }
    }

    public final void aMAjAKK(int n) {
        int n2;
        if (n < 0) {
            n2 = ((n &= 0xFF) << 16) + (n << 8) + n;
            this.AmajAKK(n2);
        }
        if (n > 0) {
            n2 = (n << 16) + (n << 8) + n;
            this.amAJakK(n2);
        }
    }

    public final void AmajAKK(int n) {
        n = (n & 0xFF) + ((n & 0xFF00) << 2) + ((n & 0xFF0000) << 4);
        int[] nArray = this.MAJakKa;
        int n2 = this.amAjakK * this.AMAjakK;
        int n3 = 0;
        while (n3 < n2) {
            int n4 = nArray[n3] + 0x10040100 - n;
            int n5 = n4 & 0x10040100;
            nArray[n3++] = n4 & n5 - (n5 >> 8);
        }
    }

    public final void amAJakK(int n) {
        n = (n & 0xFF) + ((n & 0xFF00) << 2) + ((n & 0xFF0000) << 4);
        int[] nArray = this.MAJakKa;
        int n2 = this.amAjakK * this.AMAjakK;
        int n3 = 0;
        while (n3 < n2) {
            int n4 = nArray[n3] + n;
            int n5 = n4 & 0x10040100;
            nArray[n3++] = n4 - n5 | n5 - (n5 >> 8);
        }
    }

    public final void AMAJAKK(mmaamma mmaamma2, int n, int n2, mmaamma mmaamma3, int n3, int n4, int n5, int n6, int n7, int n8, double d) {
        if (d < 0.0) {
            d = 0.0;
        }
        if (d > 1.0) {
            d = 1.0;
        }
        int n9 = (int)(d * 32.9);
        int n10 = 32 - n9;
        int n11 = n2 * mmaamma2.amAjakK + n;
        int n12 = n4 * mmaamma3.amAjakK + n3;
        int n13 = n8 * this.amAjakK + n7;
        int[] nArray = this.MAJakKa;
        int[] nArray2 = mmaamma2.MAJakKa;
        int[] nArray3 = mmaamma3.MAJakKa;
        int n14 = 0;
        while (n14 < n6) {
            int n15 = n11 + n5;
            int n16 = n12;
            int n17 = n13;
            int n18 = n11;
            while (n18 < n15) {
                int n19;
                if ((n19 = nArray2[n18++] & 0xF83E0F8) >= 0) {
                    int n20 = nArray3[n16++] & 0xF83E0F8;
                    int n21 = (n20 - n19 >> 3) * n10;
                    nArray[n17++] = (n21 >>> 2) + n19;
                    continue;
                }
                nArray[n17++] = nArray3[n16++];
            }
            n11 += mmaamma2.amAjakK;
            n12 += mmaamma3.amAjakK;
            n13 += this.amAjakK;
            ++n14;
        }
    }

    public final void aMaJakK(mmaamma mmaamma2, int n, int n2, mmaamma mmaamma3, int n3, int n4, int n5, int n6, int n7, int n8, double d, double d2) {
        if (d < 0.0) {
            d = 0.0;
        }
        if (d > 1.0) {
            d = 1.0;
        }
        if (d2 < 0.0) {
            d2 = 0.0;
        }
        if (d2 > 1.0) {
            d2 = 1.0;
        }
        int n9 = (int)(d * 32.9);
        int n10 = (int)(d2 * 32.9);
        int n11 = n2 * mmaamma2.amAjakK + n;
        int n12 = n4 * mmaamma3.amAjakK + n3;
        int n13 = n8 * this.amAjakK + n7;
        int[] nArray = this.MAJakKa;
        int[] nArray2 = mmaamma2.MAJakKa;
        int[] nArray3 = mmaamma3.MAJakKa;
        int n14 = 0;
        while (n14 < n6) {
            int n15 = n11 + n5;
            int n16 = n12;
            int n17 = n13;
            int n18 = n11;
            while (n18 < n15) {
                int n19 = nArray2[n18++] & 0xF83E0F8;
                n19 = (n19 >>> 3) * n9 >>> 2;
                int n20 = nArray3[n16++] & 0xF83E0F8;
                n20 = (n20 >>> 3) * n10 >>> 2;
                int n21 = (n19 &= 0xF83E0F8) + (n20 &= 0xF83E0F8);
                int n22 = n21 & 0x10040100;
                n21 = n21 - n22 | n22 - (n22 >> 8);
                n21 = nArray[n17] + n21;
                n22 = n21 & 0x10040100;
                nArray[n17] = n21 - n22 | n22 - (n22 >> 8);
                ++n17;
            }
            n11 += mmaamma2.amAjakK;
            n12 += mmaamma3.amAjakK;
            n13 += this.amAjakK;
            ++n14;
        }
    }

    public void aMAJakK(mmaamma mmaamma2, int n, int n2) {
        this.aMAjakK(mmaamma2, n, n2, 0, 0, mmaamma2.amAjakK, mmaamma2.AMAjakK);
    }

    public void aMAjakK(mmaamma mmaamma2, int n, int n2, int n3, int n4, int n5, int n6) {
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
        int n7 = n4 * mmaamma2.amAjakK + n3;
        int n8 = n2 * this.amAjakK + n;
        int[] nArray = mmaamma2.MAJakKa;
        int[] nArray2 = this.MAJakKa;
        if (mmaamma2.amAjakK == this.amAjakK && this.amAjakK == n5) {
            System.arraycopy(nArray, n7, nArray2, n8, n5 * n6);
            return;
        }
        int n9 = 0;
        while (n9 < n6) {
            System.arraycopy(nArray, n7, nArray2, n8, n5);
            n7 += mmaamma2.amAjakK;
            n8 += this.amAjakK;
            ++n9;
        }
    }

    public void AMajakK(kmajkka kmajkka2) {
        int n = this.amAjakK * this.AMAjakK;
        int[] nArray = this.MAJakKa;
        byte[] byArray = kmajkka2.aMAjakK;
        int[] nArray2 = kmajkka2.AMAJakK;
        int n2 = 0;
        while (n2 < n) {
            nArray[n2] = nArray2[byArray[n2++] & 0xFF];
        }
    }

    public void AmAjAKK(kmajkka kmajkka2) {
        int n = this.amAjakK * this.AMAjakK;
        int[] nArray = this.MAJakKa;
        byte[] byArray = kmajkka2.aMAjakK;
        int[] nArray2 = kmajkka2.AMAJakK;
        int n2 = 0;
        while (n2 < n) {
            int n3 = nArray[n2] + nArray2[byArray[n2] & 0xFF];
            int n4 = n3 & 0x10040100;
            nArray[n2++] = n3 - n4 | n4 - (n4 >> 8);
        }
    }

    public void AmAJAKK(mmaamma mmaamma2, int n, int n2) {
        int n3 = 0;
        int n4 = 0;
        int n5 = mmaamma2.amAjakK;
        int n6 = mmaamma2.AMAjakK;
        if (n < 0) {
            n5 += n;
            n3 = -n;
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
            n4 = -n2;
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
        int n7 = n3 + n4 * mmaamma2.amAjakK;
        int n8 = n2 * this.amAjakK + n;
        int[] nArray = this.MAJakKa;
        int[] nArray2 = mmaamma2.MAJakKa;
        int n9 = 0;
        while (n9 < n6) {
            int n10;
            int n11;
            int n12 = n7;
            int n13 = n8;
            int n14 = n5;
            switch (n14 & 3) {
                case 3: {
                    n11 = nArray[n13] + nArray2[n12++];
                    n10 = n11 & 0x10040100;
                    nArray[n13++] = n11 - n10 | n10 - (n10 >> 8);
                }
                case 2: {
                    n11 = nArray[n13] + nArray2[n12++];
                    n10 = n11 & 0x10040100;
                    nArray[n13++] = n11 - n10 | n10 - (n10 >> 8);
                }
                case 1: {
                    n11 = nArray[n13] + nArray2[n12++];
                    n10 = n11 & 0x10040100;
                    nArray[n13++] = n11 - n10 | n10 - (n10 >> 8);
                }
            }
            n14 >>= 2;
            while (n14-- > 0) {
                n11 = nArray[n13] + nArray2[n12++];
                n10 = n11 & 0x10040100;
                nArray[n13++] = n11 - n10 | n10 - (n10 >> 8);
                n11 = nArray[n13] + nArray2[n12++];
                n10 = n11 & 0x10040100;
                nArray[n13++] = n11 - n10 | n10 - (n10 >> 8);
                n11 = nArray[n13] + nArray2[n12++];
                n10 = n11 & 0x10040100;
                nArray[n13++] = n11 - n10 | n10 - (n10 >> 8);
                n11 = nArray[n13] + nArray2[n12++];
                n10 = n11 & 0x10040100;
                nArray[n13++] = n11 - n10 | n10 - (n10 >> 8);
            }
            n8 += this.amAjakK;
            n7 += mmaamma2.amAjakK;
            ++n9;
        }
    }

    public void aMajakK(mmaamma mmaamma2, int n, int n2) {
        Rectangle rectangle = new Rectangle(n, n2, mmaamma2.amAjakK, mmaamma2.AMAjakK);
        if ((rectangle = rectangle.intersection(new Rectangle(0, 0, this.amAjakK, this.AMAjakK))).isEmpty()) {
            return;
        }
        int n3 = rectangle.x - n + (rectangle.y - n2) * mmaamma2.amAjakK;
        int n4 = rectangle.y * this.amAjakK + rectangle.x;
        int[] nArray = this.MAJakKa;
        int[] nArray2 = mmaamma2.MAJakKa;
        int n5 = 0;
        while (n5 < rectangle.height) {
            int n6;
            int n7;
            int n8 = n3;
            int n9 = n4;
            int n10 = rectangle.width;
            switch (n10 & 3) {
                case 3: {
                    n7 = nArray[n9] + 0x10040100 - nArray2[n8++];
                    n6 = n7 & 0x10040100;
                    nArray[n9++] = n7 & n6 - (n6 >> 8);
                }
                case 2: {
                    n7 = nArray[n9] + 0x10040100 - nArray2[n8++];
                    n6 = n7 & 0x10040100;
                    nArray[n9++] = n7 & n6 - (n6 >> 8);
                }
                case 1: {
                    n7 = nArray[n9] + 0x10040100 - nArray2[n8++];
                    n6 = n7 & 0x10040100;
                    nArray[n9++] = n7 & n6 - (n6 >> 8);
                }
            }
            n10 >>= 2;
            while (n10-- > 0) {
                n7 = nArray[n9] + 0x10040100 - nArray2[n8++];
                n6 = n7 & 0x10040100;
                nArray[n9++] = n7 & n6 - (n6 >> 8);
                n7 = nArray[n9] + 0x10040100 - nArray2[n8++];
                n6 = n7 & 0x10040100;
                nArray[n9++] = n7 & n6 - (n6 >> 8);
                n7 = nArray[n9] + 0x10040100 - nArray2[n8++];
                n6 = n7 & 0x10040100;
                nArray[n9++] = n7 & n6 - (n6 >> 8);
                n7 = nArray[n9] + 0x10040100 - nArray2[n8++];
                n6 = n7 & 0x10040100;
                nArray[n9++] = n7 & n6 - (n6 >> 8);
            }
            n4 += this.amAjakK;
            n3 += mmaamma2.amAjakK;
            ++n5;
        }
    }

    public void aMajAKK(float f) {
        int n = (int)(31.0f * f);
        int n2 = 32 - n;
        int n3 = 0;
        int[] nArray = this.MAJakKa;
        int n4 = this.amAjakK;
        int n5 = this.AMAjakK;
        int n6 = 0;
        while (n6 < n5) {
            int n7 = nArray[n3] >> 1 & 0x7E1F87E;
            int n8 = 0;
            while (n8 < n4) {
                n7 = (n7 >> 3 & 0x1F07C1F) * n + (nArray[n3] >> 3 & 0x1F07C1F) * n2 >> 2;
                nArray[n3++] = n7 & 0xFF3FCFF;
                ++n8;
            }
            ++n6;
        }
    }

    public void amAJAKK(float f) {
        int n = (int)(31.0f * f);
        int n2 = 32 - n;
        int[] nArray = this.MAJakKa;
        int n3 = this.amAjakK;
        int n4 = this.AMAjakK;
        int n5 = 0;
        while (n5 < n3) {
            int n6 = n5;
            int n7 = nArray[n6];
            int n8 = 0;
            while (n8 < n4) {
                n7 = (n7 >> 3 & 0x1F07C1F) * n + (nArray[n6] >> 3 & 0x1F07C1F) * n2 >> 2;
                nArray[n6] = n7 & 0xFF3FCFF;
                n6 += n3;
                ++n8;
            }
            ++n5;
        }
    }

    public void amajakK() {
        int[] nArray = this.MAJakKa;
        int[] nArray2 = this.mAJakKa;
        int n = this.amAjakK * this.AMAjakK;
        int n2 = 0;
        int n3 = 0;
        switch (n & 3) {
            case 3: {
                nArray[n2] = nArray[n2++] + nArray2[n3++] >> 1 & 0xFF3FCFF;
            }
            case 2: {
                nArray[n2] = nArray[n2++] + nArray2[n3++] >> 1 & 0xFF3FCFF;
            }
            case 1: {
                nArray[n2] = nArray[n2++] + nArray2[n3++] >> 1 & 0xFF3FCFF;
            }
        }
        n >>= 2;
        while (n-- > 0) {
            nArray[n2] = nArray[n2++] + nArray2[n3++] >> 1 & 0xFF3FCFF;
            nArray[n2] = nArray[n2++] + nArray2[n3++] >> 1 & 0xFF3FCFF;
            nArray[n2] = nArray[n2++] + nArray2[n3++] >> 1 & 0xFF3FCFF;
            nArray[n2] = nArray[n2++] + nArray2[n3++] >> 1 & 0xFF3FCFF;
        }
    }

    public void AmajakK() {
        int[] nArray = this.MAJakKa;
        int[] nArray2 = this.mAJakKa;
        int n = 0;
        while (n < this.amAjakK * this.AMAjakK) {
            int n2 = nArray[n] + (nArray2[n] >> 1 & 0xFF3FCFF);
            int n3 = n2 & 0x10040100;
            nArray[n++] = n2 - n3 | n3 - (n3 >> 8);
            n2 = nArray[n] + (nArray2[n] >> 1 & 0xFF3FCFF);
            n3 = n2 & 0x10040100;
            nArray[n++] = n2 - n3 | n3 - (n3 >> 8);
            n2 = nArray[n] + (nArray2[n] >> 1 & 0xFF3FCFF);
            n3 = n2 & 0x10040100;
            nArray[n++] = n2 - n3 | n3 - (n3 >> 8);
            n2 = nArray[n] + (nArray2[n] >> 1 & 0xFF3FCFF);
            n3 = n2 & 0x10040100;
            nArray[n++] = n2 - n3 | n3 - (n3 >> 8);
        }
    }

    public void AMaJakK(int n) {
        int[] nArray = this.MAJakKa;
        int n2 = this.amAjakK * this.AMAjakK;
        int n3 = 0;
        switch (n2 & 3) {
            case 3: {
                int n4 = n3++;
                nArray[n4] = nArray[n4] ^ n;
            }
            case 2: {
                int n5 = n3++;
                nArray[n5] = nArray[n5] ^ n;
            }
            case 1: {
                int n6 = n3++;
                nArray[n6] = nArray[n6] ^ n;
            }
        }
        n2 >>= 2;
        while (n2-- > 0) {
            int n7 = n3++;
            nArray[n7] = nArray[n7] ^ n;
            int n8 = n3++;
            nArray[n8] = nArray[n8] ^ n;
            int n9 = n3++;
            nArray[n9] = nArray[n9] ^ n;
            int n10 = n3++;
            nArray[n10] = nArray[n10] ^ n;
        }
    }

    public void amajAKK(int[] nArray, int[] nArray2, int[] nArray3) {
        int[] nArray4 = this.MAJakKa;
        int n = this.amAjakK * this.AMAjakK;
        int n2 = 0;
        while (n2 < n) {
            int n3 = nArray4[n2];
            nArray4[n2] = nArray[n3 >> 20 & 0xFF] + nArray2[n3 >> 10 & 0xFF] + nArray3[n3 & 0xFF];
            ++n2;
        }
    }

    public void AmAJakK(mmaamma mmaamma2, float f, float f2, float f3, float f4) {
        int n = (int)f;
        int n2 = (int)f2;
        int n3 = (int)f3;
        int n4 = (int)f4;
        int n5 = mmaamma2.amAjakK;
        int n6 = mmaamma2.AMAjakK;
        int n7 = 0;
        int n8 = 0;
        if (n < 0) {
            n3 += n;
            n7 = -n;
            n = 0;
        }
        if (n3 <= 0) {
            return;
        }
        if (n + n3 > this.amAjakK) {
            n3 = this.amAjakK - n;
        }
        if (n3 <= 0) {
            return;
        }
        if (n2 < 0) {
            n4 += n2;
            n8 = -n2;
            n2 = 0;
        }
        if (n4 <= 0) {
            return;
        }
        if (n2 + n4 > this.AMAjakK) {
            n4 = this.AMAjakK - n2;
        }
        if (n4 <= 0) {
            return;
        }
        int n9 = n2 * this.amAjakK + n;
        int[] nArray = this.MAJakKa;
        int[] nArray2 = mmaamma2.MAJakKa;
        int n10 = (int)((float)(1024 * n5) / f3);
        int n11 = (int)((float)(1024 * n6) / f4);
        int n12 = n10 * n7;
        int n13 = n11 * n8;
        int n14 = 0;
        while (n14 < n4) {
            int n15 = n9;
            int n16 = n3;
            int n17 = n12 + (n13 & 0xFFFFFC00) * n5;
            while (n16-- > 0) {
                int n18 = nArray[n15] + nArray2[n17 >> 10];
                int n19 = n18 & 0x10040100;
                nArray[n15++] = n18 - n19 | n19 - (n19 >> 8);
                n17 += n10;
            }
            n9 += this.amAjakK;
            n13 += n11;
            ++n14;
        }
    }

    public final void AMAJakK(int n, int n2) {
        n = (n & 0xFF) + ((n & 0xFF00) << 2) + ((n & 0xFF0000) << 4);
        n2 = (n2 & 0xFF) + ((n2 & 0xFF00) << 2) + ((n2 & 0xFF0000) << 4);
        int[] nArray = this.MAJakKa;
        int n3 = this.amAjakK * this.AMAjakK;
        int n4 = 0;
        while (n4 < n3) {
            int n5;
            int n6 = nArray[n4];
            if ((n6 & Integer.MIN_VALUE) != 0) {
                n6 = (n6 & Integer.MAX_VALUE) + 0x10040100 - n;
                n5 = n6 & 0x10040100;
                nArray[n4++] = n6 & n5 - (n5 >> 8);
                continue;
            }
            n6 = n6 + 0x10040100 - n2;
            n5 = n6 & 0x10040100;
            nArray[n4++] = n6 & n5 - (n5 >> 8);
        }
    }
}
