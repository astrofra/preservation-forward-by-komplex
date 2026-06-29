/*
 * Decompiled with CFR 0.152.
 */
public class SplineTrack {
    int aKkaMAj;
    static final public int AkKAMAj = 0;
    static final public int akKAMAj = 1;
    static final public int AKKAMAj = 2;
    int aKKAMAj;
    static final public int AkkAMAj = 0;
    static final public int akkAMAj = 1;
    static final public int AKkAMAj = 2;
    int aKkAMAj = -1;
    int AkKamAj;
    TrackKeyframe[] akKamAj;
    TrackKeyframe AKKamAj;
    TrackKeyframe aKKamAj;
    public float AkkamAj;
    float[] akkamAj;
    float[] AKkamAj;
    int[] aKkamAj;
    int[] AkKAmAj;
    float akKAmAj;
    float AKKAmAj;
    float aKKAmAj;
    float AkkAmAj;
    public float[] akkAmAj;
    public int[] AKkAmAj;
    public Quaternionf[] aKkAmAj;

    public SplineTrack(TrackKeyframe[] kmjjmmaArray, int n, int n2) {
        this.akKamAj = kmjjmmaArray;
        this.AkkamAj = this.akKamAj[this.akKamAj.length - 1].render;
        this.aKKAMAj = n2;
        switch (this.aKKAMAj) {
            case 0: {
                this.AkKamAj = this.akKamAj[0].mAjakkA.length;
                this.akkamAj = new float[this.AkKamAj];
                this.AKkamAj = new float[this.AkKamAj];
                this.akkAmAj = new float[this.AkKamAj];
                break;
            }
            case 1: {
                this.AkKamAj = this.akKamAj[0].MaJAkkA.length;
                this.aKkamAj = new int[this.AkKamAj];
                this.AkKAmAj = new int[this.AkKamAj];
                this.AKkAmAj = new int[this.AkKamAj];
                break;
            }
            case 2: {
                this.AkKamAj = this.akKamAj[0].maJAkkA.length;
                this.aKkAmAj = new Quaternionf[this.AkKamAj];
                int n3 = 0;
                while (n3 < this.AkKamAj) {
                    this.aKkAmAj[n3] = new Quaternionf();
                    ++n3;
                }
                this.kkAmAJA();
                break;
            }
            default: {
                System.err.println("help head! invalid datamode.");
            }
        }
        this.aKkaMAj = n;
        if (this.aKkaMAj == 0 || this.aKkaMAj == 2) {
            this.AKKamAj = this.KkamAJA(0);
            this.aKKamAj = this.KkamAJA(this.akKamAj.length - 1);
        } else if (this.aKkaMAj == 1) {
            this.AKKamAj = this.KkamAJA(this.akKamAj.length - 2);
            this.aKKamAj = this.KkamAJA(1);
        } else {
            System.err.println("invalid spline mode");
        }
        this.kKAmAJA(0);
    }

    TrackKeyframe KkamAJA(int n) {
        if (n < 0) {
            return this.AKKamAj;
        }
        if (n >= this.akKamAj.length) {
            return this.aKKamAj;
        }
        return this.akKamAj[n];
    }

