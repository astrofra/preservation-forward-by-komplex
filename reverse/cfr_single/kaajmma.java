/*
 * Decompiled with CFR 0.152.
 */
public class kaajmma {
    public kmajkka mAJAkKA;
    public mmaamma MajAkKA;
    public mmaamma majAkKA;
    int MAjAkKA;
    int mAjAkKA;
    int MaJaKka;
    int maJaKka;
    int MAJaKka;
    int mAJaKka;
    int MajaKka;
    float majaKka;
    float MAjaKka;
    float mAjaKka;
    float MaJAKka;
    float maJAKka;
    float MAJAKka;
    float mAJAKka;
    float MajAKka;
    float majAKka;
    float MAjAKka;
    float mAjAKka;
    int MaJakka;
    int maJakka;
    int MAJakka;
    static final float mAJakka = 65536.0f;
    static final float Majakka = 4.2949673E9f;
    int majakka;
    int MAjakka;
    int mAjakka;
    int MaJAkka;
    int maJAkka;
    static final int MAJAkka = 264499452;

    static final int MaJAkKa(float f) {
        return (int)(f * 4.2949673E9f);
    }

    static final int MajakKa(float f) {
        return (int)(f * 65536.0f);
    }

    static final int mAjakKa(double d) {
        return (int)(d * 65536.0);
    }

    static final int MajAkKa(float f) {
        return (int)f;
    }

    public kaajmma(mmaamma mmaamma2) {
        this.majAkKA = mmaamma2;
        this.MAjAkKA = this.majAkKA.amAjakK;
        this.mAjAkKA = this.majAkKA.AMAjakK;
    }

