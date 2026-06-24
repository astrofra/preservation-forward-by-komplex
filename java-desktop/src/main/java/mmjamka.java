/*
 * Decompiled with CFR 0.152.
 */
import java.awt.image.ColorModel;
import java.awt.image.ImageConsumer;
import java.awt.image.IndexColorModel;
import java.util.Hashtable;

class mmjamka
implements ImageConsumer {
    ColorModel KaMaJAk;
    int kaMaJAk;
    int KAMaJAk;
    boolean kAMaJAk;
    mmajkka KamaJAk;
    boolean kamaJAk;
    boolean KAmaJAk;

    public boolean kAmaJak() {
        return !this.KAmaJAk;
    }

    public synchronized void KaMAJak() {
        try {
            while (true) {
                if (this.kamaJAk) {
                    return;
                }
                this.wait(500L);
            }
        }
        catch (Exception exception) {
            exception.printStackTrace();
            return;
        }
    }

    public synchronized void imageComplete(int n) {
        if (!this.kamaJAk) {
            if (this.KamaJAk == null || n == 1 || n == 4) {
                this.KAmaJAk = true;
            } else if (this.kAMaJAk) {
                ((mmaamma)this.KamaJAk).AMAjAKK();
            } else {
                ((kmajkka)this.KamaJAk).amAJakk((IndexColorModel)this.KaMaJAk);
            }
            this.kamaJAk = true;
            this.notify();
        }
    }

    public void setColorModel(ColorModel colorModel) {
        this.KaMaJAk = colorModel;
    }

    public void setDimensions(int n, int n2) {
        this.kaMaJAk = n;
        this.KAMaJAk = n2;
    }

    public void setHints(int n) {
        if (this.KaMaJAk instanceof IndexColorModel) {
            this.kAMaJAk = false;
            this.KamaJAk = new kmajkka(this.kaMaJAk, this.KAMaJAk, 1, false);
            return;
        }
        this.kAMaJAk = true;
        this.KamaJAk = new mmaamma(this.kaMaJAk, this.KAMaJAk, 1, false);
    }

    public void setPixels(int n, int n2, int n3, int n4, ColorModel colorModel, byte[] byArray, int n5, int n6) {
        int n7 = n2 * this.kaMaJAk + n;
        int n8 = 0;
        while (n8 < n4) {
            System.arraycopy(byArray, n5, ((kmajkka)this.KamaJAk).aMAjakK, n7, n3);
            n5 += n6;
            n7 += this.kaMaJAk;
            ++n8;
        }
    }

    public void setPixels(int n, int n2, int n3, int n4, ColorModel colorModel, int[] nArray, int n5, int n6) {
        int n7 = n2 * this.kaMaJAk + n;
        int n8 = 0;
        while (n8 < n4) {
            System.arraycopy(nArray, n5, ((mmaamma)this.KamaJAk).MAJakKa, n7, n3);
            n5 += n6;
            n7 += this.kaMaJAk;
            ++n8;
        }
    }

    public void setProperties(Hashtable hashtable) {
    }

    mmjamka() {
    }
}