    void kKAmAJA(int n) {
        if (this.aKkAMAj == n) {
            return;
        }
        this.aKkAMAj = n;
        TrackKeyframe kmjjmma2 = this.KkamAJA(n - 1);
        TrackKeyframe kmjjmma3 = this.KkamAJA(n);
        TrackKeyframe kmjjmma4 = this.KkamAJA(n + 1);
        TrackKeyframe kmjjmma5 = this.KkamAJA(n + 2);
        this.akKAmAj = 0.5f * (1.0f - kmjjmma3.handleMessage) * (1.0f + kmjjmma3.dispose) * (1.0f + kmjjmma3.init);
        this.AKKAmAj = 0.5f * (1.0f - kmjjmma3.handleMessage) * (1.0f - kmjjmma3.dispose) * (1.0f - kmjjmma3.init);
        this.aKKAmAj = 0.5f * (1.0f - kmjjmma4.handleMessage) * (1.0f + kmjjmma4.dispose) * (1.0f - kmjjmma4.init);
        this.AkkAmAj = 0.5f * (1.0f - kmjjmma4.handleMessage) * (1.0f - kmjjmma4.dispose) * (1.0f + kmjjmma4.init);
        switch (this.aKKAMAj) {
            case 0: {
                float[] fArray = kmjjmma2.mAjakkA;
                float[] fArray2 = kmjjmma3.mAjakkA;
                float[] fArray3 = kmjjmma4.mAjakkA;
                float[] fArray4 = kmjjmma5.mAjakkA;
                int n2 = 0;
                while (n2 < this.AkKamAj) {
                    this.akkamAj[n2] = (fArray2[n2] - fArray[n2]) * this.akKAmAj + (fArray3[n2] - fArray2[n2]) * this.AKKAmAj;
                    this.AKkamAj[n2] = (fArray4[n2] - fArray3[n2]) * this.aKKAmAj + (fArray3[n2] - fArray2[n2]) * this.AkkAmAj;
                    ++n2;
                }
                return;
            }
            case 2: {
                int n3 = 0;
                while (n3 < this.AkKamAj) {
                    ++n3;
                }
                break;
            }
        }
    }

    void kkAmAJA() {
        int n;
        int n2;
        int n3 = 0;
        while (n3 < this.akKamAj.length) {
            TrackKeyframe kmjjmma2 = this.akKamAj[n3];
            kmjjmma2.MAJAkkA = new Quaternionf[this.AkKamAj];
            kmjjmma2.mAJAkkA = new Quaternionf[this.AkKamAj];
            n2 = 0;
            while (n2 < this.AkKamAj) {
                kmjjmma2.MAJAkkA[n2] = new Quaternionf();
                kmjjmma2.mAJAkkA[n2] = new Quaternionf();
                ++n2;
            }
            ++n3;
        }
        int n4 = this.akKamAj.length - 1;
        n2 = 1;
        while (n2 < n4) {
            n = 0;
            while (n < this.AkKamAj) {
                this.kkamAJA(this.akKamAj[n2 - 1], this.akKamAj[n2], this.akKamAj[n2 + 1], n);
                ++n;
            }
            ++n2;
        }
        n = 0;
        while (n < this.AkKamAj) {
            this.kkamAJA(null, this.akKamAj[0], this.akKamAj[1], n);
            this.kkamAJA(this.akKamAj[0], this.akKamAj[n4], null, n);
            ++n;
        }
    }

