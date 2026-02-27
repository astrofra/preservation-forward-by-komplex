/*
 * Decompiled with CFR 0.152.
 */
public class maajkka {
    public float AMaJakK;
    public float aMaJakK;
    public float AmAjAKK;
    public float amAjAKK;
    public float AMAjAKK;
    public float aMAjAKK;
    public float AmajAKK;
    public float amajAKK;
    public float AMajAKK;

    public maajkka() {
        this.aMAjaKk();
    }

    public maajkka(mmajmma mmajmma2, mmajmma mmajmma3, mmajmma mmajmma4) {
        this.AmaJaKk(mmajmma2, mmajmma3, mmajmma4);
    }

    public maajkka(mmajmma mmajmma2) {
        this.AmajaKk(mmajmma2);
    }

    public maajkka(maajkka maajkka2) {
        this.aMajaKk(maajkka2);
    }

    public final void aMAjaKk() {
        this.AMaJakK = 1.0f;
        this.aMaJakK = 0.0f;
        this.AmAjAKK = 0.0f;
        this.amAjAKK = 0.0f;
        this.AMAjAKK = 1.0f;
        this.aMAjAKK = 0.0f;
        this.AmajAKK = 0.0f;
        this.amajAKK = 0.0f;
        this.AMajAKK = 1.0f;
    }

    public final void AmaJaKk(mmajmma mmajmma2, mmajmma mmajmma3, mmajmma mmajmma4) {
        this.aMAjAKk(mmajmma2);
        this.amAjAKk(mmajmma3);
        this.AmAjAKk(mmajmma4);
    }

    public final void AmajaKk(mmajmma mmajmma2) {
        this.aMAJaKk(mmajmma2.maJakKA, mmajmma2.MAJakKA, mmajmma2.mAJakKA);
    }

    public final void aMAJaKk(float f, float f2, float f3) {
        mmajmma mmajmma2 = new mmajmma();
        mmajmma2.mAjakKA(mmajmma.MAjAKKA);
        mmajmma2.mAjAkkA(f, f2, f3);
        this.aMAjAKk(mmajmma2);
        mmajmma2.mAjakKA(mmajmma.mAjAKKA);
        mmajmma2.mAjAkkA(f, f2, f3);
        this.amAjAKk(mmajmma2);
        mmajmma2.mAjakKA(mmajmma.MaJakKA);
        mmajmma2.mAjAkkA(f, f2, f3);
        this.AmAjAKk(mmajmma2);
        mmajmma2 = null;
    }

    public final void aMAjAKk(mmajmma mmajmma2) {
        this.amAjaKk(mmajmma2.maJakKA, mmajmma2.MAJakKA, mmajmma2.mAJakKA);
    }

    public final void amAjaKk(float f, float f2, float f3) {
        this.AMaJakK = f;
        this.aMaJakK = f2;
        this.AmAjAKK = f3;
    }

    public final void amAjAKk(mmajmma mmajmma2) {
        this.AmAjaKk(mmajmma2.maJakKA, mmajmma2.MAJakKA, mmajmma2.mAJakKA);
    }

    public final void AmAjaKk(float f, float f2, float f3) {
        this.amAjAKK = f;
        this.AMAjAKK = f2;
        this.aMAjAKK = f3;
    }

    public final void AmAjAKk(mmajmma mmajmma2) {
        this.aMaJAKk(mmajmma2.maJakKA, mmajmma2.MAJakKA, mmajmma2.mAJakKA);
    }

    public final void aMaJAKk(float f, float f2, float f3) {
        this.AmajAKK = f;
        this.amajAKK = f2;
        this.AMajAKK = f3;
    }

    public final void aMajaKk(maajkka maajkka2) {
        this.AMaJakK = maajkka2.AMaJakK;
        this.aMaJakK = maajkka2.aMaJakK;
        this.AmAjAKK = maajkka2.AmAjAKK;
        this.amAjAKK = maajkka2.amAjAKK;
        this.AMAjAKK = maajkka2.AMAjAKK;
        this.aMAjAKK = maajkka2.aMAjAKK;
        this.AmajAKK = maajkka2.AmajAKK;
        this.amajAKK = maajkka2.amajAKK;
        this.AMajAKK = maajkka2.AMajAKK;
    }

