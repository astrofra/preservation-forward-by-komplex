/*
 * Decompiled with CFR 0.152.
 */
import java.net.URL;

public class kaaakma
extends mmajmmk {
    float MaJakkA = 10000.0f;

    private static final String SAARI_BACKDROP_UV_MODE_PROPERTY = "forward.saariBackdropUvMode";

    public kaaakma(mmjamma mmjamma2, URL uRL, boolean bl) {
        boolean bl2 = true;
        mmajmmk mmajmmk2 = bl ? kajamka.kAMaJAk(mmjamma2.amAjAkK("meshes/octa8.igu")) : kajamka.kAMaJAk(mmjamma2.amAjAkK("meshes/half8.igu"));
        this.jAkkaMA = mmajmmk2.jAkkaMA;
        this.JaKKaMA = mmajmmk2.JaKKaMA;
        this.JAkkaMA = mmajmmk2.JAkkaMA;
        this.kkamaja(this.MaJakkA, this.MaJakkA, this.MaJakkA);
        this.kkAMAJa();
        this.kKAmaja();
        if (bl2) {
            mmaamma mmaamma2;
            mmaamma mmaamma3;
            if (bl) {
                mmaamma3 = (mmaamma)mmaakma.majaKkA(uRL);
                mmaamma2 = new mmaamma(256, 256, 1, false);
                mmaamma2.aMAJakK(mmaamma3, 0, 0);
                mmaamma mmaamma4 = new mmaamma(256, 256, 1, false);
                mmaamma4.aMAjakK(mmaamma3, 0, 0, 256, 0, 256, 256);
                this.KKAmaJa(mmaamma2, mmaamma4);
            } else {
                mmaamma3 = (mmaamma)mmaakma.majaKkA(uRL);
                mmaamma2 = new mmaamma(256, 256, 1, false);
                mmaamma2.aMAJakK(mmaamma3, 0, 0);
                this.kAmAjAK(mmaamma2, mmaamma2, false);
            }
        } else {
            mmaamma mmaamma5 = (mmaamma)mmaakma.majaKkA(mmjamma2.amAjAkK("images/verax/test.jpg"));
            this.KkaMaja(mmaamma5);
            this.KkAMaja();
        }
        this.jakKaMA = Float.POSITIVE_INFINITY;
    }

    private void kAmAjAK(mmaamma mmaamma2, mmaamma mmaamma3, boolean bl) {
        String string = bl ? "procedural" : System.getProperty(SAARI_BACKDROP_UV_MODE_PROPERTY, "procedural");
        if ("mesh".equalsIgnoreCase(string)) {
            this.kaMAJAk(mmaamma2, mmaamma3);
        } else if ("spherical".equalsIgnoreCase(string)) {
            this.kaMAJAk(mmaamma2, mmaamma3);
            this.KkAMaja();
        } else {
            this.KKAmaJa(mmaamma2, mmaamma3);
        }
        this.kAMAjAK();
    }

    private void kaMAJAk(mmaamma mmaamma2, mmaamma mmaamma3) {
        int n = 0;
        while (n < this.JAkkaMA.length) {
            kmjamma kmjamma2 = this.JAkkaMA[n];
            kmjamma2.maJaKKa = kmjamma2.MajaKKa < 0.0f ? mmaamma2 : mmaamma3;
            ++n;
        }
    }

    private void kAMAjAK() {
        int n = 0;
        while (n < this.JaKKaMA.length) {
            this.JaKKaMA[n].akKAMAJ = 1.0f - this.JaKKaMA[n].akKAMAJ;
            ++n;
        }
    }

    public void kkaMAJa(kaajmmk kaajmmk2, int n) {
        float f = this.jaKKaMA.maJakKA;
        float f2 = this.jaKKaMA.MAJakKA;
        float f3 = this.jaKKaMA.mAJakKA;
        this.jaKKaMA.majAkKA(kaajmmk2.JAKkaMa);
        float f4 = kaajmmk2.jAkkaMa;
        kaajmmk2.jAkkaMa = this.MaJakkA + 10.0f;
        super.kkaMAJa(kaajmmk2, n);
        kaajmmk2.jAkkaMa = f4;
        this.jaKKaMA.maJakKA = f;
        this.jaKKaMA.MAJakKA = f2;
        this.jaKKaMA.mAJakKA = f3;
    }
}
