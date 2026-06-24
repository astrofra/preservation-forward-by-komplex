/*
 * Decompiled with CFR 0.152.
 */
public class mmajmmk {
    protected String jakkaMA;
    public kmaamma[] JAkkaMA;
    public mmjakka[] jAkkaMA;
    public kmajkmk[] JaKKaMA;
    public final mmajmma jaKKaMA = new mmajmma();
    public final mmajmma JAKKaMA = new mmajmma(1.0f, 1.0f, 1.0f);
    public final mmajmma jAKKaMA = new mmajmma();
    public final maajkka JakKaMA = new maajkka();
    float jakKaMA;
    public boolean JAkKaMA;
    public boolean jAkKaMA;
    public boolean JaKkAma;
    public kaajmmk jaKkAma;
    public mmajmmk JAKkAma;
    int jAKkAma;
    maajkka JakkAma;
    float jakkAma;
    int JAkkAma;
    int jAkkAma;
    int JaKKAma;
    int jaKKAma;
    float JAKKAma;
    float jAKKAma;
    float JakKAma;
    float jakKAma;
    float JAkKAma;
    public int jAkKAma;
    public float JaKkama;
    public boolean jaKkama = false;
    public maajkka JAKkama;
    static public int jAKkama;
    static public mmjakka[] Jakkama;
    static public int jakkama;
    static public kmaamma[] JAkkama;

    public mmajmmk() {
    }

    public mmajmmk(mmajmmk mmajmmk2) {
        this.jAkkaMA = new mmjakka[mmajmmk2.jAkkaMA.length];
        int n = 0;
        while (n < this.jAkkaMA.length) {
            this.jAkkaMA[n] = new mmjakka(mmajmmk2.jAkkaMA[n]);
            ++n;
        }
        this.JaKKaMA = new kmajkmk[mmajmmk2.JaKKaMA.length];
        int n2 = 0;
        while (n2 < this.jAkkaMA.length) {
            this.JaKKaMA[n2] = new kmajkmk(mmajmmk2.JaKKaMA[n2]);
            ++n2;
        }
        this.JAkkaMA = new kmaamma[mmajmmk2.JAkkaMA.length];
        int n3 = 0;
        while (n3 < this.JAkkaMA.length) {
            kmaamma kmaamma2 = mmajmmk2.JAkkaMA[n3];
            int n4 = 0;
            while (mmajmmk2.jAkkaMA[n4] != kmaamma2.mAjakKa) {
                ++n4;
            }
            int n5 = 0;
            while (mmajmmk2.jAkkaMA[n5] != kmaamma2.MaJAkKa) {
                ++n5;
            }
            int n6 = 0;
            while (mmajmmk2.jAkkaMA[n6] != kmaamma2.maJAkKa) {
                ++n6;
            }
            int n7 = 0;
            while (mmajmmk2.JaKKaMA[n7] != kmaamma2.MAJAkKa) {
                ++n7;
            }
            int n8 = 0;
            while (mmajmmk2.JaKKaMA[n8] != kmaamma2.mAJAkKa) {
                ++n8;
            }
            int n9 = 0;
            while (mmajmmk2.JaKKaMA[n9] != kmaamma2.MajAkKa) {
                ++n9;
            }
            this.JAkkaMA[n3] = new kmaamma(this, n4, n5, n6, n7, n8, n9);
            this.JAkkaMA[n3].AmaJAKK(kmaamma2);
            ++n3;
        }
        this.jaKKaMA.mAjakKA(mmajmmk2.jaKKaMA);
        this.JAKKaMA.mAjakKA(mmajmmk2.JAKKaMA);
        this.jAKKaMA.mAjakKA(mmajmmk2.jAKKaMA);
        this.JakKaMA.aMajaKk(mmajmmk2.JakKaMA);
        this.jakKaMA = mmajmmk2.jakKaMA;
        this.JAkKaMA = mmajmmk2.JAkKaMA;
        this.JaKkAma = mmajmmk2.JaKkAma;
        this.jAkKaMA = mmajmmk2.jAkKaMA;
        this.kkAMAJa();
    }

    public void KKamaja(boolean bl) {
        this.JaKkAma = bl;
        if (!this.JaKkAma) {
            this.JAKkAma = null;
            return;
        }
        this.JAKkAma = new mmajmmk(this);
        this.JAKkAma.JaKkAma = false;
    }

    public void kkaMaja(String string) {
        this.jakkaMA = string.intern();
    }

    public String kkAMaja() {
        if (this.jakkaMA == null) {
            return "";
        }
        return this.jakkaMA.intern();
    }

