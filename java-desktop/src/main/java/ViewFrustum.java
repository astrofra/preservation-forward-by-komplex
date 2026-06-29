/*
 * Decompiled with CFR 0.152.
 */
public class ViewFrustum {
    Vec3f Kamajak = new Vec3f();
    Vec3f kamajak = new Vec3f();
    Vec3f KAmajak = new Vec3f();
    Vec3f kAmajak = new Vec3f();
    Vec3f KaMAjak = new Vec3f();
    Vec3f kaMAjak = new Vec3f();
    Vec3f KAMAjak = new Vec3f();
    Vec3f kAMAjak = new Vec3f();
    Vec3f advanceTimeline = new Vec3f();
    Vec3f kamAjak = new Vec3f();
    float KAmAjak;
    float kAmAjak;

    public void kamaJak(Camera kaajmmk2, MeshObject mmajmmk2) {
        Mat3f maajkka2 = new Mat3f(kaajmmk2.jAKkaMa);
        this.kamAjak.mAjakKA(kaajmmk2.JAKkaMa);
        this.kamAjak.mAjaKKA();
        this.KAmAjak = kaajmmk2.JAkkaMa;
        this.kAmAjak = kaajmmk2.jAkkaMa;
        if (mmajmmk2 != null) {
            Mat3f maajkka3 = mmajmmk2.JakKaMA.AmajAKk();
            maajkka2.AMajAKk(maajkka3);
            this.kamAjak.MajAkKA(mmajmmk2.jaKKaMA);
            mmajmmk2.JakKaMA.AmajAKk().amaJaKk(this.kamAjak);
        }
        float f = (float)Math.tan(kaajmmk2.jaKkaMa / 2.0f);
        float f2 = f * (float)kaajmmk2.jAkKAMa / (float)kaajmmk2.JAkKAMa;
        this.Kamajak.MaJaKka(-(f *= this.kAmAjak), f2 *= this.kAmAjak, this.kAmAjak);
        this.kamajak.MaJaKka(f, f2, this.kAmAjak);
        this.KAmajak.MaJaKka(-f, -f2, this.kAmAjak);
        this.kAmajak.MaJaKka(f, -f2, this.kAmAjak);
        this.advanceTimeline.MaJaKka(0.0f, 0.0f, 1.0f);
        maajkka2.amaJaKk(this.advanceTimeline);
        maajkka2.amaJaKk(this.Kamajak);
        maajkka2.amaJaKk(this.kamajak);
        maajkka2.amaJaKk(this.KAmajak);
        maajkka2.amaJaKk(this.kAmajak);
        this.KaMAjak.mAjakKA(this.Kamajak);
        this.KaMAjak.majakKA(this.kamajak);
        this.KaMAjak.majaKKA();
        this.kAMAjak.mAjakKA(this.kamajak);
        this.kAMAjak.majakKA(this.kAmajak);
        this.kAMAjak.majaKKA();
        this.kaMAjak.mAjakKA(this.kAmajak);
        this.kaMAjak.majakKA(this.KAmajak);
        this.kaMAjak.majaKKA();
        this.KAMAjak.mAjakKA(this.KAmajak);
        this.KAMAjak.majakKA(this.Kamajak);
        this.KAMAjak.majaKKA();
    }

    public boolean KamaJak(Vec3f mmajmma2) {
        Vec3f mmajmma3 = mmajmma2.MajAkKA(this.kamAjak);
        if (mmajmma3.majAKKA(this.KaMAjak) > 0.0f) {
            return false;
        }
        if (mmajmma3.majAKKA(this.kaMAjak) > 0.0f) {
            return false;
        }
        if (mmajmma3.majAKKA(this.KAMAjak) > 0.0f) {
            return false;
        }
        if (mmajmma3.majAKKA(this.kAMAjak) > 0.0f) {
            return false;
        }
        float f = mmajmma3.majAKKA(this.advanceTimeline);
        if (f < this.KAmAjak) {
            return false;
        }
        return !(f > this.kAmAjak);
    }

    public int KAmaJak(MeshObject mmajmmk2) {
        Vec3f mmajmma2 = mmajmmk2.jaKKaMA.MajAkKA(this.kamAjak);
        float f5 = mmajmmk2.jakKaMA * mmajmmk2.JAKKaMA.maJakKA;
        int n = 0;
        float f = mmajmma2.majAKKA(this.KaMAjak);
        if (f > -f5) {
            if (f > f5) {
                return -1;
            }
            n |= 0x40;
        }
        f = mmajmma2.majAKKA(this.kaMAjak);
        if (f > -f5) {
            if (f > f5) {
                return -1;
            }
            n |= 0x200;
        }
        f = mmajmma2.majAKKA(this.KAMAjak);
        if (f > -f5) {
            if (f > f5) {
                return -1;
            }
            n |= 1;
        }
        f = mmajmma2.majAKKA(this.kAMAjak);
        if (f > -f5) {
            if (f > f5) {
                return -1;
            }
            n |= 8;
        }
        f = mmajmma2.majAKKA(this.advanceTimeline);
        if (f - f5 < this.KAmAjak) {
            if (f + f5 < this.KAmAjak) {
                return -1;
            }
            n |= 0x8000;
        }
        if (f + f5 > this.kAmAjak) {
            if (f - f5 > this.kAmAjak) {
                return -1;
            }
            n |= 0x1000;
        }
        return n;
    }
}