    public void MAjakKa(kmaamma kmaamma2) {
        boolean bl;
        kmajkmk kmajkmk2;
        mmjakka mmjakka2;
        float f = 0.0f;
        float f2 = 0.0f;
        float f3 = 0.0f;
        mmjakka mmjakka3 = kmaamma2.mAjakKa;
        mmjakka mmjakka4 = kmaamma2.MaJAkKa;
        mmjakka mmjakka5 = kmaamma2.maJAkKa;
        kmajkmk kmajkmk3 = kmaamma2.MAJAkKa;
        kmajkmk kmajkmk4 = kmaamma2.mAJAkKa;
        kmajkmk kmajkmk5 = kmaamma2.MajAkKa;
        if (mmjakka3.AMAjAkk > mmjakka4.AMAjAkk) {
            mmjakka2 = mmjakka4;
            mmjakka4 = mmjakka3;
            mmjakka3 = mmjakka2;
            kmajkmk2 = kmajkmk4;
            kmajkmk4 = kmajkmk3;
            kmajkmk3 = kmajkmk2;
        }
        if (mmjakka4.AMAjAkk > mmjakka5.AMAjAkk) {
            mmjakka2 = mmjakka5;
            mmjakka5 = mmjakka4;
            mmjakka4 = mmjakka2;
            kmajkmk2 = kmajkmk5;
            kmajkmk5 = kmajkmk4;
            kmajkmk4 = kmajkmk2;
        }
        if (mmjakka3.AMAjAkk > mmjakka4.AMAjAkk) {
            mmjakka2 = mmjakka4;
            mmjakka4 = mmjakka3;
            mmjakka3 = mmjakka2;
            kmajkmk2 = kmajkmk4;
            kmajkmk4 = kmajkmk3;
            kmajkmk3 = kmajkmk2;
        }
        float f4 = mmjakka3.amAjAkk;
        float f5 = mmjakka3.AMAjAkk;
        float f6 = kmajkmk3.akKAMAJ;
        float f7 = kmajkmk3.AKKAMAJ;
        float f8 = mmjakka3.AmajAkk;
        float f9 = mmjakka4.amAjAkk;
        float f10 = mmjakka4.AMAjAkk;
        float f11 = kmajkmk4.akKAMAJ;
        float f12 = kmajkmk4.AKKAMAJ;
        float f13 = mmjakka4.AmajAkk;
        float f14 = mmjakka5.amAjAkk;
        float f15 = mmjakka5.AMAjAkk;
        float f16 = kmajkmk5.akKAMAJ;
        float f17 = kmajkmk5.AKKAMAJ;
        float f18 = mmjakka5.AmajAkk;
        int n = (int)f5;
        int n2 = n >> 16;
        int n3 = (int)f10;
        int n4 = n3 >> 16;
        int n5 = (int)f15;
        int n6 = n5 >> 16;
        int n7 = 0;
        int n8 = this.mAjAkKA;
        if (n2 > n8) {
            return;
        }
        if (n6 < n7) {
            return;
        }
        if (n2 < n7) {
            n2 = n7;
        }
        if (n4 < n7) {
            n4 = n7;
        }
        if (n4 > n8) {
            n4 = n8;
        }
        if (n6 > n8) {
            n6 = n8;
        }
        float f19 = f9 - f4;
        float f20 = f14 - f4;
        float f21 = f14 - f9;
        float f22 = f10 - f5;
        float f23 = f15 - f5;
        float f24 = f15 - f10;
        float f25 = f20 * f22 - f19 * f23;
        if (f25 == 0.0f) {
            return;
        }
        boolean bl2 = f8 > 0.0f && f13 > 0.0f && f18 > 0.0f;
        f25 = 65536.0f / f25;
        this.majaKka = ((f16 - f6) * f22 - (f11 - f6) * f23) * f25;
        this.MaJAKka = ((f11 - f6) * f20 - (f16 - f6) * f19) * f25;
        this.MAjaKka = ((f17 - f7) * f22 - (f12 - f7) * f23) * f25;
        this.maJAKka = ((f12 - f7) * f20 - (f17 - f7) * f19) * f25;
        this.mAjaKka = ((f18 - f8) * f22 - (f13 - f8) * f23) * f25;
        this.MAJAKka = ((f13 - f8) * f20 - (f18 - f8) * f19) * f25;
        this.MaJakka = (int)(this.majaKka * 4.2949673E9f);
        this.maJakka = (int)(this.MAjaKka * 4.2949673E9f);
        this.MAJakka = (int)(this.mAjaKka * 4.2949673E9f);
        if (f10 > f5) {
            f = f19 / f22;
        }
        if (f15 > f5) {
            f2 = f20 / f23;
        }
        if (f15 > f10) {
            f3 = f21 / f24;
        }
        boolean bl3 = bl = f2 > f;
        if (f5 == f10) {
            boolean bl4 = bl = f4 > f9;
        }
        if (f10 == f15) {
            boolean bl5 = bl = f14 > f9;
        }
        if (!bl) {
            this.mAJAKka = f2;
            this.majAKka = this.mAJAKka * this.majaKka + this.MaJAKka;
            this.MAjAKka = this.mAJAKka * this.MAjaKka + this.maJAKka;
            this.mAjAKka = this.mAJAKka * this.mAjaKka + this.MAJAKka;
            float f26 = 65536 - (n - (n2 << 16));
            this.MaJaKka = (int)(f4 + f26 * this.mAJAKka);
            this.MAJaKka = kaajmma.MajAkKa((f6 * 65536.0f + f26 * this.majAKka) * 65536.0f);
            this.mAJaKka = kaajmma.MajAkKa((f7 * 65536.0f + f26 * this.MAjAKka) * 65536.0f);
            this.MajaKka = kaajmma.MajAkKa((f8 * 65536.0f + f26 * this.mAjAKka) * 65536.0f);
            if (n2 < n4) {
                this.MajAKka = f;
                this.maJaKka = (int)(f4 + f26 * this.MajAKka);
                this.mAJAkKa(kmaamma2, n2, n4);
            }
            if (n4 < n6) {
                this.MajAKka = f3;
                this.maJaKka = (int)(f9 + (float)(65536 - (n3 - (n4 << 16))) * this.MajAKka);
                this.mAJAkKa(kmaamma2, n4, n6);
                return;
            }
        } else {
            this.MajAKka = f2;
            float f27 = 65536 - (n - (n2 << 16));
            this.maJaKka = (int)(f4 + f27 * this.MajAKka);
            if (n2 < n4) {
                this.mAJAKka = f;
                this.majAKka = this.mAJAKka * this.majaKka + this.MaJAKka;
                this.MAjAKka = this.mAJAKka * this.MAjaKka + this.maJAKka;
                this.mAjAKka = this.mAJAKka * this.mAjaKka + this.MAJAKka;
                this.MaJaKka = (int)(f4 + f27 * this.mAJAKka);
                this.MAJaKka = kaajmma.MajAkKa((f6 * 65536.0f + f27 * this.majAKka) * 65536.0f);
                this.mAJaKka = kaajmma.MajAkKa((f7 * 65536.0f + f27 * this.MAjAKka) * 65536.0f);
                this.MajaKka = kaajmma.MajAkKa((f8 * 65536.0f + f27 * this.mAjAKka) * 65536.0f);
                this.mAJAkKa(kmaamma2, n2, n4);
            }
            if (n4 < n6) {
                this.mAJAKka = f3;
                this.majAKka = this.mAJAKka * this.majaKka + this.MaJAKka;
                this.MAjAKka = this.mAJAKka * this.MAjaKka + this.maJAKka;
                this.mAjAKka = this.mAJAKka * this.mAjaKka + this.MAJAKka;
                f27 = 65536 - (n3 - (n4 << 16));
                this.MaJaKka = (int)(f9 + f27 * this.mAJAKka);
                this.MAJaKka = kaajmma.MajAkKa((f11 * 65536.0f + f27 * this.majAKka) * 65536.0f);
                this.mAJaKka = kaajmma.MajAkKa((f12 * 65536.0f + f27 * this.MAjAKka) * 65536.0f);
                this.MajaKka = kaajmma.MajAkKa((f13 * 65536.0f + f27 * this.mAjAKka) * 65536.0f);
                this.mAJAkKa(kmaamma2, n4, n6);
            }
        }
    }