    public void KKAmaJa(mmaamma mmaamma2, mmaamma mmaamma3) {
        Object object;
        mmajmma mmajmma2 = new mmajmma();
        int n = 0;
        while (n < this.jAkkaMA.length) {
            mmjakka mmjakka2 = this.jAkkaMA[n];
            object = this.JaKKaMA[n];
            mmajmma2.mAjakKA(mmjakka2);
            mmajmma2.MAJakKA(0.7853982f);
            mmajmma2.majaKKA();
            double d = mmajmma2.mAJAkKA();
            double d2 = Math.min(Math.abs(1.0 / Math.cos(d)), Math.abs(1.0 / Math.cos(1.5707963267948966 - d)));
            kmajmma kmajmma2 = new kmajmma(mmajmma2.maJakKA, mmajmma2.MAJakKA, 0.0);
            kmajmma2.MajAKka();
            kmajmma2.maJakka(Math.acos(Math.abs(mmajmma2.mAJakKA)) / 1.5707963267948966);
            kmajmma2.maJakka(d2);
            double d3 = kmajmma2.MaJAkKA;
            double d4 = kmajmma2.maJAkKA;
            ((kmajkmk)object).akKAMAJ = (float)((d3 + 1.0) / 2.0);
            ((kmajkmk)object).AKKAMAJ = (float)((d4 + 1.0) / 2.0);
            ++n;
        }
        int n2 = 0;
        while (n2 < this.JAkkaMA.length) {
            object = this.JAkkaMA[n2];
            ((kmjamma)object).maJaKKa = ((kmjamma)object).MajaKKa < 0.0f ? mmaamma2 : mmaamma3;
            ++n2;
        }
    }

    public void KkAMaja() {
        mmajmma mmajmma2 = new mmajmma();
        int n = 0;
        while (n < this.jAkkaMA.length) {
            mmjakka mmjakka2 = this.jAkkaMA[n];
            kmajkmk kmajkmk2 = this.JaKKaMA[n];
            mmajmma2.MaJaKka(mmjakka2.aMAJaKK, mmjakka2.AmaJaKK, mmjakka2.amaJaKK);
            mmajmma2.majaKKA();
            double d = 1.5707963267948966 - Math.acos(mmajmma2.MAJakKA);
            double d2 = mmajmma2.MAjAkKA();
            d2 /= Math.PI;
            d2 = -d2;
            kmajkmk2.akKAMAJ = (float)((d2 + 1.0) / 2.0);
            kmajkmk2.AKKAMAJ = (float)(((d /= 1.5707963267948966) + 1.0) / 2.0);
            ++n;
        }
    }

    public void KKaMAJa(mmajmma mmajmma2) {
        mmajmma mmajmma3 = this.jAKKaMA.mAjAKKA(mmajmma2);
        int n = 0;
        while (n < this.jAkkaMA.length) {
            mmjakka mmjakka2 = this.jAkkaMA[n];
            mmjakka2.majAkKA(mmajmma3);
            ++n;
        }
        this.jAKKaMA.mAjakKA(mmajmma2);
    }

    public void kkAmaJa(mmajmma mmajmma2) {
        this.kkamaja(mmajmma2.maJakKA, mmajmma2.MAJakKA, mmajmma2.mAJakKA);
    }

    public void kkamaja(float f, float f2, float f3) {
        int n = 0;
        while (n < this.jAkkaMA.length) {
            mmjakka mmjakka2 = this.jAkkaMA[n];
            mmjakka2.MajAKKA(f, f2, f3);
            ++n;
        }
    }

    public void kkAMAJa() {
        Object object;
        int n = 0;
        while (n < this.JAkkaMA.length) {
            object = this.JAkkaMA[n];
            ((kmjamma)object).aMaJAkK();
            ((kmjamma)object).AMaJAkK();
            ++n;
        }
        if (this.JAkKaMA) {
            this.JaKKaMA = new kmajkmk[this.jAkkaMA.length];
            int n2 = 0;
            while (n2 < this.jAkkaMA.length) {
                this.JaKKaMA[n2] = new kmajkmk();
                ++n2;
            }
            int n3 = 0;
            while (n3 < this.JAkkaMA.length) {
                kmaamma kmaamma2 = this.JAkkaMA[n3];
                int n4 = 0;
                while (this.jAkkaMA[n4] != kmaamma2.mAjakKa) {
                    ++n4;
                }
                int n5 = 0;
                while (this.jAkkaMA[n5] != kmaamma2.MaJAkKa) {
                    ++n5;
                }
                int n6 = 0;
                while (this.jAkkaMA[n6] != kmaamma2.maJAkKa) {
                    ++n6;
                }
                this.JAkkaMA[n3].MAJAkKa = this.JaKKaMA[n4];
                this.JAkkaMA[n3].mAJAkKa = this.JaKKaMA[n5];
                this.JAkkaMA[n3].MajAkKa = this.JaKKaMA[n6];
                ++n3;
            }
        }
        object = new mmajmma();
        float f = 0.0f;
        int n7 = 0;
        while (n7 < this.jAkkaMA.length) {
            mmjakka mmjakka2 = this.jAkkaMA[n7];
            ((mmajmma)object).MaJaKka(mmjakka2.aMAJaKK, mmjakka2.AmaJaKK, mmjakka2.amaJaKK);
            ((mmajmma)object).majaKKA();
            mmjakka2.aMAJaKK = ((mmajmma)object).maJakKA;
            mmjakka2.AmaJaKK = ((mmajmma)object).MAJakKA;
            mmjakka2.amaJaKK = ((mmajmma)object).mAJakKA;
            float f2 = mmjakka2.MAjaKKA();
            if (f2 > f) {
                f = f2;
            }
            ++n7;
        }
        this.jakKaMA = (float)Math.sqrt(f);
    }

