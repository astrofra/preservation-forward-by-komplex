/*
 * Decompiled with CFR 0.152.
 */
public class maaakmk {
    public float JAKKamA;
    public float jAKKamA;
    public float JakKamA;
    public float jakKamA;
    final static double JAkKamA = 1.0E-6;

    public maaakmk(float f, float f2, float f3, float f4) {
        this.JAKKamA = f;
        this.jAKKamA = f2;
        this.JakKamA = f3;
        this.jakKamA = f4;
    }

    public maaakmk(double d, double d2, double d3, double d4) {
        this.JAKKamA = (float)d;
        this.jAKKamA = (float)d2;
        this.JakKamA = (float)d3;
        this.jakKamA = (float)d4;
    }

    public maaakmk(mmajmma mmajmma2, float f) {
        this.JAKKamA = mmajmma2.maJakKA;
        this.jAKKamA = mmajmma2.MAJakKA;
        this.JakKamA = mmajmma2.mAJakKA;
        this.jakKamA = f;
    }

    public maaakmk(mmajmma mmajmma2) {
        this.JAKKamA = mmajmma2.maJakKA;
        this.jAKKamA = mmajmma2.MAJakKA;
        this.JakKamA = mmajmma2.mAJakKA;
        this.jakKamA = 0.0f;
    }

    public maaakmk(maaakmk maaakmk2) {
        this.JAKKamA = maaakmk2.JAKKamA;
        this.jAKKamA = maaakmk2.jAKKamA;
        this.JakKamA = maaakmk2.JakKamA;
        this.jakKamA = maaakmk2.jakKamA;
    }

    public maaakmk() {
    }

    public void kKAmaJA(maaakmk maaakmk2) {
        this.JAKKamA = maaakmk2.JAKKamA;
        this.jAKKamA = maaakmk2.jAKKamA;
        this.JakKamA = maaakmk2.JakKamA;
        this.jakKamA = maaakmk2.jakKamA;
    }

    public void kkAMAJA(float f, float f2, float f3, float f4) {
        this.JAKKamA = f;
        this.jAKKamA = f2;
        this.JakKamA = f3;
        this.jakKamA = f4;
    }

    public void KKamaJA(double d, double d2, double d3, double d4) {
        this.JAKKamA = (float)d;
        this.jAKKamA = (float)d2;
        this.JakKamA = (float)d3;
        this.jakKamA = (float)d4;
    }

    public float KkamaJA() {
        float f = (float)Math.sqrt(this.JAKKamA * this.JAKKamA + this.jAKKamA * this.jAKKamA + this.JakKamA * this.JakKamA + this.jakKamA * this.jakKamA);
        if (f == 0.0f) {
            f = 1.0f;
        }
        return f;
    }

    public void KkaMaJA(float f) {
        this.JAKKamA *= f;
        this.jAKKamA *= f;
        this.JakKamA *= f;
        this.jakKamA *= f;
    }

    public void KkaMAJA() {
        float f = 1.0f / this.KkamaJA();
        this.KkaMaJA(f);
    }

    public float KkAmaJA(maaakmk maaakmk2) {
        float f = (this.JAKKamA * maaakmk2.JAKKamA + this.jAKKamA * maaakmk2.jAKKamA + this.JakKamA * maaakmk2.JakKamA + this.jakKamA * maaakmk2.jakKamA) / (this.KkamaJA() * maaakmk2.KkamaJA());
        return f;
    }

    public float KkAMaJA(maaakmk maaakmk2) {
        float f = this.JAKKamA * maaakmk2.JAKKamA + this.jAKKamA * maaakmk2.jAKKamA + this.JakKamA * maaakmk2.JakKamA + this.jakKamA * maaakmk2.jakKamA;
        return f;
    }

    public void KkAMAJA(maaakmk maaakmk2) {
        float f = this.jakKamA * maaakmk2.jakKamA - this.JAKKamA * maaakmk2.JAKKamA - this.jAKKamA * maaakmk2.jAKKamA - this.JakKamA * maaakmk2.JakKamA;
        float f2 = this.jakKamA * maaakmk2.JAKKamA + this.JAKKamA * maaakmk2.jakKamA + this.jAKKamA * maaakmk2.JakKamA - this.JakKamA * maaakmk2.jAKKamA;
        float f3 = this.jakKamA * maaakmk2.jAKKamA + this.jAKKamA * maaakmk2.jakKamA + this.JakKamA * maaakmk2.JAKKamA - this.JAKKamA * maaakmk2.JakKamA;
        float f4 = this.jakKamA * maaakmk2.JakKamA + this.JakKamA * maaakmk2.jakKamA + this.JAKKamA * maaakmk2.jAKKamA - this.jAKKamA * maaakmk2.JAKKamA;
        this.jakKamA = f;
        this.JAKKamA = f2;
        this.jAKKamA = f3;
        this.JakKamA = f4;
    }

