/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Canvas;
import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.ColorModel;
import java.awt.image.DirectColorModel;
import java.awt.image.MemoryImageSource;
import java.util.Enumeration;
import java.util.Vector;
import muhmu.hifi.device.MAD;

public class majakmk
extends Canvas
implements Runnable {
    Vector AkKAmaj = new Vector(20);
    boolean akKAmaj;
    boolean AKKAmaj;
    Thread aKKAmaj;
    boolean AkkAmaj = false;
    public static boolean akkAmaj;
    Font AKkAmaj = new Font("Courier", 1, 15);
    Color aKkAmaj = Color.green;
    int AkKaMAj = 20;
    public MAD akKaMAj;
    long AKKaMAj;
    long aKKaMAj;
    int AkkaMAj = 256;
    int akkaMAj = 128;
    int AKkaMAj = 21;

    public majakmk(boolean bl, boolean bl2) {
        this.akKAmaj = bl;
        this.AKKAmaj = bl2;
        this.setBackground(new Color(0, 60, 0));
    }

    public void kKAMajA() {
        this.aKKAmaj = new Thread((Runnable)this, "Muhmu Tunari");
        this.aKKAmaj.start();
    }

    public synchronized void KKaMajA() {
        if (this.AkkAmaj) {
            return;
        }
        try {
            this.wait();
            return;
        }
        catch (Exception exception) {
            exception.printStackTrace();
            return;
        }
    }

    public void kkaMajA(long l) {
        if (akkAmaj) {
            return;
        }
        try {
            Thread.sleep(l);
            return;
        }
        catch (Exception exception) {
            exception.printStackTrace();
            return;
        }
    }

    public void paint(Graphics graphics) {
        graphics.setColor(Color.yellow);
        graphics.drawRect(0, 0, this.bounds().width - 1, this.bounds().height - 1);
        graphics.setFont(this.AKkAmaj);
        graphics.setColor(this.aKkAmaj);
        Enumeration enumeration = this.AkKAmaj.elements();
        if (enumeration != null) {
            while (enumeration.hasMoreElements()) {
                kmaakka kmaakka2 = (kmaakka)enumeration.nextElement();
                kmaakka2.KamAjaK(graphics);
            }
        }
    }

    public synchronized void run() {
        try {
            String string;
            this.AkKAmaj.addElement(new kmaakka("muhmu tuner (c) saviour", 10, this.AkKaMAj += 15));
            this.AkKAmaj.addElement(new kmaakka("measuring optimal parameters for this setup", 10, this.AkKaMAj += 15));
            this.paint(this.getGraphics());
            if (this.akKAmaj) {
                this.kkaMajA(1000L);
                this.AKKaMAj = this.KkAmAJA();
                this.AkKAmaj.addElement(new kmaakka("kunigas zoom blit " + this.AKKaMAj + "ms", 10, this.AkKaMAj += 15));
                ((Component)this).update(this.getGraphics());
                this.kkaMajA(1000L);
                this.aKKaMAj = this.kKaMajA();
                this.AkKAmaj.addElement(new kmaakka("normal zoom blit " + this.aKKaMAj + "ms", 10, this.AkKaMAj += 15));
                ((Component)this).update(this.getGraphics());
                this.kkaMajA(500L);
                if (this.AKKaMAj < this.aKKaMAj) {
                    string = "kunigas";
                    mmajkmk.akkAmaJ = true;
                } else {
                    string = "normal";
                    mmajkmk.akkAmaJ = false;
                }
                this.AkKAmaj.addElement(new kmaakka("choosing " + string + " mode", 10, this.AkKaMAj += 15));
                this.paint(this.getGraphics());
                System.out.println("kunigas=" + mmajkmk.akkAmaJ);
            }
            this.kkaMajA(1000L);
            if (!this.AKKAmaj) {
                MAD mAD = null;
                MAD mAD2 = null;
                MAD mAD3 = null;
                mAD = MAD.getDevice_IE4();
                this.AkKAmaj.addElement(new kmaakka("ie4 audio " + (mAD == null ? "not " : "") + "found", 10, this.AkKaMAj += 15));
                this.paint(this.getGraphics());
                this.kkaMajA(500L);
                mAD2 = MAD.getDevice_IE3();
                this.AkKAmaj.addElement(new kmaakka("ie3 audio " + (mAD2 == null ? "not " : "") + "found", 10, this.AkKaMAj += 15));
                this.paint(this.getGraphics());
                this.kkaMajA(500L);
                mAD3 = MAD.getDevice_Sun();
                this.AkKAmaj.addElement(new kmaakka("sun audio " + (mAD3 == null ? "not " : "") + "found", 10, this.AkKaMAj += 15));
                this.paint(this.getGraphics());
                if (mAD != null) {
                    this.akKaMAj = mAD;
                    string = "IE4 directsound";
                } else if (mAD2 != null) {
                    this.akKaMAj = mAD2;
                    string = "IE3 directsound";
                } else if (mAD3 != null) {
                    this.akKaMAj = mAD3;
                    string = "sun 8khz crap";
                } else {
                    this.akKaMAj = MAD.getDevice_NOS();
                    string = "no sound";
                }
            } else {
                this.akKaMAj = MAD.getDevice_NOS();
                string = "no audio";
            }
            this.kkaMajA(500L);
            this.AkKAmaj.addElement(new kmaakka("choosing " + string, 10, this.AkKaMAj += 15));
            this.paint(this.getGraphics());
            this.AkKAmaj.addElement(new kmaakka("OK. ready to rab!", 10, 190));
            this.paint(this.getGraphics());
            this.kkaMajA(1500L);
        }
        catch (Exception exception) {
            exception.printStackTrace();
        }
        this.AkkAmaj = true;
        this.notifyAll();
    }

    long KkAmAJA() {
        Graphics graphics = this.getGraphics();
        int[] nArray = new int[this.AkkaMAj * this.akkaMAj];
        Image image = this.createImage(new MemoryImageSource(this.AkkaMAj, this.akkaMAj, (ColorModel)new DirectColorModel(24, 0xFF0000, 65280, 255), nArray, 0, this.AkkaMAj));
        Image image2 = this.createImage(this.AkkaMAj, this.akkaMAj);
        this.KkaMajA(nArray, this.AkkaMAj, this.akkaMAj, 0, 0, this.AkkaMAj - 1, this.akkaMAj - 1, 0xFFFFFF);
        long l = System.currentTimeMillis();
        int n = 0;
        while (n < this.AKkaMAj) {
            int n2 = n * 3;
            this.KkaMajA(nArray, this.AkkaMAj, this.akkaMAj, n2, n2, this.AkkaMAj - 1 - n2 * 2, this.akkaMAj - 1 - n2 * 2, 0xFFFFFF);
            image.flush();
            image2.getGraphics().drawImage(image, 0, 0, null);
            graphics.drawImage(image2, 0, 0, this.AkkaMAj * 2, this.akkaMAj * 2, Color.black, null);
            ++n;
        }
        long l2 = System.currentTimeMillis();
        return (l2 - l) / (long)this.AKkaMAj;
    }

    long kKaMajA() {
        Graphics graphics = this.getGraphics();
        int[] nArray = new int[this.AkkaMAj * this.akkaMAj];
        Image image = this.createImage(new MemoryImageSource(this.AkkaMAj, this.akkaMAj, (ColorModel)new DirectColorModel(24, 0xFF0000, 65280, 255), nArray, 0, this.AkkaMAj));
        this.KkaMajA(nArray, this.AkkaMAj, this.akkaMAj, 0, 0, this.AkkaMAj - 1, this.akkaMAj - 1, 0xFFFFFF);
        long l = System.currentTimeMillis();
        int n = 0;
        while (n < this.AKkaMAj) {
            int n2 = n * 3;
            this.KkaMajA(nArray, this.AkkaMAj, this.akkaMAj, n2, n2, this.AkkaMAj - 1 - n2 * 2, this.akkaMAj - 1 - n2 * 2, 0xFFFFFF);
            image.flush();
            graphics.drawImage(image, 0, 0, this.AkkaMAj * 2, this.akkaMAj * 2, Color.black, null);
            ++n;
        }
        long l2 = System.currentTimeMillis();
        return (l2 - l) / (long)this.AKkaMAj;
    }

    void KkaMajA(int[] nArray, int n, int n2, int n3, int n4, int n5, int n6, int n7) {
        int n8 = n3;
        while (n8 < n3 + n5 + 1) {
            nArray[n4 * n + n8] = n7;
            nArray[(n4 + n6) * n + n8] = n7;
            ++n8;
        }
        int n9 = n4 + 1;
        while (n9 < n4 + n6) {
            nArray[n9 * n + n3] = n7;
            nArray[n9 * n + n3 + n5] = n7;
            ++n9;
        }
    }
}