    void kkamAJA(TrackKeyframe kmjjmma2, TrackKeyframe kmjjmma3, TrackKeyframe kmjjmma4, int n) {
        Quaternionf maaakmk2 = null;
        Quaternionf maaakmk3 = null;
        Quaternionf maaakmk4 = new Quaternionf();
        Quaternionf maaakmk5 = new Quaternionf();
        Quaternionf maaakmk6 = new Quaternionf(kmjjmma3.maJAkkA[n]);
        Quaternionf maaakmk7 = new Quaternionf(kmjjmma3.maJAkkA[n]);
        maaakmk7.showDebugTimecode();
        Quaternionf maaakmk8 = null;
        Quaternionf maaakmk9 = null;
        Quaternionf maaakmk10 = null;
        Quaternionf maaakmk11 = null;
        if (kmjjmma2 != null) {
            maaakmk8 = new Quaternionf(kmjjmma2.maJAkkA[n]);
            maaakmk9 = new Quaternionf(kmjjmma2.maJAkkA[n]);
            maaakmk9.showDebugTimecode();
            if ((double)Math.abs(maaakmk6.jakKamA - maaakmk8.jakKamA) > 6.283175307179587) {
                maaakmk3 = new Quaternionf(maaakmk6);
                maaakmk3.jakKamA = 0.0f;
                maaakmk3.sceneTimeSeconds();
            } else {
                Quaternionf maaakmk12 = new Quaternionf(maaakmk9);
                if (maaakmk12.KkAMaJA(maaakmk7) < 0.0f) {
                    maaakmk12.KKaMaJA();
                }
                maaakmk3 = new Quaternionf(maaakmk12);
                maaakmk3.kKaMAJA(maaakmk7);
            }
        }
        if (kmjjmma4 != null) {
            maaakmk10 = new Quaternionf(kmjjmma4.maJAkkA[n]);
            maaakmk11 = new Quaternionf(kmjjmma4.maJAkkA[n]);
            maaakmk11.showDebugTimecode();
            if ((double)Math.abs(maaakmk10.jakKamA - maaakmk6.jakKamA) > 6.283175307179587) {
                maaakmk2 = new Quaternionf(maaakmk10);
                maaakmk2.jakKamA = 0.0f;
                maaakmk2.sceneTimeSeconds();
            } else {
                Quaternionf maaakmk13 = new Quaternionf(maaakmk11);
                if (maaakmk13.KkAMaJA(maaakmk7) < 0.0f) {
                    maaakmk13.KKaMaJA();
                }
                maaakmk2 = new Quaternionf(maaakmk7);
                maaakmk2.kKaMAJA(maaakmk13);
            }
        }
        if (kmjjmma2 == null) {
            maaakmk3 = new Quaternionf(maaakmk2);
        }
        if (kmjjmma4 == null) {
            maaakmk2 = new Quaternionf(maaakmk3);
        }
        float f = 1.0f;
        float f2 = 1.0f;
        float f3 = 1.0f - kmjjmma3.init;
        if (kmjjmma2 != null && kmjjmma4 != null) {
            float f4 = 0.5f * (kmjjmma4.render - kmjjmma2.render);
            f2 = (kmjjmma3.render - kmjjmma2.render) / f4;
            f = (kmjjmma4.render - kmjjmma3.render) / f4;
            float f5 = Math.abs(kmjjmma3.init);
            f2 = f2 + f5 - f5 * f2;
            f = f + f5 - f5 * f;
        }
        float f6 = 0.5f * (1.0f - kmjjmma3.handleMessage);
        float f7 = 2.0f - f3;
        float f8 = 1.0f - kmjjmma3.dispose;
        float f9 = 2.0f - f8;
        float f10 = f6 * f3;
        float f11 = f6 * f7;
        float f12 = 1.0f - f10 * f9 * f2;
        float f13 = -f11 * f8 * f2;
        float f14 = f11 * f9 * f;
        float f15 = f10 * f8 * f - 1.0f;
        maaakmk4.JAKKamA = 0.5f * (f14 * maaakmk3.JAKKamA + f15 * maaakmk2.JAKKamA);
        maaakmk5.JAKKamA = 0.5f * (f12 * maaakmk3.JAKKamA + f13 * maaakmk2.JAKKamA);
        maaakmk4.jAKKamA = 0.5f * (f14 * maaakmk3.jAKKamA + f15 * maaakmk2.jAKKamA);
        maaakmk5.jAKKamA = 0.5f * (f12 * maaakmk3.jAKKamA + f13 * maaakmk2.jAKKamA);
        maaakmk4.JakKamA = 0.5f * (f14 * maaakmk3.JakKamA + f15 * maaakmk2.JakKamA);
        maaakmk5.JakKamA = 0.5f * (f12 * maaakmk3.JakKamA + f13 * maaakmk2.JakKamA);
        maaakmk4.jakKamA = 0.5f * (f14 * maaakmk3.jakKamA + f15 * maaakmk2.jakKamA);
        maaakmk5.jakKamA = 0.5f * (f12 * maaakmk3.jakKamA + f13 * maaakmk2.jakKamA);
        Quaternionf maaakmk14 = new Quaternionf(maaakmk4);
        maaakmk14.kkaMAJA();
        Quaternionf maaakmk15 = new Quaternionf(maaakmk5);
        maaakmk15.kkaMAJA();
        Quaternionf maaakmk16 = kmjjmma3.MAJAkkA[n];
        Quaternionf maaakmk17 = kmjjmma3.mAJAkkA[n];
        maaakmk16.kKAmaJA(maaakmk7);
        maaakmk16.renderFrame(maaakmk14);
        maaakmk17.kKAmaJA(maaakmk7);
        maaakmk17.renderFrame(maaakmk15);
    }

