/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Color;

public class SaariScene
extends Scene {
    Camera KAmaJak;
    kmjakmk kAmaJak;
    MeshObject KaMAJak;
    AseSceneLoader kaMAJak;
    RgbSurface KAMAJak;
    IndexedSurface showScene;
    RgbSurface KamAJak;
    RgbSurface kamAJak;
    MeshObject KAmAJak;
    DesktopAppletBase kAmAJak;
    float executeScriptCommand;
    float kaMajak;
    public static int[] killScene = new int[1000];
    public static int[] kAMajak;

    public String scriptName() {
        return "saari";
    }

    public void init(DesktopAppletBase mmjamma2) {
        this.kAmAJak = mmjamma2;
        this.KAmaJak = new Camera();
        this.KAmaJak.jaKkaMa = 1.4f;
        this.KAmaJak.JAkKAMa = 512;
        this.KAmaJak.jAkKAMa = 256;
        this.KAmaJak.jAkkaMa = 250.0f;
        this.kAmaJak = SaariScene.KAmAjAK(this.KAmaJak, this.kAmAJak);
        ForwardDemoApp.kkamAJA.kAMajak();
        this.KaMAJak = this.kamAjAK();
        ForwardDemoApp.kkamAJA.kAMajak();
        this.kaMAJak = new AseSceneLoader();
        this.kaMAJak.KamAJaK(this.kAmAJak.amAjAkK("asses/alku6.ase"));
        ForwardDemoApp.kkamAJA.kAMajak();
        AseSceneLoader kaajkka2 = this.kaMAJak;
        int n = 0;
        while (n < kaajkka2.MaJAKkA.size()) {
            MeshObject mmajmmk2 = (MeshObject)kaajkka2.MaJAKkA.elementAt(n);
            mmajmmk2.JAkKaMA = true;
            mmajmmk2.jAkKaMA = true;
            mmajmmk2.kkAMAJa();
            mmajmmk2.JAKKaMA.MajaKka(1.0);
            mmajmmk2.KkAMAJa(this.KamAJak, this.showScene);
            mmajmmk2.KkAmAJa(3);
            if (mmajmmk2.kkAMaja().equals("klunssi")) {
                this.KaMAJak = mmajmmk2;
                mmajmmk2.JAKkama = new Mat3f().AmAJAKk(-1.5707964f);
                mmajmmk2.KKamaja(true);
                mmajmmk2.JAKkAma.KkAMAJa(this.kamAJak, this.showScene);
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
        ForwardDemoApp.kkamAJA.kAMajak();
        this.kaMAJak.mAJAKkA();
        SaariScene.kAMaJak(195);
    }

    public void render(RgbSurface mmaamma2, float f, float f2) {
        ForwardDemoApp.kkAMAjA.kAMaJAK(0xFFFFFF);
        mmaamma2.amaJakK();
        ForwardDemoApp.kkAMAjA.KkAMAjA();
        this.kaMAJak.kAMAJaK(f * 1.16f, this.KAmaJak);
        this.KAmAJak.JakKaMA.AMaJAKk((float)Math.PI);
        this.KaMAJak.JakKaMA.aMAjaKk();
        this.KaMAJak.JakKaMA.amAJAKk(f / 3.0f);
        this.KaMAJak.JakKaMA.aMaJaKk(f / 3.0f * 2.0f);
        this.KaMAJak.JakKaMA.AMaJAKk(f / 3.0f * 3.0f);
        if (this.KAmaJak.JAKkaMa.mAJakKA < 0.3f) {
            this.KAmaJak.JAKkaMa.mAJakKA = 0.3f;
        }
        this.kaMAJak.MajAKkA(this.KAmaJak, ForwardDemoApp.kkAMAjA);
        this.kaMAJak.MAJAKkA(ForwardDemoApp.kkAMAjA);
        if (this.executeScriptCommand > 0.0f) {
            this.executeScriptCommand -= this.kaMajak * f2;
            SaariScene.KaMaJak(mmaamma2, (int)(this.executeScriptCommand * (float)ForwardDemoApp.kAmajAk / 100.0f));
        }
    }

    public void handleMessage(String string, float f) {
        if (string.equals("suh")) {
            this.executeScriptCommand = 100.0f;
            this.kaMajak = 200.0f;
        }
        if (string.equals("suh0")) {
            this.executeScriptCommand = 68.0f;
            this.kaMajak = 0.0f;
        }
    }

    public static void KaMaJak(RgbSurface mmaamma2, int n) {
        if (n > mmaamma2.AMAjakK) {
            n = mmaamma2.AMAjakK - 1;
        }
        int n2 = (int)(Math.random() * 1000.0);
        int n3 = 0;
        while (n3 < n) {
            int n4 = kAMajak[(n3 + n2) % kAMajak.length];
            int n5 = (int)(Math.random() * (double)(killScene.length - 1 - ForwardDemoApp.KAmajAk));
            int n6 = mmaamma2.amAjakK * n4;
            int[] nArray = killScene;
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
        kAMajak = new int[ForwardDemoApp.kAmajAk];
        int n4 = 0;
        while (n4 < killScene.length) {
            n3 = (int)(Math.random() * (double)n);
            SaariScene.killScene[n4] = n2 = RgbSurface.aMAJAKK(n3, n3, n3);
            ++n4;
        }
        n3 = 0;
        while (n3 < kAMajak.length) {
            SaariScene.kAMajak[n3] = n3;
            ++n3;
        }
        n2 = 0;
        while (n2 < 3000) {
            int n5 = n2 % kAMajak.length;
            int n6 = (int)(Math.random() * (double)(kAMajak.length - 2));
            int n7 = kAMajak[n5];
            SaariScene.kAMajak[n5] = kAMajak[n6];
            SaariScene.kAMajak[n6] = n7;
            ++n2;
        }
    }

    MeshObject kamAjAK() {
        this.KaMAJak = kajjmka.kamAJAK();
        this.KaMAJak.jaKKaMA.maJAkKA(0.0, 1.9, 0.0);
        this.KaMAJak.JAKKaMA.MajaKka(0.9);
        this.KaMAJak.JAkKaMA = true;
        this.KaMAJak.jAkKAma = 0;
        this.showScene = (IndexedSurface)mmaakma.majaKkA(this.kAmAJak.amAjAkK("images/scape/envi_klu.gif"));
        this.KamAJak = SaariScene.kaMaJak(this.showScene, 255, 255, 255);
        this.kamAJak = SaariScene.kaMaJak(this.showScene, 0, 0, 0);
        this.KaMAJak.jAkKaMA = true;
        this.KaMAJak.KkAMAJa(this.kamAJak, this.showScene);
        this.KaMAJak.KkAmAJa(259);
        this.KaMAJak.KKamaja(true);
        this.KaMAJak.KkAMAJa(this.KamAJak, this.showScene);
        this.KaMAJak.KkAmAJa(3);
        return this.KaMAJak;
    }

    public static RgbSurface kaMaJak(IndexedSurface kmajkka2, int n, int n2, int n3) {
        RgbSurface mmaamma2 = new RgbSurface(256, 256, 1, false);
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

    public static RgbSurface KamAjAK(IndexedSurface kmajkka2, int n, int n2, int n3, int n4, int n5, int n6) {
        RgbSurface mmaamma2 = new RgbSurface(256, 256, 1, false);
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

    public static byte[] kAmAjAK(IndexedSurface kmajkka2) {
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

    public static short[][] KAMaJak(IndexedSurface kmajkka2, int n) {
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

    public static kmjakmk KAmAjAK(Camera kaajmmk2, DesktopAppletBase mmjamma2) {
        RgbSurface mmaamma2;
        int n;
        int n2;
        RgbSurface mmaamma3;
        String string = "images/scape/saarih15.gif";
        String string2 = "images/scape/saari.gif";
        IndexedSurface kmajkka2 = (IndexedSurface)mmaakma.majaKkA(mmjamma2.amAjAkK(string));
        int n3 = kmajkka2.amAjakK;
        int n4 = kmajkka2.AMAjakK;
        short[][] sArray = SaariScene.KAMaJak(kmajkka2, -16);
        kaajmmk2.JAKkaMa.MaJaKka(0.0f, 2.0f, 0.0f);
        kmjakmk kmjakmk2 = new kmjakmk(sArray, 200.0f / (float)n3, 0.16f, kaajmmk2, true);
        kmjakmk2.AKkAmAJ = -n3 / 2;
        kmjakmk2.aKkAmAJ = -n4 / 2;
        kaajmmk2.JaKKaMa = kaajmmk2.jAkkaMa * 0.4f;
        IndexedSurface kmajkka3 = (IndexedSurface)mmaakma.majaKkA(mmjamma2.amAjAkK(string2));
        IndexedSurface kmajkka4 = new IndexedSurface(256, 256, 1, false);
        IndexedSurface kmajkka5 = new IndexedSurface(256, 256, 1, false);
        kmajkka4.aMAJakk(kmajkka3, 0, 0, 0, 0, 256, 256);
        kmajkka5.aMAJakk(kmajkka3, 0, 0, 0, 256, 256, 256);
        kmjakmk2.aKkaMaj = kmajkka4;
        kmjakmk2.AkKAMaj = kmajkka5;
        kmjakmk2.akKamaj = 1.0f / (float)n3;
        kmjakmk2.AKKamaj = 1.0f / (float)n4;
        byte[] byArray = SaariScene.kAmAjAK(kmajkka3);
        kmjakmk2.AKkaMaj = mmaamma3 = new RgbSurface(256, 256, 1, false);
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
        kmjakmk2.akKAMaj = mmaamma2 = new RgbSurface(256, 256, 1, false);
        int n8 = 0;
        while (n8 < 256) {
            int n9 = 0;
            while (n9 < 256) {
                float f = 1.0f - (float)n8 / 255.0f;
                n2 = kmajkka3.aMajakK[n9] & 0xFF;
                n = kmajkka3.AmAJakK[n9] & 0xFF;
                int n10 = kmajkka3.aMajakK[n9] & 0xFF;
                int n11 = mmaakma.MAjaKkA(n2, n, n10, 0.0f, 0.0f, 0.0f, f);
                mmaamma2.MAJakKa[n8 * 256 + n9] = RgbSurface.AMajAKK(n11);
                ++n9;
            }
            ++n8;
        }
        kmjakmk2.AKkaMaj = mmaamma3;
        kmjakmk2.akKAMaj = mmaamma2;
        return kmjakmk2;
    }
}