    void mAJAkKa(kmaamma kmaamma2, int n, int n2) {
        this.mAJAkKA = kmaamma2.MaJaKKa;
        this.MajAkKA = kmaamma2.maJaKKa;
        this.majakka = (int)(this.mAJAKka * 65536.0f);
        this.mAjakka = (int)(this.majAKka * 4.2949673E9f);
        this.MaJAkka = (int)(this.MAjAKka * 4.2949673E9f);
        this.maJAkka = (int)(this.mAjAKka * 4.2949673E9f);
        this.MAjakka = (int)(this.MajAKka * 65536.0f);
        n *= this.MAjAkKA;
        n2 *= this.MAjAkKA;
        switch (kmaamma2.mAjAkka) {
            default: {
                this.majakKa(n, n2);
                return;
            }
            case 51: {
                this.mAJakKa(n, n2);
                return;
            }
            case 35: {
                this.maJakKa(n, n2);
                return;
            }
            case 259: {
                this.maJAkKa(n, n2);
                return;
            }
            case 3: {
                this.majAkKa(n, n2);
                return;
            }
            case 49: {
                this.MAJakKa(n, n2);
                return;
            }
            case 33: {
                this.MAjAkKa(n, n2);
                return;
            }
            case 17: 
        }
        this.MAJAkKa(n, n2);
    }