    public void KKAmAJA(float f) {
        if (this.aKkaMAj == 0) {
            if (f < 0.0f) {
                f = 0.0f;
            }
            if (f > this.AkkamAj) {
                f = this.AkkamAj;
            }
        } else if ((this.aKkaMAj == 1 || this.aKkaMAj == 2) && (f %= this.AkkamAj) < 0.0f) {
            f += this.AkkamAj;
        }
        TrackKeyframe kmjjmma2 = this.KkamAJA(this.aKkAMAj);
        TrackKeyframe kmjjmma3 = this.KkamAJA(this.aKkAMAj + 1);
        int n = this.aKkAMAj;
        while (f < kmjjmma2.render && n > 0) {
            kmjjmma2 = this.KkamAJA(--n);
            kmjjmma3 = this.KkamAJA(n + 1);
        }
        while (f >= kmjjmma3.render && n < this.akKamAj.length - 2) {
            kmjjmma2 = this.KkamAJA(++n);
            kmjjmma3 = this.KkamAJA(n + 1);
        }
        if (n != this.aKkAMAj) {
            this.kKAmAJA(n);
        }
        float f2 = (f - kmjjmma2.render) / (kmjjmma3.render - kmjjmma2.render);
        float f3 = f2 * f2;
        float f4 = f3 * f2;
        float f5 = 2.0f * f4 - 3.0f * f3 + 1.0f;
        float f6 = -2.0f * f4 + 3.0f * f3;
        float f7 = f4 - 2.0f * f3 + f2;
        float f8 = f4 - f3;
        int n2 = this.AkKamAj;
        switch (this.aKKAMAj) {
            case 0: {
                float[] fArray = this.akkamAj;
                float[] fArray2 = this.AKkamAj;
                float[] fArray3 = kmjjmma2.mAjakkA;
                float[] fArray4 = kmjjmma3.mAjakkA;
                float[] fArray5 = this.akkAmAj;
                int n3 = 0;
                while (n3 < n2) {
                    fArray5[n3] = fArray3[n3] * f5 + fArray4[n3] * f6 + fArray[n3] * f7 + fArray2[n3] * f8;
                    ++n3;
                }
                return;
            }
            case 2: {
                float f9 = (1.0f - f2) * 2.0f * f2;
                int n4 = 0;
                while (n4 < n2) {
                    Quaternionf maaakmk2;
                    Quaternionf maaakmk3;
                    Quaternionf maaakmk4 = new Quaternionf(kmjjmma2.maJAkkA[n4]);
                    maaakmk4.showDebugTimecode();
                    Quaternionf maaakmk5 = new Quaternionf(kmjjmma3.maJAkkA[n4]);
                    maaakmk5.showDebugTimecode();
                    float f10 = kmjjmma3.maJAkkA[n4].jakKamA - kmjjmma2.maJAkkA[n4].jakKamA;
                    float f11 = f10 > 0.0f ? (float)Math.floor((double)f10 / (Math.PI * 2)) : (float)Math.ceil((double)f10 / (Math.PI * 2));
                    f10 -= (float)Math.PI * 2 * f11;
                    Quaternionf maaakmk6 = new Quaternionf();
                    if ((double)Math.abs(f10) > Math.PI) {
                        maaakmk3 = new Quaternionf();
                        maaakmk3.kkAmaJA(maaakmk4, maaakmk5, f2, f11);
                        maaakmk2 = new Quaternionf();
                        maaakmk2.kkAmaJA(kmjjmma2.mAJAkkA[n4], kmjjmma3.MAJAkkA[n4], f2, f11);
                        maaakmk6.kkAmaJA(maaakmk3, maaakmk2, f9, 0.0f);
                    } else {
                        maaakmk3 = new Quaternionf();
                        maaakmk3.kkamaJA(maaakmk4, maaakmk5, f2, f11);
                        maaakmk2 = new Quaternionf();
                        maaakmk2.kkamaJA(kmjjmma2.mAJAkkA[n4], kmjjmma3.MAJAkkA[n4], f2, f11);
                        maaakmk6.kkamaJA(maaakmk3, maaakmk2, f9, 0.0f);
                    }
                    this.aKkAmAj[n4].kKAmaJA(maaakmk6);
                    ++n4;
                }
                return;
            }
        }
    }
}
