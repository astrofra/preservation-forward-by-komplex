/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Image;

public class kmjamka
extends mmajkmk {
    public kmajkka kAmaJAk;
    public majjkmk KaMAJAk;
    Image kaMAJAk;
    Graphics KAMAJAk;
    int kAMAJAk = 0;

    public void KKaMAjA(Graphics graphics, int n, int n2) {
        if (mmajkmk.akkAmaJ) {
            this.kAmaJAk.amajAkk();
            this.kAmaJAk.AmAJAkk(this.KAMAJAk, 0, 0);
            graphics.drawImage(this.kaMAJAk, n, n2, this.AkKAmaJ, this.akKAmaJ, Color.green, null);
            return;
        }
        this.kAmaJAk.amajAkk();
        this.kAmaJAk.aMajAkk(graphics, n, n2, this.AkKAmaJ, this.akKAmaJ);
    }

    public void kkAMAjA(Graphics graphics, int n, int n2) {
        this.kAmaJAk.amajAkk();
        this.kAmaJAk.AmAJAkk(graphics, n, n2);
    }

    public void KkAMAjA() {
        int n = 0;
        while (n < this.kAmaJAk.aMAjakK.length) {
            this.kAmaJAk.aMAjakK[n] = 10;
            ++n;
        }
        ++this.kAMAJAk;
    }

    public void kKamAjA(mmjamma mmjamma2) {
        this.kAmaJAk = new kmajkka(this.AKkamaJ, this.aKkamaJ, 2, true);
        this.KaMAJAk = new majjkmk(this.kAmaJAk);
        if (this.AkkAmaJ && mmajkmk.akkAmaJ) {
            this.kaMAJAk = mmjamma2.createImage(this.AKkamaJ, this.aKkamaJ);
            this.KAMAJAk = this.kaMAJAk.getGraphics();
        }
    }

    public void kkaMAjA(kmaamma kmaamma2) {
        this.KaMAJAk.KkamAjA(kmaamma2);
    }
}
