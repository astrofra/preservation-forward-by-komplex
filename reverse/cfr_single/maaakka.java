/*
 * Decompiled with CFR 0.152.
 */
public class maaakka {
    private int AMaJAKk;
    private long[] aMaJAKk;
    private long AmAjaKk;
    private long amAjaKk;
    private long AMAjaKk;

    public maaakka(int n) {
        this(n, 10);
    }

    public maaakka(int n, int n2) {
        this.AMaJAKk = n2;
        this.AMAjaKk = n;
        this.aMaJAKk = new long[this.AMaJAKk];
        this.kAmAjaK(this.AMAjaKk);
    }

    public void KaMaJAK() {
        this.kAmAjaK(this.AMAjaKk);
    }

    public void kAmAjaK(long l) {
        this.AMAjaKk = l;
        this.amAjaKk = System.currentTimeMillis();
        int n = 0;
        while (n < this.AMaJAKk) {
            this.aMaJAKk[this.AMaJAKk - 1 - n] = this.amAjaKk;
            this.amAjaKk -= l;
            ++n;
        }
        this.amAjaKk = 0L;
        this.KAmAjaK();
        this.amAjaKk = this.AmAjaKk;
        this.AmAjaKk = 0L;
    }

    public long kamAjaK() {
        return this.AmAjaKk;
    }

    public void KAmAjaK() {
        int n = 0;
        while (n < this.AMaJAKk - 1) {
            this.aMaJAKk[n] = this.aMaJAKk[n + 1];
            ++n;
        }
        this.aMaJAKk[this.AMaJAKk - 1] = System.currentTimeMillis();
        long l = 0L;
        int n2 = 0;
        while (n2 < this.AMaJAKk) {
            l += this.aMaJAKk[n2] / (long)this.AMaJAKk;
            ++n2;
        }
        this.AmAjaKk = l;
        this.AmAjaKk -= this.amAjaKk;
    }

    public float kaMaJAK() {
        return (this.aMaJAKk[this.AMaJAKk - 1] - this.aMaJAKk[0]) / (long)(this.AMaJAKk - 2);
    }
}