    public final void AMAJaKk(mmajmma mmajmma2) {
        mmajmma2.MaJaKka(this.AMaJakK, this.aMaJakK, this.AmAjAKK);
    }

    public final void AMAJAKk(mmajmma mmajmma2) {
        mmajmma2.MaJaKka(this.amAjAKK, this.AMAjAKK, this.aMAjAKK);
    }

    public final void KAMaJaK(mmajmma mmajmma2) {
        mmajmma2.MaJaKka(this.AmajAKK, this.amajAKK, this.AMajAKK);
    }

    public final mmajmma kaMaJaK() {
        mmajmma mmajmma2 = new mmajmma(this.AMaJakK, this.aMaJakK, this.AmAjAKK);
        return mmajmma2;
    }

    public final mmajmma KaMaJaK() {
        mmajmma mmajmma2 = new mmajmma(this.amAjAKK, this.AMAjAKK, this.aMAjAKK);
        return mmajmma2;
    }

    public final mmajmma AMaJaKk() {
        mmajmma mmajmma2 = new mmajmma(this.AmajAKK, this.amajAKK, this.AMajAKK);
        return mmajmma2;
    }

    public final void amajaKk(float f) {
        this.AMaJakK *= f;
        this.aMaJakK *= f;
        this.AmAjAKK *= f;
        this.amAjAKK *= f;
        this.AMAjAKK *= f;
        this.aMAjAKK *= f;
        this.AmajAKK *= f;
        this.amajAKK *= f;
        this.AMajAKK *= f;
    }

    public final maajkka AMajaKk(float f) {
        maajkka maajkka2 = new maajkka(this);
        maajkka2.amajaKk(f);
        return maajkka2;
    }

    public final void amaJaKk(mmajmma mmajmma2) {
        float f = mmajmma2.maJakKA;
        float f2 = mmajmma2.MAJakKA;
        float f3 = mmajmma2.mAJakKA;
        mmajmma2.maJakKA = this.AMaJakK * f + this.amAjAKK * f2 + this.AmajAKK * f3;
        mmajmma2.MAJakKA = this.aMaJakK * f + this.AMAjAKK * f2 + this.amajAKK * f3;
        mmajmma2.mAJakKA = this.AmAjAKK * f + this.aMAjAKK * f2 + this.AMajAKK * f3;
    }

    public final mmajmma AmAJaKk(mmajmma mmajmma2) {
        mmajmma mmajmma3 = new mmajmma(mmajmma2);
        this.amaJaKk(mmajmma3);
        return mmajmma3;
    }

    public final void AMajAKk(maajkka maajkka2) {
        float f = this.AMaJakK * maajkka2.AMaJakK + this.aMaJakK * maajkka2.amAjAKK + this.AmAjAKK * maajkka2.AmajAKK;
        float f2 = this.AMaJakK * maajkka2.aMaJakK + this.aMaJakK * maajkka2.AMAjAKK + this.AmAjAKK * maajkka2.amajAKK;
        float f3 = this.AMaJakK * maajkka2.AmAjAKK + this.aMaJakK * maajkka2.aMAjAKK + this.AmAjAKK * maajkka2.AMajAKK;
        float f4 = this.amAjAKK * maajkka2.AMaJakK + this.AMAjAKK * maajkka2.amAjAKK + this.aMAjAKK * maajkka2.AmajAKK;
        float f5 = this.amAjAKK * maajkka2.aMaJakK + this.AMAjAKK * maajkka2.AMAjAKK + this.aMAjAKK * maajkka2.amajAKK;
        float f6 = this.amAjAKK * maajkka2.AmAjAKK + this.AMAjAKK * maajkka2.aMAjAKK + this.aMAjAKK * maajkka2.AMajAKK;
        float f7 = this.AmajAKK * maajkka2.AMaJakK + this.amajAKK * maajkka2.amAjAKK + this.AMajAKK * maajkka2.AmajAKK;
        float f8 = this.AmajAKK * maajkka2.aMaJakK + this.amajAKK * maajkka2.AMAjAKK + this.AMajAKK * maajkka2.amajAKK;
        float f9 = this.AmajAKK * maajkka2.AmAjAKK + this.amajAKK * maajkka2.aMAjAKK + this.AMajAKK * maajkka2.AMajAKK;
        this.AMaJakK = f;
        this.aMaJakK = f2;
        this.AmAjAKK = f3;
        this.amAjAKK = f4;
        this.AMAjAKK = f5;
        this.aMAjAKK = f6;
        this.AmajAKK = f7;
        this.amajAKK = f8;
        this.AMajAKK = f9;
    }

