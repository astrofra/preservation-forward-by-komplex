/*
 * Decompiled with CFR 0.152.
 */
public final class Camera {
    public int JAkKAMa;
    public int jAkKAMa;
    public float JaKkaMa = 1.0f;
    public float jaKkaMa = 1.57f;
    public Vec3f JAKkaMa = new Vec3f(6.0f, 0.0f, -40.0f);
    public Mat3f jAKkaMa = new Mat3f();
    public Vec3f JakkaMa = new Vec3f(0.0f, 0.0f, 1.0f);
    public float jakkaMa;
    public float JAkkaMa = 0.1f;
    public float jAkkaMa = 26.0f;
    public float JaKKaMa = 15.0f;

    public void AkKAMaJ(Vec3f mmajmma2) {
        Vec3f mmajmma3 = mmajmma2.mAjAKKA(this.JAKkaMa);
        mmajmma3.majaKKA();
        this.jAKkaMa.AmAjAKk(mmajmma3);
        Vec3f mmajmma4 = this.JakkaMa.MAJaKka(mmajmma3);
        mmajmma4.majaKKA();
        mmajmma3.majakKA(mmajmma4);
        if (this.jakkaMa == 0.0f) {
            this.jAKkaMa.aMAjAKk(mmajmma4);
            this.jAKkaMa.amAjAKk(mmajmma3);
            return;
        }
        Vec3f mmajmma5 = mmajmma4.maJakKA((float)Math.cos(this.jakkaMa)).MajAkKA(mmajmma3.maJakKA((float)Math.sin(this.jakkaMa)));
        Vec3f mmajmma6 = mmajmma4.maJakKA((float)(-Math.sin(this.jakkaMa))).MajAkKA(mmajmma3.maJakKA((float)Math.cos(this.jakkaMa)));
        this.jAKkaMa.aMAjAKk(mmajmma5);
        this.jAKkaMa.amAjAKk(mmajmma6);
    }

    public void akKAMaJ(Vec3f mmajmma2) {
        this.AkKAMaJ(this.JAKkaMa.MajAkKA(mmajmma2));
    }
}
