// 
// Decompiled by Procyon v0.6.0
// 

package muhmu.hifi.device;

final class ULaw
{
    static final int sizeROR = 13;
    static final int size = 8192;
    static final int ROR2 = 3;
    static final byte[] encodingTable;
    static final int encodingTable_base = 4096;
    static final int ULAW_BIAS = 132;
    static final int ULAW_CLIP = 32635;
    static final int[] ULAW_LUT;
    
    static final void initEncodingTable() {
        int n = 0;
        ULaw.ULAW_LUT[n++] = 0;
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 1 << i; ++j) {
                ULaw.ULAW_LUT[n++] = i;
            }
        }
        double n2 = -32635.0;
        final double n3 = -n2 * 2.0 / 8192.0;
        for (int k = 0; k < 8192; ++k) {
            ULaw.encodingTable[k] = encode((int)n2);
            n2 += n3;
        }
    }
    
    static final void encodeBlock(final int[] array, int i, final byte[] array2, int n, final int n2) {
        if (i < 0 || i >= array.length) {
            return;
        }
        if (i + n2 < 0 || i + n2 > array.length) {
            return;
        }
        if (n < 0 || n >= array2.length) {
            return;
        }
        if (n + n2 < 0 || n + n2 > array2.length) {
            return;
        }
        int j = 0;
        final byte encode = encode(-32635);
        final byte encode2 = encode(32635);
        final int n3 = i + n2;
        if (i == n) {
            while (i < n3) {
                try {
                    while (i < n3) {
                        int n4 = (array[i] >> 3) + 4096;
                        if (n4 >> 13 != 0) {
                            n4 = ((n4 < 0) ? 0 : 8191);
                        }
                        array2[i] = ULaw.encodingTable[n4];
                        ++i;
                    }
                }
                catch (final Exception ex) {
                    array2[i] = ((array[i] < 0) ? encode : encode2);
                    ++j;
                    ++i;
                }
            }
        }
        else {
            System.out.println("m\u00e4ski 3");
            while (i < n3) {
                try {
                    while (i < n3) {
                        int n5 = (array[i] >> 3) + 4096;
                        if (n5 >> 13 != 0) {
                            n5 = ((n5 < 0) ? 0 : 8191);
                        }
                        array2[n] = ULaw.encodingTable[n5];
                        ++i;
                        ++n;
                    }
                }
                catch (final Exception ex2) {
                    array2[i] = ((array[i] < 0) ? encode : encode2);
                    ++j;
                    ++i;
                    ++n;
                    System.out.println("m\u00e4ski 2");
                    return;
                }
            }
        }
        if (j > 0) {
            System.out.println("cropped " + j);
        }
    }
    
    static final byte encode(int n) {
        final int[] ulaw_LUT = ULaw.ULAW_LUT;
        if (n >= 0) {
            if (n > 32635) {
                n = 32635;
            }
            n += 132;
            final int n2 = ulaw_LUT[n >> 7];
            n = ((n2 << 4 | (n >> n2 + 3 & 0xF)) ^ 0xFF);
        }
        else {
            n = -n;
            if (n > 32635) {
                n = 32635;
            }
            n += 132;
            final int n3 = ulaw_LUT[n >> 7];
            n = ((n3 << 4 | (n >> n3 + 3 & 0xF)) ^ 0x7F);
        }
        return (byte)n;
    }
    
    static {
        encodingTable = new byte[8192];
        ULAW_LUT = new int[256];
    }
}
