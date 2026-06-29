/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Image;

public class IndexedSurfacePresenter
extends SurfacePresenter {
    public IndexedSurface kAmaJAk;
    public majjkmk KaMAJAk;
    Image kaMAJAk;
    Graphics KAMAJAk;
    int kAMAJAk = 0;

    public void nextScriptTimeHex(Graphics graphics, int n, int n2) {
        if (SurfacePresenter.akkAmaJ) {
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

    public void kKamAjA(DesktopAppletBase mmjamma2) {
        this.kAmaJAk = new IndexedSurface(this.AKkamaJ, this.aKkamaJ, 2, true);
        this.KaMAJAk = new majjkmk(this.kAmaJAk);
        if (this.AkkAmaJ && SurfacePresenter.akkAmaJ) {
            this.kaMAJAk = mmjamma2.createImage(this.AKkamaJ, this.aKkamaJ);
            this.KAMAJAk = this.kaMAJAk.getGraphics();
        }
    }

    public void kkaMAjA(Triangle kmaamma2) {
        this.KaMAJAk.KkamAjA(kmaamma2);
    }
}