    public void kkAMaJA() {
        float f = this.JAKKamA * this.JAKKamA + this.jAKKamA * this.jAKKamA + this.JakKamA * this.JakKamA + this.jakKamA * this.jakKamA;
        f = f != 0.0f ? 1.0f / f : 1.0f;
        this.jakKamA *= f;
        this.JAKKamA = -this.JAKKamA * f;
        this.jAKKamA = -this.jAKKamA * f;
        this.JakKamA = -this.JakKamA * f;
    }

    public void KKaMaJA() {
        float f = 1.0f / this.KkamaJA();
        this.jakKamA *= f;
        this.JAKKamA *= -f;
        this.jAKKamA *= -f;
        this.JakKamA *= -f;
    }

    public void kkaMAJA() {
        double d = Math.sqrt(this.JAKKamA * this.JAKKamA + this.jAKKamA * this.jAKKamA + this.JakKamA * this.JakKamA);
        float f = d > 0.0 ? (float)(Math.sin(d) / d) : 1.0f;
        this.jakKamA = (float)Math.cos(d);
        this.JAKKamA *= f;
        this.jAKKamA *= f;
        this.JakKamA *= f;
    }

    public void kKAMAJA() {
        float f = (float)Math.sqrt(this.JAKKamA * this.JAKKamA + this.jAKKamA * this.jAKKamA + this.JakKamA * this.JakKamA);
        f = this.jakKamA != 0.0f ? (float)Math.atan(f / this.jakKamA) : 1.5707964f;
        this.jakKamA = 0.0f;
        this.JAKKamA *= f;
        this.jAKKamA *= f;
        this.JakKamA *= f;
    }

    public void kKaMAJA(maaakmk maaakmk2) {
        maaakmk maaakmk3 = new maaakmk(this);
        maaakmk3.kkAMaJA();
        maaakmk maaakmk4 = new maaakmk(maaakmk3);
        maaakmk4.KkAMAJA(maaakmk2);
        double d = Math.sqrt(maaakmk4.JAKKamA * maaakmk4.JAKKamA + maaakmk4.jAKKamA * maaakmk4.jAKKamA + maaakmk4.JakKamA * maaakmk4.JakKamA);
        float f = this.JAKKamA * maaakmk2.JAKKamA + this.jAKKamA * maaakmk2.jAKKamA + this.JakKamA * maaakmk2.JakKamA + this.jakKamA * maaakmk2.jakKamA;
        float f2 = f != 0.0f ? (float)Math.atan(d / (double)f) : 1.5707964f;
        if (d != 0.0) {
            f2 /= (float)d;
        }
        this.jakKamA = 0.0f;
        this.JAKKamA = maaakmk4.JAKKamA * f2;
        this.jAKKamA = maaakmk4.jAKKamA * f2;
        this.JakKamA = maaakmk4.JakKamA * f2;
    }

    public void kKamaJA(maajkka maajkka2) {
        float f = this.JAKKamA * this.JAKKamA + this.jAKKamA * this.jAKKamA + this.JakKamA * this.JakKamA + this.jakKamA * this.jakKamA;
        float f2 = f == 0.0f ? 1.0f : 2.0f / f;
        float f3 = this.JAKKamA * f2;
        float f4 = this.jAKKamA * f2;
        float f5 = this.JakKamA * f2;
        float f6 = this.jakKamA * f3;
        float f7 = this.jakKamA * f4;
        float f8 = this.jakKamA * f5;
        float f9 = this.JAKKamA * f3;
        float f10 = this.JAKKamA * f4;
        float f11 = this.JAKKamA * f5;
        float f12 = this.jAKKamA * f4;
        float f13 = this.jAKKamA * f5;
        float f14 = this.JakKamA * f5;
        maajkka2.AMaJakK = 1.0f - (f12 + f14);
        maajkka2.aMaJakK = f10 - f8;
        maajkka2.AmAjAKK = f11 + f7;
        maajkka2.amAjAKK = f10 + f8;
        maajkka2.AMAjAKK = 1.0f - (f9 + f14);
        maajkka2.aMAjAKK = f13 - f6;
        maajkka2.AmajAKK = f11 - f7;
        maajkka2.amajAKK = f13 + f6;
        maajkka2.AMajAKK = 1.0f - (f9 + f12);
    }

