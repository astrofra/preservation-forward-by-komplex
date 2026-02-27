/*
 * Decompiled with CFR 0.152.
 */
public class kmjjmmk {
    public int[] JAkkAMA = new int[1000];
    public int[] jAkkAMA = new int[forward.kAmajAk];

    /*
     * Unable to fully structure code
     */
    public void KkaMAja(mmaamma var1_1, int var2_2) {
        var3_3 = var2_2;
        if ((var2_2 = Math.abs(var2_2)) > var1_1.AMAjakK) {
            var2_2 = var1_1.AMAjakK - 1;
        }
        var4_4 = (int)(Math.random() * 1000.0);
        var5_5 = 0;
        while (var5_5 < var2_2) {
            block4: {
                var6_6 = this.jAkkAMA[(var5_5 + var4_4) % this.jAkkAMA.length];
                var7_7 = (int)(Math.random() * (double)(this.JAkkAMA.length - 1 - 512));
                var8_8 = var1_1.amAjakK * var6_6;
                var9_9 = this.JAkkAMA;
                var10_10 = var1_1.MAJakKa;
                var13_13 = var7_7 + var1_1.amAjakK;
                if (var3_3 <= 0) ** GOTO lbl41
                while (var7_7 < var13_13) {
                    var12_12 = var10_10[var8_8] + var9_9[var7_7++];
                    var11_11 = var12_12 & 0x10040100;
                    var10_10[var8_8++] = var12_12 - var11_11 | var11_11 - (var11_11 >> 8);
                    var12_12 = var10_10[var8_8] + var9_9[var7_7++];
                    var11_11 = var12_12 & 0x10040100;
                    var10_10[var8_8++] = var12_12 - var11_11 | var11_11 - (var11_11 >> 8);
                    var12_12 = var10_10[var8_8] + var9_9[var7_7++];
                    var11_11 = var12_12 & 0x10040100;
                    var10_10[var8_8++] = var12_12 - var11_11 | var11_11 - (var11_11 >> 8);
                    var12_12 = var10_10[var8_8] + var9_9[var7_7++];
                    var11_11 = var12_12 & 0x10040100;
                    var10_10[var8_8++] = var12_12 - var11_11 | var11_11 - (var11_11 >> 8);
                }
                break block4;
lbl-1000:
                // 1 sources

                {
                    var12_12 = var10_10[var8_8] + 0x10040100 - var9_9[var7_7++];
                    var11_11 = var12_12 & 0x10040100;
                    var10_10[var8_8++] = var12_12 & var11_11 - (var11_11 >> 8);
                    var12_12 = var10_10[var8_8] + 0x10040100 - var9_9[var7_7++];
                    var11_11 = var12_12 & 0x10040100;
                    var10_10[var8_8++] = var12_12 & var11_11 - (var11_11 >> 8);
                    var12_12 = var10_10[var8_8] + 0x10040100 - var9_9[var7_7++];
                    var11_11 = var12_12 & 0x10040100;
                    var10_10[var8_8++] = var12_12 & var11_11 - (var11_11 >> 8);
                    var12_12 = var10_10[var8_8] + 0x10040100 - var9_9[var7_7++];
                    var11_11 = var12_12 & 0x10040100;
                    var10_10[var8_8++] = var12_12 & var11_11 - (var11_11 >> 8);
lbl41:
                    // 2 sources

                    ** while (var7_7 < var13_13)
                }
            }
            ++var5_5;
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