    public void kkAmAJa(mmaamma mmaamma2, float f) {
        int n = 0;
        while (n < this.jAkkaMA.length) {
            mmjakka mmjakka2 = this.jAkkaMA[n];
            kmajkmk kmajkmk2 = this.JaKKaMA[n];
            kmajkmk2.akKAMAJ = -mmjakka2.MAJakKA / f * (float)mmaamma2.amAjakK;
            kmajkmk2.AKKAMAJ = 0.0f;
            ++n;
        }
        int n2 = 0;
        while (n2 < this.JAkkaMA.length) {
            this.JAkkaMA[n2].maJaKKa = mmaamma2;
            ++n2;
        }
    }

    public void KkaMaja(mmaamma mmaamma2) {
        int n = 0;
        while (n < this.JAkkaMA.length) {
            this.JAkkaMA[n].maJaKKa = mmaamma2;
            ++n;
        }
    }

    public void KkAMAJa(mmaamma mmaamma2, kmajkka kmajkka2) {
        int n = 0;
        while (n < this.JAkkaMA.length) {
            this.JAkkaMA[n].maJaKKa = mmaamma2;
            this.JAkkaMA[n].MaJaKKa = kmajkka2;
            ++n;
        }
    }

    public void KkAmAJa(int n) {
        int n2 = 0;
        while (n2 < this.JAkkaMA.length) {
            this.JAkkaMA[n2].mAjAkka = n;
            ++n2;
        }
    }

    public void kKAmaja() {
        int n = 0;
        while (n < this.JAkkaMA.length) {
            kmaamma kmaamma2 = this.JAkkaMA[n];
            kmaamma2.MAJaKKa = -kmaamma2.MAJaKKa;
            kmaamma2.mAJaKKa = -kmaamma2.mAJaKKa;
            kmaamma2.MajaKKa = -kmaamma2.MajaKKa;
            ++n;
        }
    }

    public void kkaMAJa(kaajmmk kaajmmk2, int n) {
        this.jaKkAma = kaajmmk2;
        this.jAKkAma = kmjamma.MAjaKKa;
        this.kKAmAJa();
        if (this.jAkKAma != 0) {
            this.KKAmaja();
            if (this.jAkKaMA) {
                this.KKaMaja();
            }
            if (this.JAkKaMA) {
                this.KKAMaja();
            }
            this.kKamaja();
            return;
        }
        if (this.JAkKaMA) {
            this.KKAMaja();
        }
        if (n == 0) {
            this.KkAmaja();
            if (this.jAkKaMA) {
                this.KKaMaja();
            }
            this.kkAmaja();
            return;
        }
        this.kKAmaJa();
        if (this.jAkKaMA) {
            this.KKaMaja();
        }
        this.KkaMAJa();
        this.kkamAJa();
    }

    mmajmma kKAMaja() {
        mmajmma mmajmma2 = new mmajmma(this.jaKKaMA);
        mmajmma2.maJAKKA(this.jaKkAma.JAKkaMa);
        this.JakKaMA.AmajAKk().amaJaKk(mmajmma2);
        return new mmajmma(mmajmma2.maJakKA / this.JAKKaMA.maJakKA, mmajmma2.MAJakKA / this.JAKKaMA.MAJakKA, mmajmma2.mAJakKA / this.JAKKaMA.mAJakKA);
    }

    void KKaMaja() {
        float f = this.jaKkAma.JaKKaMa;
        float f2 = this.jaKkAma.jAkkaMa;
        mmjakka[] mmjakkaArray = this.jAkkaMA;
        int n = mmjakkaArray.length;
        int n2 = 0;
        while (n2 < n) {
            mmjakka mmjakka2 = mmjakkaArray[n2];
            float f3 = (mmjakka2.AmAjAkk - f) / (f2 - f);
            if (f3 < 0.0f) {
                f3 = 0.0f;
            }
            if (f3 > 0.99609375f) {
                f3 = 0.99609375f;
            }
            mmjakka2.AmajAkk = f3;
            ++n2;
        }
    }

    public void kKaMaja() {
        int n = 0;
        while (n < this.JAkkaMA.length) {
            this.JAkkaMA[n].amaJAkK();
            ++n;
        }
    }

    public void kKAmAJa() {
        this.JakkAma = this.JakKaMA.amAJaKk(this.jaKkAma.jAKkaMa.AmajAKk());
        this.JakkAma.AmaJAKk(this.JAKKaMA);
        this.jakkAma = (float)((double)(65536 * (this.jaKkAma.JAkKAMa >> 1)) / Math.tan(this.jaKkAma.jaKkaMa / 2.0f));
        this.JAkkAma = this.jaKkAma.JAkKAMa << 16 >> 1;
        this.jAkkAma = this.jaKkAma.jAkKAMa << 16 >> 1;
        this.JaKKAma = this.jaKkAma.JAkKAMa << 16;
        this.jaKKAma = this.jaKkAma.jAkKAMa << 16;
        this.JAKKAma = this.jaKkAma.JAkkaMa;
        this.jAKKAma = this.jaKkAma.jAkkaMa;
        mmajmma mmajmma2 = new mmajmma(this.jaKKaMA);
        mmajmma2.maJAKKA(this.jaKkAma.JAKkaMa);
        this.jaKkAma.jAKkaMa.AmajAKk().amaJaKk(mmajmma2);
        this.JakKAma = mmajmma2.maJakKA;
        this.jakKAma = mmajmma2.MAJakKA;
        this.JAkKAma = mmajmma2.mAJakKA;
    }