    public final maajkka amAJaKk(maajkka maajkka2) {
        maajkka maajkka3 = new maajkka(this);
        maajkka3.AMajAKk(maajkka2);
        return maajkka3;
    }

    public final void amaJAKk() {
        float f = this.amAjAKK;
        float f2 = this.AmajAKK;
        float f3 = this.aMaJakK;
        float f4 = this.amajAKK;
        float f5 = this.AmAjAKK;
        float f6 = this.aMAjAKK;
        this.aMaJakK = f;
        this.AmAjAKK = f2;
        this.amAjAKK = f3;
        this.aMAjAKK = f4;
        this.AmajAKK = f5;
        this.amajAKK = f6;
    }

    public final maajkka AmajAKk() {
        maajkka maajkka2 = new maajkka();
        maajkka2.AMaJakK = this.AMaJakK;
        maajkka2.aMaJakK = this.amAjAKK;
        maajkka2.AmAjAKK = this.AmajAKK;
        maajkka2.amAjAKK = this.aMaJakK;
        maajkka2.AMAjAKK = this.AMAjAKK;
        maajkka2.aMAjAKK = this.amajAKK;
        maajkka2.AmajAKK = this.AmAjAKK;
        maajkka2.amajAKK = this.aMAjAKK;
        maajkka2.AMajAKK = this.AMajAKK;
        return maajkka2;
    }

    public final void amAJAKk(float f) {
        float f2 = (float)Math.sin(f);
        float f3 = (float)Math.cos(f);
        float f4 = this.aMaJakK * f3 - f2 * this.AmAjAKK;
        float f5 = this.AmAjAKK * f3 + f2 * this.aMaJakK;
        this.aMaJakK = f4;
        this.AmAjAKK = f5;
        f4 = this.AMAjAKK * f3 - f2 * this.aMAjAKK;
        f5 = this.aMAjAKK * f3 + f2 * this.AMAjAKK;
        this.AMAjAKK = f4;
        this.aMAjAKK = f5;
        f4 = this.amajAKK * f3 - f2 * this.AMajAKK;
        f5 = this.AMajAKK * f3 + f2 * this.amajAKK;
        this.amajAKK = f4;
        this.AMajAKK = f5;
    }

    public final void aMaJaKk(float f) {
        float f2 = (float)Math.sin(f);
        float f3 = (float)Math.cos(f);
        float f4 = this.AMaJakK * f3 + f2 * this.AmAjAKK;
        float f5 = this.AmAjAKK * f3 - f2 * this.AMaJakK;
        this.AMaJakK = f4;
        this.AmAjAKK = f5;
        f4 = this.amAjAKK * f3 + f2 * this.aMAjAKK;
        f5 = this.aMAjAKK * f3 - f2 * this.amAjAKK;
        this.amAjAKK = f4;
        this.aMAjAKK = f5;
        f4 = this.AmajAKK * f3 + f2 * this.AMajAKK;
        f5 = this.AMajAKK * f3 - f2 * this.AmajAKK;
        this.AmajAKK = f4;
        this.AMajAKK = f5;
    }

    public final void AMaJAKk(float f) {
        float f2 = (float)Math.sin(f);
        float f3 = (float)Math.cos(f);
        float f4 = this.AMaJakK * f3 - f2 * this.aMaJakK;
        float f5 = this.aMaJakK * f3 + f2 * this.AMaJakK;
        this.AMaJakK = f4;
        this.aMaJakK = f5;
        f4 = this.amAjAKK * f3 - f2 * this.AMAjAKK;
        f5 = this.AMAjAKK * f3 + f2 * this.amAjAKK;
        this.amAjAKK = f4;
        this.AMAjAKK = f5;
        f4 = this.AmajAKK * f3 - f2 * this.amajAKK;
        f5 = this.amajAKK * f3 + f2 * this.AmajAKK;
        this.AmajAKK = f4;
        this.amajAKK = f5;
    }

