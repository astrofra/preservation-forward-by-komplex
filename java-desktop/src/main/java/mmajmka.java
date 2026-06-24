/*
 * Decompiled with CFR 0.152.
 */
final class mmajmka {
    static final byte KamajaK = 0;
    static final byte kamajaK = 1;
    static final byte KAmajaK = 2;
    String kAmajaK = "unnamed";
    byte[] KaMAjaK;
    int kaMAjaK;
    int KAMAjaK = 32;
    int kAMAjaK;
    int KamAjaK;
    byte kamAjaK = 0;
    int KAmAjaK;
    int kAmAjaK;
    int KaMaJAK;
    int kaMaJAK = 128;
    static final int KAMaJAK = 12;
    static final int kAMaJAK = 4096;
    static final int KamaJAK = 4095;

    final int KamajAK(int n) {
        n += this.KAMAjaK;
        if (--n < 0) {
            n = 0;
        }
        if (n > 118) {
            n = 118;
        }
        return 7680 - n * 16 * 4 - this.kAMAjaK / 2;
    }

    static final int kAmajAK(int n) {
        if (n < 0) {
            return 1111111;
        }
        int n2 = mmjjkmk.kKAmaJa[n % 768] >> n / 768;
        int n3 = n2 << 12;
        return n3;
    }

    public void kAMajAK(byte[] byArray, int n, int n2) {
        this.kaMAjaK = n2;
        this.KaMAjaK = new byte[n2];
        System.arraycopy(byArray, n, this.KaMAjaK, 0, n2);
    }

    public void kAmAJAK(String string) {
        this.kAmajaK = string;
    }

    public void kaMajAK(int n, int n2, int n3) {
        this.kamAjaK = (byte)n;
        this.KAmAjaK = n2 << 12;
        this.KaMaJAK = n3 << 12;
        this.kAmAjaK = this.KAmAjaK + this.KaMaJAK;
    }

    public void KAmAJAK(int n) {
        this.KAMAjaK = n;
    }

    public void KAMajAK(int n) {
        this.kAMAjaK = n;
    }

    public void KaMajAK(int n) {
        this.KamAjaK = n;
    }

    public void KAmajAK(int n) {
        this.kaMaJAK = n;
    }