    public void KkAmaja() {
        this.KKAMAJa(this.jAkkaMA, this.jAkkaMA.length);
    }

    public void KKAMAJa(mmjakka[] mmjakkaArray, int n) {
        maajkka maajkka2 = this.JakkAma;
        float f = maajkka2.AMaJakK;
        float f2 = maajkka2.aMaJakK;
        float f3 = maajkka2.AmAjAKK;
        float f4 = maajkka2.amAjAKK;
        float f5 = maajkka2.AMAjAKK;
        float f6 = maajkka2.aMAjAKK;
        float f7 = maajkka2.AmajAKK;
        float f8 = maajkka2.amajAKK;
        float f9 = maajkka2.AMajAKK;
        float f10 = this.JakKAma;
        float f11 = this.jakKAma;
        float f12 = this.JAkKAma;
        float f13 = this.jakkAma;
        float f14 = this.JAkkAma;
        float f15 = this.jAkkAma;
        int n2 = 0;
        while (n2 < n) {
            mmjakka mmjakka2 = mmjakkaArray[n2++];
            float f16 = mmjakka2.maJakKA;
            float f17 = mmjakka2.MAJakKA;
            float f18 = mmjakka2.mAJakKA;
            float f19 = f * f16 + f4 * f17 + f7 * f18 + f10;
            float f20 = f2 * f16 + f5 * f17 + f8 * f18 + f11;
            float f21 = f3 * f16 + f6 * f17 + f9 * f18 + f12;
            float f22 = f13 / f21;
            mmjakka2.amAjAkk = f19 * f22 + f14;
            mmjakka2.AMAjAkk = -(f20 * f22) + f15;
            mmjakka2.AmAjAkk = f21;
        }
    }

    public void kKAmaJa() {
        this.kKamAJa(this.jAkkaMA, this.jAkkaMA.length);
    }

    public void kKamAJa(mmjakka[] mmjakkaArray, int n) {
        maajkka maajkka2 = this.JakkAma;
        float f = maajkka2.AMaJakK;
        float f2 = maajkka2.aMaJakK;
        float f3 = maajkka2.AmAjAKK;
        float f4 = maajkka2.amAjAKK;
        float f5 = maajkka2.AMAjAKK;
        float f6 = maajkka2.aMAjAKK;
        float f7 = maajkka2.AmajAKK;
        float f8 = maajkka2.amajAKK;
        float f9 = maajkka2.AMajAKK;
        float f10 = this.JakKAma;
        float f11 = this.jakKAma;
        float f12 = this.JAkKAma;
        float f13 = this.jakkAma;
        float f14 = this.JAkkAma;
        float f15 = this.jAkkAma;
        float f16 = this.JAKKAma;
        float f17 = this.jAKKAma;
        float f18 = this.JaKKAma;
        float f19 = this.jaKKAma;
        int n2 = 0;
        while (n2 < n) {
            mmjakka mmjakka2 = mmjakkaArray[n2++];
            float f20 = f * mmjakka2.maJakKA + f4 * mmjakka2.MAJakKA + f7 * mmjakka2.mAJakKA + f10;
            float f21 = f2 * mmjakka2.maJakKA + f5 * mmjakka2.MAJakKA + f8 * mmjakka2.mAJakKA + f11;
            float f22 = f3 * mmjakka2.maJakKA + f6 * mmjakka2.MAJakKA + f9 * mmjakka2.mAJakKA + f12;
            if (f22 < f16) {
                mmjakka2.aMAjAkk = 32768;
                f22 = this.JAKKAma;
            } else {
                mmjakka2.aMAjAkk = f22 > f17 ? 4096 : 0;
            }
            float f23 = f13 / f22;
            mmjakka2.amAjAkk = f20 * f23 + f14;
            mmjakka2.AMAjAkk = -(f21 * f23) + f15;
            mmjakka2.AMaJaKK = f20;
            mmjakka2.aMaJaKK = f21;
            mmjakka2.AmAjAkk = f22;
            if (mmjakka2.amAjAkk < 0.0f) {
                mmjakka2.aMAjAkk |= 1;
            } else if (mmjakka2.amAjAkk >= f18) {
                mmjakka2.aMAjAkk |= 8;
            }
            if (mmjakka2.AMAjAkk < 0.0f) {
                mmjakka2.aMAjAkk |= 0x40;
                continue;
            }
            if (!(mmjakka2.AMAjAkk >= f19)) continue;
            mmjakka2.aMAjAkk |= 0x200;
        }
    }

    public void KKAmaja() {
        this.kKAMAJa(this.jAkkaMA, this.jAkkaMA.length);
    }

