/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;

class kmaakka {
    String amAJAKk;
    int AMAJAKk;
    int aMAJAKk;
    Color AmaJAKk;
    Font amaJAKk;

    kmaakka(String string, int n, int n2, Color color, Font font) {
        this.amAJAKk = string;
        this.AMAJAKk = n;
        this.aMAJAKk = n2;
        this.AmaJAKk = color;
        this.amaJAKk = font;
    }

    kmaakka(String string, int n, int n2) {
        this.amAJAKk = string;
        this.AMAJAKk = n;
        this.aMAJAKk = n2;
    }

    kmaakka(Color color, Font font) {
        this.AmaJAKk = color;
        this.amaJAKk = font;
    }

    public void KamAjaK(Graphics graphics) {
        if (this.AmaJAKk != null) {
            graphics.setColor(this.AmaJAKk);
        }
        if (this.amaJAKk != null) {
            graphics.setFont(this.amaJAKk);
        }
        if (this.amAJAKk != null) {
            graphics.drawString(this.amAJAKk, this.AMAJAKk, this.aMAJAKk);
        }
    }
}
