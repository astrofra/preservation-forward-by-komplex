/*
 * Decompiled with CFR 0.152.
 */
public class Vec3d {
    static final public Vec3d MajakKA = new Vec3d(0.0, 0.0, 0.0);
    static final public Vec3d majakKA = new Vec3d(1.0, 0.0, 0.0);
    static final public Vec3d MAjakKA = new Vec3d(0.0, 1.0, 0.0);
    static final public Vec3d mAjakKA = new Vec3d(0.0, 0.0, 1.0);
    public double MaJAkKA;
    public double maJAkKA;
    public double MAJAkKA;

    public Vec3d() {
    }

    public Vec3d(Vec3d kmajmma2) {
        this.MaJAkKA = kmajmma2.MaJAkKA;
        this.maJAkKA = kmajmma2.maJAkKA;
        this.MAJAkKA = kmajmma2.MAJAkKA;
    }

    public Vec3d(double d, double d2, double d3) {
        this.MaJAkKA = d;
        this.maJAkKA = d2;
        this.MAJAkKA = d3;
    }

    public final void MajAKKa(Vec3d kmajmma2) {
        this.MaJAkKA = kmajmma2.MaJAkKA;
        this.maJAkKA = kmajmma2.maJAkKA;
        this.MAJAkKA = kmajmma2.MAJAkKA;
    }

    public final void MAJaKKa(double d, double d2, double d3) {
        this.MaJAkKA = d;
        this.maJAkKA = d2;
        this.MAJAkKA = d3;
    }

    public final void MAjAKKa(double d) {
        this.MaJAkKA = d;
        this.maJAkKA = d;
        this.MAJAkKA = d;
    }

    public final double MajaKKa(Vec3d kmajmma2) {
        return this.MaJAkKA * kmajmma2.MaJAkKA + this.maJAkKA * kmajmma2.maJAkKA + this.MAJAkKA * kmajmma2.MAJAkKA;
    }

    public final double majAkka() {
        return Math.sqrt(this.MaJAkKA * this.MaJAkKA + this.maJAkKA * this.maJAkKA + this.MAJAkKA * this.MAJAkKA);
    }

    public final double majaKKa() {
        return this.MaJAkKA * this.MaJAkKA + this.maJAkKA * this.maJAkKA + this.MAJAkKA * this.MAJAkKA;
    }

    public final double mAjakka() {
        return Math.atan2(this.MAJAkKA, this.maJAkKA);
    }

    public final double MAjakka() {
        return Math.atan2(this.MaJAkKA, this.MAJAkKA);
    }

    public final double majakka() {
        return Math.atan2(this.maJAkKA, this.MaJAkKA);
    }

    public final void majAKka() {
        this.MaJAkKA = -this.MaJAkKA;
        this.maJAkKA = -this.maJAkKA;
        this.MAJAkKA = -this.MAJAkKA;
    }

    public final void MAjAKka(Vec3d kmajmma2) {
        this.MaJAkKA += kmajmma2.MaJAkKA;
        this.maJAkKA += kmajmma2.maJAkKA;
        this.MAJAkKA += kmajmma2.MAJAkKA;
    }

    public final void mAjAkka(Vec3d kmajmma2) {
        this.MaJAkKA -= kmajmma2.MaJAkKA;
        this.maJAkKA -= kmajmma2.maJAkKA;
        this.MAJAkKA -= kmajmma2.MAJAkKA;
    }

    public final void mAJAKKa(double d, double d2, double d3) {
        this.MaJAkKA += d;
        this.maJAkKA += d2;
        this.MAJAkKA += d3;
    }

    public final void MaJAkka(double d, double d2, double d3) {
        this.MaJAkKA -= d;
        this.maJAkKA -= d2;
        this.MAJAkKA -= d3;
    }

    public final void maJakka(double d) {
        this.MaJAkKA *= d;
        this.maJAkKA *= d;
        this.MAJAkKA *= d;
    }

    public final void mAjaKKa(double d) {
        this.MaJAkKA /= d;
        this.maJAkKA /= d;
        this.MAJAkKA /= d;
    }

    public final void maJAkka(Vec3d kmajmma2) {
        this.MaJAkKA *= kmajmma2.MaJAkKA;
        this.maJAkKA *= kmajmma2.maJAkKA;
        this.MAJAkKA *= kmajmma2.MAJAkKA;
    }

    public final void MAJAKka(double d, double d2, double d3) {
        this.MaJAkKA *= d;
        this.maJAkKA *= d2;
        this.MAJAkKA *= d3;
    }

    public final void MAJAKKa(Vec3d kmajmma2) {
        double d = this.maJAkKA * kmajmma2.MAJAkKA - this.MAJAkKA * kmajmma2.maJAkKA;
        double d2 = this.MAJAkKA * kmajmma2.MaJAkKA - this.MaJAkKA * kmajmma2.MAJAkKA;
        double d3 = this.MaJAkKA * kmajmma2.maJAkKA - this.maJAkKA * kmajmma2.MaJAkKA;
        this.MaJAkKA = d;
        this.maJAkKA = d2;
        this.MAJAkKA = d3;
    }