    public void kKAMAJa(mmjakka[] mmjakkaArray, int n) {
        maajkka maajkka2 = this.JakkAma;
        float f = maajkka2.AMaJakK;
        float f2 = maajkka2.aMaJakK;
        float f3 = maajkka2.AmAjAKK;
        float f4 = maajkka2.amAjAKK;
        float f5 = maajkka2.AMAjAKK;
        float f6 = maajkka2.aMAjAKK;
        float f7 = maajkka2.AmajAKK;
        float f8 = maajkka2.amajAKK;
        float f9 = maajkka2.AMajAKK;
        maajkka maajkka3 = this.JakKaMA;
        float f10 = this.JakKAma;
        float f11 = this.jakKAma;
        float f12 = this.JAkKAma;
        float f13 = this.jakkAma;
        float f14 = this.JAkkAma;
        float f15 = this.jAkkAma;
        float f16 = this.JAKKAma;
        float f17 = this.jAKKAma;
        float f18 = this.JaKKAma;
        float f19 = this.jaKKAma;
        int n2 = this.jAkKAma % 5;
        mmajmma mmajmma2 = new mmajmma();
        int n3 = 0;
        while (n3 < n) {
            mmjakka mmjakka2 = mmjakkaArray[n3++];
            float f20 = mmjakka2.maJakKA;
            float f21 = mmjakka2.MAJakKA;
            float f22 = mmjakka2.mAJakKA;
            float f23 = 0.0f;
            float f24 = 0.8f;
            float f25 = 0.0f;
            f20 -= f23;
            f21 -= f24;
            f22 -= f25;
            switch (n2) {
                case 1: {
                    float f26 = (float)Math.atan2(f20, f21) * 4.0f + this.JaKkama;
                    f26 = (float)Math.sin(f26);
                    f26 = 1.0f + f26 * 0.5f;
                    f20 *= f26;
                    f21 *= f26;
                    f22 *= f26;
                    break;
                }
                case 2: {
                    float f26 = f20 * f20 + f21 * f21 + f22 * f22;
                    f26 = (float)((double)f26 * 0.015);
                    mmajmma2.MaJaKka(f20, f21, f22);
                    mmajmma2.MAJakKA(f26 *= (float)Math.sin(this.JaKkama + f22 * 0.1f));
                    f20 = mmajmma2.maJakKA;
                    f21 = mmajmma2.MAJakKA;
                    f22 = mmajmma2.mAJakKA;
                    break;
                }
                case 3: {
                    float f26 = (float)Math.sqrt(f20 * f20 + f21 * f21 + f22 * f22);
                    f26 = (float)Math.sin(f26 * 9.3f + this.JaKkama * 3.0f);
                    f26 = 1.0f + f26 * 0.09f;
                    f20 *= f26;
                    f21 *= f26;
                    f22 *= f26;
                    break;
                }
            }
            float f27 = f * (f20 += f23) + f4 * (f21 += f24) + f7 * (f22 += f25) + f10;
            float f28 = f2 * f20 + f5 * f21 + f8 * f22 + f11;
            float f29 = f3 * f20 + f6 * f21 + f9 * f22 + f12;
            if (f29 < f16) {
                mmjakka2.aMAjAkk = 32768;
                f29 = this.JAKKAma;
            } else {
                mmjakka2.aMAjAkk = f29 > f17 ? 4096 : 0;
            }
            float f30 = f13 / f29;
            mmjakka2.amAjAkk = f27 * f30 + f14;
            mmjakka2.AMAjAkk = -(f28 * f30) + f15;
            mmjakka2.AMaJaKK = f27;
            mmjakka2.aMaJaKK = f28;
            mmjakka2.AmAjAkk = f29;
            if (mmjakka2.amAjAkk < 0.0f) {
                mmjakka2.aMAjAkk |= 1;
            } else if (mmjakka2.amAjAkk >= f18) {
                mmjakka2.aMAjAkk |= 8;
            }
            if (mmjakka2.AMAjAkk < 0.0f) {
                mmjakka2.aMAjAkk |= 0x40;
                continue;
            }
            if (!(mmjakka2.AMAjAkk >= f19)) continue;
            mmjakka2.aMAjAkk |= 0x200;
        }
    }

    public void kkAmaja() {
        this.Kkamaja(this.JAkkaMA, this.JAkkaMA.length);
    }

    public void Kkamaja(kmaamma[] kmaammaArray, int n) {
        kmaamma[] kmaammaArray2 = kmjamma.majaKKa;
        int n2 = kmjamma.MAjaKKa;
        mmajmma mmajmma2 = this.kKAMaja();
        float f = mmajmma2.maJakKA;
        float f2 = mmajmma2.MAJakKA;
        float f3 = mmajmma2.mAJakKA;
        int n3 = 0;
        while (n3 < n) {
            kmaamma kmaamma2 = kmaammaArray[n3++];
            if (!((f + kmaamma2.mAjakKa.maJakKA) * kmaamma2.MAJaKKa + (f2 + kmaamma2.mAjakKa.MAJakKA) * kmaamma2.mAJaKKa + (f3 + kmaamma2.mAjakKa.mAJakKA) * kmaamma2.MajaKKa < 0.0f)) continue;
            kmaamma2.majAkKa = -(kmaamma2.mAjakKa.AmAjAkk + kmaamma2.MaJAkKa.AmAjAkk + kmaamma2.maJAkKa.AmAjAkk);
            kmaammaArray2[n2++] = kmaamma2;
        }
        kmjamma.MAjaKKa = n2;
    }

