/*
 * Decompiled with CFR 0.152.
 */
public class kaajmka {
    mmajmma Kamajak = new mmajmma();
    mmajmma kamajak = new mmajmma();
    mmajmma KAmajak = new mmajmma();
    mmajmma kAmajak = new mmajmma();
    mmajmma KaMAjak = new mmajmma();
    mmajmma kaMAjak = new mmajmma();
    mmajmma KAMAjak = new mmajmma();
    mmajmma kAMAjak = new mmajmma();
    mmajmma KamAjak = new mmajmma();
    mmajmma kamAjak = new mmajmma();
    float KAmAjak;
    float kAmAjak;

    public void kamaJak(kaajmmk kaajmmk2, mmajmmk mmajmmk2) {
        maajkka maajkka2 = new maajkka(kaajmmk2.jAKkaMa);
        this.kamAjak.mAjakKA(kaajmmk2.JAKkaMa);
        this.kamAjak.mAjaKKA();
        this.KAmAjak = kaajmmk2.JAkkaMa;
        this.kAmAjak = kaajmmk2.jAkkaMa;
        if (mmajmmk2 != null) {
            maajkka maajkka3 = mmajmmk2.JakKaMA.AmajAKk();
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
        this.KamAjak.MaJaKka(0.0f, 0.0f, 1.0f);
        maajkka2.amaJaKk(this.KamAjak);
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

    public boolean KamaJak(mmajmma mmajmma2) {
        mmajmma mmajmma3 = mmajmma2.MajAkKA(this.kamAjak);
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
        float f = mmajmma3.majAKKA(this.KamAjak);
        if (f < this.KAmAjak) {
            return false;
        }
        return !(f > this.kAmAjak);
    }

    public int KAmaJak(mmajmmk mmajmmk2) {
        float f;
        float f2;
        float f3;
        float f4;
        mmajmma mmajmma2 = mmajmmk2.jaKKaMA.MajAkKA(this.kamAjak);
        float f5 = mmajmmk2.jakKaMA * mmajmmk2.JAKKaMA.maJakKA;
        int n = 0;
        float f6 = mmajmma2.majAKKA(this.KaMAjak);
        if (f4 > -f5) {
            if (f6 > f5) {
                return -1;
            }
            n |= 0x40;
        }
        f6 = mmajmma2.majAKKA(this.kaMAjak);
        if (f3 > -f5) {
            if (f6 > f5) {
                return -1;
            }
            n |= 0x200;
        }
        f6 = mmajmma2.majAKKA(this.KAMAjak);
        if (f2 > -f5) {
            if (f6 > f5) {
                return -1;
            }
            n |= 1;
        }
        f6 = mmajmma2.majAKKA(this.kAMAjak);
        if (f > -f5) {
            if (f6 > f5) {
                return -1;
            }
            n |= 8;
        }
        if ((f6 = mmajmma2.majAKKA(this.KamAjak)) - f5 < this.KAmAjak) {
            if (f6 + f5 < this.KAmAjak) {
                return -1;
            }
            n |= 0x8000;
        }
        if (f6 + f5 > this.kAmAjak) {
            if (f6 - f5 > this.kAmAjak) {
                return -1;
            }
            n |= 0x1000;
        }
        return n;
    }
}
