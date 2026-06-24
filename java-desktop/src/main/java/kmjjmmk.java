/*
 * Decompiled with CFR 0.152.
 */
public class kmjjmmk {
    public int[] JAkkAMA = new int[1000];
    public int[] jAkkAMA = new int[forward.kAmajAk];

    public void KkaMAja(mmaamma var1_1, int var2_2) {
        int n = var2_2;
        if ((var2_2 = Math.abs(var2_2)) > var1_1.AMAjakK) {
            var2_2 = var1_1.AMAjakK - 1;
        }
        int n2 = (int)(Math.random() * 1000.0);
        for (int n3 = 0; n3 < var2_2; ++n3) {
            int n4 = this.jAkkAMA[(n3 + n2) % this.jAkkAMA.length];
            int n5 = (int)(Math.random() * (double)(this.JAkkAMA.length - 1 - 512));
            int n6 = var1_1.amAjakK * n4;
            int[] nArray = this.JAkkAMA;
            int[] nArray2 = var1_1.MAJakKa;
            int n7 = n5 + var1_1.amAjakK;
            if (n > 0) {
                while (n5 < n7) {
                    int n8 = nArray2[n6] + nArray[n5++];
                    int n9 = n8 & 0x10040100;
                    nArray2[n6++] = n8 - n9 | n9 - (n9 >> 8);
                }
                continue;
            }
            while (n5 < n7) {
                int n10 = nArray2[n6] + 0x10040100 - nArray[n5++];
                int n11 = n10 & 0x10040100;
                nArray2[n6++] = n10 & (n11 - (n11 >> 8));
            }
        }
    }

    public kmjjmmk(int n, int n2, int n3) {
        int n4;
        int n5;
        int n6;
        int n7 = 0;
        while (n7 < this.JAkkAMA.length) {
            n6 = (int)(Math.random() * (double)n);
            n5 = (int)(Math.random() * (double)n2);
            n4 = (int)(Math.random() * (double)n3);
            this.JAkkAMA[n7] = mmaamma.aMAJAKK(n6, n5, n4);
            ++n7;
        }
        n6 = 0;
        while (n6 < this.jAkkAMA.length) {
            this.jAkkAMA[n6] = n6;
            ++n6;
        }
        n5 = 0;
        while (n5 < 3000) {
            n4 = n5 % this.jAkkAMA.length;
            int n8 = (int)(Math.random() * (double)(this.jAkkAMA.length - 2));
            int n9 = this.jAkkAMA[n4];
            this.jAkkAMA[n4] = this.jAkkAMA[n8];
            this.jAkkAMA[n8] = n9;
            ++n5;
        }
    }
}
