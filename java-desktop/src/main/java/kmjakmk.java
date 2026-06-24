/*
 * Decompiled with CFR 0.152.
 */
public class kmjakmk
extends mmajmmk {
    int aKKAmAJ;
    int AkkAmAJ;
    short[][] akkAmAJ;
    public int AKkAmAJ;
    public int aKkAmAJ;
    int AkKaMaj;
    int akKaMaj;
    int AKKaMaj;
    int aKKaMaj;
    float AkkaMaj;
    float akkaMaj;
    public mmaamma AKkaMaj;
    public kmajkka aKkaMaj;
    public kmajkka AkKAMaj;
    public mmaamma akKAMaj;
    public boolean AKKAMaj = true;
    mmjakka[] aKKAMaj;
    kmaamma[] AkkAMaj;
    int akkAMaj;
    int AKkAMaj;
    int aKkAMaj;
    int AkKamaj;
    public float akKamaj = 0.028571429f;
    public float AKKamaj = 0.028571429f;
    public final static int aKKamaj = 3;
    public final static int Akkamaj = 259;
    public boolean akkamaj = false;
    boolean AKkamaj = false;
    final static float aKkamaj = 0.0f;

    public kmjakmk(short[][] sArray, float f, float f2, kaajmmk kaajmmk2, boolean bl) {
        this.AKKAMaj = bl;
        this.akkAmAJ = sArray;
        this.AkkAmAJ = this.akkAmAJ.length;
        this.aKKAmAJ = this.akkAmAJ[0].length;
        this.AkkaMaj = f;
        this.akkaMaj = f2;
        kaajmka kaajmka2 = new kaajmka();
        kaajmka2.kamaJak(kaajmmk2, null);
        float f3 = kaajmka2.Kamajak.MAjaKka();
        float f4 = kaajmka2.kAmajak.mAjAKKA(kaajmka2.Kamajak).MAjaKka();
        float f5 = Math.max(f3, f4);
        this.AkKaMaj = this.akKaMaj = (int)(f5 /= this.AkkaMaj) + 4;
        this.JaKKaMA = new kmajkmk[this.AkKaMaj * this.akKaMaj];
        int n = 0;
        while (n < this.AkKaMaj * this.akKaMaj) {
            this.JaKKaMA[n] = new kmajkmk((float)n / 10.0f, (float)n / 20.0f);
            ++n;
        }
        if (this.AKKAMaj) {
            this.kkamajA(this.AkKaMaj, this.akKaMaj, this.AkkaMaj, 259);
            this.aKKAMaj = this.jAkkaMA;
            this.AkkAMaj = this.JAkkaMA;
            this.kkamajA(this.AkKaMaj, this.akKaMaj, this.AkkaMaj, 3);
        } else {
            this.kkamajA(this.AkKaMaj, this.akKaMaj, this.AkkaMaj, 3);
        }
        this.jakKaMA = Float.POSITIVE_INFINITY;
    }

    public void kkamajA(int n, int n2, float f, int n3) {
        int n4;
        this.jAkkaMA = new mmjakka[n * n2];
        int n5 = 0;
        int n6 = 0;
        while (n6 < n2) {
            n4 = 0;
            while (n4 < n) {
                float f2 = (float)(-(n4 - n / 2)) * f;
                float f3 = (float)(n6 - n2 / 2) * f;
                float f4 = 0.0f;
                this.jAkkaMA[n5] = new mmjakka(f2, f3, f4);
                ++n5;
                ++n4;
            }
            ++n6;
        }
        this.JAkkaMA = new kmaamma[(n - 1) * (n2 - 1) * 2];
        n4 = 0;
        int n7 = 0;
        while (n7 < n2 - 1) {
            int n8 = 0;
            while (n8 < n - 1) {
                int n9 = n7 * n2 + n8;
                int n10 = n7 * n2 + n8 + 1;
                int n11 = (n7 + 1) * n2 + n8;
                int n12 = (n7 + 1) * n2 + n8 + 1;
                this.JAkkaMA[n4++] = new kmaamma(this, n9, n12, n10, n9, n12, n10);
                this.JAkkaMA[n4++] = new kmaamma(this, n12, n9, n11, n12, n9, n11);
                this.JAkkaMA[n4 - 2].mAjAkka = n3;
                this.JAkkaMA[n4 - 1].mAjAkka = n3;
                ++n8;
            }
            ++n7;
        }
        this.kkAMAJa();
    }

    public void kKamajA() {
    }

    public float KkAMajA(float f, float f2, float f3, float f4, float f5) {
        return Math.min(f, Math.min(f2, Math.min(f3, Math.min(f4, f5))));
    }

    public float kkAMajA(float f, float f2, float f3, float f4, float f5) {
        return Math.max(f, Math.max(f2, Math.max(f3, Math.max(f4, f5))));
    }

    public void KKamajA() {
        kaajmka kaajmka2 = new kaajmka();
        kaajmka2.kamaJak(this.jaKkAma, null);
        float f = this.KkAMajA(0.0f, kaajmka2.Kamajak.maJakKA, kaajmka2.kamajak.maJakKA, kaajmka2.KAmajak.maJakKA, kaajmka2.kAmajak.maJakKA);
        float f2 = this.kkAMajA(0.0f, kaajmka2.Kamajak.maJakKA, kaajmka2.kamajak.maJakKA, kaajmka2.KAmajak.maJakKA, kaajmka2.kAmajak.maJakKA);
        float f3 = this.KkAMajA(0.0f, kaajmka2.Kamajak.MAJakKA, kaajmka2.kamajak.MAJakKA, kaajmka2.KAmajak.MAJakKA, kaajmka2.kAmajak.MAJakKA);
        float f4 = this.kkAMajA(0.0f, kaajmka2.Kamajak.MAJakKA, kaajmka2.kamajak.MAJakKA, kaajmka2.KAmajak.MAJakKA, kaajmka2.kAmajak.MAJakKA);
        f += this.jaKkAma.JAKkaMa.maJakKA;
        f2 += this.jaKkAma.JAKkaMa.maJakKA;
        f3 += this.jaKkAma.JAKkaMa.MAJakKA;
        f4 += this.jaKkAma.JAKkaMa.MAJakKA;
        this.akkAMaj = (int)(f /= this.AkkaMaj);
        this.AKkAMaj = (int)(f2 /= this.AkkaMaj);
        this.aKkAMaj = (int)(f3 /= this.AkkaMaj);
        this.AkKamaj = (int)(f4 /= this.AkkaMaj);
        this.AKkAMaj += 2;
        this.AkKamaj += 2;
        --this.akkAMaj;
        --this.aKkAMaj;
        if (this.AKkAMaj - this.akkAMaj > this.AkKaMaj) {
            System.err.println("dem cull X over" + (this.AKkAMaj - this.akkAMaj - this.aKKAmAJ));
            this.AKkAMaj = this.akkAMaj + this.AkKaMaj;
        }
        if (this.AkKamaj - this.aKkAMaj > this.akKaMaj) {
            System.err.println("dem cull Y over " + (this.AkKamaj - this.aKkAMaj - this.AkkAmAJ));
            this.AkKamaj = this.aKkAMaj + this.akKaMaj;
        }
    }

    public void KkAmaja() {
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
        float f20 = this.jaKkAma.JaKKaMa;
        float f21 = this.AkkaMaj;
        float f22 = (float)this.aKkAMaj * f21;
        int n = this.aKkAMaj;
        while (n < this.AkKamaj) {
            float f23 = (float)this.akkAMaj * f21;
            int n2 = (n - this.aKkAMaj) * this.AkKaMaj;
            int n3 = this.akkAMaj;
            while (n3 < this.AKkAMaj) {
                float f24;
                int n4;
                int n5;
                mmjakka mmjakka2 = this.jAkkaMA[n2];
                kmajkmk kmajkmk2 = this.JaKKaMA[n2];
                n3 -= this.AKkAmAJ;
                n -= this.aKkAmAJ;
                if (this.akkamaj) {
                    n5 = n3 % this.aKKAmAJ;
                    if (n5 < 0) {
                        n5 += this.aKKAmAJ;
                    }
                    if ((n4 = n % this.AkkAmAJ) < 0) {
                        n4 += this.AkkAmAJ;
                    }
                    n4 = this.aKKAmAJ - 1 - n4;
                    f24 = (float)this.akkAmAJ[n4][n5] * this.akkaMaj;
                    kmajkmk2.akKAMAJ = (float)n3 * this.akKamaj;
                    kmajkmk2.AKKAMAJ = (float)n * this.AKKamaj;
                } else if (n >= 0 && n < this.AkkAmAJ && n3 >= 0 && n3 < this.aKKAmAJ) {
                    n5 = n3;
                    n4 = n;
                    n4 = this.aKKAmAJ - 1 - n4;
                    kmajkmk2.akKAMAJ = (float)n3 * this.akKamaj;
                    kmajkmk2.AKKAMAJ = (float)(-n) * this.akKamaj;
                    f24 = (float)this.akkAmAJ[n4][n5] * this.akkaMaj;
                } else {
                    kmajkmk2.akKAMAJ = (float)n3 * this.akKamaj;
                    kmajkmk2.AKKAMAJ = (float)(-n) * this.akKamaj;
                    f24 = -0.001f;
                }
                n3 += this.AKkAmAJ;
                n += this.aKkAmAJ;
                mmjakka2.maJakKA = f23;
                mmjakka2.MAJakKA = f22;
                mmjakka2.mAJakKA = f24;
                float f25 = f * f23 + f4 * f22 + f7 * f24 + f10;
                float f26 = f2 * f23 + f5 * f22 + f8 * f24 + f11;
                float f27 = f3 * f23 + f6 * f22 + f9 * f24 + f12;
                if (f27 < f16) {
                    mmjakka2.aMAjAkk = 32768;
                    f27 = this.JAKKAma;
                } else {
                    mmjakka2.aMAjAkk = f27 > f17 ? 4096 : 0;
                }
                float f28 = f13 / f27;
                mmjakka2.amAjAkk = f25 * f28 + f14;
                mmjakka2.AMAjAkk = -(f26 * f28) + f15;
                if (mmjakka2.amAjAkk < 0.0f) {
                    mmjakka2.aMAjAkk |= 1;
                } else if (mmjakka2.amAjAkk >= f18) {
                    mmjakka2.aMAjAkk |= 8;
                }
                if (mmjakka2.AMAjAkk < 0.0f) {
                    mmjakka2.aMAjAkk |= 0x40;
                } else if (mmjakka2.AMAjAkk >= f19) {
                    mmjakka2.aMAjAkk |= 0x200;
                }
                mmjakka2.AMaJaKK = f25;
                mmjakka2.aMaJaKK = f26;
                mmjakka2.AmAjAkk = f27;
                float f29 = (f27 - f20) / (this.jAKKAma - f20);
                if (f29 < 0.0f) {
                    f29 = 0.0f;
                }
                if (f29 > 0.99609375f) {
                    f29 = 0.99609375f;
                }
                mmjakka2.AmajAkk = f29;
                if (this.AKKAMaj) {
                    mmjakka mmjakka3 = this.aKKAMaj[n2];
                    mmjakka3.AmajAkk = mmjakka2.AmajAkk;
                    f25 -= 2.0f * f7 * f24;
                    f26 -= 2.0f * f8 * f24;
                    if ((f27 -= 2.0f * f9 * f24) < f16) {
                        mmjakka3.aMAjAkk = 32768;
                        f27 = this.JAKKAma;
                    } else {
                        mmjakka3.aMAjAkk = f27 > f17 ? 4096 : 0;
                    }
                    f28 = f13 / f27;
                    mmjakka3.amAjAkk = f25 * f28 + f14;
                    mmjakka3.AMAjAkk = -(f26 * f28) + f15;
                    mmjakka3.AmAjAkk = f27;
                    if (mmjakka3.amAjAkk < 0.0f) {
                        mmjakka3.aMAjAkk |= 1;
                    } else if (mmjakka3.amAjAkk >= f18) {
                        mmjakka3.aMAjAkk |= 8;
                    }
                    if (mmjakka3.AMAjAkk < 0.0f) {
                        mmjakka3.aMAjAkk |= 0x40;
                    } else if (mmjakka3.AMAjAkk >= f19) {
                        mmjakka3.aMAjAkk |= 0x200;
                    }
                }
                f23 += f21;
                ++n2;
                ++n3;
            }
            f22 += f21;
            ++n;
        }
    }

    public void KKAMajA() {
        kmaamma[] kmaammaArray = kmjamma.majaKKa;
        int n = kmjamma.MAjaKKa;
        mmajmma mmajmma2 = this.kKAMaja();
        float f = mmajmma2.maJakKA;
        float f2 = mmajmma2.MAJakKA;
        float f3 = mmajmma2.mAJakKA;
        int n2 = this.akkAMaj;
        int n3 = (this.AKkAMaj - n2) * 2 + n2;
        int n4 = this.aKkAMaj + 1;
        while (n4 < this.AkKamaj) {
            int n5 = (n4 - this.aKkAMaj - 1) * (this.AkKaMaj - 1) * 2;
            int n6 = n5 + (n3 - n2 - 2);
            while (n5 < n6) {
                float f4;
                float f5;
                kmaamma kmaamma2 = this.JAkkaMA[n5++];
                if ((n5 & 1) == 0) {
                    f5 = kmaamma2.mAjakKa.mAJakKA - kmaamma2.maJAkKa.mAJakKA;
                    f4 = -(kmaamma2.MaJAkKa.mAJakKA - kmaamma2.maJAkKa.mAJakKA);
                } else {
                    f5 = -(kmaamma2.mAjakKa.mAJakKA - kmaamma2.maJAkKa.mAJakKA);
                    f4 = kmaamma2.MaJAkKa.mAJakKA - kmaamma2.maJAkKa.mAJakKA;
                }
                int n7 = 37449 + kmaamma2.mAjakKa.aMAjAkk + kmaamma2.MaJAkKa.aMAjAkk + kmaamma2.maJAkKa.aMAjAkk;
                if ((n7 & 0x34924) == 0) {
                    if ((f + kmaamma2.mAjakKa.maJakKA) * f5 + (f2 + kmaamma2.mAjakKa.MAJakKA) * f4 + (f3 + kmaamma2.mAjakKa.mAJakKA) * -this.AkkaMaj > 0.0f) {
                        kmaamma2.MaJaKKa = kmaamma2.mAjakKa.mAJakKA < 0.0f || kmaamma2.MaJAkKa.mAJakKA < 0.0f || kmaamma2.maJAkKa.mAJakKA < 0.0f ? this.AkKAMaj : this.aKkaMaj;
                        kmaamma2.maJaKKa = this.AKkaMaj;
                        kmaamma2.majAkKa = -(kmaamma2.mAjakKa.AmAjAkk + kmaamma2.MaJAkKa.AmAjAkk + kmaamma2.maJAkKa.AmAjAkk);
                        kmaammaArray[n++] = kmaamma2;
                    }
                } else if ((n7 & 0x24924) == 0 && (f + kmaamma2.mAjakKa.maJakKA) * f5 + (f2 + kmaamma2.mAjakKa.MAJakKA) * f4 + (f3 + kmaamma2.mAjakKa.mAJakKA) * -this.AkkaMaj > 0.0f) {
                    kmaamma2.MaJaKKa = kmaamma2.mAjakKa.mAJakKA < 0.0f || kmaamma2.MaJAkKa.mAJakKA < 0.0f || kmaamma2.maJAkKa.mAJakKA < 0.0f ? this.AkKAMaj : this.aKkaMaj;
                    kmaamma2.maJaKKa = this.AKkaMaj;
                    kmaamma2.majAkKa = -(kmaamma2.mAjakKa.AmAjAkk + kmaamma2.MaJAkKa.AmAjAkk + kmaamma2.maJAkKa.AmAjAkk);
                    this.KKamAJa(kmaamma2);
                }
                if (!this.AKKAMaj) continue;
                kmaamma kmaamma3 = this.AkkAMaj[n5 - 1];
                n7 = 37449 + kmaamma3.mAjakKa.aMAjAkk + kmaamma3.MaJAkKa.aMAjAkk + kmaamma3.maJAkKa.aMAjAkk;
                if ((n7 & 0x34924) != 0 || !((f + kmaamma2.mAjakKa.maJakKA) * -f5 + (f2 + kmaamma2.mAjakKa.MAJakKA) * -f4 + (f3 - kmaamma2.mAjakKa.mAJakKA) * -this.AkkaMaj < 0.0f)) continue;
                kmaamma3.mAjAkka = 259;
                kmaamma3.maJaKKa = this.akKAMaj;
                kmaamma3.MaJaKKa = this.aKkaMaj;
                kmaamma3.majAkKa = kmaamma3.mAjakKa.AmAjAkk + kmaamma3.MaJAkKa.AmAjAkk + kmaamma3.maJAkKa.AmAjAkk;
                kmaammaArray[n++] = kmaamma3;
            }
            ++n4;
        }
        kmjamma.MAjaKKa = n;
    }

    public void kkaMAJa(kaajmmk kaajmmk2, int n) {
        this.jaKkAma = kaajmmk2;
        this.jAKkAma = kmjamma.MAjaKKa;
        this.AKkamaj = this.jaKkAma.JAKkaMa.mAJakKA < 0.0f;
        this.kKAmAJa();
        this.KKamajA();
        this.KkAmaja();
        this.KKAMajA();
        this.kkamAJa();
    }
}