    public void KkaMAJa() {
        this.KkAmaJa(this.JAkkaMA, this.JAkkaMA.length);
    }

    public void KkAmaJa(kmaamma[] kmaammaArray, int n) {
        kmaamma[] kmaammaArray2 = kmjamma.majaKKa;
        int n2 = kmjamma.MAjaKKa;
        mmajmma mmajmma2 = this.kKAMaja();
        float f = mmajmma2.maJakKA;
        float f2 = mmajmma2.MAJakKA;
        float f3 = mmajmma2.mAJakKA;
        int n3 = 0;
        while (n3 < n) {
            kmaamma kmaamma2 = kmaammaArray[n3++];
            int n4 = 37449 + kmaamma2.mAjakKa.aMAjAkk + kmaamma2.MaJAkKa.aMAjAkk + kmaamma2.maJAkKa.aMAjAkk;
            if ((n4 & 0x34924) == 0) {
                if (!((f + kmaamma2.mAjakKa.maJakKA) * kmaamma2.MAJaKKa + (f2 + kmaamma2.mAjakKa.MAJakKA) * kmaamma2.mAJaKKa + (f3 + kmaamma2.mAjakKa.mAJakKA) * kmaamma2.MajaKKa < 0.0f)) continue;
                kmaamma2.majAkKa = -(kmaamma2.mAjakKa.AmAjAkk + kmaamma2.MaJAkKa.AmAjAkk + kmaamma2.maJAkKa.AmAjAkk);
                kmaammaArray2[n2++] = kmaamma2;
                continue;
            }
            if ((n4 & 0x24924) != 0 || (n4 & 0x30000) == 0 || !((f + kmaamma2.mAjakKa.maJakKA) * kmaamma2.MAJaKKa + (f2 + kmaamma2.mAjakKa.MAJakKA) * kmaamma2.mAJaKKa + (f3 + kmaamma2.mAjakKa.mAJakKA) * kmaamma2.MajaKKa < 0.0f)) continue;
            kmaamma2.majAkKa = -(kmaamma2.mAjakKa.AmAjAkk + kmaamma2.MaJAkKa.AmAjAkk + kmaamma2.maJAkKa.AmAjAkk);
            this.KKamAJa(kmaamma2);
        }
        kmjamma.MAjaKKa = n2;
    }

    public void KKAmAJa() {
        kmaamma[] kmaammaArray = this.JAkkaMA;
        int n = kmaammaArray.length;
        kmaamma[] kmaammaArray2 = kmjamma.majaKKa;
        int n2 = kmjamma.MAjaKKa;
        int n3 = 0;
        while (n3 < n) {
            kmaamma kmaamma2 = kmaammaArray[n3++];
            float f = kmaamma2.mAjakKa.amAjAkk;
            float f2 = kmaamma2.MaJAkKa.amAjAkk - f;
            float f3 = kmaamma2.mAjakKa.AMAjAkk;
            float f4 = kmaamma2.maJAkKa.AMAjAkk - f3;
            float f5 = kmaamma2.maJAkKa.amAjAkk - f;
            float f6 = kmaamma2.MaJAkKa.AMAjAkk - f3;
            if (!(f2 * f4 - f5 * f6 < 0.0f)) continue;
            kmaamma2.majAkKa = -(kmaamma2.mAjakKa.AmAjAkk + kmaamma2.MaJAkKa.AmAjAkk + kmaamma2.maJAkKa.AmAjAkk);
            kmaammaArray2[n2++] = kmaamma2;
        }
        kmjamma.MAjaKKa = n2;
    }

    public void kKamaja() {
        kmaamma[] kmaammaArray = this.JAkkaMA;
        int n = kmaammaArray.length;
        kmaamma[] kmaammaArray2 = kmjamma.majaKKa;
        int n2 = kmjamma.MAjaKKa;
        float f = this.JAKKAma;
        int n3 = 0;
        while (n3 < n) {
            kmaamma kmaamma2 = kmaammaArray[n3++];
            if (kmaamma2.mAjakKa.AmAjAkk < f || kmaamma2.MaJAkKa.AmAjAkk < f || kmaamma2.maJAkKa.AmAjAkk < f) break;
            float f2 = kmaamma2.mAjakKa.amAjAkk;
            float f3 = kmaamma2.MaJAkKa.amAjAkk - f2;
            float f4 = kmaamma2.mAjakKa.AMAjAkk;
            float f5 = kmaamma2.maJAkKa.AMAjAkk - f4;
            float f6 = kmaamma2.maJAkKa.amAjAkk - f2;
            float f7 = kmaamma2.MaJAkKa.AMAjAkk - f4;
            if (!(f3 * f5 - f6 * f7 < 0.0f)) continue;
            kmaamma2.majAkKa = -(kmaamma2.mAjakKa.AmAjAkk + kmaamma2.MaJAkKa.AmAjAkk + kmaamma2.maJAkKa.AmAjAkk);
            kmaammaArray2[n2++] = kmaamma2;
        }
        kmjamma.MAjaKKa = n2;
    }