    public final void MajAKka() {
        double d = Math.sqrt(this.MaJAkKA * this.MaJAkKA + this.maJAkKA * this.maJAkKA + this.MAJAkKA * this.MAJAkKA);
        this.MaJAkKA /= d;
        this.maJAkKA /= d;
        this.MAJAkKA /= d;
    }

    public final void maJAKka(double d) {
        double d2 = this.maJAkKA * Math.cos(d) - Math.sin(d) * this.MAJAkKA;
        double d3 = this.MAJAkKA * Math.cos(d) + Math.sin(d) * this.maJAkKA;
        this.maJAkKA = d2;
        this.MAJAkKA = d3;
    }

    public final void MAjAkka(double d) {
        double d2 = this.MaJAkKA * Math.cos(d) + Math.sin(d) * this.MAJAkKA;
        double d3 = this.MAJAkKA * Math.cos(d) - Math.sin(d) * this.MaJAkKA;
        this.MaJAkKA = d2;
        this.MAJAkKA = d3;
    }

    public final void mAjAKka(double d) {
        double d2 = this.MaJAkKA * Math.cos(d) - Math.sin(d) * this.maJAkKA;
        double d3 = this.maJAkKA * Math.cos(d) + Math.sin(d) * this.MaJAkKA;
        this.MaJAkKA = d2;
        this.maJAkKA = d3;
    }

    public final void mAJAKka(Vec3d kmajmma2) {
        this.MaJakka(kmajmma2.MaJAkKA, kmajmma2.maJAkKA, kmajmma2.MAJAkKA);
    }

    public final void maJAKKa(double d, double d2, double d3) {
        this.MaJaKKa(d);
        this.mAJaKKa(d2);
        this.maJaKKa(d3);
    }

    public final Vec3d MAjaKKa(Vec3d kmajmma2) {
        Vec3d kmajmma3 = new Vec3d(this);
        kmajmma3.MAjAKka(kmajmma2);
        return kmajmma3;
    }

    public final Vec3d Majakka(Vec3d kmajmma2) {
        Vec3d kmajmma3 = new Vec3d(this);
        kmajmma3.mAjAkka(kmajmma2);
        return kmajmma3;
    }

    public final Vec3d mAjAKKa(double d, double d2, double d3) {
        return new Vec3d(this.MaJAkKA + d, this.maJAkKA + d2, this.MAJAkKA + d3);
    }

    public final Vec3d mAJAkka(double d, double d2, double d3) {
        return new Vec3d(this.MaJAkKA - d, this.maJAkKA - d2, this.MAJAkKA - d3);
    }

    public final Vec3d MAJakka() {
        return new Vec3d(-this.MaJAkKA, -this.maJAkKA, -this.MAJAkKA);
    }

    public final Vec3d majAKKa(double d) {
        Vec3d kmajmma2 = new Vec3d(this);
        kmajmma2.maJakka(d);
        return kmajmma2;
    }

    public final Vec3d MajAkka(double d) {
        Vec3d kmajmma2 = new Vec3d(this);
        kmajmma2.mAjaKKa(d);
        return kmajmma2;
    }

    public final Vec3d MAJAkka(Vec3d kmajmma2) {
        Vec3d kmajmma3 = new Vec3d(this);
        kmajmma3.maJAkka(kmajmma2);
        return kmajmma3;
    }

    public final Vec3d MaJAKKa(Vec3d kmajmma2) {
        Vec3d kmajmma3 = new Vec3d(this);
        kmajmma3.MAJAKKa(kmajmma2);
        return kmajmma3;
    }

    public final Vec3d mAJakka() {
        Vec3d kmajmma2 = new Vec3d(this);
        kmajmma2.MajAKka();
        return kmajmma2;
    }

    public final Vec3d mAJaKKa(double d) {
        Vec3d kmajmma2 = new Vec3d(this);
        kmajmma2.maJAKka(d);
        return kmajmma2;
    }

    public final Vec3d maJaKKa(double d) {
        Vec3d kmajmma2 = new Vec3d(this);
        kmajmma2.MAjAkka(d);
        return kmajmma2;
    }

    public final Vec3d MaJaKKa(double d) {
        Vec3d kmajmma2 = new Vec3d(this);
        kmajmma2.mAjAKka(d);
        return kmajmma2;
    }

    public final Vec3d MaJAKka(Vec3d kmajmma2) {
        Vec3d kmajmma3 = new Vec3d(this);
        kmajmma3.mAjAKka(kmajmma2.MaJAkKA);
        kmajmma3.maJAKka(kmajmma2.maJAkKA);
        kmajmma3.MAjAkka(kmajmma2.MAJAkKA);
        return kmajmma3;
    }

    public final Vec3d MaJakka(double d, double d2, double d3) {
        Vec3d kmajmma2 = new Vec3d(this);
        kmajmma2.mAjAKka(d);
        kmajmma2.maJAKka(d2);
        kmajmma2.MAjAkka(d3);
        return kmajmma2;
    }

    public final String toString() {
        return "{" + this.MaJAkKA + "; " + this.maJAkKA + "; " + this.MAJAkKA + "} ";
    }
}
