/*
 * Decompiled with CFR 0.152.
 */
import java.util.Random;

public class kmjjkmk
extends mmjjmma {
    int KKamaJa = 8;
    int kKamaJa = 8;
    int KkAMaJa;
    int kkAMaJa;
    byte[] KKAMaJa = new byte[256];
    byte[] kKAMaJa = new byte[256];
    byte[] KkaMaJa = new byte[256];
    int kkaMaJa = 256;
    int KKaMaJa = 50;
    int kKaMaJa;
    int AkKaMaJ = 40;
    int akKaMaJ;
    int AKKaMaJ = 40;
    mmaamma aKKaMaJ;
    mmaamma AkkaMaJ;
    mmaamma akkaMaJ;
    mmaamma AKkaMaJ;
    mmaamma aKkaMaJ;
    mmaamma AkKAMaJ;
    mmaamma akKAMaJ;
    mmaamma AKKAMaJ;
    mmaamma aKKAMaJ;
    mmaamma AkkAMaJ;
    mmaamma akkAMaJ;
    mmaamma AKkAMaJ;
    Random aKkAMaJ = new Random(999L);
    int AkKamaJ;
    float akKamaJ = -1.0f;
    int AKKamaJ;
    int aKKamaJ;
    float[][] AkkamaJ;
    float[][] akkamaJ;

    public kmjjkmk() {
        this.KkAMaJa = forward.KAmajAk;
        this.kkAMaJa = forward.kAmajAk;
        this.AKKamaJ = this.KkAMaJa / this.KKamaJa;
        this.aKKamaJ = this.kkAMaJa / this.kKamaJa;
        this.AkkamaJ = new float[this.aKKamaJ][this.AKKamaJ];
        this.akkamaJ = new float[this.aKKamaJ][this.AKKamaJ];
        this.akKaMaJ = (this.KkAMaJa - this.kkaMaJa) / 2;
        this.AKKaMaJ = (this.kkAMaJa - this.KKaMaJa) / 2;
    }

    public String majakkA() {
        return "mute95";
    }

    public void MajakkA(mmjamma mmjamma2) {
        forward.KkaMAjA.amAjakk(0);
        this.akkaMaJ = (mmaamma)mmaakma.majaKkA(mmjamma2.amAjAkK("images/kosmos/sav1.jpg"));
        this.AKkaMaJ = (mmaamma)mmaakma.majaKkA(mmjamma2.amAjAkK("images/kosmos/sav2.jpg"));
        forward.kkamAJA.kAMajak();
        this.aKkaMaJ = (mmaamma)mmaakma.majaKkA(mmjamma2.amAjAkK("images/kosmos/jmag1.jpg"));
        this.AkKAMaJ = (mmaamma)mmaakma.majaKkA(mmjamma2.amAjAkK("images/kosmos/jmag2.jpg"));
        forward.kkamAJA.kAMajak();
        this.akKAMaJ = (mmaamma)mmaakma.majaKkA(mmjamma2.amAjAkK("images/kosmos/jugi1.jpg"));
        this.AKKAMaJ = (mmaamma)mmaakma.majaKkA(mmjamma2.amAjAkK("images/kosmos/jugi2.jpg"));
        forward.kkamAJA.kAMajak();
        this.aKKAMaJ = (mmaamma)mmaakma.majaKkA(mmjamma2.amAjAkK("images/kosmos/car1.jpg"));
        this.AkkAMaJ = (mmaamma)mmaakma.majaKkA(mmjamma2.amAjAkK("images/kosmos/car2.jpg"));
        forward.kkamAJA.kAMajak();
        this.akkAMaJ = (mmaamma)mmaakma.majaKkA(mmjamma2.amAjAkK("images/kosmos/anis1.jpg"));
        this.AKkAMaJ = (mmaamma)mmaakma.majaKkA(mmjamma2.amAjAkK("images/kosmos/anis2.jpg"));
        kmajkka kmajkka2 = (kmajkka)mmaakma.majaKkA(mmjamma2.amAjAkK("images/kosmos/krad3.gif"));
        System.arraycopy(kmajkka2.aMajakK, 0, this.KKAMaJa, 0, 256);
        System.arraycopy(kmajkka2.AmAJakK, 0, this.kKAMaJa, 0, 256);
        System.arraycopy(kmajkka2.amAJakK, 0, this.KkaMaJa, 0, 256);
        forward.kkamAJA.kAMajak();
    }

    public void MAjakkA() {
        kmajkka kmajkka2 = forward.KkaMAjA;
        System.arraycopy(this.KKAMaJa, 0, kmajkka2.aMajakK, 0, 256);
        System.arraycopy(this.kKAMaJa, 0, kmajkka2.AmAJakK, 0, 256);
        System.arraycopy(this.KkaMaJa, 0, kmajkka2.amAJakK, 0, 256);
        kmajkka2.aMaJAkk();
    }

    public void mAJakkA() {
        this.AkkaMaJ = null;
        this.aKKaMaJ = null;
        this.AKkaMaJ = null;
        this.akkaMaJ = null;
        this.AkKAMaJ = null;
        this.aKkaMaJ = null;
        this.AKKAMaJ = null;
        this.akKAMaJ = null;
        this.AkkAMaJ = null;
        this.aKKAMaJ = null;
        this.AKkAMaJ = null;
        this.akkAMaJ = null;
    }

    public void maJakkA(mmaamma mmaamma2, float f, float f2) {
        kmajkka kmajkka2 = forward.KkaMAjA;
        float f3 = f2 * 10.0f;
        if ((double)f3 < 0.05) {
            f3 = 0.05f;
        }
        this.kKAmAjA(kmajkka2, f3);
        int n = (int)Math.min(f * 1.8f + 22.0f, 255.0f);
        int n2 = kmajkka2.amAjakK * kmajkka2.AMAjakK;
        int n3 = 0;
        while (n3 < 220) {
            int n4 = (int)(this.aKkAMaJ.nextFloat() * (float)(n2 - 1));
            int n5 = Math.min(n, (kmajkka2.aMAjakK[n4] & 0xFF) + 45);
            kmajkka2.aMAjakK[n4] = (byte)n5;
            ++n3;
        }
        kmajkka2.amAJAkk();
        kmajkka2.Amajakk();
        mmaamma2.AMajakK(kmajkka2);
        if (this.akKamaJ != -1.0f && f - this.akKamaJ >= 0.0f) {
            float f4 = f - this.akKamaJ;
            float f5 = 0.0f;
            float f6 = 0.0f;
            float f7 = 1.5f;
            float f8 = 4.0f;
            float f9 = 6.0f;
            float f10 = 9.0f;
            if (f4 < f7) {
                f5 = f4 / f7;
                f6 = 0.0f;
            } else if (f4 < f8) {
                f5 = 1.0f;
                f6 = (f4 - f7) / (f8 - f7);
            } else if (f4 < f9) {
                f5 = 1.0f - (f4 - f8) / (f9 - f8);
                f6 = 1.0f;
            } else if (f4 < f10) {
                f5 = 0.0f;
                f6 = 1.0f - (f4 - f9) / (f10 - f9);
            }
            if (f5 != 0.0f || f6 != 0.0f) {
                mmaamma2.aMaJakK(this.aKKaMaJ, this.kKaMaJa, this.AkKaMaJ, this.AkkaMaJ, this.kKaMaJa, this.AkKaMaJ, this.kkaMaJa, this.KKaMaJa, this.akKaMaJ, this.AKKaMaJ, f5, f6);
            }
        }
        ++this.AkKamaJ;
    }

    public void MAJakkA(String string, float f) {
        if (string.equals("saviour")) {
            this.akKamaJ = f;
            this.aKKaMaJ = this.akkaMaJ;
            this.AkkaMaJ = this.AKkaMaJ;
        }
        if (string.equals("jmagic")) {
            this.akKamaJ = f;
            this.aKKaMaJ = this.aKkaMaJ;
            this.AkkaMaJ = this.AkKAMaJ;
        }
        if (string.equals("jugi")) {
            this.akKamaJ = f;
            this.aKKaMaJ = this.akKAMaJ;
            this.AkkaMaJ = this.AKKAMaJ;
        }
        if (string.equals("anis")) {
            this.akKamaJ = f;
            this.aKKaMaJ = this.akkAMaJ;
            this.AkkaMaJ = this.AKkAMaJ;
        }
        if (string.equals("carebear")) {
            this.akKamaJ = f;
            this.aKKaMaJ = this.aKKAMaJ;
            this.AkkaMaJ = this.AkkAMaJ;
        }
    }

    void kKAmAjA(kmajkka kmajkka2, float f) {
        int n = this.AKKamaJ / 2;
        int n2 = this.aKKamaJ / 2;
        float f2 = (float)(this.AkKamaJ % 4) * 0.2f;
        float f3 = (float)(this.AkKamaJ % 5) * 0.2f;
        int n3 = 0;
        while (n3 < this.aKKamaJ) {
            int n4 = 0;
            while (n4 < this.AKKamaJ) {
                float f4;
                float f5;
                float f6 = (float)(n4 - n) * f + f2;
                float f7 = (float)(n3 - n2) * f + f3;
                float f8 = f5 = this.AkkamaJ[n3][n4];
                float f9 = f4 = this.akkamaJ[n3][n4];
                this.AkkamaJ[n3][n4] = f5 += f6;
                this.akkamaJ[n3][n4] = f4 += f7;
                int n5 = (int)f5 - (int)f8;
                int n6 = (int)f4 - (int)f9;
                kmajkka2.AMAJakk(n4 * this.KKamaJa - n5, n3 * this.kKamaJa - n6, n4 * this.KKamaJa, n3 * this.kKamaJa, this.KKamaJa, this.kKamaJa);
                ++n4;
            }
            ++n3;
        }
    }
}
