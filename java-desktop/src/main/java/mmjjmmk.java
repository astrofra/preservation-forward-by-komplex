/*
 * Decompiled with CFR 0.152.
 */
class mmjjmmk {
    kajamma jaKkAMA;
    public int JAKkAMA;
    boolean jAKkAMA;
    boolean JakkAMA;
    int jakkAMA;

    mmjjmmk() {
        this.KKAMAja(null);
    }

    mmjjmmk(kajamma kajamma2) {
        this.KKAMAja(kajamma2);
    }

    void KKAMAja(kajamma kajamma2) {
        if (kajamma2 == null || !kajamma2.majAKKa) {
            this.JakkAMA = false;
            this.jaKkAMA = null;
            this.kkAMAja();
            return;
        }
        this.JakkAMA = true;
        this.jaKkAMA = kajamma2;
        this.kkAMAja();
    }

    void kkAMAja() {
        this.JAKkAMA = 0;
        this.jAKkAMA = false;
    }

    void KkAMAja() {
        this.jAKkAMA = true;
        this.jakkAMA = 32768;
    }

    byte kKAMAja() {
        byte by = this.jaKkAMA.AmAjakK(this.JAKkAMA);
        this.JAKkAMA = this.jaKkAMA.AMAjakK(this.JAKkAMA, this.jAKkAMA);
        if (this.jAKkAMA) {
            this.jakkAMA -= this.jaKkAMA.MajAKKa;
            if (this.jakkAMA <= 0) {
                this.jakkAMA = 0;
                return 0;
            }
            byte by2 = (byte)(by * this.jakkAMA >> 15);
            return by2;
        }
        return by;
    }
}
