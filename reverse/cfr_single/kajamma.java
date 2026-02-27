/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Point;

class kajamma {
    byte[] MaJAKKa;
    int maJAKKa;
    int MAJAKKa;
    int mAJAKKa;
    int MajAKKa;
    public boolean majAKKa = true;
    public boolean MAjAKKa = false;
    public boolean mAjAKKa = false;

    kajamma(byte[] byArray, int n, int n2, int n3) {
        this.MaJAKKa = byArray;
        this.maJAKKa = n;
        this.MAJAKKa = n2;
        this.mAJAKKa = n3;
    }

    kajamma(Point[] pointArray, int n, int n2, int n3) {
        if (n < pointArray.length) {
            this.maJAKKa = pointArray[n].x;
        }
        if (n2 < pointArray.length) {
            this.MAJAKKa = pointArray[n2].x;
        }
        if (n3 < pointArray.length) {
            this.mAJAKKa = pointArray[n3].x;
        }
        if (pointArray.length < 1) {
            this.MaJAKKa = null;
            return;
        }
        this.MaJAKKa = new byte[pointArray[pointArray.length - 1].x + 1];
        int n4 = 0;
        int n5 = 0;
        while (n5 < pointArray.length - 1) {
            int n6 = pointArray[n5].x;
            int n7 = pointArray[n5].y;
            int n8 = pointArray[n5 + 1].x;
            int n9 = pointArray[n5 + 1].y;
            int n10 = n7 <<= 8;
            int n11 = ((n9 <<= 8) - n7) / (n8 - n6);
            n4 = n6;
            while (n4 < n8) {
                if (n4 >= this.MaJAKKa.length) {
                    return;
                }
                this.MaJAKKa[n4] = (byte)(n10 >> 8);
                n10 += n11;
                ++n4;
            }
            ++n5;
        }
        this.MaJAKKa[n4] = (byte)pointArray[pointArray.length - 1].y;
    }

    public void amAjakK(int n) {
        this.MajAKKa = n;
    }

    public byte AmAjakK(int n) {
        if (n > this.MaJAKKa.length) {
            return this.MaJAKKa[this.MaJAKKa.length - 1];
        }
        return this.MaJAKKa[n];
    }

    public int AMAjakK(int n, boolean bl) {
        ++n;
        if (bl) {
            if (this.MAjAKKa && n != this.maJAKKa && n >= this.mAJAKKa) {
                n = this.MAJAKKa;
            }
        } else {
            if (this.mAjAKKa && n >= this.maJAKKa) {
                n = this.maJAKKa;
            }
            if (this.MAjAKKa && n >= this.mAJAKKa) {
                n = this.MAJAKKa;
            }
        }
        if (n >= this.MaJAKKa.length) {
            n = this.MaJAKKa.length - 1;
        }
        return n;
    }

    public String toString() {
        String string = "[" + this.MaJAKKa.length + "] {";
        int n = 0;
        while (n < this.MaJAKKa.length) {
            string = String.valueOf(string) + this.MaJAKKa[n] + ", ";
            ++n;
        }
        return string;
    }
}
