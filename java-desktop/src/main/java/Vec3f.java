/*
 * Decompiled with CFR 0.152.
 */
public class Vec3f {
    public static final Vec3f majAKKA = new Vec3f(0.0f, 0.0f, 0.0f);
    public static final Vec3f MAjAKKA = new Vec3f(1.0f, 0.0f, 0.0f);
    public static final Vec3f mAjAKKA = new Vec3f(0.0f, 1.0f, 0.0f);
    public static final Vec3f MaJakKA = new Vec3f(0.0f, 0.0f, 1.0f);
    public float maJakKA;
    public float MAJakKA;
    public float mAJakKA;

    public Vec3f() {
    }

    public Vec3f(Vec3f mmajmma2) {
        this.maJakKA = mmajmma2.maJakKA;
        this.MAJakKA = mmajmma2.MAJakKA;
        this.mAJakKA = mmajmma2.mAJakKA;
    }

    public Vec3f(float f, float f2, float f3) {
        this.maJakKA = f;
        this.MAJakKA = f2;
        this.mAJakKA = f3;
    }

    public Vec3f(double d, double d2, double d3) {
        this.maJakKA = (float)d;
        this.MAJakKA = (float)d2;
        this.mAJakKA = (float)d3;
    }

    public final void mAjakKA(Vec3f mmajmma2) {
        this.maJakKA = mmajmma2.maJakKA;
        this.MAJakKA = mmajmma2.MAJakKA;
        this.mAJakKA = mmajmma2.mAJakKA;
    }

    public final void MaJaKka(float f, float f2, float f3) {
        this.maJakKA = f;
        this.MAJakKA = f2;
        this.mAJakKA = f3;
    }

    public final void maJAkKA(double d, double d2, double d3) {
        this.maJakKA = (float)d;
        this.MAJakKA = (float)d2;
        this.mAJakKA = (float)d3;
    }

    public final void MaJAKKA(float f) {
        this.maJakKA = f;
        this.MAJakKA = f;
        this.mAJakKA = f;
    }

    public final void MajaKka(double d) {
        this.maJakKA = (float)d;
        this.MAJakKA = (float)d;
        this.mAJakKA = (float)d;
    }

    public final float majAKKA(Vec3f mmajmma2) {
        return this.maJakKA * mmajmma2.maJakKA + this.MAJakKA * mmajmma2.MAJakKA + this.mAJakKA * mmajmma2.mAJakKA;
    }

    public final float MAjaKka() {
        return (float)Math.sqrt(this.maJakKA * this.maJakKA + this.MAJakKA * this.MAJakKA + this.mAJakKA * this.mAJakKA);
    }

    public final float MAjaKKA() {
        return this.maJakKA * this.maJakKA + this.MAJakKA * this.MAJakKA + this.mAJakKA * this.mAJakKA;
    }

    public final float mAjAkKA() {
        return (float)Math.atan2(this.mAJakKA, this.MAJakKA);
    }

    public final float MAjAkKA() {
        return (float)Math.atan2(this.maJakKA, this.mAJakKA);
    }

    public final float mAJAkKA() {
        return (float)Math.atan2(this.MAJakKA, this.maJakKA);
    }

    public final void mAjaKKA() {
        this.maJakKA = -this.maJakKA;
        this.MAJakKA = -this.MAJakKA;
        this.mAJakKA = -this.mAJakKA;
    }

    public final void majAkKA(Vec3f mmajmma2) {
        this.maJakKA += mmajmma2.maJakKA;
        this.MAJakKA += mmajmma2.MAJakKA;
        this.mAJakKA += mmajmma2.mAJakKA;
    }

    public final void maJAKKA(Vec3f mmajmma2) {
        this.maJakKA -= mmajmma2.maJakKA;
        this.MAJakKA -= mmajmma2.MAJakKA;
        this.mAJakKA -= mmajmma2.mAJakKA;
    }

    public final void maJaKKA(float f, float f2, float f3) {
        this.maJakKA += f;
        this.MAJakKA += f2;
        this.mAJakKA += f3;
    }

    public final void mAJakKA(float f, float f2, float f3) {
        this.maJakKA -= f;
        this.MAJakKA -= f2;
        this.mAJakKA -= f3;
    }

    public final void MajakKA(float f) {
        this.maJakKA *= f;
        this.MAJakKA *= f;
        this.mAJakKA *= f;
    }

    public final void MaJaKKA(float f) {
        this.maJakKA /= f;
        this.MAJakKA /= f;
        this.mAJakKA /= f;
    }

    public final void mAjaKka(Vec3f mmajmma2) {
        this.maJakKA *= mmajmma2.maJakKA;
        this.MAJakKA *= mmajmma2.MAJakKA;
        this.mAJakKA *= mmajmma2.mAJakKA;
    }

    public final void MajAKKA(float f, float f2, float f3) {
        this.maJakKA *= f;
        this.MAJakKA *= f2;
        this.mAJakKA *= f3;
    }

    public final void majakKA(Vec3f mmajmma2) {
        float f = this.MAJakKA * mmajmma2.mAJakKA - this.mAJakKA * mmajmma2.MAJakKA;
        float f2 = this.mAJakKA * mmajmma2.maJakKA - this.maJakKA * mmajmma2.mAJakKA;
        float f3 = this.maJakKA * mmajmma2.MAJakKA - this.MAJakKA * mmajmma2.maJakKA;
        this.maJakKA = f;
        this.MAJakKA = f2;
        this.mAJakKA = f3;
    }