    public final void KKAMaja() {
        maajkka maajkka2 = this.JakKaMA;
        if (this.jaKkama) {
            maajkka2 = this.jaKkAma.jAKkaMa.AmajAKk();
        }
        if (this.JAKkama != null) {
            maajkka2 = maajkka2.amAJaKk(this.JAKkama);
        }
        float f = maajkka2.AMaJakK;
        float f2 = maajkka2.aMaJakK;
        float f3 = maajkka2.amAjAKK;
        float f4 = maajkka2.AMAjAKK;
        float f5 = maajkka2.AmajAKK;
        float f6 = maajkka2.amajAKK;
        kmajkmk[] kmajkmkArray = this.JaKKaMA;
        int n = kmajkmkArray.length;
        mmjakka[] mmjakkaArray = this.jAkkaMA;
        int n2 = 0;
        while (n2 < n) {
            kmajkmk kmajkmk2 = kmajkmkArray[n2];
            mmjakka mmjakka2 = mmjakkaArray[n2++];
            kmajkmk2.akKAMAJ = (f * mmjakka2.aMAJaKK + f3 * mmjakka2.AmaJaKK + f5 * mmjakka2.amaJaKK + 1.0f) * 0.5f;
            kmajkmk2.AKKAMAJ = (f2 * mmjakka2.aMAJaKK + f4 * mmjakka2.AmaJaKK + f6 * mmjakka2.amaJaKK + 1.0f) * 0.5f;
        }
    }

    public void KKamAJa(kmaamma kmaamma2) {
        if (jakkama < JAkkama.length) {
            mmajmmk.JAkkama[mmajmmk.jakkama++] = kmaamma2;
        }
    }

    public void kkamAJa() {
        if (jakkama == 0) {
            return;
        }
        int n = 0;
        while (n < jakkama) {
            kmaamma kmaamma2 = JAkkama[n];
            kmajkmk kmajkmk2 = kmaamma2.MAJAkKa;
            kmajkmk kmajkmk3 = kmaamma2.mAJAkKa;
            kmajkmk kmajkmk4 = kmaamma2.MajAkKa;
            mmjakka mmjakka2 = kmaamma2.mAjakKa;
            mmjakka mmjakka3 = kmaamma2.MaJAkKa;
            mmjakka mmjakka4 = kmaamma2.maJAkKa;
            int n2 = mmjakka2.aMAjAkk;
            int n3 = mmjakka3.aMAjAkk;
            int n4 = mmjakka4.aMAjAkk;
            int n5 = ((n2 & 0x8000) + (n3 & 0x8000) * 2 + (n4 & 0x8000) * 4) / 32768;
            switch (n5) {
                case 1: {
                    this.KkamaJa(kmaamma2, mmjakka3, mmjakka4, mmjakka2, kmajkmk3, kmajkmk4, kmajkmk2);
                    break;
                }
                case 2: {
                    this.KkamaJa(kmaamma2, mmjakka2, mmjakka4, mmjakka3, kmajkmk2, kmajkmk4, kmajkmk3);
                    break;
                }
                case 4: {
                    this.KkamaJa(kmaamma2, mmjakka2, mmjakka3, mmjakka4, kmajkmk2, kmajkmk3, kmajkmk4);
                    break;
                }
                case 5: {
                    this.KkamAJa(kmaamma2, mmjakka3, mmjakka2, mmjakka4, kmajkmk3, kmajkmk2, kmajkmk4);
                    break;
                }
                case 3: {
                    this.KkamAJa(kmaamma2, mmjakka4, mmjakka3, mmjakka2, kmajkmk4, kmajkmk3, kmajkmk2);
                    break;
                }
                case 6: {
                    this.KkamAJa(kmaamma2, mmjakka2, mmjakka3, mmjakka4, kmajkmk2, kmajkmk3, kmajkmk4);
                    break;
                }
            }
            ++n;
        }
        this.kKaMAJa(Jakkama, jAKkama);
        jAKkama = 0;
        jakkama = 0;
    }