    /*
     * Unable to fully structure code
     */
    void majakKa(int var1_1, int var2_2) {
        var3_3 = this.majakka;
        var4_4 = this.MAjakka;
        var5_5 = this.mAjakka;
        var6_6 = this.MaJAkka;
        var7_7 = this.MAJaKka;
        var8_8 = this.mAJaKka;
        var9_9 = this.MaJakka;
        var10_10 = this.maJakka;
        var11_11 = var9_9 >> 8;
        var12_12 = var10_10 >> 8;
        var13_13 = this.MajAkKA.MAJakKa;
        var14_14 = this.majAkKA.MAJakKa;
        while (var1_1 < var2_2) {
            var15_15 = this.MaJaKka >> 16;
            var16_16 = this.maJaKka >> 16;
            if (var15_15 < 0) {
                var15_15 = 0;
            } else if (var15_15 > this.MAjAkKA) {
                var15_15 = this.MAjAkKA;
            }
            if (var16_16 < 0) {
                var16_16 = 0;
            } else if (var16_16 > this.MAjAkKA) {
                var16_16 = this.MAjAkKA;
            }
            var17_17 = -(this.MaJaKka - (var15_15 << 16) + 255 >> 8);
            var18_18 = var7_7 + var17_17 * var11_11;
            var19_19 = var8_8 + var17_17 * var12_12;
            var20_20 = var1_1 + var15_15;
            var21_21 = var1_1 + var16_16;
            switch (var21_21 - var20_20 & 3) {
                case 3: {
                    var14_14[var20_20++] = var13_13[((var19_19 += var10_10) >>> 24 << 8) + ((var18_18 += var9_9) >>> 24)];
                }
                case 2: {
                    var14_14[var20_20++] = var13_13[((var19_19 += var10_10) >>> 24 << 8) + ((var18_18 += var9_9) >>> 24)];
                }
                case 1: {
                    var14_14[var20_20++] = var13_13[((var19_19 += var10_10) >>> 24 << 8) + ((var18_18 += var9_9) >>> 24)];
                    ** GOTO lbl47
                }
                default: {
                    if (var20_20 < var21_21) ** continue;
                }
            }
            while (true) lbl-1000:
            // 2 sources

            {
                var14_14[var20_20] = var13_13[((var19_19 += var10_10) >>> 24 << 8) + ((var18_18 += var9_9) >>> 24)];
                var14_14[var20_20 + 1] = var13_13[((var19_19 += var10_10) >>> 24 << 8) + ((var18_18 += var9_9) >>> 24)];
                var14_14[var20_20 + 2] = var13_13[((var19_19 += var10_10) >>> 24 << 8) + ((var18_18 += var9_9) >>> 24)];
                var14_14[var20_20 + 3] = var13_13[((var19_19 += var10_10) >>> 24 << 8) + ((var18_18 += var9_9) >>> 24)];
                var20_20 += 4;
lbl47:
                // 2 sources

                if (var20_20 < var21_21) ** GOTO lbl-1000
                break;
            }
            this.MaJaKka += var3_3;
            this.maJaKka += var4_4;
            var7_7 += var5_5;
            var8_8 += var6_6;
            var1_1 += this.MAjAkKA;
        }
        this.MAJaKka = var7_7;
        this.mAJaKka = var8_8;
    }

    void MAJakKa(int n, int n2) {
        int n3 = this.majakka;
        int n4 = this.MAjakka;
        int n5 = this.mAjakka;
        int n6 = this.MaJAkka;
        int n7 = this.MAJaKka;
        int n8 = this.mAJaKka;
        int n9 = this.MaJakka;
        int n10 = this.maJakka;
        int n11 = n9 >> 8;
        int n12 = n10 >> 8;
        int[] nArray = this.MajAkKA.MAJakKa;
        int[] nArray2 = this.majAkKA.MAJakKa;
        while (n < n2) {
            int n13 = this.MaJaKka >> 16;
            int n14 = this.maJaKka >> 16;
            if (n13 < 0) {
                n13 = 0;
            } else if (n13 > this.MAjAkKA) {
                n13 = this.MAjAkKA;
            }
            if (n14 < 0) {
                n14 = 0;
            } else if (n14 > this.MAjAkKA) {
                n14 = this.MAjAkKA;
            }
            int n15 = -(this.MaJaKka - (n13 << 16) >> 8);
            int n16 = n7 + n15 * n11;
            int n17 = n8 + n15 * n12;
            int n18 = n + n13;
            int n19 = n + n14;
            while (n18 < n19) {
                int n20 = nArray[((n17 += n10) >>> 24 << 8) + ((n16 += n9) >>> 24)] + nArray2[n18];
                int n21 = n20 & 0x10040100;
                nArray2[n18++] = n20 - n21 | n21 - (n21 >> 8);
            }
            this.MaJaKka += n3;
            this.maJaKka += n4;
            n7 += n5;
            n8 += n6;
            n += this.MAjAkKA;
        }
        this.MAJaKka = n7;
        this.mAJaKka = n8;
    }

