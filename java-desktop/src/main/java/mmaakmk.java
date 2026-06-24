/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Image;
import java.net.URL;

public class mmaakmk
extends majjkka {
    mmjamma JaKkAmA;
    int jaKkAmA;
    int JAKkAmA;
    String jAKkAmA;
    kmajkka JakkAmA;
    kmajkka jakkAmA;
    Image JAkkAmA;
    Graphics jAkkAmA;
    boolean JaKKAmA;
    int jaKKAmA = 26;
    Font JAKKAmA = new Font("Courier", 1, 16);
    Color jAKKAmA = new Color(255, 255, 255);
    Color JakKAmA = new Color(200, 255, 200);
    double jakKAmA = 25.0;
    String[] JAkKAmA = new String[]{"", "forward", "komplex", "", "", "", "", "", "code", "", "saviour", "jmagic", "anis", "", "", "graphics", "", "jugi", "", "", "intro theme", "", "jugi", "", "", "main theme", "", "carebear/orange", "", "", "klunssi object", "", "reward", "", "", "", "", "", "", "", "rebellion will not be televised", "", "", "", "__mailto:komplex@jyu.fi", "__http://www.jyu.fi/komplex", "", ""};
    int jAkKAmA;
    int JaKkamA;
    int jaKkamA;
    int JAKkamA;
    int jAKkamA = 512;
    int JakkamA = 256;
    Color jakkamA = new Color(0, 0, 0);
    Color JAkkamA = new Color(255, 255, 0);
    boolean jAkkamA = false;

    public String amAjAkk() {
        return "uppol";
    }

    public void AmAjAkk(mmjamma mmjamma2) {
        this.JaKkAmA = mmjamma2;
        this.JaKkamA = 0;
        this.jaKkamA = 512 - this.JaKkamA - 150;
        this.jaKkAmA = 0;
        this.JAKkAmA = 0;
        this.JakkAmA = kajakka.kAMAjaK(this.JaKkAmA);
        this.jakkAmA = new kmajkka(512, 256, 1, true);
        this.JAkkAmA = this.JaKkAmA.createImage(this.jAKkamA, this.JakkamA);
        this.jAkkAmA = this.JAkkAmA.getGraphics();
        int n = 0;
        while (n < 256) {
            this.jakkAmA.aMajakK[n] = 0;
            this.jakkAmA.AmAJakK[n] = 0;
            this.jakkAmA.amAJakK[n] = (byte)(this.JakkAmA.amAJakK[n] & 0xFF);
            ++n;
        }
        this.jakkAmA.aMaJAkk();
    }

    String kKamAJA(int n) {
        this.jAkKAmA = 0;
        if (n < 0) {
            return "";
        }
        if (n > this.JAkKAmA.length - 1) {
            this.JaKKAmA = true;
            return "";
        }
        String string = this.JAkKAmA[n];
        if (string.indexOf("l_") == 0) {
            string = string.substring(2, string.length());
            this.jAkKAmA = 1;
        }
        if (string.indexOf("r_") == 0) {
            string = string.substring(2, string.length());
            this.jAkKAmA = 2;
        }
        if (string.indexOf("__") == 0) {
            string = string.substring(2, string.length());
            this.jAkKAmA = 3;
        }
        return string;
    }

    public void amaJaKK(Graphics graphics, float f, float f2) {
        this.jakkAmA.AMaJAkk(this.JakkAmA, 0, -(this.JAKkamA * 256 % this.JakkAmA.AMAjakK));
        this.jakkAmA.amajAkk();
        this.jakkAmA.AmAJAkk(this.jAkkAmA, 0, 0);
        double d = (double)f * this.jakKAmA;
        int n = (this.JaKkamA + this.jaKkamA) / 2;
        int n2 = (int)d - (this.JakkamA + this.jaKKAmA);
        int n3 = this.JakkamA / this.jaKKAmA + 2;
        if (n2 / this.jaKKAmA + n3 >= this.JAkKAmA.length) {
            n2 = (this.JAkKAmA.length - n3) * this.jaKKAmA;
        }
        int n4 = this.jaKKAmA - n2 % this.jaKKAmA;
        int n5 = n2 / this.jaKKAmA;
        boolean bl = forward.kAmAjAk;
        forward.kAmAjAk = false;
        int n6 = 0;
        while (n6 < n3) {
            Font font = this.JAKKAmA;
            this.jAkkAmA.setColor(this.jAKKAmA);
            this.jAkkAmA.setFont(font);
            String string = this.kKamAJA(n6 + n5);
            int n7 = this.JaKkAmA.getFontMetrics(font).stringWidth(string);
            switch (this.jAkKAmA) {
                case 0: {
                    this.jAkkAmA.drawString(string, n - (n7 >> 1), n4 - 5);
                    break;
                }
                case 1: {
                    this.jAkkAmA.drawString(string, this.JaKkamA, n4 - 5);
                    break;
                }
                case 2: {
                    this.jAkkAmA.drawString(string, this.jaKkamA - n7, n4 - 5);
                    break;
                }
                case 3: {
                    int n8 = n - (n7 >> 1);
                    int n9 = n4 - 4 - 16;
                    int n10 = n8 + n7;
                    int n11 = n9 + 20;
                    if (forward.KAMAjAk > n8 && forward.KAMAjAk < n10 && forward.kAMAjAk > n9 && forward.kAMAjAk < n11) {
                        if (forward.KamAjAk) {
                            this.jAkkAmA.setColor(this.jakkamA);
                        } else {
                            this.jAkkAmA.setColor(this.JAkkamA);
                            this.JaKkAmA.showStatus(string);
                        }
                        if (bl) {
                            this.KKamAJA(string);
                            this.jAkkamA = true;
                            this.aMaJaKK();
                        }
                    }
                    this.jAkkAmA.drawLine(n - (n7 >> 1), n4 - 4, n + (n7 >> 1), n4 - 4);
                    this.jAkkAmA.drawString(string, n - (n7 >> 1), n4 - 5);
                    break;
                }
            }
            n4 += this.jaKKAmA;
            ++n6;
        }
        graphics.drawImage(this.JAkkAmA, 0, 0, null);
        if (this.jAkkamA) {
            try {
                Thread.sleep(50L);
            }
            catch (Exception exception) {}
        }
        ++this.JAKkamA;
    }

    void KKamAJA(String string) {
        try {
            URL uRL = new URL(string);
            this.JaKkAmA.getAppletContext().showDocument(uRL);
            return;
        }
        catch (Exception exception) {
            return;
        }
    }
}
