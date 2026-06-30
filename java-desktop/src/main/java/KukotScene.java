/*
 * Decompiled with CFR 0.152.
 */
public class KukotScene
extends Scene {
    Camera AMajAkK;
    AseSceneLoader aMajAkK;
    RgbSurface AmAJAkK;
    RgbSurface amAJAkK;
    FlashNoiseOverlay AMAJAkK;
    float aMAJAkK;
    float AmaJAkK;
    DesktopAppletBase amaJAkK;

    public String scriptName() {
        return "kukot";
    }

    public void dispose() {
        this.aMajAkK = null;
        this.AmAJAkK.MAJakKa = null;
        this.AmAJAkK = null;
        this.amAJAkK = null;
    }

    public void init(DesktopAppletBase mmjamma2) {
        this.amaJAkK = mmjamma2;
        this.AMajAkK = new Camera();
        this.AMajAkK.jaKkaMa = 1.4f;
        this.AMajAkK.JAkKAMa = 512;
        this.AMajAkK.jAkKAMa = 256;
        this.AMajAkK.jAkkaMa = 150.0f;
        this.aMajAkK = new AseSceneLoader();
        this.aMajAkK.KamAJaK(this.amaJAkK.amAjAkK("asses/under1.ase"));
        ForwardDemoApp.kkamAJA.kAMajak();
        IndexedSurface kmajkka2 = (IndexedSurface)ImageSupport.majaKkA(this.amaJAkK.amAjAkK("images/envplane.gif"));
        ForwardDemoApp.kkamAJA.kAMajak();
        this.AmAJAkK = KukotScene.aMAjAkk(kmajkka2, 48.0f, 192.0f, 80.0f);
        ForwardDemoApp.kkamAJA.kAMajak();
        int n = 0;
        while (n < this.aMajAkK.MaJAKkA.size()) {
            MeshObject mmajmmk2 = (MeshObject)this.aMajAkK.MaJAKkA.elementAt(n);
            mmajmmk2.JAkKaMA = true;
            mmajmmk2.jAkKaMA = true;
            mmajmmk2.kkAMAJa();
            mmajmmk2.KkAMAJa(this.AmAJAkK, kmajkka2);
            mmajmmk2.KkAmAJa(3);
            mmajmmk2.jAkKAma = 2;
            ++n;
        }
        RgbSurface mmaamma2 = (RgbSurface)ImageSupport.majaKkA(this.amaJAkK.amAjAkK("images/flare1.jpg"));
        ForwardDemoApp.kkamAJA.kAMajak();
        ParticleCloudMesh mmaamka2 = new ParticleCloudMesh(180, 2 * ForwardDemoApp.kAmajAk);
        mmaamka2.KkamaJA = 110.0f;
        mmaamka2.KamaJAk(0);
        mmaamka2.jaKKaMA.mAjakKA(new Vec3f(-5.0f, 35.0f, 5.501f));
        mmaamka2.KkaMaja(mmaamma2);
        this.aMajAkK.maJAKkA(mmaamka2);
        this.aMajAkK.mAJAKkA();
        this.amAJAkK = new RgbSurface(256, 256, 1, false);
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
        ForwardDemoApp.kkamAJA.kAMajak();
        this.AMAJAkK = new FlashNoiseOverlay(38, 16, 87);
    }

    public void render(RgbSurface mmaamma2, float f, float f2) {
        mmaamma2.amaJakK();
        ForwardDemoApp.kkAMAjA.kAMaJAK(0xFFFFFF);
        ForwardDemoApp.kkAMAjA.KkAMAjA();
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
        this.aMajAkK.MajAKkA(this.AMajAkK, ForwardDemoApp.kkAMAjA);
        this.aMajAkK.MAJAKkA(ForwardDemoApp.kkAMAjA);
        mmaamma2.aMajAKK(0.875f);
        if (this.aMAJAkK > 0.0f) {
            this.aMAJAkK -= this.AmaJAkK * f2;
            this.AMAJAkK.KkaMAja(mmaamma2, (int)this.aMAJAkK);
        }
        mmaamma2.AmajakK();
    }

    public void handleMessage(String string, float f) {
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

    public static RgbSurface aMAjAkk(IndexedSurface kmajkka2, float f, float f2, float f3) {
        RgbSurface mmaamma2 = new RgbSurface(256, 256, 1, false);
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