    void MAjAkKa(int n, int n2) {
        int n3 = this.majakka;
        int n4 = this.MAjakka;
        int n5 = this.mAjakka;
        int n6 = this.MaJAkka;
        int n7 = this.MAJaKka;
        int n8 = this.mAJaKka;
        int n9 = this.MaJakka;
        int n10 = this.maJakka;
        int n11 = n9 >> 8;
        int n12 = n10 >> 8;
        int[] nArray = this.MajAkKA.MAJakKa;
        int[] nArray2 = this.majAkKA.MAJakKa;
        while (n < n2) {
            int n13 = this.MaJaKka >> 16;
            int n14 = this.maJaKka >> 16;
            if (n13 < 0) {
                n13 = 0;
            } else if (n13 > this.MAjAkKA) {
                n13 = this.MAjAkKA;
            }
            if (n14 < 0) {
                n14 = 0;
            } else if (n14 > this.MAjAkKA) {
                n14 = this.MAjAkKA;
            }
            int n15 = -(this.MaJaKka - (n13 << 16) + 255 >> 8);
            int n16 = n7 + n15 * n11;
            int n17 = n8 + n15 * n12;
            int n18 = n + n13;
            int n19 = n + n14;
            while (n18 < n19) {
                int n20 = nArray[((n17 += n10) >>> 24 << 8) + ((n16 += n9) >>> 24)] + (nArray2[n18] >> 1);
                int n21 = n20 & 0x10040100;
                nArray2[n18++] = n20 - n21 | n21 - (n21 >> 8);
            }
            this.MaJaKka += n3;
            this.maJaKka += n4;
            n7 += n5;
            n8 += n6;
            n += this.MAjAkKA;
        }
        this.MAJaKka = n7;
        this.mAJaKka = n8;
    }

    void MAJAkKa(int n, int n2) {
        int n3 = this.majakka;
        int n4 = this.MAjakka;
        int n5 = this.mAjakka;
        int n6 = this.MaJAkka;
        int n7 = this.MAJaKka;
        int n8 = this.mAJaKka;
        int n9 = this.MaJakka;
        int n10 = this.maJakka;
        int n11 = n9 >> 8;
        int n12 = n10 >> 8;
        int[] nArray = this.MajAkKA.MAJakKa;
        int[] nArray2 = this.majAkKA.MAJakKa;
        while (n < n2) {
            int n13 = this.MaJaKka >> 16;
            int n14 = this.maJaKka >> 16;
            if (n13 < 0) {
                n13 = 0;
            } else if (n13 > this.MAjAkKA) {
                n13 = this.MAjAkKA;
            }
            if (n14 < 0) {
                n14 = 0;
            } else if (n14 > this.MAjAkKA) {
                n14 = this.MAjAkKA;
            }
            int n15 = -(this.MaJaKka - (n13 << 16) + 255 >> 8);
            int n16 = n7 + n15 * n11;
            int n17 = n8 + n15 * n12;
            int n18 = n + n13;
            int n19 = n + n14;
            while (n18 < n19) {
                int n20 = nArray[((n17 += n10) >>> 24 << 8) + ((n16 += n9) >>> 24)] + ((nArray2[n18] & 0xFC3F0FC) >> 2);
                int n21 = n20 & 0x10040100;
                nArray2[n18++] = n20 - n21 | n21 - (n21 >> 8);
            }
            this.MaJaKka += n3;
            this.maJaKka += n4;
            n7 += n5;
            n8 += n6;
            n += this.MAjAkKA;
        }
        this.MAJaKka = n7;
        this.mAJaKka = n8;
    }

