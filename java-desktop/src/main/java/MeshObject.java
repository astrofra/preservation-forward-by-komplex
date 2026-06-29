/*
 * Decompiled with CFR 0.152.
 */
public class MeshObject {
    protected String jakkaMA;
    public Triangle[] JAkkaMA;
    public Vertex[] jAkkaMA;
    public UvCoord[] JaKKaMA;
    public final Vec3f jaKKaMA = new Vec3f();
    public final Vec3f JAKKaMA = new Vec3f(1.0f, 1.0f, 1.0f);
    public final Vec3f jAKKaMA = new Vec3f();
    public final Mat3f JakKaMA = new Mat3f();
    float jakKaMA;
    public boolean JAkKaMA;
    public boolean jAkKaMA;
    public boolean JaKkAma;
    public Camera jaKkAma;
    public MeshObject JAKkAma;
    int jAKkAma;
    Mat3f JakkAma;
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
    public Mat3f JAKkama;
    static public int jAKkama;
    static public Vertex[] Jakkama;
    static public int jakkama;
    static public Triangle[] JAkkama;

    public MeshObject() {
    }

    public MeshObject(MeshObject mmajmmk2) {
        this.jAkkaMA = new Vertex[mmajmmk2.jAkkaMA.length];
        int n = 0;
        while (n < this.jAkkaMA.length) {
            this.jAkkaMA[n] = new Vertex(mmajmmk2.jAkkaMA[n]);
            ++n;
        }
        this.JaKKaMA = new UvCoord[mmajmmk2.JaKKaMA.length];
        int n2 = 0;
        while (n2 < this.jAkkaMA.length) {
            this.JaKKaMA[n2] = new UvCoord(mmajmmk2.JaKKaMA[n2]);
            ++n2;
        }
        this.JAkkaMA = new Triangle[mmajmmk2.JAkkaMA.length];
        int n3 = 0;
        while (n3 < this.JAkkaMA.length) {
            Triangle kmaamma2 = mmajmmk2.JAkkaMA[n3];
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
            this.JAkkaMA[n3] = new Triangle(this, n4, n5, n6, n7, n8, n9);
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
        this.JAKkAma = new MeshObject(this);
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

    public void KKAmaJa(RgbSurface mmaamma2, RgbSurface mmaamma3) {
        Object object;
        Vec3f mmajmma2 = new Vec3f();
        int n = 0;
        while (n < this.jAkkaMA.length) {
            Vertex mmjakka2 = this.jAkkaMA[n];
            object = this.JaKKaMA[n];
            mmajmma2.mAjakKA(mmjakka2);
            mmajmma2.MAJakKA(0.7853982f);
            mmajmma2.majaKKA();
            double d = mmajmma2.mAJAkKA();
            double d2 = Math.min(Math.abs(1.0 / Math.cos(d)), Math.abs(1.0 / Math.cos(1.5707963267948966 - d)));
            Vec3d kmajmma2 = new Vec3d(mmajmma2.maJakKA, mmajmma2.MAJakKA, 0.0);
            kmajmma2.MajAKka();
            kmajmma2.maJakka(Math.acos(Math.abs(mmajmma2.mAJakKA)) / 1.5707963267948966);
            kmajmma2.maJakka(d2);
            double d3 = kmajmma2.MaJAkKA;
            double d4 = kmajmma2.maJAkKA;
            ((UvCoord)object).akKAMAJ = (float)((d3 + 1.0) / 2.0);
            ((UvCoord)object).AKKAMAJ = (float)((d4 + 1.0) / 2.0);
            ++n;
        }
        int n2 = 0;
        while (n2 < this.JAkkaMA.length) {
            object = this.JAkkaMA[n2];
            ((RenderPrimitive)object).maJaKKa = ((RenderPrimitive)object).MajaKKa < 0.0f ? mmaamma2 : mmaamma3;
            ++n2;
        }
    }

    public void KkAMaja() {
        Vec3f mmajmma2 = new Vec3f();
        int n = 0;
        while (n < this.jAkkaMA.length) {
            Vertex mmjakka2 = this.jAkkaMA[n];
            UvCoord kmajkmk2 = this.JaKKaMA[n];
            mmajmma2.MaJaKka(mmjakka2.aMAJaKK, mmjakka2.AmaJaKK, mmjakka2.render);
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

    public void KKaMAJa(Vec3f mmajmma2) {
        Vec3f mmajmma3 = this.jAKKaMA.mAjAKKA(mmajmma2);
        int n = 0;
        while (n < this.jAkkaMA.length) {
            Vertex mmjakka2 = this.jAkkaMA[n];
            mmjakka2.majAkKA(mmajmma3);
            ++n;
        }
        this.jAKKaMA.mAjakKA(mmajmma2);
    }

    public void kkAmaJa(Vec3f mmajmma2) {
        this.kkamaja(mmajmma2.maJakKA, mmajmma2.MAJakKA, mmajmma2.mAJakKA);
    }

    public void kkamaja(float f, float f2, float f3) {
        int n = 0;
        while (n < this.jAkkaMA.length) {
            Vertex mmjakka2 = this.jAkkaMA[n];
            mmjakka2.MajAKKA(f, f2, f3);
            ++n;
        }
    }

    public void kkAMAJa() {
        Object object;
        int n = 0;
        while (n < this.JAkkaMA.length) {
            object = this.JAkkaMA[n];
            ((RenderPrimitive)object).aMaJAkK();
            ((RenderPrimitive)object).AMaJAkK();
            ++n;
        }
        if (this.JAkKaMA) {
            this.JaKKaMA = new UvCoord[this.jAkkaMA.length];
            int n2 = 0;
            while (n2 < this.jAkkaMA.length) {
                this.JaKKaMA[n2] = new UvCoord();
                ++n2;
            }
            int n3 = 0;
            while (n3 < this.JAkkaMA.length) {
                Triangle kmaamma2 = this.JAkkaMA[n3];
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
        object = new Vec3f();
        float f = 0.0f;
        int n7 = 0;
        while (n7 < this.jAkkaMA.length) {
            Vertex mmjakka2 = this.jAkkaMA[n7];
            ((Vec3f)object).MaJaKka(mmjakka2.aMAJaKK, mmjakka2.AmaJaKK, mmjakka2.render);
            ((Vec3f)object).majaKKA();
            mmjakka2.aMAJaKK = ((Vec3f)object).maJakKA;
            mmjakka2.AmaJaKK = ((Vec3f)object).MAJakKA;
            mmjakka2.render = ((Vec3f)object).mAJakKA;
            float f2 = mmjakka2.MAjaKKA();
            if (f2 > f) {
                f = f2;
            }
            ++n7;
        }
        this.jakKaMA = (float)Math.sqrt(f);
    }

    public void kkAmAJa(RgbSurface mmaamma2, float f) {
        int n = 0;
        while (n < this.jAkkaMA.length) {
            Vertex mmjakka2 = this.jAkkaMA[n];
            UvCoord kmajkmk2 = this.JaKKaMA[n];
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

    public void KkaMaja(RgbSurface mmaamma2) {
        int n = 0;
        while (n < this.JAkkaMA.length) {
            this.JAkkaMA[n].maJaKKa = mmaamma2;
            ++n;
        }
    }

    public void KkAMAJa(RgbSurface mmaamma2, IndexedSurface kmajkka2) {
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
            Triangle kmaamma2 = this.JAkkaMA[n];
            kmaamma2.MAJaKKa = -kmaamma2.MAJaKKa;
            kmaamma2.mAJaKKa = -kmaamma2.mAJaKKa;
            kmaamma2.MajaKKa = -kmaamma2.MajaKKa;
            ++n;
        }
    }

    public void kkaMAJa(Camera kaajmmk2, int n) {
        this.jaKkAma = kaajmmk2;
        this.jAKkAma = RenderPrimitive.MAjaKKa;
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

    Vec3f kKAMaja() {
        Vec3f mmajmma2 = new Vec3f(this.jaKKaMA);
        mmajmma2.maJAKKA(this.jaKkAma.JAKkaMa);
        this.JakKaMA.AmajAKk().amaJaKk(mmajmma2);
        return new Vec3f(mmajmma2.maJakKA / this.JAKKaMA.maJakKA, mmajmma2.MAJakKA / this.JAKKaMA.MAJakKA, mmajmma2.mAJakKA / this.JAKKaMA.mAJakKA);
    }

    void KKaMaja() {
        float f = this.jaKkAma.JaKKaMa;
        float f2 = this.jaKkAma.jAkkaMa;
        Vertex[] mmjakkaArray = this.jAkkaMA;
        int n = mmjakkaArray.length;
        int n2 = 0;
        while (n2 < n) {
            Vertex mmjakka2 = mmjakkaArray[n2];
            float f3 = (mmjakka2.init - f) / (f2 - f);
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
        Vec3f mmajmma2 = new Vec3f(this.jaKKaMA);
        mmajmma2.maJAKKA(this.jaKkAma.JAKkaMa);
        this.jaKkAma.jAKkaMa.AmajAKk().amaJaKk(mmajmma2);
        this.JakKAma = mmajmma2.maJakKA;
        this.jakKAma = mmajmma2.MAJakKA;
        this.JAkKAma = mmajmma2.mAJakKA;
    }

    public void KkAmaja() {
        this.KKAMAJa(this.jAkkaMA, this.jAkkaMA.length);
    }

    public void KKAMAJa(Vertex[] mmjakkaArray, int n) {
        Mat3f maajkka2 = this.JakkAma;
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
            Vertex mmjakka2 = mmjakkaArray[n2++];
            float f16 = mmjakka2.maJakKA;
            float f17 = mmjakka2.MAJakKA;
            float f18 = mmjakka2.mAJakKA;
            float f19 = f * f16 + f4 * f17 + f7 * f18 + f10;
            float f20 = f2 * f16 + f5 * f17 + f8 * f18 + f11;
            float f21 = f3 * f16 + f6 * f17 + f9 * f18 + f12;
            float f22 = f13 / f21;
            mmjakka2.scriptName = f19 * f22 + f14;
            mmjakka2.onShow = -(f20 * f22) + f15;
            mmjakka2.init = f21;
        }
    }

    public void kKAmaJa() {
        this.kKamAJa(this.jAkkaMA, this.jAkkaMA.length);
    }

    public void kKamAJa(Vertex[] mmjakkaArray, int n) {
        Mat3f maajkka2 = this.JakkAma;
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
            Vertex mmjakka2 = mmjakkaArray[n2++];
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
            mmjakka2.scriptName = f20 * f23 + f14;
            mmjakka2.onShow = -(f21 * f23) + f15;
            mmjakka2.handleMessage = f20;
            mmjakka2.dispose = f21;
            mmjakka2.init = f22;
            if (mmjakka2.scriptName < 0.0f) {
                mmjakka2.aMAjAkk |= 1;
            } else if (mmjakka2.scriptName >= f18) {
                mmjakka2.aMAjAkk |= 8;
            }
            if (mmjakka2.onShow < 0.0f) {
                mmjakka2.aMAjAkk |= 0x40;
                continue;
            }
            if (!(mmjakka2.onShow >= f19)) continue;
            mmjakka2.aMAjAkk |= 0x200;
        }
    }

    public void KKAmaja() {
        this.kKAMAJa(this.jAkkaMA, this.jAkkaMA.length);
    }

    public void kKAMAJa(Vertex[] mmjakkaArray, int n) {
        Mat3f maajkka2 = this.JakkAma;
        float f = maajkka2.AMaJakK;
        float f2 = maajkka2.aMaJakK;
        float f3 = maajkka2.AmAjAKK;
        float f4 = maajkka2.amAjAKK;
        float f5 = maajkka2.AMAjAKK;
        float f6 = maajkka2.aMAjAKK;
        float f7 = maajkka2.AmajAKK;
        float f8 = maajkka2.amajAKK;
        float f9 = maajkka2.AMajAKK;
        Mat3f maajkka3 = this.JakKaMA;
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
        Vec3f mmajmma2 = new Vec3f();
        int n3 = 0;
        while (n3 < n) {
            Vertex mmjakka2 = mmjakkaArray[n3++];
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
            mmjakka2.scriptName = f27 * f30 + f14;
            mmjakka2.onShow = -(f28 * f30) + f15;
            mmjakka2.handleMessage = f27;
            mmjakka2.dispose = f28;
            mmjakka2.init = f29;
            if (mmjakka2.scriptName < 0.0f) {
                mmjakka2.aMAjAkk |= 1;
            } else if (mmjakka2.scriptName >= f18) {
                mmjakka2.aMAjAkk |= 8;
            }
            if (mmjakka2.onShow < 0.0f) {
                mmjakka2.aMAjAkk |= 0x40;
                continue;
            }
            if (!(mmjakka2.onShow >= f19)) continue;
            mmjakka2.aMAjAkk |= 0x200;
        }
    }

    public void kkAmaja() {
        this.Kkamaja(this.JAkkaMA, this.JAkkaMA.length);
    }

    public void Kkamaja(Triangle[] kmaammaArray, int n) {
        Triangle[] kmaammaArray2 = RenderPrimitive.majaKKa;
        int n2 = RenderPrimitive.MAjaKKa;
        Vec3f mmajmma2 = this.kKAMaja();
        float f = mmajmma2.maJakKA;
        float f2 = mmajmma2.MAJakKA;
        float f3 = mmajmma2.mAJakKA;
        int n3 = 0;
        while (n3 < n) {
            Triangle kmaamma2 = kmaammaArray[n3++];
            if (!((f + kmaamma2.mAjakKa.maJakKA) * kmaamma2.MAJaKKa + (f2 + kmaamma2.mAjakKa.MAJakKA) * kmaamma2.mAJaKKa + (f3 + kmaamma2.mAjakKa.mAJakKA) * kmaamma2.MajaKKa < 0.0f)) continue;
            kmaamma2.majAkKa = -(kmaamma2.mAjakKa.init + kmaamma2.MaJAkKa.init + kmaamma2.maJAkKa.init);
            kmaammaArray2[n2++] = kmaamma2;
        }
        RenderPrimitive.MAjaKKa = n2;
    }

    public void KkaMAJa() {
        this.KkAmaJa(this.JAkkaMA, this.JAkkaMA.length);
    }

    public void KkAmaJa(Triangle[] kmaammaArray, int n) {
        Triangle[] kmaammaArray2 = RenderPrimitive.majaKKa;
        int n2 = RenderPrimitive.MAjaKKa;
        Vec3f mmajmma2 = this.kKAMaja();
        float f = mmajmma2.maJakKA;
        float f2 = mmajmma2.MAJakKA;
        float f3 = mmajmma2.mAJakKA;
        int n3 = 0;
        while (n3 < n) {
            Triangle kmaamma2 = kmaammaArray[n3++];
            int n4 = 37449 + kmaamma2.mAjakKa.aMAjAkk + kmaamma2.MaJAkKa.aMAjAkk + kmaamma2.maJAkKa.aMAjAkk;
            if ((n4 & 0x34924) == 0) {
                if (!((f + kmaamma2.mAjakKa.maJakKA) * kmaamma2.MAJaKKa + (f2 + kmaamma2.mAjakKa.MAJakKA) * kmaamma2.mAJaKKa + (f3 + kmaamma2.mAjakKa.mAJakKA) * kmaamma2.MajaKKa < 0.0f)) continue;
                kmaamma2.majAkKa = -(kmaamma2.mAjakKa.init + kmaamma2.MaJAkKa.init + kmaamma2.maJAkKa.init);
                kmaammaArray2[n2++] = kmaamma2;
                continue;
            }
            if ((n4 & 0x24924) != 0 || (n4 & 0x30000) == 0 || !((f + kmaamma2.mAjakKa.maJakKA) * kmaamma2.MAJaKKa + (f2 + kmaamma2.mAjakKa.MAJakKA) * kmaamma2.mAJaKKa + (f3 + kmaamma2.mAjakKa.mAJakKA) * kmaamma2.MajaKKa < 0.0f)) continue;
            kmaamma2.majAkKa = -(kmaamma2.mAjakKa.init + kmaamma2.MaJAkKa.init + kmaamma2.maJAkKa.init);
            this.KKamAJa(kmaamma2);
        }
        RenderPrimitive.MAjaKKa = n2;
    }

    public void KKAmAJa() {
        Triangle[] kmaammaArray = this.JAkkaMA;
        int n = kmaammaArray.length;
        Triangle[] kmaammaArray2 = RenderPrimitive.majaKKa;
        int n2 = RenderPrimitive.MAjaKKa;
        int n3 = 0;
        while (n3 < n) {
            Triangle kmaamma2 = kmaammaArray[n3++];
            float f = kmaamma2.mAjakKa.scriptName;
            float f2 = kmaamma2.MaJAkKa.scriptName - f;
            float f3 = kmaamma2.mAjakKa.onShow;
            float f4 = kmaamma2.maJAkKa.onShow - f3;
            float f5 = kmaamma2.maJAkKa.scriptName - f;
            float f6 = kmaamma2.MaJAkKa.onShow - f3;
            if (!(f2 * f4 - f5 * f6 < 0.0f)) continue;
            kmaamma2.majAkKa = -(kmaamma2.mAjakKa.init + kmaamma2.MaJAkKa.init + kmaamma2.maJAkKa.init);
            kmaammaArray2[n2++] = kmaamma2;
        }
        RenderPrimitive.MAjaKKa = n2;
    }

    public void kKamaja() {
        Triangle[] kmaammaArray = this.JAkkaMA;
        int n = kmaammaArray.length;
        Triangle[] kmaammaArray2 = RenderPrimitive.majaKKa;
        int n2 = RenderPrimitive.MAjaKKa;
        float f = this.JAKKAma;
        int n3 = 0;
        while (n3 < n) {
            Triangle kmaamma2 = kmaammaArray[n3++];
            if (kmaamma2.mAjakKa.init < f || kmaamma2.MaJAkKa.init < f || kmaamma2.maJAkKa.init < f) break;
            float f2 = kmaamma2.mAjakKa.scriptName;
            float f3 = kmaamma2.MaJAkKa.scriptName - f2;
            float f4 = kmaamma2.mAjakKa.onShow;
            float f5 = kmaamma2.maJAkKa.onShow - f4;
            float f6 = kmaamma2.maJAkKa.scriptName - f2;
            float f7 = kmaamma2.MaJAkKa.onShow - f4;
            if (!(f3 * f5 - f6 * f7 < 0.0f)) continue;
            kmaamma2.majAkKa = -(kmaamma2.mAjakKa.init + kmaamma2.MaJAkKa.init + kmaamma2.maJAkKa.init);
            kmaammaArray2[n2++] = kmaamma2;
        }
        RenderPrimitive.MAjaKKa = n2;
    }

    public final void KKAMaja() {
        Mat3f maajkka2 = this.JakKaMA;
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
        UvCoord[] kmajkmkArray = this.JaKKaMA;
        int n = kmajkmkArray.length;
        Vertex[] mmjakkaArray = this.jAkkaMA;
        int n2 = 0;
        while (n2 < n) {
            UvCoord kmajkmk2 = kmajkmkArray[n2];
            Vertex mmjakka2 = mmjakkaArray[n2++];
            kmajkmk2.akKAMAJ = (f * mmjakka2.aMAJaKK + f3 * mmjakka2.AmaJaKK + f5 * mmjakka2.render + 1.0f) * 0.5f;
            kmajkmk2.AKKAMAJ = (f2 * mmjakka2.aMAJaKK + f4 * mmjakka2.AmaJaKK + f6 * mmjakka2.render + 1.0f) * 0.5f;
        }
    }

    public void KKamAJa(Triangle kmaamma2) {
        if (jakkama < JAkkama.length) {
            MeshObject.JAkkama[MeshObject.jakkama++] = kmaamma2;
        }
    }

    public void kkamAJa() {
        if (jakkama == 0) {
            return;
        }
        int n = 0;
        while (n < jakkama) {
            Triangle kmaamma2 = JAkkama[n];
            UvCoord kmajkmk2 = kmaamma2.MAJAkKa;
            UvCoord kmajkmk3 = kmaamma2.mAJAkKa;
            UvCoord kmajkmk4 = kmaamma2.MajAkKa;
            Vertex mmjakka2 = kmaamma2.mAjakKa;
            Vertex mmjakka3 = kmaamma2.MaJAkKa;
            Vertex mmjakka4 = kmaamma2.maJAkKa;
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

    void KkamAJa(Triangle kmaamma2, Vertex mmjakka2, Vertex mmjakka3, Vertex mmjakka4, UvCoord kmajkmk2, UvCoord kmajkmk3, UvCoord kmajkmk4) {
        float f = (mmjakka2.init - this.JAKKAma) / (mmjakka2.init - mmjakka3.init);
        float f2 = mmjakka2.handleMessage + f * (mmjakka3.handleMessage - mmjakka2.handleMessage);
        float f3 = mmjakka2.dispose + f * (mmjakka3.dispose - mmjakka2.dispose);
        Vertex mmjakka5 = new Vertex(f2, f3, this.JAKKAma);
        mmjakka5.KaMAjaK(f2, f3, this.JAKKAma);
        UvCoord kmajkmk5 = new UvCoord(kmajkmk2.akKAMAJ + f * (kmajkmk3.akKAMAJ - kmajkmk2.akKAMAJ), kmajkmk2.AKKAMAJ + f * (kmajkmk3.AKKAMAJ - kmajkmk2.AKKAMAJ));
        f = (mmjakka2.init - this.JAKKAma) / (mmjakka2.init - mmjakka4.init);
        float f4 = mmjakka2.handleMessage + f * (mmjakka4.handleMessage - mmjakka2.handleMessage);
        float f5 = mmjakka2.dispose + f * (mmjakka4.dispose - mmjakka2.dispose);
        Vertex mmjakka6 = new Vertex();
        mmjakka6.KaMAjaK(f4, f5, this.JAKKAma);
        UvCoord kmajkmk6 = new UvCoord(kmajkmk2.akKAMAJ + f * (kmajkmk4.akKAMAJ - kmajkmk2.akKAMAJ), kmajkmk2.AKKAMAJ + f * (kmajkmk4.AKKAMAJ - kmajkmk2.AKKAMAJ));
        MeshObject.Jakkama[MeshObject.jAKkama++] = mmjakka5;
        MeshObject.Jakkama[MeshObject.jAKkama++] = mmjakka6;
        Triangle kmaamma3 = new Triangle(kmaamma2, mmjakka2, mmjakka5, mmjakka6, kmajkmk2, kmajkmk5, kmajkmk6);
        kmaamma3.majAkKa = kmaamma2.majAkKa;
        RenderPrimitive.majaKKa[RenderPrimitive.MAjaKKa++] = kmaamma3;
    }

    void KkamaJa(Triangle kmaamma2, Vertex mmjakka2, Vertex mmjakka3, Vertex mmjakka4, UvCoord kmajkmk2, UvCoord kmajkmk3, UvCoord kmajkmk4) {
        float f = (mmjakka2.init - this.JAKKAma) / (mmjakka2.init - mmjakka4.init);
        float f2 = mmjakka2.handleMessage + f * (mmjakka4.handleMessage - mmjakka2.handleMessage);
        float f3 = mmjakka2.dispose + f * (mmjakka4.dispose - mmjakka2.dispose);
        Vertex mmjakka5 = new Vertex();
        mmjakka5.KaMAjaK(f2, f3, this.JAKKAma);
        UvCoord kmajkmk5 = new UvCoord(kmajkmk2.akKAMAJ + f * (kmajkmk4.akKAMAJ - kmajkmk2.akKAMAJ), kmajkmk2.AKKAMAJ + f * (kmajkmk4.AKKAMAJ - kmajkmk2.AKKAMAJ));
        f = (mmjakka3.init - this.JAKKAma) / (mmjakka3.init - mmjakka4.init);
        float f4 = mmjakka3.handleMessage + f * (mmjakka4.handleMessage - mmjakka3.handleMessage);
        float f5 = mmjakka3.dispose + f * (mmjakka4.dispose - mmjakka3.dispose);
        Vertex mmjakka6 = new Vertex();
        mmjakka6.KaMAjaK(f4, f5, this.JAKKAma);
        UvCoord kmajkmk6 = new UvCoord(kmajkmk3.akKAMAJ + f * (kmajkmk4.akKAMAJ - kmajkmk3.akKAMAJ), kmajkmk3.AKKAMAJ + f * (kmajkmk4.AKKAMAJ - kmajkmk3.AKKAMAJ));
        MeshObject.Jakkama[MeshObject.jAKkama++] = mmjakka5;
        MeshObject.Jakkama[MeshObject.jAKkama++] = mmjakka6;
        Triangle kmaamma3 = new Triangle(kmaamma2, mmjakka5, mmjakka2, mmjakka3, kmajkmk5, kmajkmk2, kmajkmk3);
        kmaamma3.majAkKa = kmaamma2.majAkKa;
        RenderPrimitive.majaKKa[RenderPrimitive.MAjaKKa++] = kmaamma3;
        kmaamma3 = new Triangle(kmaamma2, mmjakka5, mmjakka6, mmjakka3, kmajkmk5, kmajkmk6, kmajkmk3);
        kmaamma3.majAkKa = kmaamma2.majAkKa;
        RenderPrimitive.majaKKa[RenderPrimitive.MAjaKKa++] = kmaamma3;
    }

    public void kKaMAJa(Vertex[] mmjakkaArray, int n) {
        float f = this.JAkkAma;
        float f2 = this.jAkkAma;
        float f3 = this.jakkAma;
        int n2 = 0;
        while (n2 < n) {
            Vertex mmjakka2 = mmjakkaArray[n2];
            float f4 = f3 / mmjakka2.init;
            mmjakka2.scriptName = mmjakka2.handleMessage * f4 + f;
            mmjakka2.onShow = -(mmjakka2.dispose * f4) + f2;
            ++n2;
        }
    }

    static {
        Jakkama = new Vertex[1000];
        JAkkama = new Triangle[1000];
    }
}