    void KkamAJa(kmaamma kmaamma2, mmjakka mmjakka2, mmjakka mmjakka3, mmjakka mmjakka4, kmajkmk kmajkmk2, kmajkmk kmajkmk3, kmajkmk kmajkmk4) {
        float f = (mmjakka2.AmAjAkk - this.JAKKAma) / (mmjakka2.AmAjAkk - mmjakka3.AmAjAkk);
        float f2 = mmjakka2.AMaJaKK + f * (mmjakka3.AMaJaKK - mmjakka2.AMaJaKK);
        float f3 = mmjakka2.aMaJaKK + f * (mmjakka3.aMaJaKK - mmjakka2.aMaJaKK);
        mmjakka mmjakka5 = new mmjakka(f2, f3, this.JAKKAma);
        mmjakka5.KaMAjaK(f2, f3, this.JAKKAma);
        kmajkmk kmajkmk5 = new kmajkmk(kmajkmk2.akKAMAJ + f * (kmajkmk3.akKAMAJ - kmajkmk2.akKAMAJ), kmajkmk2.AKKAMAJ + f * (kmajkmk3.AKKAMAJ - kmajkmk2.AKKAMAJ));
        f = (mmjakka2.AmAjAkk - this.JAKKAma) / (mmjakka2.AmAjAkk - mmjakka4.AmAjAkk);
        float f4 = mmjakka2.AMaJaKK + f * (mmjakka4.AMaJaKK - mmjakka2.AMaJaKK);
        float f5 = mmjakka2.aMaJaKK + f * (mmjakka4.aMaJaKK - mmjakka2.aMaJaKK);
        mmjakka mmjakka6 = new mmjakka();
        mmjakka6.KaMAjaK(f4, f5, this.JAKKAma);
        kmajkmk kmajkmk6 = new kmajkmk(kmajkmk2.akKAMAJ + f * (kmajkmk4.akKAMAJ - kmajkmk2.akKAMAJ), kmajkmk2.AKKAMAJ + f * (kmajkmk4.AKKAMAJ - kmajkmk2.AKKAMAJ));
        mmajmmk.Jakkama[mmajmmk.jAKkama++] = mmjakka5;
        mmajmmk.Jakkama[mmajmmk.jAKkama++] = mmjakka6;
        kmaamma kmaamma3 = new kmaamma(kmaamma2, mmjakka2, mmjakka5, mmjakka6, kmajkmk2, kmajkmk5, kmajkmk6);
        kmaamma3.majAkKa = kmaamma2.majAkKa;
        kmjamma.majaKKa[kmjamma.MAjaKKa++] = kmaamma3;
    }

    void KkamaJa(kmaamma kmaamma2, mmjakka mmjakka2, mmjakka mmjakka3, mmjakka mmjakka4, kmajkmk kmajkmk2, kmajkmk kmajkmk3, kmajkmk kmajkmk4) {
        float f = (mmjakka2.AmAjAkk - this.JAKKAma) / (mmjakka2.AmAjAkk - mmjakka4.AmAjAkk);
        float f2 = mmjakka2.AMaJaKK + f * (mmjakka4.AMaJaKK - mmjakka2.AMaJaKK);
        float f3 = mmjakka2.aMaJaKK + f * (mmjakka4.aMaJaKK - mmjakka2.aMaJaKK);
        mmjakka mmjakka5 = new mmjakka();
        mmjakka5.KaMAjaK(f2, f3, this.JAKKAma);
        kmajkmk kmajkmk5 = new kmajkmk(kmajkmk2.akKAMAJ + f * (kmajkmk4.akKAMAJ - kmajkmk2.akKAMAJ), kmajkmk2.AKKAMAJ + f * (kmajkmk4.AKKAMAJ - kmajkmk2.AKKAMAJ));
        f = (mmjakka3.AmAjAkk - this.JAKKAma) / (mmjakka3.AmAjAkk - mmjakka4.AmAjAkk);
        float f4 = mmjakka3.AMaJaKK + f * (mmjakka4.AMaJaKK - mmjakka3.AMaJaKK);
        float f5 = mmjakka3.aMaJaKK + f * (mmjakka4.aMaJaKK - mmjakka3.aMaJaKK);
        mmjakka mmjakka6 = new mmjakka();
        mmjakka6.KaMAjaK(f4, f5, this.JAKKAma);
        kmajkmk kmajkmk6 = new kmajkmk(kmajkmk3.akKAMAJ + f * (kmajkmk4.akKAMAJ - kmajkmk3.akKAMAJ), kmajkmk3.AKKAMAJ + f * (kmajkmk4.AKKAMAJ - kmajkmk3.AKKAMAJ));
        mmajmmk.Jakkama[mmajmmk.jAKkama++] = mmjakka5;
        mmajmmk.Jakkama[mmajmmk.jAKkama++] = mmjakka6;
        kmaamma kmaamma3 = new kmaamma(kmaamma2, mmjakka5, mmjakka2, mmjakka3, kmajkmk5, kmajkmk2, kmajkmk3);
        kmaamma3.majAkKa = kmaamma2.majAkKa;
        kmjamma.majaKKa[kmjamma.MAjaKKa++] = kmaamma3;
        kmaamma3 = new kmaamma(kmaamma2, mmjakka5, mmjakka6, mmjakka3, kmajkmk5, kmajkmk6, kmajkmk3);
        kmaamma3.majAkKa = kmaamma2.majAkKa;
        kmjamma.majaKKa[kmjamma.MAjaKKa++] = kmaamma3;
    }

    public void kKaMAJa(mmjakka[] mmjakkaArray, int n) {
        float f = this.JAkkAma;
        float f2 = this.jAkkAma;
        float f3 = this.jakkAma;
        int n2 = 0;
        while (n2 < n) {
            mmjakka mmjakka2 = mmjakkaArray[n2];
            float f4 = f3 / mmjakka2.AmAjAkk;
            mmjakka2.amAjAkk = mmjakka2.AMaJaKK * f4 + f;
            mmjakka2.AMAjAkk = -(mmjakka2.aMaJaKK * f4) + f2;
            ++n2;
        }
    }

    static {
        Jakkama = new mmjakka[1000];
        JAkkama = new kmaamma[1000];
    }
}
