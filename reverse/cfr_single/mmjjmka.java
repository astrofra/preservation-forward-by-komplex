/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.Panel;
import java.awt.Window;

public final class mmjjmka
extends Window {
    static public final int AmAJaKk = 0;
    static public final int amAJaKk = 36;
    static public final int AMAJaKk = 640;
    static public final int aMAJaKk = 480;
    public Panel AmaJaKk;
    public Dimension amaJaKk = new Dimension(640, 480);
    public Dimension AMaJaKk = this.getToolkit().getScreenSize();

    public mmjjmka(String string) {
        super(new Frame());
        ((Component)this).setBackground(Color.black);
        this.setLayout(null);
        ((Component)this).reshape(0, 0, this.AMaJaKk.width, this.AMaJaKk.height + 36);
        this.AmaJaKk = new Panel();
        this.AmaJaKk.setLayout(null);
        this.AmaJaKk.setBackground(Color.black);
        this.AmaJaKk.reshape(this.AMaJaKk.width / 2 - this.amaJaKk.width / 2, this.AMaJaKk.height / 2 - this.amaJaKk.height / 2, this.amaJaKk.width, this.amaJaKk.height);
        this.add(this.AmaJaKk);
    }

    public void kamaJAK(Component component) {
        int n = (this.amaJaKk.width - component.bounds().width) / 2;
        int n2 = (this.amaJaKk.height - component.bounds().height) / 2;
        component.move(n, n2);
        this.AmaJaKk.add(component);
    }

    public void KamaJAK(Component component) {
        component.reshape(0, 0, this.amaJaKk.width, this.amaJaKk.height);
        this.AmaJaKk.add(component);
    }

    public void dispose() {
        ((Container)this).removeNotify();
        this.removeAll();
        super.dispose();
    }

    public void show() {
        super.show();
        this.toFront();
    }
}
