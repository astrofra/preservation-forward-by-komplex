/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Toolkit;
import java.awt.image.ColorModel;
import java.awt.image.ImageConsumer;
import java.awt.image.ImageProducer;

public abstract class mmajkka
implements ImageProducer {
    protected ImageConsumer AMaJAkK;
    public Image aMaJAkK;
    protected ColorModel AmAjakK;
    public int amAjakK;
    public int AMAjakK;

    public mmajkka(int n, int n2) {
        this.amAjakK = n;
        this.AMAjakK = n2;
    }

    public final void addConsumer(ImageConsumer imageConsumer) {
    }

    public final boolean isConsumer(ImageConsumer imageConsumer) {
        return false;
    }

    public final void removeConsumer(ImageConsumer imageConsumer) {
    }

    public final void requestTopDownLeftRightResend(ImageConsumer imageConsumer) {
    }

    public final void startProduction(ImageConsumer imageConsumer) {
        this.AMaJAkK = imageConsumer;
        this.AMaJAkK.setDimensions(this.amAjakK, this.AMAjakK);
        this.AMaJAkK.setProperties(null);
        this.AMaJAkK.setColorModel(this.AmAjakK);
        this.AMaJAkK.setHints(30);
    }

    public final void amajAkk() {
        this.AmajAkk(0, 0, this.amAjakK, this.AMAjakK);
    }

    public abstract void AmajAkk(int var1, int var2, int var3, int var4);

    public final void AmAJAkk(Graphics graphics, int n, int n2) {
        graphics.drawImage(this.aMaJAkK, n, n2, null);
    }

    public final void aMajAkk(Graphics graphics, int n, int n2, int n3, int n4) {
        graphics.drawImage(this.aMaJAkK, n, n2, n3, n4, Color.green, null);
    }

    public final void AMajAkk(boolean bl) {
        if (bl) {
            if (this.aMaJAkK == null) {
                this.aMaJAkK = Toolkit.getDefaultToolkit().createImage(this);
                Toolkit.getDefaultToolkit().prepareImage(this.aMaJAkK, this.amAjakK, this.AMAjakK, null);
                this.amajAkk();
                return;
            }
        } else {
            this.aMaJAkK = null;
        }
    }
}
