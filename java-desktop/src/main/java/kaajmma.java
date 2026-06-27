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
        // Match the original bytecode sequence `f2l; l2i` rather than Java's usual `f2i`.
        // This preserves the original wraparound semantics for oversized fixed-point values.
        return (int)(long)f;
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

    void KAMAjKa(kmaamma kmaamma2, mmjakka mmjakka2, mmjakka mmjakka3, mmjakka mmjakka4, kmajkmk kmajkmk2, kmajkmk kmajkmk3, kmajkmk kmajkmk4) {
        float f = mmjakka2.amAjAkk / 65536.0f;
        float f2 = mmjakka2.AMAjAkk / 65536.0f;
        float f3 = mmjakka3.amAjAkk / 65536.0f;
        float f4 = mmjakka3.AMAjAkk / 65536.0f;
        float f5 = mmjakka4.amAjAkk / 65536.0f;
        float f6 = mmjakka4.AMAjAkk / 65536.0f;
        float f7 = (f3 - f) * (f6 - f2) - (f4 - f2) * (f5 - f);
        if (f7 == 0.0f) {
            return;
        }
        int n = Math.max(0, (int)Math.floor(Math.min(f, Math.min(f3, f5))));
        int n2 = Math.min(this.MAjAkKA - 1, (int)Math.ceil(Math.max(f, Math.max(f3, f5))));
        int n3 = Math.max(0, (int)Math.floor(Math.min(f2, Math.min(f4, f6))));
        int n4 = Math.min(this.mAjAkKA - 1, (int)Math.ceil(Math.max(f2, Math.max(f4, f6))));
        if (n > n2 || n3 > n4) {
            return;
        }
        float f8 = 1.0f / f7;
        float f9 = 1.0f / mmjakka2.AmAjAkk;
        float f10 = 1.0f / mmjakka3.AmAjAkk;
        float f11 = 1.0f / mmjakka4.AmAjAkk;
        float f12 = kmajkmk2.akKAMAJ * f9;
        float f13 = kmajkmk3.akKAMAJ * f10;
        float f14 = kmajkmk4.akKAMAJ * f11;
        float f15 = kmajkmk2.AKKAMAJ * f9;
        float f16 = kmajkmk3.AKKAMAJ * f10;
        float f17 = kmajkmk4.AKKAMAJ * f11;
        float f18 = mmjakka2.AmajAkk * f9;
        float f19 = mmjakka3.AmajAkk * f10;
        float f20 = mmjakka4.AmajAkk * f11;
        byte[] byArray = kmaamma2.MaJaKKa.aMAjakK;
        int[] nArray = kmaamma2.maJaKKa.MAJakKa;
        int[] nArray2 = this.majAkKA.MAJakKa;
        boolean bl = kmaamma2.mAjAkka == 259;
        int n5 = n3;
        while (n5 <= n4) {
            float f21 = (float)n5 + 0.5f;
            int n6 = n5 * this.MAjAkKA;
            int n7 = n;
            while (n7 <= n2) {
                float f22 = (float)n7 + 0.5f;
                float f23 = ((f3 - f22) * (f6 - f21) - (f4 - f21) * (f5 - f22)) * f8;
                float f24 = ((f5 - f22) * (f2 - f21) - (f6 - f21) * (f - f22)) * f8;
                float f25 = 1.0f - f23 - f24;
                if (f23 >= 0.0f && f24 >= 0.0f && f25 >= 0.0f) {
                    float f26 = f23 * f9 + f24 * f10 + f25 * f11;
                    if (f26 > 0.0f) {
                        float f27 = (f23 * f12 + f24 * f13 + f25 * f14) / f26;
                        float f28 = (f23 * f15 + f24 * f16 + f25 * f17) / f26;
                        float f29 = (f23 * f18 + f24 * f19 + f25 * f20) / f26;
                        int n8 = (int)Math.floor(f27 * 256.0f) & 0xFF;
                        int n9 = (int)Math.floor(f28 * 256.0f) & 0xFF;
                        int n10 = Math.max(0, Math.min(255, (int)(f29 * 256.0f)));
                        int n11 = n6 + n7;
                        int n12 = nArray[(n10 << 8) + (byArray[(n9 << 8) + n8] & 0xFF)];
                        if (bl) {
                            int n13 = nArray2[n11];
                            if (n13 < 0) {
                                int n14 = (n13 += n12) & 0x10040100;
                                nArray2[n11] = (n13 - n14 | n14 - (n14 >> 8)) & Integer.MAX_VALUE;
                            }
                        } else {
                            nArray2[n11] = n12;
                        }
                    }
                }
                ++n7;
            }
            ++n5;
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

    void majakKa(int var1_1, int var2_2) {
        int n = this.majakka;
        int n2 = this.MAjakka;
        int n3 = this.mAjakka;
        int n4 = this.MaJAkka;
        int n5 = this.MAJaKka;
        int n6 = this.mAJaKka;
        int n7 = this.MaJakka;
        int n8 = this.maJakka;
        int n9 = n7 >> 8;
        int n10 = n8 >> 8;
        int[] nArray = this.MajAkKA.MAJakKa;
        int[] nArray2 = this.majAkKA.MAJakKa;
        while (var1_1 < var2_2) {
            int n11 = this.MaJaKka >> 16;
            int n12 = this.maJaKka >> 16;
            if (n11 < 0) {
                n11 = 0;
            } else if (n11 > this.MAjAkKA) {
                n11 = this.MAjAkKA;
            }
            if (n12 < 0) {
                n12 = 0;
            } else if (n12 > this.MAjAkKA) {
                n12 = this.MAjAkKA;
            }
            int n13 = -((this.MaJaKka - (n11 << 16) + 255) >> 8);
            int n14 = n5 + n13 * n9;
            int n15 = n6 + n13 * n10;
            int n16 = var1_1 + n11;
            int n17 = var1_1 + n12;
            while (n16 < n17) {
                nArray2[n16++] = nArray[((n15 += n8) >>> 24 << 8) + ((n14 += n7) >>> 24)];
            }
            this.MaJaKka += n;
            this.maJaKka += n2;
            n5 += n3;
            n6 += n4;
            var1_1 += this.MAjAkKA;
        }
        this.MAJaKka = n5;
        this.mAJaKka = n6;
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