    public void kKAMaJA(maajkka maajkka2) {
        float f = this.JAKKamA * this.JAKKamA + this.jAKKamA * this.jAKKamA + this.JakKamA * this.JakKamA + this.jakKamA * this.jakKamA;
        float f2 = f == 0.0f ? 1.0f : 2.0f / f;
        float f3 = this.JAKKamA * f2;
        float f4 = this.jAKKamA * f2;
        float f5 = this.JakKamA * f2;
        float f6 = this.jakKamA * f3;
        float f7 = this.jakKamA * f4;
        float f8 = this.jakKamA * f5;
        float f9 = this.JAKKamA * f3;
        float f10 = this.JAKKamA * f4;
        float f11 = this.JAKKamA * f5;
        float f12 = this.jAKKamA * f4;
        float f13 = this.jAKKamA * f5;
        float f14 = this.JakKamA * f5;
        maajkka2.AMaJakK = 1.0f - (f12 + f14);
        maajkka2.aMaJakK = f10 + f8;
        maajkka2.AmAjAKK = f11 - f7;
        maajkka2.amAjAKK = f10 - f8;
        maajkka2.AMAjAKK = 1.0f - (f9 + f14);
        maajkka2.aMAjAKK = f13 + f6;
        maajkka2.AmajAKK = f11 + f7;
        maajkka2.amajAKK = f13 - f6;
        maajkka2.AMajAKK = 1.0f - (f9 + f12);
    }

    public void KKAmaJA(float f, float f2, float f3, float f4) {
        float f5 = (float)Math.sin(f4 / 2.0f);
        this.JAKKamA = f * f5;
        this.jAKKamA = f2 * f5;
        this.JakKamA = f3 * f5;
        this.jakKamA = (float)Math.cos(f4 / 2.0f);
    }

    public void KKAMAJA(mmajmma mmajmma2, float f) {
        float f2 = (float)Math.sin(f / 2.0f);
        this.JAKKamA = mmajmma2.maJakKA * f2;
        this.jAKKamA = mmajmma2.MAJakKA * f2;
        this.JakKamA = mmajmma2.mAJakKA * f2;
        this.jakKamA = (float)Math.cos(f / 2.0f);
    }

    public void KKaMAJA() {
        float f = (float)Math.sin(this.jakKamA / 2.0f);
        this.JAKKamA *= f;
        this.jAKKamA *= f;
        this.JakKamA *= f;
        this.jakKamA = (float)Math.cos(this.jakKamA / 2.0f);
    }

    public float kkaMaJA(mmajmma mmajmma2) {
        maaakmk maaakmk2 = new maaakmk(this);
        maaakmk2.KkaMAJA();
        double d = Math.acos(maaakmk2.jakKamA);
        float f = (float)Math.sin(d);
        mmajmma2.maJakKA = maaakmk2.JAKKamA / f;
        mmajmma2.MAJakKA = maaakmk2.jAKKamA / f;
        mmajmma2.mAJakKA = maaakmk2.JakKamA / f;
        return 2.0f * (float)d;
    }