    void majAkKa(int n, int n2) {
        int n3 = this.majakka;
        int n4 = this.MAjakka;
        int n5 = this.mAjakka;
        int n6 = this.MaJAkka;
        int n7 = this.maJAkka;
        int n8 = this.MAJaKka;
        int n9 = this.mAJaKka;
        int n10 = this.MajaKka;
        int n11 = this.MaJakka;
        int n12 = this.maJakka;
        int n13 = this.MAJakka;
        int n14 = n11 >> 8;
        int n15 = n12 >> 8;
        int n16 = n13 >> 8;
        byte[] byArray = this.mAJAkKA.aMAjakK;
        int[] nArray = this.MajAkKA.MAJakKa;
        int[] nArray2 = this.majAkKA.MAJakKa;
        while (n < n2) {
            int n17 = this.MaJaKka >> 16;
            int n18 = this.maJaKka >> 16;
            if (n17 < 0) {
                n17 = 0;
            } else if (n17 > this.MAjAkKA) {
                n17 = this.MAjAkKA;
            }
            if (n18 < 0) {
                n18 = 0;
            } else if (n18 > this.MAjAkKA) {
                n18 = this.MAjAkKA;
            }
            int n19 = -(this.MaJaKka - (n17 << 16) + 255 >> 8);
            int n20 = n8 + n19 * n14;
            int n21 = n9 + n19 * n15;
            int n22 = n10 + n19 * n16;
            int n23 = n + n17;
            int n24 = n + n18;
            while (n23 < n24) {
                nArray2[n23++] = nArray[((n22 += n13) >>> 24 << 8) + (byArray[((n21 += n12) >>> 24 << 8) + ((n20 += n11) >>> 24)] & 0xFF)];
            }
            this.MaJaKka += n3;
            this.maJaKka += n4;
            n8 += n5;
            n9 += n6;
            n10 += n7;
            n += this.MAjAkKA;
        }
        this.MAJaKka = n8;
        this.mAJaKka = n9;
        this.MajaKka = n10;
    }

    void maJAkKa(int n, int n2) {
        int n3 = this.majakka;
        int n4 = this.MAjakka;
        int n5 = this.mAjakka;
        int n6 = this.MaJAkka;
        int n7 = this.maJAkka;
        int n8 = this.MAJaKka;
        int n9 = this.mAJaKka;
        int n10 = this.MajaKka;
        int n11 = this.MaJakka;
        int n12 = this.maJakka;
        int n13 = this.MAJakka;
        int n14 = n11 >> 8;
        int n15 = n12 >> 8;
        int n16 = n13 >> 8;
        byte[] byArray = this.mAJAkKA.aMAjakK;
        int[] nArray = this.MajAkKA.MAJakKa;
        int[] nArray2 = this.majAkKA.MAJakKa;
        while (n < n2) {
            int n17 = this.MaJaKka >> 16;
            int n18 = this.maJaKka >> 16;
            if (n17 < 0) {
                n17 = 0;
            } else if (n17 > this.MAjAkKA) {
                n17 = this.MAjAkKA;
            }
            if (n18 < 0) {
                n18 = 0;
            } else if (n18 > this.MAjAkKA) {
                n18 = this.MAjAkKA;
            }
            int n19 = -(this.MaJaKka - (n17 << 16) + 255 >> 8);
            int n20 = n8 + n19 * n14;
            int n21 = n9 + n19 * n15;
            int n22 = n10 + n19 * n16;
            int n23 = n + n17;
            int n24 = n + n18;
            while (n23 < n24) {
                n22 += n13;
                n21 += n12;
                n20 += n11;
                int n25 = nArray2[n23];
                if (n25 < 0) {
                    int n26 = (n25 += nArray[(n22 >>> 24 << 8) + (byArray[(n21 >>> 24 << 8) + (n20 >>> 24)] & 0xFF)]) & 0x10040100;
                    nArray2[n23] = (n25 - n26 | n26 - (n26 >> 8)) & Integer.MAX_VALUE;
                }
                ++n23;
            }
            this.MaJaKka += n3;
            this.maJaKka += n4;
            n8 += n5;
            n9 += n6;
            n10 += n7;
            n += this.MAjAkKA;
        }
        this.MAJaKka = n8;
        this.mAJaKka = n9;
        this.MajaKka = n10;
    }

