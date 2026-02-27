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

    /*
     * Unable to fully structure code
     */
    void KamAJAk(kmajkka var1_1, mmaamma var2_2, boolean var3_3) {
        block20: {
            block18: {
                var4_4 = var1_1.AmajakK;
                var5_5 = var1_1.aMAjakK;
                var6_6 = var1_1.AMAJakK;
                var7_7 = var2_2.MAJakKa;
                var8_8 = var1_1.amAjakK;
                var9_9 = var1_1.AMAjakK;
                var10_10 = 1.100000023841858;
                var14_12 = var12_11 = 1.0 / var10_10;
                var16_13 = 0.0;
                var18_14 = 0.0;
                var20_15 = var12_11;
                var22_16 = var8_8 / 2;
                var24_17 = var9_9 / 2;
                var26_18 = (int)(var14_12 * 65536.0);
                var27_19 = (int)(var16_13 * 65536.0);
                var28_20 = (int)(var18_14 * 65536.0);
                var29_21 = (int)(var20_15 * 65536.0);
                var30_22 = (int)(-(var22_16 * var14_12 + var24_17 * var18_14) * 65536.0);
                var31_23 = (int)(-(var22_16 * var16_13 + var24_17 * var20_15) * 65536.0);
                var30_22 += (int)(var22_16 * 65536.0);
                var31_23 += (int)(var24_17 * 65536.0);
                var32_24 = 0;
                if (var8_8 != 256 || var9_9 != 128) break block18;
                var35_25 = 0;
                while (var35_25 < var9_9) {
                    block19: {
                        var33_27 = var30_22;
                        var34_29 = var31_23;
                        var36_31 = var32_24 + var8_8;
                        if (!var3_3) ** GOTO lbl54
                        while (var32_24 < var36_31) {
                            if (var7_7[var32_24] < 0) {
                                var4_4[var32_24++] = -1;
                            } else {
                                var4_4[var32_24] = var37_33 = (byte)((var5_5[var33_27 << 8 >>> 24 | var34_29 << 9 >>> 25 << 8] & 255) >>> 1);
                                if (var37_33 != 0) {
                                    var39_37 = var7_7[var32_24] + var6_6[var37_33 & 255];
                                    var38_35 = var39_37 & 0x10040100;
                                    var7_7[var32_24++] = var39_37 - var38_35 | var38_35 - (var38_35 >> 8);
                                } else {
                                    ++var32_24;
                                }
                            }
                            var33_27 += var26_18;
                            var34_29 += var27_19;
                        }
                        break block19;
lbl-1000:
                        // 1 sources

                        {
                            var4_4[var32_24] = var37_33 = (byte)((var5_5[var33_27 << 8 >>> 24 | var34_29 << 9 >>> 25 << 8] & 255) >>> 1);
                            if (var37_33 != 0) {
                                var39_37 = var7_7[var32_24] + var6_6[var37_33 & 255];
                                var38_35 = var39_37 & 0x10040100;
                                var7_7[var32_24++] = var39_37 - var38_35 | var38_35 - (var38_35 >> 8);
                            } else {
                                ++var32_24;
                            }
                            var33_27 += var26_18;
                            var34_29 += var27_19;
lbl54:
                            // 2 sources

                            ** while (var32_24 < var36_31)
                        }
                    }
                    var30_22 += var28_20;
                    var31_23 += var29_21;
                    ++var35_25;
                }
                break block20;
            }
            if (var8_8 != 512 || var9_9 != 256) break block20;
            var35_26 = 0;
            while (var35_26 < var9_9) {
                block21: {
                    var33_28 = var30_22;
                    var34_30 = var31_23;
                    var36_32 = var32_24 + var8_8;
                    if (!var3_3) ** GOTO lbl93
                    while (var32_24 < var36_32) {
                        if (var7_7[var32_24] < 0) {
                            var4_4[var32_24++] = -1;
                        } else {
                            var4_4[var32_24] = var37_34 = (byte)((var5_5[var33_28 << 7 >>> 23 | var34_30 << 8 >>> 24 << 9] & 255) >>> 1);
                            if (var37_34 != 0) {
                                var39_38 = var7_7[var32_24] + var6_6[var37_34 & 255];
                                var38_36 = var39_38 & 0x10040100;
                                var7_7[var32_24++] = var39_38 - var38_36 | var38_36 - (var38_36 >> 8);
                            } else {
                                ++var32_24;
                            }
                        }
                        var33_28 += var26_18;
                        var34_30 += var27_19;
                    }
                    break block21;
lbl-1000:
                    // 1 sources

                    {
                        var4_4[var32_24] = var37_34 = (byte)((var5_5[var33_28 << 7 >>> 23 | var34_30 << 8 >>> 24 << 9] & 255) >>> 1);
                        if (var37_34 != 0) {
                            var39_38 = var7_7[var32_24] + var6_6[var37_34 & 255];
                            var38_36 = var39_38 & 0x10040100;
                            var7_7[var32_24++] = var39_38 - var38_36 | var38_36 - (var38_36 >> 8);
                        } else {
                            ++var32_24;
                        }
                        var33_28 += var26_18;
                        var34_30 += var27_19;
lbl93:
                        // 2 sources

                        ** while (var32_24 < var36_32)
                    }
                }
                var30_22 += var28_20;
                var31_23 += var29_21;
                ++var35_26;
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
