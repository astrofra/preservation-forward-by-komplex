/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Color;

public class maajmka
extends mmjjmma {
    kaajmmk KAmaJak;
    kmjakmk kAmaJak;
    mmajmmk KaMAJak;
    kaajkka kaMAJak;
    mmaamma KAMAJak;
    kmajkka kAMAJak;
    mmaamma KamAJak;
    mmaamma kamAJak;
    mmajmmk KAmAJak;
    mmjamma kAmAJak;
    float KaMajak;
    float kaMajak;
    public static int[] KAMajak = new int[1000];
    public static int[] kAMajak;

    public String majakkA() {
        return "saari";
    }

    public void MajakkA(mmjamma mmjamma2) {
        this.kAmAJak = mmjamma2;
        this.KAmaJak = new kaajmmk();
        this.KAmaJak.jaKkaMa = 1.4f;
        this.KAmaJak.JAkKAMa = 512;
        this.KAmaJak.jAkKAMa = 256;
        this.KAmaJak.jAkkaMa = 250.0f;
        this.kAmaJak = maajmka.KAmAjAK(this.KAmaJak, this.kAmAJak);
        forward.kkamAJA.kAMajak();
        this.KaMAJak = this.kamAjAK();
        forward.kkamAJA.kAMajak();
        this.kaMAJak = new kaajkka();
        this.kaMAJak.KamAJaK(this.kAmAJak.amAjAkK("asses/alku6.ase"));
        forward.kkamAJA.kAMajak();
        kaajkka kaajkka2 = this.kaMAJak;
        int n = 0;
        while (n < kaajkka2.MaJAKkA.size()) {
            mmajmmk mmajmmk2 = (mmajmmk)kaajkka2.MaJAKkA.elementAt(n);
            mmajmmk2.JAkKaMA = true;
            mmajmmk2.jAkKaMA = true;
            mmajmmk2.kkAMAJa();
            mmajmmk2.JAKKaMA.MajaKka(1.0);
            mmajmmk2.KkAMAJa(this.KamAJak, this.kAMAJak);
            mmajmmk2.KkAmAJa(3);
            if (mmajmmk2.kkAMaja().equals("klunssi")) {
                this.KaMAJak = mmajmmk2;
                mmajmmk2.JAKkama = new maajkka().AmAJAKk(-1.5707964f);
                mmajmmk2.KKamaja(true);
                mmajmmk2.JAKkAma.KkAMAJa(this.kamAJak, this.kAMAJak);
                mmajmmk2.JAKkAma.KkAmAJa(259);
            }
            if (mmajmmk2.kkAMaja().equals("meditate")) {
                mmajmmk2.JAkKaMA = true;
                mmajmmk2.jaKkama = true;
                this.KAmAJak = mmajmmk2;
            }
            ++n;
        }
        this.kaMAJak.maJAKkA(this.kAmaJak);
        kaaakma kaaakma2 = new kaaakma(this.kAmAJak, this.kAmAJak.amAjAkK("images/verax/tai1sp.jpg"), false);
        kaajkka2.maJAKkA(kaaakma2);
        kaaakma2.jaKKaMA.MaJaKka(0.0f, 0.0f, -1000.0f);
        forward.kkamAJA.kAMajak();
        this.kaMAJak.mAJAKkA();
        maajmka.kAMaJak(195);
    }

    public void maJakkA(mmaamma mmaamma2, float f, float f2) {
        forward.kkAMAjA.kAMaJAK(0xFFFFFF);
        mmaamma2.amaJakK();
        forward.kkAMAjA.KkAMAjA();
        this.kaMAJak.kAMAJaK(f * 1.16f, this.KAmaJak);
        this.KAmAJak.JakKaMA.AMaJAKk((float)Math.PI);
        this.KaMAJak.JakKaMA.aMAjaKk();
        this.KaMAJak.JakKaMA.amAJAKk(f / 3.0f);
        this.KaMAJak.JakKaMA.aMaJaKk(f / 3.0f * 2.0f);
        this.KaMAJak.JakKaMA.AMaJAKk(f / 3.0f * 3.0f);
        if (this.KAmaJak.JAKkaMa.mAJakKA < 0.3f) {
            this.KAmaJak.JAKkaMa.mAJakKA = 0.3f;
        }
        this.kaMAJak.MajAKkA(this.KAmaJak, forward.kkAMAjA);
        this.kaMAJak.MAJAKkA(forward.kkAMAjA);
        if (this.KaMajak > 0.0f) {
            this.KaMajak -= this.kaMajak * f2;
            maajmka.KaMaJak(mmaamma2, (int)(this.KaMajak * (float)forward.kAmajAk / 100.0f));
        }
    }

    public void MAJakkA(String string, float f) {
        if (string.equals("suh")) {
            this.KaMajak = 100.0f;
            this.kaMajak = 200.0f;
        }
        if (string.equals("suh0")) {
            this.KaMajak = 68.0f;
            this.kaMajak = 0.0f;
        }
    }

    public static void KaMaJak(mmaamma mmaamma2, int n) {
        if (n > mmaamma2.AMAjakK) {
            n = mmaamma2.AMAjakK - 1;
        }
        int n2 = (int)(Math.random() * 1000.0);
        int n3 = 0;
        while (n3 < n) {
            int n4 = kAMajak[(n3 + n2) % kAMajak.length];
            int n5 = (int)(Math.random() * (double)(KAMajak.length - 1 - forward.KAmajAk));
            int n6 = mmaamma2.amAjakK * n4;
            int[] nArray = KAMajak;
            int[] nArray2 = mmaamma2.MAJakKa;
            int n7 = n5 + mmaamma2.amAjakK;
            while (n5 < n7) {
                int n8 = nArray2[n6] + 0x10040100 - nArray[n5++];
                int n9 = n8 & 0x10040100;
                nArray2[n6++] = n8 & n9 - (n9 >> 8);
            }
            ++n3;
        }
    }

    public static void kAMaJak(int n) {
        int n2;
        int n3;
        kAMajak = new int[forward.kAmajAk];
        int n4 = 0;
        while (n4 < KAMajak.length) {
            n3 = (int)(Math.random() * (double)n);
            maajmka.KAMajak[n4] = n2 = mmaamma.aMAJAKK(n3, n3, n3);
            ++n4;
        }
        n3 = 0;
        while (n3 < kAMajak.length) {
            maajmka.kAMajak[n3] = n3;
            ++n3;
        }
        n2 = 0;
        while (n2 < 3000) {
            int n5 = n2 % kAMajak.length;
            int n6 = (int)(Math.random() * (double)(kAMajak.length - 2));
            int n7 = kAMajak[n5];
            maajmka.kAMajak[n5] = kAMajak[n6];
            maajmka.kAMajak[n6] = n7;
            ++n2;
        }
    }

    mmajmmk kamAjAK() {
        this.KaMAJak = kajjmka.kamAJAK();
        this.KaMAJak.jaKKaMA.maJAkKA(0.0, 1.9, 0.0);
        this.KaMAJak.JAKKaMA.MajaKka(0.9);
        this.KaMAJak.JAkKaMA = true;
        this.KaMAJak.jAkKAma = 0;
        this.kAMAJak = (kmajkka)mmaakma.majaKkA(this.kAmAJak.amAjAkK("images/scape/envi_klu.gif"));
        this.KamAJak = maajmka.kaMaJak(this.kAMAJak, 255, 255, 255);
        this.kamAJak = maajmka.kaMaJak(this.kAMAJak, 0, 0, 0);
        this.KaMAJak.jAkKaMA = true;
        this.KaMAJak.KkAMAJa(this.kamAJak, this.kAMAJak);
        this.KaMAJak.KkAmAJa(259);
        this.KaMAJak.KKamaja(true);
        this.KaMAJak.KkAMAJa(this.KamAJak, this.kAMAJak);
        this.KaMAJak.KkAmAJa(3);
        return this.KaMAJak;
    }

    public static mmaamma kaMaJak(kmajkka kmajkka2, int n, int n2, int n3) {
        mmaamma mmaamma2 = new mmaamma(256, 256, 1, false);
        int n4 = 0;
        while (n4 < 256) {
            int n5 = 0;
            while (n5 < 256) {
                double d = 1.0 - (double)n4 / 255.0;
                int n6 = (int)Math.min(255.0, (double)(kmajkka2.aMajakK[n5] & 0xFF) * d + (1.0 - d) * (double)n);
                int n7 = (int)Math.min(255.0, (double)(kmajkka2.AmAJakK[n5] & 0xFF) * d + (1.0 - d) * (double)n2);
                int n8 = (int)Math.min(255.0, (double)(kmajkka2.amAJakK[n5] & 0xFF) * d + (1.0 - d) * (double)n3);
                mmaamma2.MAJakKa[n4 * 256 + n5] = n6 << 20 | n7 << 10 | n8;
                ++n5;
            }
            ++n4;
        }
        return mmaamma2;
    }

    public static mmaamma KamAjAK(kmajkka kmajkka2, int n, int n2, int n3, int n4, int n5, int n6) {
        mmaamma mmaamma2 = new mmaamma(256, 256, 1, false);
        int[] nArray = new int[256];
        int n7 = 0;
        while (n7 < 128) {
            nArray[n7] = mmaakma.maJaKkA(0, 0, 0, n, n2, n3, 1.0f - (float)n7 / 128.0f);
            ++n7;
        }
        int n8 = 0;
        while (n8 < 128) {
            nArray[n8 + 128] = mmaakma.maJaKkA(n, n2, n3, n4, n5, n6, 1.0f - (float)n8 / 128.0f);
            ++n8;
        }
        int n9 = 0;
        while (n9 < 256) {
            int n10 = 0;
            while (n10 < 256) {
                double d = 1.0 - (double)n9 / 255.0;
                int n11 = nArray[n9];
                int n12 = n11 >> 16 & 0xFF;
                int n13 = n11 >> 8 & 0xFF;
                int n14 = n11 & 0xFF;
                int n15 = (int)Math.min(255.0, (double)(kmajkka2.aMajakK[n10] & 0xFF) * d + (1.0 - d) * (double)n12);
                int n16 = (int)Math.min(255.0, (double)(kmajkka2.AmAJakK[n10] & 0xFF) * d + (1.0 - d) * (double)n13);
                int n17 = (int)Math.min(255.0, (double)(kmajkka2.amAJakK[n10] & 0xFF) * d + (1.0 - d) * (double)n14);
                mmaamma2.MAJakKa[n9 * 256 + n10] = n15 << 20 | n16 << 10 | n17;
                ++n10;
            }
            ++n9;
        }
        return mmaamma2;
    }

    public static byte[] kAmAjAK(kmajkka kmajkka2) {
        float[] fArray = new float[3];
        byte[] byArray = new byte[256];
        int n = 0;
        while (n < 256) {
            int n2 = kmajkka2.aMajakK[n] & 0xFF;
            int n3 = kmajkka2.AmAJakK[n] & 0xFF;
            int n4 = kmajkka2.amAJakK[n] & 0xFF;
            Color.RGBtoHSB(n2, n3, n4, fArray);
            if (fArray[0] > 0.5f && fArray[0] < 0.7f) {
                byArray[n] = 1;
            }
            ++n;
        }
        return byArray;
    }

    public static short[][] KAMaJak(kmajkka kmajkka2, int n) {
        short[][] sArray = new short[kmajkka2.AMAjakK][kmajkka2.amAjakK];
        int n2 = 0;
        while (n2 < kmajkka2.AMAjakK * kmajkka2.amAjakK) {
            int n3 = kmajkka2.aMajakK[kmajkka2.aMAjakK[n2] & 0xFF] & 0xFF;
            if ((n3 += n) < 0) {
                n3 = 0;
            }
            sArray[n2 / kmajkka2.amAjakK][n2 % kmajkka2.amAjakK] = (short)n3;
            ++n2;
        }
        return sArray;
    }

    public static kmjakmk KAmAjAK(kaajmmk kaajmmk2, mmjamma mmjamma2) {
        mmaamma mmaamma2;
        int n;
        int n2;
        mmaamma mmaamma3;
        String string = "images/scape/saarih15.gif";
        String string2 = "images/scape/saari.gif";
        kmajkka kmajkka2 = (kmajkka)mmaakma.majaKkA(mmjamma2.amAjAkK(string));
        int n3 = kmajkka2.amAjakK;
        int n4 = kmajkka2.AMAjakK;
        short[][] sArray = maajmka.KAMaJak(kmajkka2, -16);
        kaajmmk2.JAKkaMa.MaJaKka(0.0f, 2.0f, 0.0f);
        kmjakmk kmjakmk2 = new kmjakmk(sArray, 200.0f / (float)n3, 0.16f, kaajmmk2, true);
        kmjakmk2.AKkAmAJ = -n3 / 2;
        kmjakmk2.aKkAmAJ = -n4 / 2;
        kaajmmk2.JaKKaMa = kaajmmk2.jAkkaMa * 0.4f;
        kmajkka kmajkka3 = (kmajkka)mmaakma.majaKkA(mmjamma2.amAjAkK(string2));
        kmajkka kmajkka4 = new kmajkka(256, 256, 1, false);
        kmajkka kmajkka5 = new kmajkka(256, 256, 1, false);
        kmajkka4.aMAJakk(kmajkka3, 0, 0, 0, 0, 256, 256);
        kmajkka5.aMAJakk(kmajkka3, 0, 0, 0, 256, 256, 256);
        kmjakmk2.aKkaMaj = kmajkka4;
        kmjakmk2.AkKAMaj = kmajkka5;
        kmjakmk2.akKamaj = 1.0f / (float)n3;
        kmjakmk2.AKKamaj = 1.0f / (float)n4;
        byte[] byArray = maajmka.kAmAjAK(kmajkka3);
        kmjakmk2.AKkaMaj = mmaamma3 = new mmaamma(256, 256, 1, false);
        int n5 = 0;
        while (n5 < 256) {
            int n6 = 0;
            while (n6 < 256) {
                double d = 1.0 - (double)n5 / 255.0;
                int n7 = (int)Math.min(255.0, (double)(kmajkka3.aMajakK[n6] & 0xFF) * d + (1.0 - d) * 255.0);
                n2 = (int)Math.min(255.0, (double)(kmajkka3.AmAJakK[n6] & 0xFF) * d + (1.0 - d) * 255.0);
                n = (int)Math.min(255.0, (double)(kmajkka3.amAJakK[n6] & 0xFF) * d + (1.0 - d) * 255.0);
                mmaamma3.MAJakKa[n5 * 256 + n6] = byArray[n6] != 0 ? n7 << 20 | n2 << 10 | n | Integer.MIN_VALUE : n7 << 20 | n2 << 10 | n;
                ++n6;
            }
            ++n5;
        }
        kmjakmk2.akKAMaj = mmaamma2 = new mmaamma(256, 256, 1, false);
        int n8 = 0;
        while (n8 < 256) {
            int n9 = 0;
            while (n9 < 256) {
                float f = 1.0f - (float)n8 / 255.0f;
                n2 = kmajkka3.aMajakK[n9] & 0xFF;
                n = kmajkka3.AmAJakK[n9] & 0xFF;
                int n10 = kmajkka3.aMajakK[n9] & 0xFF;
                int n11 = mmaakma.MAjaKkA(n2, n, n10, 0.0f, 0.0f, 0.0f, f);
                mmaamma2.MAJakKa[n8 * 256 + n9] = mmaamma.AMajAKK(n11);
                ++n9;
            }
            ++n8;
        }
        kmjakmk2.AKkaMaj = mmaamma3;
        kmjakmk2.akKAMaj = mmaamma2;
        return kmjakmk2;
    }
}
