/*
 * Decompiled with CFR 0.152.
 */
public class kmjjmka
extends mmjjmma {
    kaajmmk aMaJaKk;
    kmjakmk KaMaJaK;
    kaajkka kaMaJaK;
    kmajkka KAMaJaK;
    mmaamma kAMaJaK;
    int KamaJaK;
    mmjamma kamaJaK;
    boolean KAmaJaK;
    boolean kAmaJaK;
    boolean KaMAJaK;
    boolean kaMAJaK;
    float KAMAJaK;
    float kAMAJaK;
    float KamAJaK;
    float kamAJaK = 3.0f;
    float KAmAJaK;
    float kAmAJaK;

    public String majakkA() {
        return "maku";
    }

    public void MajakkA(mmjamma mmjamma2) {
        this.kamaJaK = mmjamma2;
        this.aMaJaKk = new kaajmmk();
        this.aMaJaKk.JAkKAMa = 512;
        this.aMaJaKk.jAkKAMa = 256;
        this.aMaJaKk.jaKkaMa = 1.2f;
        this.aMaJaKk.jAkkaMa = 200.0f;
        this.aMaJaKk.JaKKaMa = 81.0f;
        this.KaMaJaK = this.KAmaJAK(this.aMaJaKk, this.kamaJaK);
        this.kaMaJaK = new kaajkka();
        this.kaMaJaK.KamAJaK(this.kamaJaK.amAjAkK("asses/vuori5.ase"));
        kaajkka kaajkka2 = this.kaMaJaK;
        int n = 0;
        while (n < kaajkka2.MaJAKkA.size()) {
            mmajmmk mmajmmk2 = (mmajmmk)kaajkka2.MaJAKkA.elementAt(n);
            mmajmmk2.JAkKaMA = true;
            mmajmmk2.jAkKaMA = true;
            mmajmmk2.kkAMAJa();
            mmajmmk2.JAKKaMA.MajaKka(1.0);
            mmajmmk2.KkAMAJa(this.kAMaJaK, this.KAMaJaK);
            mmajmmk2.KkAmAJa(3);
            ++n;
        }
        this.kaMaJaK.maJAKkA(this.KaMaJaK);
        this.kaMaJaK.mAJAKkA();
        maajmka.kAMaJak(105);
    }

    public void maJakkA(mmaamma mmaamma2, float f, float f2) {
        forward.kkAMAjA.kAMaJAK(0xFFFFFF);
        mmaamma2.amaJakK();
        forward.kkAMAjA.KkAMAjA();
        if (this.kaMAJaK) {
            this.KAMAJaK += f2 * 3.0f;
            this.aMaJaKk.jakkaMa = this.KAMAJaK;
        }
        this.kaMaJaK.kAMAJaK((f -= this.kAmAJaK) * this.kamAJaK + this.KAmAJaK, this.aMaJaKk);
        this.kaMaJaK.MajAKkA(this.aMaJaKk, forward.kkAMAjA);
        this.kaMaJaK.MAJAKkA(forward.kkAMAjA);
        if (this.kAMAJaK > 0.0f) {
            this.kAMAJaK -= this.KamAJaK * f2;
            maajmka.KaMaJak(mmaamma2, (int)this.kAMAJaK);
        }
        if (this.kAmaJaK) {
            mmaamma2.AMaJakK(mmaamma.aMAJAKK(255, 255, 255));
            mmaamma2.aMajAKK(0.625f);
            mmaamma2.AmAJAKK(mmaamma2, 0, 0);
        } else {
            mmaamma2.amajakK();
        }
        switch (this.KamaJaK) {
            case 1: {
                boolean bl = false;
                return;
            }
            case 2: {
                int n = 40;
                return;
            }
        }
    }

    public void MAJakkA(String string, float f) {
        float f2;
        String string2;
        if (string.equals("suh")) {
            this.kAMAJaK = 120.0f;
            this.KamAJaK = 200.0f;
        }
        if (string.equals("suh0")) {
            this.kAMAJaK = 128.0f;
            this.KamAJaK = 50.0f;
        }
        if (string.equals("ksor")) {
            boolean bl = this.kAmaJaK = !this.kAmaJaK;
        }
        if (string.equals("low")) {
            boolean bl = this.KaMAJaK = !this.KaMAJaK;
        }
        if (string.equals("roll")) {
            boolean bl = this.kaMAJaK = !this.kaMAJaK;
        }
        if (string.startsWith("go ")) {
            string2 = string.substring(3, string.length());
            this.KAmAJaK = f2 = Float.valueOf(string2).floatValue();
            this.kAmAJaK = f;
        }
        if (string.startsWith("speed ")) {
            string2 = string.substring(6, string.length());
            this.kamAJaK = f2 = Float.valueOf(string2).floatValue();
        }
    }

    public static short[][] kAmaJAK(kmajkka kmajkka2, int n) {
        short[][] sArray = new short[kmajkka2.AMAjakK][kmajkka2.amAjakK];
        int n2 = 0;
        while (n2 < kmajkka2.AMAjakK * kmajkka2.amAjakK) {
            int n3 = kmajkka2.aMajakK[kmajkka2.aMAjakK[n2] & 0xFF] & 0xFF;
            sArray[n2 / kmajkka2.amAjakK][n2 % kmajkka2.amAjakK] = (short)(n3 += n);
            ++n2;
        }
        return sArray;
    }

    kmjakmk KAmaJAK(kaajmmk kaajmmk2, mmjamma mmjamma2) {
        mmaamma mmaamma2;
        kaajmmk2.JAKkaMa.MaJaKka(0.0f, 0.0f, 500.0f);
        String string = "images/scape/loopk40.gif";
        String string2 = "images/scape/loopa2.gif";
        kmajkka kmajkka2 = (kmajkka)mmaakma.majaKkA(mmjamma2.amAjAkK(string));
        forward.kkamAJA.kAMajak();
        int n = kmajkka2.amAjakK;
        int n2 = kmajkka2.AMAjakK;
        short[][] sArray = kmjjmka.kAmaJAK(kmajkka2, 0);
        kmjakmk kmjakmk2 = new kmjakmk(sArray, 200.0f / (float)n, 1.94f, kaajmmk2, false);
        kmjakmk2.akkamaj = true;
        kmajkka kmajkka3 = (kmajkka)mmaakma.majaKkA(mmjamma2.amAjAkK(string2));
        forward.kkamAJA.kAMajak();
        kmjakmk2.aKkaMaj = kmajkka3;
        kmjakmk2.AkKAMaj = kmajkka3;
        kmjakmk2.akKamaj = 1.0f / (float)n;
        kmjakmk2.AKKamaj = 1.0f / (float)n2;
        kmjakmk2.AKkAmAJ = -n / 2;
        kmjakmk2.aKkAmAJ = -n2 / 2;
        kmjakmk2.AKkaMaj = mmaamma2 = new mmaamma(256, 256, 1, false);
        int[] nArray = new int[256];
        int n3 = 0;
        while (n3 < 128) {
            nArray[n3] = mmaakma.maJaKkA(0, 0, 0, 80, 140, 200, 1.0f - (float)n3 / 128.0f);
            ++n3;
        }
        int n4 = 0;
        while (n4 < 128) {
            nArray[n4 + 128] = mmaakma.maJaKkA(80, 140, 200, 255, 255, 255, 1.0f - (float)n4 / 128.0f);
            ++n4;
        }
        int n5 = 0;
        while (n5 < 256) {
            int n6 = 0;
            while (n6 < 256) {
                double d = 1.0 - (double)n5 / 255.0;
                int n7 = nArray[n5];
                int n8 = n7 >> 16 & 0xFF;
                int n9 = n7 >> 8 & 0xFF;
                int n10 = n7 & 0xFF;
                int n11 = (int)Math.min(255.0, (double)(kmajkka3.aMajakK[n6] & 0xFF) * d + (1.0 - d) * (double)n8);
                int n12 = (int)Math.min(255.0, (double)(kmajkka3.AmAJakK[n6] & 0xFF) * d + (1.0 - d) * (double)n9);
                int n13 = (int)Math.min(255.0, (double)(kmajkka3.amAJakK[n6] & 0xFF) * d + (1.0 - d) * (double)n10);
                mmaamma2.MAJakKa[n5 * 256 + n6] = n11 << 20 | n12 << 10 | n13;
                ++n6;
            }
            ++n5;
        }
        forward.kkamAJA.kAMajak();
        return kmjakmk2;
    }
}