    public final void majaKKA() {
        float f = (float)Math.sqrt(this.maJakKA * this.maJakKA + this.MAJakKA * this.MAJakKA + this.mAJakKA * this.mAJakKA);
        this.maJakKA /= f;
        this.MAJakKA /= f;
        this.mAJakKA /= f;
    }

    public final void MAJAKKA(float f) {
        float f2 = this.MAJakKA * (float)Math.cos(f) - (float)Math.sin(f) * this.mAJakKA;
        float f3 = this.mAJakKA * (float)Math.cos(f) + (float)Math.sin(f) * this.MAJakKA;
        this.MAJakKA = f2;
        this.mAJakKA = f3;
    }

    public final void maJaKka(float f) {
        float f2 = this.maJakKA * (float)Math.cos(f) + (float)Math.sin(f) * this.mAJakKA;
        float f3 = this.mAJakKA * (float)Math.cos(f) - (float)Math.sin(f) * this.maJakKA;
        this.maJakKA = f2;
        this.mAJakKA = f3;
    }

    public final void MAJakKA(float f) {
        float f2 = this.maJakKA * (float)Math.cos(f) - (float)Math.sin(f) * this.MAJakKA;
        float f3 = this.MAJakKA * (float)Math.cos(f) + (float)Math.sin(f) * this.maJakKA;
        this.maJakKA = f2;
        this.MAJakKA = f3;
    }

    public final void MAJAkKA(Vec3f mmajmma2) {
        this.MaJAkKA(mmajmma2.maJakKA, mmajmma2.MAJakKA, mmajmma2.mAJakKA);
    }

    public final void mAjAkkA(float f, float f2, float f3) {
        this.MAJaKKA(f);
        this.MajaKKA(f2);
        this.mAJaKKA(f3);
    }

    public final Vec3f MajAkKA(Vec3f mmajmma2) {
        Vec3f mmajmma3 = new Vec3f(this);
        mmajmma3.majAkKA(mmajmma2);
        return mmajmma3;
    }

    public final Vec3f mAjAKKA(Vec3f mmajmma2) {
        Vec3f mmajmma3 = new Vec3f(this);
        mmajmma3.maJAKKA(mmajmma2);
        return mmajmma3;
    }

    public final Vec3f MaJakKA(float f, float f2, float f3) {
        return new Vec3f(this.maJakKA + f, this.MAJakKA + f2, this.mAJakKA + f3);
    }

    public final Vec3f mAJaKka(float f, float f2, float f3) {
        return new Vec3f(this.maJakKA - f, this.MAJakKA - f2, this.mAJakKA - f3);
    }

    public final Vec3f mAJAKKA() {
        return new Vec3f(-this.maJakKA, -this.MAJakKA, -this.mAJakKA);
    }

    public final Vec3f maJakKA(float f) {
        Vec3f mmajmma2 = new Vec3f(this);
        mmajmma2.MajakKA(f);
        return mmajmma2;
    }

    public final Vec3f majaKka(float f) {
        Vec3f mmajmma2 = new Vec3f(this);
        mmajmma2.MaJaKKA(f);
        return mmajmma2;
    }

    public final Vec3f MAjakKA(Vec3f mmajmma2) {
        Vec3f mmajmma3 = new Vec3f(this);
        mmajmma3.mAjaKka(mmajmma2);
        return mmajmma3;
    }

    public final Vec3f MAJaKka(Vec3f mmajmma2) {
        Vec3f mmajmma3 = new Vec3f(this);
        mmajmma3.majakKA(mmajmma2);
        return mmajmma3;
    }

    public final Vec3f MAjAKKA() {
        Vec3f mmajmma2 = new Vec3f(this);
        mmajmma2.majaKKA();
        return mmajmma2;
    }

    public final Vec3f MajaKKA(float f) {
        Vec3f mmajmma2 = new Vec3f(this);
        mmajmma2.MAJAKKA(f);
        return mmajmma2;
    }

    public final Vec3f mAJaKKA(float f) {
        Vec3f mmajmma2 = new Vec3f(this);
        mmajmma2.maJaKka(f);
        return mmajmma2;
    }

    public final Vec3f MAJaKKA(float f) {
        Vec3f mmajmma2 = new Vec3f(this);
        mmajmma2.MAJakKA(f);
        return mmajmma2;
    }

    public final Vec3f MAjAkkA(Vec3f mmajmma2) {
        Vec3f mmajmma3 = new Vec3f(this);
        mmajmma3.MAJakKA(mmajmma2.maJakKA);
        mmajmma3.MAJAKKA(mmajmma2.MAJakKA);
        mmajmma3.maJaKka(mmajmma2.mAJakKA);
        return mmajmma3;
    }

    public final Vec3f MaJAkKA(float f, float f2, float f3) {
        Vec3f mmajmma2 = new Vec3f(this);
        mmajmma2.MAJakKA(f);
        mmajmma2.MAJAKKA(f2);
        mmajmma2.maJaKka(f3);
        return mmajmma2;
    }

    public final String toString() {
        return "Vector3f:{" + this.maJakKA + "; " + this.MAJakKA + "; " + this.mAJakKA + "} ";
    }
}
