/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Graphics;

public class DominaRoutine
extends GraphicsRoutine {
    byte[] amaJakk = new byte[256];
    byte[] AMaJakk = new byte[256];
    byte[] aMaJakk = new byte[256];
    IndexedSurface AmAjAKk;
    boolean amAjAKk = false;
    IndexedSurface AMAjAKk;
    static IndexedSurface aMAjAKk;
    int AmajAKk;
    float amajAKk;
    boolean AMajAKk = false;
    float aMajAKk;

    public String scriptName() {
        return "domina";
    }

    public static IndexedSurface kAMAjaK(DesktopAppletBase mmjamma2) {
        if (aMAjAKk == null) {
            aMAjAKk = (IndexedSurface)ImageSupport.majaKkA(mmjamma2.amAjAkK("images/phorward.gif"));
        }
        return aMAjAKk;
    }

    public void init(DesktopAppletBase mmjamma2) {
        if (this.amAjAKk) {
            this.AmAjAKk = (IndexedSurface)ImageSupport.majaKkA(mmjamma2.amAjAkK("images/komplex.gif"));
            this.AmAjAKk.AMajAkk(true);
            ForwardDemoApp.kkamAJA.kAMajak();
        } else {
            this.AmAjAKk = DominaRoutine.kAMAjaK(mmjamma2);
            this.AMAjAKk = new IndexedSurface(512, 256, 1, true);
            ForwardDemoApp.kkamAJA.kAMajak();
        }
        System.arraycopy(this.AmAjAKk.aMajakK, 0, this.amaJakk, 0, 256);
        System.arraycopy(this.AmAjAKk.AmAJakK, 0, this.AMaJakk, 0, 256);
        System.arraycopy(this.AmAjAKk.amAJakK, 0, this.aMaJakk, 0, 256);
    }

    public void dispose() {
        this.AMAjAKk = null;
        this.AmAjAKk = null;
    }

    public void render(Graphics graphics, float f, float f2) {
        this.AmajAKk = (int)(Math.max(0.0f, f) * 50.0f);
        this.AMAjAKk.AMaJAkk(this.AmAjAKk, 0, -(this.AmajAKk * 256 % this.AmAjAKk.AMAjakK));
        float f3 = (f - 0.2f) / 8.0f;
        if (this.AMajAKk) {
            f3 = 1.0f - (f - this.aMajAKk) * 0.1f;
        }
        if (f3 > 1.0f) {
            f3 = 1.0f;
        }
        if (f3 < 0.0f) {
            f3 = 0.0f;
        }
        this.amajAKk = f3;
        int n = 0;
        while (n < 256) {
            int n2 = this.AMajAKk ? ImageSupport.maJaKkA(0, 0, 0, this.amaJakk[n], this.AMaJakk[n], this.aMaJakk[n], 1.0f - this.amajAKk) : ImageSupport.maJaKkA(255, 255, 255, this.amaJakk[n], this.AMaJakk[n], this.aMaJakk[n], 1.0f - this.amajAKk);
            this.AMAjAKk.aMajakK[n] = (byte)(n2 >> 16 & 0xFF);
            this.AMAjAKk.AmAJakK[n] = (byte)(n2 >> 8 & 0xFF);
            this.AMAjAKk.amAJakK[n] = (byte)(n2 & 0xFF);
            ++n;
        }
        this.AMAjAKk.aMaJAkk();
        this.AMAjAKk.amajAkk();
        this.AMAjAKk.AmAJAkk(graphics, 0, 0);
    }

    public void handleMessage(String string, float f) {
        if (string.equals("fade2black")) {
            this.AMajAKk = true;
            this.aMajAKk = f;
        }
    }
}
