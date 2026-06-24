/*
 * Decompiled with CFR 0.152.
 */
public class mmaamka
extends mmajmmk {
    float kKaMAJA;
    mmaakka[] KkAmaJA;
    public final static int kkAmaJA = 0;
    public final static int KKAmaJA = 1;
    int kKAmaJA;
    public float KkamaJA = 10.0f;
    float kkamaJA;
    int KKamaJA;
    float kKamaJA;

    public mmaamka(int n, float f) {
        this.kKaMAJA = f;
        this.kaMAJAk(n);
        this.kkAMAJa();
        this.jakKaMA = Float.POSITIVE_INFINITY;
        this.KamaJAk(0);
    }

    public void kaMAJAk(int n) {
        this.JAkkaMA = new kmaamma[n];
        this.KkAmaJA = new mmaakka[n];
        this.jAkkaMA = this.KkAmaJA;
        this.JaKKaMA = new kmajkmk[3];
        this.JaKKaMA[0] = new kmajkmk(0.0f, 0.0f);
        this.JaKKaMA[1] = new kmajkmk(1.0f, 0.0f);
        this.JaKKaMA[2] = new kmajkmk(0.0f, 1.0f);
        kmajkmk kmajkmk2 = this.JaKKaMA[0];
        kmajkmk kmajkmk3 = this.JaKKaMA[0];
        kmajkmk kmajkmk4 = this.JaKKaMA[0];
        float f = 20.0f;
        int n2 = 0;
        while (n2 < n) {
            mmaakka mmaakka2;
            float f2 = 0.0f;
            float f3 = 0.0f;
            float f4 = 0.0f;
            f2 = (float)((Math.random() - 0.5) * (double)f);
            f3 = (float)((Math.random() - 0.5) * (double)f);
            f4 = (float)((Math.random() - 0.5) * (double)f);
            this.KkAmaJA[n2] = mmaakka2 = new mmaakka(f2, f3, f4);
            this.JAkkaMA[n2] = new kmaamma(mmaakka2, mmaakka2, mmaakka2, kmajkmk2, kmajkmk3, kmajkmk4);
            this.JAkkaMA[n2].mAjAkka = 1024;
            ++n2;
        }
    }

    public int KaMAJAk() {
        return this.kKAmaJA;
    }

    public void KamaJAk(int n) {
        this.kKAmaJA = n;
        int n2 = this.KkAmaJA.length;
        int n3 = 0;
        while (n3 < n2) {
            switch (this.kKAmaJA) {
                case 1: {
                    this.KkAmaJA[n3].MaJaKka(0.0f, 0.0f, 0.0f);
                    break;
                }
                case 0: {
                    float f = this.KkamaJA;
                    float f2 = 0.0f;
                    float f3 = 0.0f;
                    float f4 = 0.0f;
                    f2 = (float)((Math.random() - 0.5) * (double)f);
                    f3 = (float)((Math.random() - 0.5) * (double)f);
                    f4 = (float)((Math.random() - 0.5) * (double)f);
                    this.KkAmaJA[n3].MaJaKka(f2, f3, f4);
                    break;
                }
            }
            ++n3;
        }
    }

    public void kkaMAJa(kaajmmk kaajmmk2, int n) {
        this.jaKkAma = kaajmmk2;
        switch (this.kKAmaJA) {
            case 1: {
                this.kAmaJAk(new mmajmma(0.0f, 0.0f, 1.0f), new mmajmma(0.0f, 0.0f, 1.0f), 0.1f, 0.5f);
                break;
            }
        }
        this.kKAmAJa();
        this.KkAmaja();
        this.kkamaJA = this.jaKkAma.JAkkaMa;
        this.jAKKAma = this.jaKkAma.jAkkaMa;
        this.kamaJAk();
    }

    void kamaJAk() {
        int n = 0;
        int n2 = this.KkAmaJA.length;
        int n3 = 0;
        while (n3 < n2) {
            mmaakka mmaakka2 = this.KkAmaJA[n3];
            if (mmaakka2.AmAjAkk > this.kkamaJA && mmaakka2.AmAjAkk < this.jAKKAma) {
                float f = mmaakka2.amAjAkk;
                float f2 = mmaakka2.AMAjAkk;
                float f3 = this.kKaMAJA / mmaakka2.AmAjAkk;
                this.KkAmaJA[n++].AmajAkk = f3;
                kmaamma kmaamma2 = this.JAkkaMA[n3];
                kmaamma2.majAkKa = -mmaakka2.AmAjAkk * 3.0f;
                kmjamma.majaKKa[kmjamma.MAjaKKa++] = kmaamma2;
            } else {
                ++n;
            }
            ++n3;
        }
    }

    public void kAmaJAk(mmajmma mmajmma2, mmajmma mmajmma3, float f, float f2) {
        float f3;
        mmajmma mmajmma4 = new mmajmma(0.0f, 0.0f, -0.05f);
        int n = this.KkAmaJA.length;
        mmaakka[] mmaakkaArray = this.KkAmaJA;
        int n2 = 0;
        while (n2 < n) {
            mmaakka mmaakka2 = mmaakkaArray[n2];
            mmaakka2.AmAJAKk.majAkKA(mmajmma4);
            mmaakka2.majAkKA(mmaakka2.AmAJAKk);
            f3 = 0.7f;
            if (mmaakka2.mAJakKA < f3 && mmaakka2.AmAJAKk.mAJakKA < 0.0f) {
                mmaakka2.mAJakKA = f3 - mmaakka2.mAJakKA;
                mmaakka2.AmAJAKk.mAJakKA = -mmaakka2.AmAJAKk.mAJakKA * 0.7f;
                mmaakka2.mAJakKA += mmaakka2.AmAJAKk.mAJakKA;
            }
            ++n2;
        }
        f2 = 0.0f;
        f = 0.5f;
        int n3 = 0;
        while (n3 < 7) {
            f3 = (float)((double)((float)n3 / 7.0f) * Math.PI * 2.0);
            float f4 = 0.2f;
            this.KAmaJAk(mmajmma2, f3 += this.kKamaJA, f4, 1.0f);
            ++n3;
        }
        this.kKamaJA += 0.05f;
    }

    void KAmaJAk(mmajmma mmajmma2, float f, float f2, float f3) {
        mmajmma mmajmma3 = new mmajmma();
        mmaakka mmaakka2 = this.KkAmaJA[this.KKamaJA++];
        mmaakka2.mAjakKA(mmajmma2);
        mmajmma3.MaJaKka(0.0f, 0.0f, f3);
        mmajmma3.maJaKka(f2);
        mmajmma3.MAJakKA(f);
        mmaakka2.AmAJAKk.mAjakKA(mmajmma3);
        if (this.KKamaJA == this.KkAmaJA.length) {
            this.KKamaJA = 0;
        }
    }
}