    public int kamajAK(int[] nArray, int n, int n2, int n3, int n4, int n5, int n6) {
        int n7 = 255 - n6 + (n6 << 16);
        n7 = n7 * n5 >> 8;
        n7 &= 0xFF00FF;
        byte[] byArray = this.KaMAjaK;
        switch (this.kamAjaK) {
            case 0: {
                int n8 = this.kaMAjaK << 12;
                int n9 = (n8 - 1 - n3) / n4 + 1;
                if (n9 > n2 - n) {
                    n9 = n2 - n;
                }
                switch (n9 & 3) {
                    case 3: {
                        int n10 = n++;
                        nArray[n10] = nArray[n10] + byArray[n3 >>> 12] * n7;
                        n3 += n4;
                    }
                    case 2: {
                        int n11 = n++;
                        nArray[n11] = nArray[n11] + byArray[n3 >>> 12] * n7;
                        n3 += n4;
                    }
                    case 1: {
                        int n12 = n++;
                        nArray[n12] = nArray[n12] + byArray[n3 >>> 12] * n7;
                        n3 += n4;
                    }
                    case 0: {
                        n9 >>>= 2;
                        while (n9-- > 0) {
                            int n13 = n++;
                            nArray[n13] = nArray[n13] + byArray[n3 >>> 12] * n7;
                            int n14 = n++;
                            nArray[n14] = nArray[n14] + byArray[(n3 += n4) >>> 12] * n7;
                            int n15 = n++;
                            nArray[n15] = nArray[n15] + byArray[(n3 += n4) >>> 12] * n7;
                            int n16 = n++;
                            nArray[n16] = nArray[n16] + byArray[(n3 += n4) >>> 12] * n7;
                            n3 += n4;
                        }
                        break;
                    }
                }
                if (n3 < n8) break;
                n3 = -1;
                break;
            }
            case 1: {
                int n17 = this.kAmAjaK;
                int n18 = (n17 - 1 - n3) / n4 + 1;
                while (n < n2) {
                    if (n18 > n2 - n) {
                        n18 = n2 - n;
                    }
                    switch (n18 & 3) {
                        case 3: {
                            int n19 = n++;
                            nArray[n19] = nArray[n19] + byArray[n3 >>> 12] * n7;
                            n3 += n4;
                        }
                        case 2: {
                            int n20 = n++;
                            nArray[n20] = nArray[n20] + byArray[n3 >>> 12] * n7;
                            n3 += n4;
                        }
                        case 1: {
                            int n21 = n++;
                            nArray[n21] = nArray[n21] + byArray[n3 >>> 12] * n7;
                            n3 += n4;
                        }
                        case 0: {
                            n18 >>>= 2;
                            while (n18-- > 0) {
                                int n22 = n++;
                                nArray[n22] = nArray[n22] + byArray[n3 >>> 12] * n7;
                                int n23 = n++;
                                nArray[n23] = nArray[n23] + byArray[(n3 += n4) >>> 12] * n7;
                                int n24 = n++;
                                nArray[n24] = nArray[n24] + byArray[(n3 += n4) >>> 12] * n7;
                                int n25 = n++;
                                nArray[n25] = nArray[n25] + byArray[(n3 += n4) >>> 12] * n7;
                                n3 += n4;
                            }
                            break;
                        }
                    }
                    if (n3 < n17) continue;
                    n3 = this.KAmAjaK + (n3 - this.KAmAjaK) % this.KaMaJAK;
                    n18 = (n17 - 1 - n3) / n4 + 1;
                }
                break;
            }
            case 2: {
                int n26 = n3 >= this.kAmAjaK ? this.kAmAjaK + this.KaMaJAK : this.kAmAjaK;
                int n27 = (n26 - 1 - n3) / n4 + 1;
                while (n < n2) {
                    if (n27 > n2 - n) {
                        n27 = n2 - n;
                    }
                    if (n3 >= this.kAmAjaK) {
                        n3 = this.kAmAjaK + (this.kAmAjaK - n3) - 1;
                        switch (n27 & 3) {
                            case 3: {
                                int n28 = n++;
                                nArray[n28] = nArray[n28] + byArray[n3 >>> 12] * n7;
                                n3 -= n4;
                            }
                            case 2: {
                                int n29 = n++;
                                nArray[n29] = nArray[n29] + byArray[n3 >>> 12] * n7;
                                n3 -= n4;
                            }
                            case 1: {
                                int n30 = n++;
                                nArray[n30] = nArray[n30] + byArray[n3 >>> 12] * n7;
                                n3 -= n4;
                            }
                            case 0: {
                                n27 >>>= 2;
                                while (n27-- > 0) {
                                    int n31 = n++;
                                    nArray[n31] = nArray[n31] + byArray[n3 >>> 12] * n7;
                                    int n32 = n++;
                                    nArray[n32] = nArray[n32] + byArray[(n3 -= n4) >>> 12] * n7;
                                    int n33 = n++;
                                    nArray[n33] = nArray[n33] + byArray[(n3 -= n4) >>> 12] * n7;
                                    int n34 = n++;
                                    nArray[n34] = nArray[n34] + byArray[(n3 -= n4) >>> 12] * n7;
                                    n3 -= n4;
                                }
                                break;
                            }
                        }
                        n3 = this.kAmAjaK + (this.kAmAjaK - n3) + 1;
                    } else {
                        switch (n27 & 3) {
                            case 3: {
                                int n35 = n++;
                                nArray[n35] = nArray[n35] + byArray[n3 >>> 12] * n7;
                                n3 += n4;
                            }
                            case 2: {
                                int n36 = n++;
                                nArray[n36] = nArray[n36] + byArray[n3 >>> 12] * n7;
                                n3 += n4;
                            }
                            case 1: {
                                int n37 = n++;
                                nArray[n37] = nArray[n37] + byArray[n3 >>> 12] * n7;
                                n3 += n4;
                            }
                            case 0: {
                                n27 >>>= 2;
                                while (n27-- > 0) {
                                    int n38 = n++;
                                    nArray[n38] = nArray[n38] + byArray[n3 >>> 12] * n7;
                                    int n39 = n++;
                                    nArray[n39] = nArray[n39] + byArray[(n3 += n4) >>> 12] * n7;
                                    int n40 = n++;
                                    nArray[n40] = nArray[n40] + byArray[(n3 += n4) >>> 12] * n7;
                                    int n41 = n++;
                                    nArray[n41] = nArray[n41] + byArray[(n3 += n4) >>> 12] * n7;
                                    n3 += n4;
                                }
                                break;
                            }
                        }
                    }
                    if (n3 < n26) continue;
                    if ((n3 = this.KAmAjaK + (n3 - this.KAmAjaK) % (this.KaMaJAK << 1)) >= this.kAmAjaK) {
                        n26 = this.kAmAjaK + this.KaMaJAK;
                        n27 = (n26 - 1 - n3) / n4 + 1;
                        continue;
                    }
                    n26 = this.kAmAjaK;
                    n27 = (n26 - 1 - n3) / n4 + 1;
                }
                break;
            }
        }
        return n3;
    }

    mmajmka() {
    }
}
