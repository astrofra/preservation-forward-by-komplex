/*
 * Decompiled with CFR 0.152.
 */
public class ParticleCloudMesh
extends MeshObject {
    float kKaMAJA;
    ParticleVertex[] KkAmaJA;
    public final static int kkAmaJA = 0;
    public final static int KKAmaJA = 1;
    int kKAmaJA;
    public float KkamaJA = 10.0f;
    float kkamaJA;
    int KKamaJA;
    float kKamaJA;

    public ParticleCloudMesh(int n, float f) {
        this.kKaMAJA = f;
        this.kaMAJAk(n);
        this.kkAMAJa();
        this.jakKaMA = Float.POSITIVE_INFINITY;
        this.KamaJAk(0);
    }

    public void kaMAJAk(int n) {
        this.JAkkaMA = new Triangle[n];
        this.KkAmaJA = new ParticleVertex[n];
        this.jAkkaMA = this.KkAmaJA;
        this.JaKKaMA = new UvCoord[3];
        this.JaKKaMA[0] = new UvCoord(0.0f, 0.0f);
        this.JaKKaMA[1] = new UvCoord(1.0f, 0.0f);
        this.JaKKaMA[2] = new UvCoord(0.0f, 1.0f);
        UvCoord kmajkmk2 = this.JaKKaMA[0];
        UvCoord kmajkmk3 = this.JaKKaMA[0];
        UvCoord kmajkmk4 = this.JaKKaMA[0];
        float f = 20.0f;
        int n2 = 0;
        while (n2 < n) {
            ParticleVertex mmaakka2;
            float f2 = 0.0f;
            float f3 = 0.0f;
            float f4 = 0.0f;
            f2 = (float)((Math.random() - 0.5) * (double)f);
            f3 = (float)((Math.random() - 0.5) * (double)f);
            f4 = (float)((Math.random() - 0.5) * (double)f);
            this.KkAmaJA[n2] = mmaakka2 = new ParticleVertex(f2, f3, f4);
            this.JAkkaMA[n2] = new Triangle(mmaakka2, mmaakka2, mmaakka2, kmajkmk2, kmajkmk3, kmajkmk4);
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

    public void kkaMAJa(Camera kaajmmk2, int n) {
        this.jaKkAma = kaajmmk2;
        switch (this.kKAmaJA) {
            case 1: {
                this.kAmaJAk(new Vec3f(0.0f, 0.0f, 1.0f), new Vec3f(0.0f, 0.0f, 1.0f), 0.1f, 0.5f);
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
            ParticleVertex mmaakka2 = this.KkAmaJA[n3];
            if (mmaakka2.init > this.kkamaJA && mmaakka2.init < this.jAKKAma) {
                float f = mmaakka2.scriptName;
                float f2 = mmaakka2.onShow;
                float f3 = this.kKaMAJA / mmaakka2.init;
                this.KkAmaJA[n++].AmajAkk = f3;
                Triangle kmaamma2 = this.JAkkaMA[n3];
                kmaamma2.majAkKa = -mmaakka2.init * 3.0f;
                RenderPrimitive.majaKKa[RenderPrimitive.MAjaKKa++] = kmaamma2;
            } else {
                ++n;
            }
            ++n3;
        }
    }

    public void kAmaJAk(Vec3f mmajmma2, Vec3f mmajmma3, float f, float f2) {
        float f3;
        Vec3f mmajmma4 = new Vec3f(0.0f, 0.0f, -0.05f);
        int n = this.KkAmaJA.length;
        ParticleVertex[] mmaakkaArray = this.KkAmaJA;
        int n2 = 0;
        while (n2 < n) {
            ParticleVertex mmaakka2 = mmaakkaArray[n2];
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

    void KAmaJAk(Vec3f mmajmma2, float f, float f2, float f3) {
        Vec3f mmajmma3 = new Vec3f();
        ParticleVertex mmaakka2 = this.KkAmaJA[this.KKamaJA++];
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