    public maajkka AmAJAKk(float f) {
        maajkka maajkka2 = new maajkka(this);
        maajkka2.amAJAKk(f);
        return maajkka2;
    }

    public maajkka aMajAKk(float f) {
        maajkka maajkka2 = new maajkka(this);
        maajkka2.aMaJaKk(f);
        return maajkka2;
    }

    public maajkka amajAKk(float f) {
        maajkka maajkka2 = new maajkka(this);
        maajkka2.AMaJAKk(f);
        return maajkka2;
    }

    public void AMAjAKk() {
        float f = (float)Math.sqrt(this.AMaJakK * this.AMaJakK + this.aMaJakK * this.aMaJakK + this.AmAjAKK * this.AmAjAKK);
        this.AMaJakK /= f;
        this.aMaJakK /= f;
        this.AmAjAKK /= f;
        f = (float)Math.sqrt(this.amAjAKK * this.amAjAKK + this.AMAjAKK * this.AMAjAKK + this.aMAjAKK * this.aMAjAKK);
        this.amAjAKK /= f;
        this.AMAjAKK /= f;
        this.aMAjAKK /= f;
        f = (float)Math.sqrt(this.AmajAKK * this.AmajAKK + this.amajAKK * this.amajAKK + this.AMajAKK * this.AMajAKK);
        this.AmajAKK /= f;
        this.amajAKK /= f;
        this.AMajAKK /= f;
    }

    public maajkka aMaJakk() {
        maajkka maajkka2 = new maajkka(this);
        maajkka2.aMaJakk();
        return maajkka2;
    }

    public void AmaJAKk(mmajmma mmajmma2) {
        this.AMaJakK *= mmajmma2.maJakKA;
        this.aMaJakK *= mmajmma2.maJakKA;
        this.AmAjAKK *= mmajmma2.maJakKA;
        this.amAjAKK *= mmajmma2.MAJakKA;
        this.AMAjAKK *= mmajmma2.MAJakKA;
        this.aMAjAKK *= mmajmma2.MAJakKA;
        this.AmajAKK *= mmajmma2.mAJakKA;
        this.amajAKK *= mmajmma2.mAJakKA;
        this.AMajAKK *= mmajmma2.mAJakKA;
    }

    public void aMAJAKk(mmajmma mmajmma2) {
        this.AMaJakK *= mmajmma2.maJakKA;
        this.amAjAKK *= mmajmma2.maJakKA;
        this.AmajAKK *= mmajmma2.maJakKA;
        this.aMaJakK *= mmajmma2.MAJakKA;
        this.AMAjAKK *= mmajmma2.MAJakKA;
        this.amajAKK *= mmajmma2.MAJakKA;
        this.AmAjAKK *= mmajmma2.mAJakKA;
        this.aMAjAKK *= mmajmma2.mAJakKA;
        this.AMajAKK *= mmajmma2.mAJakKA;
    }

    public maajkka kAMaJaK(mmajmma mmajmma2) {
        maajkka maajkka2 = new maajkka(this);
        maajkka2.AmaJAKk(mmajmma2);
        return maajkka2;
    }

    public maajkka AMAjaKk(mmajmma mmajmma2) {
        maajkka maajkka2 = new maajkka(this);
        maajkka2.aMAJAKk(mmajmma2);
        return maajkka2;
    }

    public String toString() {
        double d = 10000.0;
        String string = "";
        string = String.valueOf(string) + (double)((int)((double)this.AMaJakK * d)) / d + "\t" + (double)((int)((double)this.aMaJakK * d)) / d + "\t" + (double)((int)((double)this.AmAjAKK * d)) / d + "\t \\ X\n";
        string = String.valueOf(string) + (double)((int)((double)this.amAjAKK * d)) / d + "\t" + (double)((int)((double)this.AMAjAKK * d)) / d + "\t" + (double)((int)((double)this.aMAjAKK * d)) / d + "\t \\ Y\n";
        string = String.valueOf(string) + (double)((int)((double)this.AmajAKK * d)) / d + "\t" + (double)((int)((double)this.amajAKK * d)) / d + "\t" + (double)((int)((double)this.AMajAKK * d)) / d + "\t \\ Z\n";
        return string;
    }
}
