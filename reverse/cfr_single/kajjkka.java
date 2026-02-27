/*
 * Decompiled with CFR 0.152.
 */
public class kajjkka
extends mmjjmma {
    kaajmmk AMajAkK;
    kaajkka aMajAkK;
    mmaamma AmAJAkK;
    mmaamma amAJAkK;
    kmjjmmk AMAJAkK;
    float aMAJAkK;
    float AmaJAkK;
    mmjamma amaJAkK;

    public String majakkA() {
        return "kukot";
    }

    public void mAJakkA() {
        this.aMajAkK = null;
        this.AmAJAkK.MAJakKa = null;
        this.AmAJAkK = null;
        this.amAJAkK = null;
    }

    public void MajakkA(mmjamma mmjamma2) {
        this.amaJAkK = mmjamma2;
        this.AMajAkK = new kaajmmk();
        this.AMajAkK.jaKkaMa = 1.4f;
        this.AMajAkK.JAkKAMa = 512;
        this.AMajAkK.jAkKAMa = 256;
        this.AMajAkK.jAkkaMa = 150.0f;
        this.aMajAkK = new kaajkka();
        this.aMajAkK.KamAJaK(this.amaJAkK.amAjAkK("asses/under1.ase"));
        forward.kkamAJA.kAMajak();
        kmajkka kmajkka2 = (kmajkka)mmaakma.majaKkA(this.amaJAkK.amAjAkK("images/envplane.gif"));
        forward.kkamAJA.kAMajak();
        this.AmAJAkK = kajjkka.aMAjAkk(kmajkka2, 48.0f, 192.0f, 80.0f);
        forward.kkamAJA.kAMajak();
        int n = 0;
        while (n < this.aMajAkK.MaJAKkA.size()) {
            mmajmmk mmajmmk2 = (mmajmmk)this.aMajAkK.MaJAKkA.elementAt(n);
            mmajmmk2.JAkKaMA = true;
            mmajmmk2.jAkKaMA = true;
            mmajmmk2.kkAMAJa();
            mmajmmk2.KkAMAJa(this.AmAJAkK, kmajkka2);
            mmajmmk2.KkAmAJa(3);
            mmajmmk2.jAkKAma = 2;
            ++n;
        }
        mmaamma mmaamma2 = (mmaamma)mmaakma.majaKkA(this.amaJAkK.amAjAkK("images/flare1.jpg"));
        forward.kkamAJA.kAMajak();
        mmaamka mmaamka2 = new mmaamka(180, 2 * forward.kAmajAk);
        mmaamka2.KkamaJA = 110.0f;
        mmaamka2.KamaJAk(0);
        mmaamka2.jaKKaMA.mAjakKA(new mmajmma(-5.0f, 35.0f, 5.501f));
        mmaamka2.KkaMaja(mmaamma2);
        this.aMajAkK.maJAKkA(mmaamka2);
        this.aMajAkK.mAJAKkA();
        this.amAJAkK = new mmaamma(256, 256, 1, false);
        int n2 = 0;
        while (n2 < 256) {
            int n3 = 0;
            while (n3 < 256) {
                int n4 = (int)(20.0 + Math.random() * Math.random() * Math.random() * Math.random() * 200.0);
                int n5 = (int)(26.0 + Math.random() * 50.0);
                int n6 = (int)(22.0 + Math.random() * 26.0);
                this.amAJAkK.MAJakKa[n2 * 256 + n3] = n4 << 20 | n5 << 10 | n6;
                ++n3;
            }
            ++n2;
        }
        forward.kkamAJA.kAMajak();
        this.AMAJAkK = new kmjjmmk(38, 16, 87);
    }

    public void maJakkA(mmaamma mmaamma2, float f, float f2) {
        mmaamma2.amaJakK();
        forward.kkAMAjA.kAMaJAK(0xFFFFFF);
        forward.kkAMAjA.KkAMAjA();
        int n = (int)(Math.random() * 256.0);
        int n2 = (int)(Math.random() * 128.0);
        mmaamma2.aMAJakK(this.amAJAkK, -n, -n2);
        mmaamma2.aMAJakK(this.amAJAkK, -n + 256, -n2);
        mmaamma2.aMAJakK(this.amAJAkK, -n + 384, -n2);
        mmaamma2.aMAJakK(this.amAJAkK, -n + 512, -n2);
        mmaamma2.aMAJakK(this.amAJAkK, -n, -n2 + 128);
        mmaamma2.aMAJakK(this.amAJAkK, -n + 256, -n2 + 128);
        mmaamma2.aMAJakK(this.amAJAkK, -n + 384, -n2 + 128);
        mmaamma2.aMAJakK(this.amAJAkK, -n + 512, -n2 + 128);
        this.aMajAkK.kAMAJaK(f * 1.9f, this.AMajAkK);
        this.aMajAkK.MajAKkA(this.AMajAkK, forward.kkAMAjA);
        this.aMajAkK.MAJAKkA(forward.kkAMAjA);
        mmaamma2.aMajAKK(0.875f);
        if (this.aMAJAkK > 0.0f) {
            this.aMAJAkK -= this.AmaJAkK * f2;
            this.AMAJAkK.KkaMAja(mmaamma2, (int)this.aMAJAkK);
        }
        mmaamma2.AmajakK();
    }

    public void MAJakkA(String string, float f) {
        if (string.equals("suh")) {
            this.aMAJAkK = 50.0f;
            this.AmaJAkK = 200.0f;
        }
        if (string.equals("suh0")) {
            this.aMAJAkK = 100.0f;
            this.AmaJAkK = 150.0f;
        }
        if (string.equals("suh1")) {
            this.aMAJAkK = 128.0f;
            this.AmaJAkK = 50.0f;
        }
        if (string.equals("suh2")) {
            this.aMAJAkK = 256.0f;
            this.AmaJAkK = 70.0f;
        }
    }

    public static mmaamma aMAjAkk(kmajkka kmajkka2, float f, float f2, float f3) {
        mmaamma mmaamma2 = new mmaamma(256, 256, 1, false);
        int n = 0;
        while (n < 256) {
            int n2 = 0;
            while (n2 < 256) {
                double d = 1.0 - (double)n / 255.0;
                int n3 = (int)Math.min(255.0, (double)(kmajkka2.aMajakK[n2] & 0xFF) * d + (1.0 - d) * (double)f);
                int n4 = (int)Math.min(255.0, (double)(kmajkka2.AmAJakK[n2] & 0xFF) * d + (1.0 - d) * (double)f2);
                int n5 = (int)Math.min(255.0, (double)(kmajkka2.amAJakK[n2] & 0xFF) * d + (1.0 - d) * (double)f3);
                mmaamma2.MAJakKa[n * 256 + n2] = n3 << 20 | n4 << 10 | n5;
                ++n2;
            }
            ++n;
        }
        return mmaamma2;
    }
}
