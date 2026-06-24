/*
 * Decompiled with CFR 0.152.
 */
public class kmaamka
extends mmjjmma {
    kaajmmk KkAMaJA;
    kmaakma kkAMaJA;
    mmajmmk KKAMaJA;
    mmaamka kKAMaJA;
    kmajkka KkaMaJA;
    mmjamma kkaMaJA;
    float KKaMaJA;
    float kKaMaJA;

    public String majakkA() {
        return "feta";
    }

    public void MajakkA(mmjamma mmjamma2) {
        this.kkaMaJA = mmjamma2;
        this.kkAMaJA = new kmaakma();
        this.KkAMaJA = new kaajmmk();
        this.KkAMaJA.JAkKAMa = 512;
        this.KkAMaJA.jAkKAMa = 256;
        this.KkAMaJA.JAKkaMa.MaJaKka(0.0f, 0.0f, 2.0f);
        kaaakma kaaakma2 = new kaaakma(this.kkaMaJA, this.kkaMaJA.amAjAkK("images/verax/kosmusp.jpg"), true);
        this.kkAMaJA.maJAKkA(kaaakma2);
        forward.kkamAJA.kAMajak();
        this.KkAMaJA.jaKkaMa = 1.9f;
        this.KKAMaJA = this.KAmAJAk();
        this.kkAMaJA.maJAKkA(this.KKAMaJA);
        this.KkaMaJA = forward.kKAMAjA.kAmaJAk;
        this.KkAMaJA.JAKkaMa.MaJaKka(0.0f, -2.0f, 1.0f);
        this.KkAMaJA.AkKAMaJ(this.KKAMaJA.jaKKaMA);
        this.kKAMaJA = new mmaamka(300, 20.0f);
        this.kKAMaJA.jaKKaMA.mAjakKA(new mmajmma(0.0f, 0.0f, 0.0f));
        this.kKAMaJA.KamaJAk(0);
        mmaamma mmaamma2 = (mmaamma)mmaakma.majaKkA(this.kkaMaJA.amAjAkK("images/flare1.jpg"));
        this.kKAMaJA.KkaMaja(mmaamma2);
        forward.kkamAJA.kAMajak();
        this.kkAMaJA.maJAKkA(this.kKAMaJA);
        this.kkAMaJA.mAJAKkA();
        this.kAMAJAk();
    }

    void kamAJAk(mmaamma mmaamma2, float f) {
        if (this.KKaMaJA != 0.0f) {
            float f2 = (f - this.KKaMaJA) * 0.7f;
            this.KamAJAk(this.KkaMaJA, mmaamma2, false);
            int n = (int)Math.min(255.0f, f2 * 255.0f);
            int n2 = 0;
            if (this.kKaMaJA != 0.0f) {
                float f3 = (f - this.kKaMaJA) * 0.4f;
                n2 = (int)Math.min(255.0f, f3 * 255.0f);
            }
            mmaamma2.AMAJakK(n *= 65793, n2 *= 65793);
            return;
        }
        this.KamAJAk(this.KkaMaJA, mmaamma2, true);
    }

    void KamAJAk(kmajkka var1_1, mmaamma var2_2, boolean var3_3) {
        byte[] byArray = var1_1.AmajakK;
        byte[] byArray2 = var1_1.aMAjakK;
        int[] nArray = var1_1.AMAJakK;
        int[] nArray2 = var2_2.MAJakKa;
        int n = var1_1.amAjakK;
        int n2 = var1_1.AMAjakK;
        double d = 1.0 / 1.100000023841858;
        int n3 = (int)(d * 65536.0);
        int n4 = 0;
        int n5 = 0;
        int n6 = n3;
        double d2 = n / 2.0;
        double d3 = n2 / 2.0;
        int n7 = (int)(-(d2 * d) * 65536.0) + (int)(d2 * 65536.0);
        int n8 = (int)(-(d3 * d) * 65536.0) + (int)(d3 * 65536.0);
        int n9 = 0;
        if (n == 256 && n2 == 128) {
            for (int n10 = 0; n10 < n2; ++n10) {
                int n11 = n7;
                int n12 = n8;
                int n13 = n9 + n;
                if (var3_3) {
                    while (n9 < n13) {
                        if (nArray2[n9] < 0) {
                            byArray[n9++] = -1;
                        } else {
                            byte by = (byte)((byArray2[((n11 >>> 16) & 255) | (((n12 >>> 16) & 127) << 8)] & 255) >>> 1);
                            byArray[n9] = by;
                            if (by != 0) {
                                int n14 = nArray2[n9] + nArray[by & 255];
                                int n15 = n14 & 0x10040100;
                                nArray2[n9++] = n14 - n15 | n15 - (n15 >> 8);
                            } else {
                                ++n9;
                            }
                        }
                        n11 += n3;
                        n12 += n4;
                    }
                } else {
                    while (n9 < n13) {
                        byte by = (byte)((byArray2[((n11 >>> 16) & 255) | (((n12 >>> 16) & 127) << 8)] & 255) >>> 1);
                        byArray[n9] = by;
                        if (by != 0) {
                            int n16 = nArray2[n9] + nArray[by & 255];
                            int n17 = n16 & 0x10040100;
                            nArray2[n9++] = n16 - n17 | n17 - (n17 >> 8);
                        } else {
                            ++n9;
                        }
                        n11 += n3;
                        n12 += n4;
                    }
                }
                n7 += n5;
                n8 += n6;
            }
        } else if (n == 512 && n2 == 256) {
            for (int n18 = 0; n18 < n2; ++n18) {
                int n19 = n7;
                int n20 = n8;
                int n21 = n9 + n;
                if (var3_3) {
                    while (n9 < n21) {
                        if (nArray2[n9] < 0) {
                            byArray[n9++] = -1;
                        } else {
                            byte by = (byte)((byArray2[((n19 >>> 16) & 511) | (((n20 >>> 16) & 255) << 9)] & 255) >>> 1);
                            byArray[n9] = by;
                            if (by != 0) {
                                int n22 = nArray2[n9] + nArray[by & 255];
                                int n23 = n22 & 0x10040100;
                                nArray2[n9++] = n22 - n23 | n23 - (n23 >> 8);
                            } else {
                                ++n9;
                            }
                        }
                        n19 += n3;
                        n20 += n4;
                    }
                } else {
                    while (n9 < n21) {
                        byte by = (byte)((byArray2[((n19 >>> 16) & 511) | (((n20 >>> 16) & 255) << 9)] & 255) >>> 1);
                        byArray[n9] = by;
                        if (by != 0) {
                            int n24 = nArray2[n9] + nArray[by & 255];
                            int n25 = n24 & 0x10040100;
                            nArray2[n9++] = n24 - n25 | n25 - (n25 >> 8);
                        } else {
                            ++n9;
                        }
                        n19 += n3;
                        n20 += n4;
                    }
                }
                n7 += n5;
                n8 += n6;
            }
        }
        var1_1.Amajakk();
    }

    public void maJakkA(mmaamma mmaamma2, float f, float f2) {
        mmaamma2.amaJakK();
        this.KkAMaJA.JAKkaMa.MaJaKka(0.0f, 5.0f, 0.0f);
        this.KkAMaJA.JAKkaMa.MAJAKKA((float)Math.sin(f / 10.0f));
        this.KkAMaJA.JAKkaMa.MAJakKA(f / 4.0f);
        this.kKAMaJA.JakKaMA.aMAjaKk();
        this.kKAMaJA.JakKaMA.AMaJAKk(-f / 2.0f);
        this.KkAMaJA.AkKAMaJ(mmajmma.majAKKA);
        this.kkAMaJA.MajAKkA(this.KkAMaJA, forward.kkAMAjA);
        this.kkAMaJA.MAJAKkA(forward.kkAMAjA);
        if (this.KkaMaJA != null) {
            this.kamAJAk(forward.kkAMAjA.aMAjaKk, f);
        }
        mmaamma2.amajakK();
    }

    void kAMAJAk() {
        kmajkka kmajkka2 = forward.kKAMAjA.kAmaJAk;
        int n = 0;
        while (n < 256) {
            kmajkka2.aMajakK[n] = (byte)Math.min(255.0, (double)(n * 2));
            kmajkka2.AmAJakK[n] = (byte)Math.min(255.0, (double)(n * 3));
            kmajkka2.amAJakK[n] = (byte)Math.min(255.0, (double)n);
            ++n;
        }
        kmajkka2.amAJakK[255] = 0;
        kmajkka2.AmAJakK[255] = 0;
        kmajkka2.aMajakK[255] = 0;
        kmajkka2.aMaJAkk();
    }

    void KAMAJAk() {
        kmajkka kmajkka2 = forward.kKAMAjA.kAmaJAk;
        int n = 0;
        while (n < 256) {
            kmajkka2.aMajakK[n] = (byte)Math.min(255.0, (double)(n * 2));
            kmajkka2.AmAJakK[n] = (byte)Math.min(255.0, (double)(n * 3));
            kmajkka2.amAJakK[n] = (byte)Math.min(255.0, (double)n);
            ++n;
        }
        kmajkka2.aMaJAkk();
    }

    public void MAJakkA(String string, float f) {
        if (string.equals("1")) {
            this.kAMAJAk();
        }
        if (string.equals("2")) {
            this.KAMAJAk();
        }
        if (string.equals("blackfeta")) {
            this.KKaMaJA = f;
        }
        if (string.equals("blackmuna")) {
            this.kKaMaJA = f;
        }
    }

    mmajmmk KAmAJAk() {
        mmajmmk mmajmmk2 = kajamka.kAMaJAk(this.kkaMaJA.amAjAkK("meshes/fetus.igu"));
        mmajmmk2.jaKKaMA.MaJaKka(0.0f, 0.0f, 0.0f);
        mmajmmk2.JAKKaMA.MajaKka(0.09);
        mmajmmk2.JAkKaMA = true;
        mmajmmk2.jAkKAma = 0;
        mmaamma mmaamma2 = (mmaamma)mmaakma.majaKkA(this.kkaMaJA.amAjAkK("images/babyenv.jpg"));
        mmajmmk2.KkaMaja(mmaamma2);
        int n = 0;
        while (n < 65536) {
            int n2 = n++;
            mmaamma2.MAJakKa[n2] = mmaamma2.MAJakKa[n2] | Integer.MIN_VALUE;
        }
        mmajmmk2.jaKkama = true;
        mmajmmk2.kkAMAJa();
        return mmajmmk2;
    }
}