    public void KKAMaJA(maajkka maajkka2) {
        float f = maajkka2.AMaJakK + maajkka2.AMAjAKK + maajkka2.AMajAKK;
        if (f > 0.0f) {
            float f2 = (float)Math.sqrt((double)f + 1.0);
            this.jakKamA = 0.5f * f2;
            f2 = 0.5f / f2;
            this.JAKKamA = (maajkka2.amajAKK - maajkka2.aMAjAKK) * f2;
            this.jAKKamA = (maajkka2.AmAjAKK - maajkka2.AmajAKK) * f2;
            this.JakKamA = (maajkka2.amAjAKK - maajkka2.aMaJakK) * f2;
            return;
        }
        if (maajkka2.AMAjAKK > maajkka2.AMaJakK) {
            if (maajkka2.AMajAKK > maajkka2.AMAjAKK) {
                float f3 = (float)Math.sqrt((double)(maajkka2.AMajAKK - (maajkka2.AMaJakK + maajkka2.AMAjAKK)) + 1.0);
                this.JakKamA = f3 * 0.5f;
                if (f3 != 0.0f) {
                    f3 = 0.5f / f3;
                }
                this.jakKamA = (maajkka2.amAjAKK - maajkka2.aMaJakK) * f3;
                this.JAKKamA = (maajkka2.AmAjAKK + maajkka2.AmajAKK) * f3;
                this.jAKKamA = (maajkka2.aMAjAKK + maajkka2.amajAKK) * f3;
                return;
            }
            float f4 = (float)Math.sqrt((double)(maajkka2.AMAjAKK - (maajkka2.AMajAKK + maajkka2.AMaJakK)) + 1.0);
            this.jAKKamA = f4 * 0.5f;
            if (f4 != 0.0f) {
                f4 = 0.5f / f4;
            }
            this.jakKamA = (maajkka2.AmAjAKK - maajkka2.AmajAKK) * f4;
            this.JakKamA = (maajkka2.amajAKK + maajkka2.aMAjAKK) * f4;
            this.JAKKamA = (maajkka2.aMaJakK + maajkka2.amAjAKK) * f4;
            return;
        }
        if (maajkka2.AMajAKK > maajkka2.AMaJakK) {
            float f5 = (float)Math.sqrt((double)(maajkka2.AMajAKK - (maajkka2.AMaJakK + maajkka2.AMAjAKK)) + 1.0);
            this.JakKamA = f5 * 0.5f;
            if (f5 != 0.0f) {
                f5 = 0.5f / f5;
            }
            this.jakKamA = (maajkka2.amAjAKK - maajkka2.aMaJakK) * f5;
            this.JAKKamA = (maajkka2.AmAjAKK + maajkka2.AmajAKK) * f5;
            this.jAKKamA = (maajkka2.aMAjAKK + maajkka2.amajAKK) * f5;
            return;
        }
        float f6 = (float)Math.sqrt((double)(maajkka2.AMaJakK - (maajkka2.AMAjAKK + maajkka2.AMajAKK)) + 1.0);
        this.JAKKamA = f6 * 0.5f;
        if (f6 != 0.0f) {
            f6 = 0.5f / f6;
        }
        this.jakKamA = (maajkka2.amajAKK - maajkka2.aMAjAKK) * f6;
        this.jAKKamA = (maajkka2.amAjAKK + maajkka2.aMaJakK) * f6;
        this.JakKamA = (maajkka2.AmajAKK + maajkka2.AmAjAKK) * f6;
    }

    public void kkamaJA(maaakmk maaakmk2, maaakmk maaakmk3, float f, float f2) {
        float f3;
        float f4;
        float f5;
        double d = maaakmk2.KkAMaJA(maaakmk3);
        if (d < 0.0) {
            d = -d;
            f5 = -1.0f;
        } else {
            f5 = 1.0f;
        }
        if (1.0 - d < 1.0E-6) {
            f4 = 1.0f - f;
            f3 = f;
        } else {
            double d2 = Math.acos(d);
            double d3 = Math.sin(d2);
            double d4 = d2 + (double)f2 * Math.PI;
            f4 = (float)(Math.sin(d2 - (double)f * d4) / d3);
            f3 = (float)(Math.sin((double)f * d4) / d3);
        }
        this.JAKKamA = f4 * maaakmk2.JAKKamA + (f3 *= f5) * maaakmk3.JAKKamA;
        this.jAKKamA = f4 * maaakmk2.jAKKamA + f3 * maaakmk3.jAKKamA;
        this.JakKamA = f4 * maaakmk2.JakKamA + f3 * maaakmk3.JakKamA;
        this.jakKamA = f4 * maaakmk2.jakKamA + f3 * maaakmk3.jakKamA;
    }

    public void kkAmaJA(maaakmk maaakmk2, maaakmk maaakmk3, float f, float f2) {
        float f3;
        float f4;
        double d = maaakmk2.KkAMaJA(maaakmk3);
        if (1.0 - Math.abs(d) < 1.0E-6) {
            f4 = 1.0f - f;
            f3 = f;
        } else {
            double d2 = Math.acos(d);
            double d3 = Math.sin(d2);
            double d4 = d2 + (double)f2 * Math.PI;
            f4 = (float)(Math.sin(d2 - (double)f * d4) / d3);
            f3 = (float)(Math.sin((double)f * d4) / d3);
        }
        this.JAKKamA = f4 * maaakmk2.JAKKamA + f3 * maaakmk3.JAKKamA;
        this.jAKKamA = f4 * maaakmk2.jAKKamA + f3 * maaakmk3.jAKKamA;
        this.JakKamA = f4 * maaakmk2.JakKamA + f3 * maaakmk3.JakKamA;
        this.jakKamA = f4 * maaakmk2.jakKamA + f3 * maaakmk3.jakKamA;
    }

    public String toString() {
        return "quat (" + this.JAKKamA + " ," + this.jAKKamA + " ," + this.JakKamA + " / " + this.jakKamA + ")";
    }
}
