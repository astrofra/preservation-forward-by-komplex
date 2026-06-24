/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Image;

public final class kaaakka
extends mmajkmk {
    public mmaamma aMAjaKk;
    public kaajmma AmajaKk;
    Image amajaKk;
    Graphics AMajaKk;
    int aMajaKk;

    public void kKamAjA(mmjamma mmjamma2) {
        this.aMAjaKk = new mmaamma(this.AKkamaJ, this.aKkamaJ, 2, true);
        this.AmajaKk = new kaajmma(this.aMAjaKk);
        if (this.AkkAmaJ && mmajkmk.akkAmaJ) {
            this.amajaKk = mmjamma2.createImage(this.AKkamaJ, this.aKkamaJ);
            this.AMajaKk = this.amajaKk.getGraphics();
        }
    }

    public void kkaMAjA(kmaamma kmaamma2) {
        if (kmaamma2.mAjAkka == 1024) {
            float f = kmaamma2.mAjakKa.AmajAkk;
            int n = (int)(kmaamma2.mAjakKa.amAjAkk / 65536.0f - f * 0.5f);
            int n2 = (int)(kmaamma2.mAjakKa.AMAjAkk / 65536.0f - f * 0.5f);
            this.aMAjaKk.AmAJakK(kmaamma2.maJaKKa, n, n2, f, f);
            return;
        }
        this.AmajaKk.MAjakKa(kmaamma2);
    }

    public void kkAMAjA(Graphics graphics, int n, int n2) {
        this.aMAjaKk.amajAkk();
        this.aMAjaKk.AmAJAkk(graphics, n, n2);
    }

    public void KKaMAjA(Graphics graphics, int n, int n2) {
        if (mmajkmk.akkAmaJ) {
            this.aMAjaKk.amajAkk();
            this.aMAjaKk.AmAJAkk(this.AMajaKk, 0, 0);
            graphics.drawImage(this.amajaKk, n, n2, this.AkKAmaJ, this.akKAmaJ, Color.black, null);
            return;
        }
        this.aMAjaKk.amajAkk();
        this.aMAjaKk.aMajAkk(graphics, n, n2, this.AkKAmaJ, this.akKAmaJ);
    }

    public void kAMaJAK(int n) {
        this.aMajaKk = mmaamma.AMajAKK(n);
    }

    public void KkAMAjA() {
        this.KAMaJAK(this.aMajaKk);
    }

    public void KAMaJAK(int n) {
        int[] nArray = this.aMAjaKk.MAJakKa;
        int n2 = nArray.length;
        int n3 = 0;
        while (n3 < n2) {
            nArray[n3++] = n;
        }
    }
}
