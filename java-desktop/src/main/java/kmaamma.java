/*
 * Decompiled with CFR 0.152.
 */
public final class kmaamma
extends kmjamma {
    public mmjakka mAjakKa;
    public mmjakka MaJAkKa;
    public mmjakka maJAkKa;
    public kmajkmk MAJAkKa;
    public kmajkmk mAJAkKa;
    public kmajkmk MajAkKa;

    public kmaamma(mmajmmk mmajmmk2, int n, int n2, int n3) {
        this.mAjakKa = mmajmmk2.jAkkaMA[n];
        this.MaJAkKa = mmajmmk2.jAkkaMA[n2];
        this.maJAkKa = mmajmmk2.jAkkaMA[n3];
    }

    public kmaamma(mmajmmk mmajmmk2, int n, int n2, int n3, int n4, int n5, int n6) {
        this.mAjakKa = mmajmmk2.jAkkaMA[n];
        this.MaJAkKa = mmajmmk2.jAkkaMA[n2];
        this.maJAkKa = mmajmmk2.jAkkaMA[n3];
        this.MAJAkKa = mmajmmk2.JaKKaMA[n4];
        this.mAJAkKa = mmajmmk2.JaKKaMA[n5];
        this.MajAkKa = mmajmmk2.JaKKaMA[n6];
    }

    public kmaamma(mmjakka mmjakka2, mmjakka mmjakka3, mmjakka mmjakka4) {
        this.mAjakKa = mmjakka2;
        this.MaJAkKa = mmjakka3;
        this.maJAkKa = mmjakka4;
    }

    public kmaamma(mmjakka mmjakka2, mmjakka mmjakka3, mmjakka mmjakka4, kmajkmk kmajkmk2, kmajkmk kmajkmk3, kmajkmk kmajkmk4) {
        this.mAjakKa = mmjakka2;
        this.MaJAkKa = mmjakka3;
        this.maJAkKa = mmjakka4;
        this.MAJAkKa = kmajkmk2;
        this.mAJAkKa = kmajkmk3;
        this.MajAkKa = kmajkmk4;
    }

    public kmaamma(kmaamma kmaamma2) {
        this.mAjAkka = kmaamma2.mAjAkka;
        this.MaJaKKa = kmaamma2.MaJaKKa;
        this.maJaKKa = kmaamma2.maJaKKa;
    }

    public kmaamma(kmaamma kmaamma2, mmjakka mmjakka2, mmjakka mmjakka3, mmjakka mmjakka4) {
        this.mAjAkka = kmaamma2.mAjAkka;
        this.MaJaKKa = kmaamma2.MaJaKKa;
        this.maJaKKa = kmaamma2.maJaKKa;
        this.mAjakKa = mmjakka2;
        this.MaJAkKa = mmjakka3;
        this.maJAkKa = mmjakka4;
        this.MAJAkKa = new kmajkmk();
        this.mAJAkKa = new kmajkmk();
        this.MajAkKa = new kmajkmk();
    }

    public kmaamma(kmaamma kmaamma2, mmjakka mmjakka2, mmjakka mmjakka3, mmjakka mmjakka4, kmajkmk kmajkmk2, kmajkmk kmajkmk3, kmajkmk kmajkmk4) {
        this.mAjAkka = kmaamma2.mAjAkka;
        this.MaJaKKa = kmaamma2.MaJaKKa;
        this.maJaKKa = kmaamma2.maJaKKa;
        this.mAjakKa = mmjakka2;
        this.MaJAkKa = mmjakka3;
        this.maJAkKa = mmjakka4;
        this.MAJAkKa = kmajkmk2;
        this.mAJAkKa = kmajkmk3;
        this.MajAkKa = kmajkmk4;
    }

    public void AmaJAKK(kmaamma kmaamma2) {
        this.mAjAkka = kmaamma2.mAjAkka;
        this.MaJaKKa = kmaamma2.MaJaKKa;
        this.maJaKKa = kmaamma2.maJaKKa;
    }

    public final void amaJAkK() {
        kmjamma.majaKKa[kmjamma.MAjaKKa++] = this;
    }

    public final void aMaJAkK() {
        mmajmma mmajmma2 = this.MaJAkKa.mAjAKKA(this.mAjakKa);
        mmajmma mmajmma3 = this.maJAkKa.mAjAKKA(this.mAjakKa);
        mmajmma2.majakKA(mmajmma3);
        mmajmma2.majaKKA();
        this.MAJaKKa = mmajmma2.maJakKA;
        this.mAJaKKa = mmajmma2.MAJakKA;
        this.MajaKKa = mmajmma2.mAJakKA;
    }

    public final void AMaJAkK() {
        this.mAjakKa.aMAJaKK += this.MAJaKKa;
        this.mAjakKa.AmaJaKK += this.mAJaKKa;
        this.mAjakKa.amaJaKK += this.MajaKKa;
        this.MaJAkKa.aMAJaKK += this.MAJaKKa;
        this.MaJAkKa.AmaJaKK += this.mAJaKKa;
        this.MaJAkKa.amaJaKK += this.MajaKKa;
        this.maJAkKa.aMAJaKK += this.MAJaKKa;
        this.maJAkKa.AmaJaKK += this.mAJaKKa;
        this.maJAkKa.amaJaKK += this.MajaKKa;
    }
}
