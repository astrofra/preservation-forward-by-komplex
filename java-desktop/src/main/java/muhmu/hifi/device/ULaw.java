/*
 * Decompiled with CFR 0.152.
 */
package muhmu.hifi.device;

final class ULaw {
    final static int sizeROR = 13;
    final static int size = 8192;
    final static int ROR2 = 3;
    final static byte[] encodingTable = new byte[8192];
    final static int encodingTable_base = 4096;
    final static int ULAW_BIAS = 132;
    final static int ULAW_CLIP = 32635;
    final static int[] ULAW_LUT = new int[256];

    static final void initEncodingTable() {
        int n = 0;
        ULaw.ULAW_LUT[n++] = 0;
        int n2 = 0;
        while (n2 < 8) {
            int n3 = 0;
            while (n3 < 1 << n2) {
                ULaw.ULAW_LUT[n++] = n2;
                ++n3;
            }
            ++n2;
        }
        double d = -32635.0;
        double d2 = -d * 2.0 / 8192.0;
        int n4 = 0;
        while (n4 < 8192) {
            ULaw.encodingTable[n4] = ULaw.encode((int)d);
            d += d2;
            ++n4;
        }
    }

    static final void encodeBlock(int[] nArray, int n, byte[] byArray, int n2, int n3) {
        if (n < 0 || n >= nArray.length) {
            return;
        }
        if (n + n3 < 0 || n + n3 > nArray.length) {
            return;
        }
        if (n2 < 0 || n2 >= byArray.length) {
            return;
        }
        if (n2 + n3 < 0 || n2 + n3 > byArray.length) {
            return;
        }
        int n4 = 0;
        byte by = ULaw.encode(-32635);
        byte by2 = ULaw.encode(32635);
        int n5 = n + n3;
        if (n == n2) {
            while (n < n5) {
                try {
                    while (n < n5) {
                        int n6 = (nArray[n] >> 3) + 4096;
                        if (n6 >> 13 != 0) {
                            n6 = n6 < 0 ? 0 : 8191;
                        }
                        byArray[n] = encodingTable[n6];
                        ++n;
                    }
                }
                catch (Exception exception) {
                    byArray[n] = nArray[n] < 0 ? by : by2;
                    ++n4;
                    ++n;
                }
            }
        } else {
            System.out.println("m\u00e4ski 3");
            while (n < n5) {
                try {
                    while (n < n5) {
                        int n7 = (nArray[n] >> 3) + 4096;
                        if (n7 >> 13 != 0) {
                            n7 = n7 < 0 ? 0 : 8191;
                        }
                        byArray[n2] = encodingTable[n7];
                        ++n;
                        ++n2;
                    }
                }
                catch (Exception exception) {
                    byArray[n] = nArray[n] < 0 ? by : by2;
                    ++n4;
                    ++n;
                    ++n2;
                    System.out.println("m\u00e4ski 2");
                    return;
                }
            }
        }
        if (n4 > 0) {
            System.out.println("cropped " + n4);
        }
    }

    static final byte encode(int n) {
        int[] nArray = ULAW_LUT;
        if (n >= 0) {
            if (n > 32635) {
                n = 32635;
            }
            int n2 = nArray[(n += 132) >> 7];
            int n3 = n >> n2 + 3 & 0xF;
            n = (n2 << 4 | n3) ^ 0xFF;
        } else {
            if ((n = -n) > 32635) {
                n = 32635;
            }
            int n4 = nArray[(n += 132) >> 7];
            int n5 = n >> n4 + 3 & 0xF;
            n = (n4 << 4 | n5) ^ 0x7F;
        }
        return (byte)n;
    }

    ULaw() {
    }
}