    void mAJakKa(int n, int n2) {
        int n3 = this.majakka;
        int n4 = this.MAjakka;
        int n5 = this.mAjakka;
        int n6 = this.MaJAkka;
        int n7 = this.maJAkka;
        int n8 = this.MAJaKka;
        int n9 = this.mAJaKka;
        int n10 = this.MajaKka;
        int n11 = this.MaJakka;
        int n12 = this.maJakka;
        int n13 = this.MAJakka;
        int n14 = n11 >> 8;
        int n15 = n12 >> 8;
        int n16 = n13 >> 8;
        byte[] byArray = this.mAJAkKA.aMAjakK;
        int[] nArray = this.MajAkKA.MAJakKa;
        int[] nArray2 = this.majAkKA.MAJakKa;
        while (n < n2) {
            int n17 = this.MaJaKka >> 16;
            int n18 = this.maJaKka >> 16;
            if (n17 < 0) {
                n17 = 0;
            } else if (n17 > this.MAjAkKA) {
                n17 = this.MAjAkKA;
            }
            if (n18 < 0) {
                n18 = 0;
            } else if (n18 > this.MAjAkKA) {
                n18 = this.MAjAkKA;
            }
            int n19 = -(this.MaJaKka - (n17 << 16) + 255 >> 8);
            int n20 = n8 + n19 * n14;
            int n21 = n9 + n19 * n15;
            int n22 = n10 + n19 * n16;
            int n23 = n + n17;
            int n24 = n + n18;
            while (n23 < n24) {
                int n25 = nArray2[n23] + nArray[((n22 += n13) >>> 24 << 8) + (byArray[((n21 += n12) >>> 24 << 8) + ((n20 += n11) >>> 24)] & 0xFF)];
                int n26 = n25 & 0x10040100;
                nArray2[n23++] = n25 - n26 | n26 - (n26 >> 8);
            }
            this.MaJaKka += n3;
            this.maJaKka += n4;
            n8 += n5;
            n9 += n6;
            n10 += n7;
            n += this.MAjAkKA;
        }
        this.MAJaKka = n8;
        this.mAJaKka = n9;
        this.MajaKka = n10;
    }

    void maJakKa(int n, int n2) {
        int n3 = this.majakka;
        int n4 = this.MAjakka;
        int n5 = this.mAjakka;
        int n6 = this.MaJAkka;
        int n7 = this.maJAkka;
        int n8 = this.MAJaKka;
        int n9 = this.mAJaKka;
        int n10 = this.MajaKka;
        int n11 = this.MaJakka;
        int n12 = this.maJakka;
        int n13 = this.MAJakka;
        int n14 = n11 >> 8;
        int n15 = n12 >> 8;
        int n16 = n13 >> 8;
        byte[] byArray = this.mAJAkKA.aMAjakK;
        int[] nArray = this.MajAkKA.MAJakKa;
        int[] nArray2 = this.majAkKA.MAJakKa;
        while (n < n2) {
            int n17 = this.MaJaKka >> 16;
            int n18 = this.maJaKka >> 16;
            if (n17 < 0) {
                n17 = 0;
            } else if (n17 > this.MAjAkKA) {
                n17 = this.MAjAkKA;
            }
            if (n18 < 0) {
                n18 = 0;
            } else if (n18 > this.MAjAkKA) {
                n18 = this.MAjAkKA;
            }
            int n19 = -(this.MaJaKka - (n17 << 16) + 255 >> 8);
            int n20 = n8 + n19 * n14;
            int n21 = n9 + n19 * n15;
            int n22 = n10 + n19 * n16;
            int n23 = n + n17;
            int n24 = n + n18;
            while (n23 < n24) {
                int n25 = (nArray2[n23] >> 1) + nArray[((n22 += n13) >>> 24 << 8) + (byArray[((n21 += n12) >>> 24 << 8) + ((n20 += n11) >>> 24)] & 0xFF)];
                int n26 = n25 & 0x10040100;
                nArray2[n23++] = n25 - n26 | n26 - (n26 >> 8);
            }
            this.MaJaKka += n3;
            this.maJaKka += n4;
            n8 += n5;
            n9 += n6;
            n10 += n7;
            n += this.MAjAkKA;
        }
        this.MAJaKka = n8;
        this.mAJaKka = n9;
        this.MajaKka = n10;
    }
}
