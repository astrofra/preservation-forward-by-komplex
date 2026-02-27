/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Graphics;

public abstract class mmajkmk {
    public int AKkamaJ;
    public int aKkamaJ;
    public int AkKAmaJ;
    public int akKAmaJ;
    public int AKKAmaJ;
    public int aKKAmaJ;
    public boolean AkkAmaJ;
    public static boolean akkAmaJ = true;
    public static final int AKkAmaJ = 1;
    public static final int aKkAmaJ = 2;
    public static final int AkKaMAJ = 16;
    public static final int akKaMAJ = 32;
    public static final int AKKaMAJ = 48;
    public static final int aKKaMAJ = 128;
    public static final int AkkaMAJ = 256;
    public static final int akkaMAJ = 1024;
    public static final int AKkaMAJ = 17;
    public static final int aKkaMAJ = 33;
    public static final int AkKAMAJ = 49;

    public final void KkaMAjA(mmjamma mmjamma2, int n, int n2) {
        this.KKamAjA(mmjamma2, n, n2, 1);
    }

    public final void KKamAjA(mmjamma mmjamma2, int n, int n2, int n3) {
        this.kkamAjA(mmjamma2, n, n2, n3, n3);
    }

    public final void kkamAjA(mmjamma mmjamma2, int n, int n2, int n3, int n4) {
        this.AKKAmaJ = n3;
        this.aKKAmaJ = n4;
        this.AkKAmaJ = n;
        this.akKAmaJ = n2;
        this.AKkamaJ = n / this.AKKAmaJ;
        this.aKkamaJ = n2 / this.aKKAmaJ;
        this.AkkAmaJ = this.AKKAmaJ != 1 || this.aKKAmaJ != 1;
        this.kKamAjA(mmjamma2);
    }

    public final void KKAMAjA(Graphics graphics, int n, int n2) {
        if (!this.AkkAmaJ) {
            this.kkAMAjA(graphics, n, n2);
            return;
        }
        this.KKaMAjA(graphics, n, n2);
    }

    public final void kKAMAjA(kmaamma[] kmaammaArray, int n) {
        int n2 = 0;
        while (n2 < n) {
            this.kkaMAjA(kmaammaArray[n2++]);
        }
    }

    public abstract void kKamAjA(mmjamma var1);

    public abstract void kkAMAjA(Graphics var1, int var2, int var3);

    public abstract void KKaMAjA(Graphics var1, int var2, int var3);

    public abstract void kkaMAjA(kmaamma var1);

    public abstract void KkAMAjA();
}
