/*
 * Decompiled with CFR 0.152.
 */
public final class kaaakmk {
    final static int jAkKamA = 128;
    final static int[] JaKkAMA;

    static final int kKaMaJA(byte[] byArray, int n) {
        return JaKkAMA[byArray[n] + 128];
    }

    static final int KkAmAja(byte[] byArray, int n) {
        return JaKkAMA[byArray[n++] + 128] + (JaKkAMA[byArray[n++] + 128] << 8);
    }

    static final int KkamAja(byte[] byArray, int n) {
        return (JaKkAMA[byArray[n++] + 128] << 8) + JaKkAMA[byArray[n++] + 128];
    }

    static final int kKamAja(byte[] byArray, int n) {
        return JaKkAMA[byArray[n++] + 128] + (JaKkAMA[byArray[n++] + 128] << 8) + (JaKkAMA[byArray[n++] + 128] << 16) + (JaKkAMA[byArray[n++] + 128] << 24);
    }

    static final int kKAmAja(byte[] byArray, int n) {
        return byArray[n];
    }

    static final int KKAmAja(byte[] byArray, int n) {
        return (byArray[n] & 0xFF) + (byArray[n + 1] << 8);
    }

    static final int KKamAja(byte[] byArray, int n) {
        return (byArray[n + 1] & 0xFF) + (byArray[n] << 8);
    }

    static final int kkAmAja(byte by) {
        return JaKkAMA[by + 128];
    }

    static final int kkamAja(int n) {
        return JaKkAMA[(byte)n + 128];
    }

    static {
        int[] nArray = new int[256];
        nArray[0] = 128;
        nArray[1] = 129;
        nArray[2] = 130;
        nArray[3] = 131;
        nArray[4] = 132;
        nArray[5] = 133;
        nArray[6] = 134;
        nArray[7] = 135;
        nArray[8] = 136;
        nArray[9] = 137;
        nArray[10] = 138;
        nArray[11] = 139;
        nArray[12] = 140;
        nArray[13] = 141;
        nArray[14] = 142;
        nArray[15] = 143;
        nArray[16] = 144;
        nArray[17] = 145;
        nArray[18] = 146;
        nArray[19] = 147;
        nArray[20] = 148;
        nArray[21] = 149;
        nArray[22] = 150;
        nArray[23] = 151;
        nArray[24] = 152;
        nArray[25] = 153;
        nArray[26] = 154;
        nArray[27] = 155;
        nArray[28] = 156;
        nArray[29] = 157;
        nArray[30] = 158;
        nArray[31] = 159;
        nArray[32] = 160;
        nArray[33] = 161;
        nArray[34] = 162;
        nArray[35] = 163;
        nArray[36] = 164;
        nArray[37] = 165;
        nArray[38] = 166;
        nArray[39] = 167;
        nArray[40] = 168;
        nArray[41] = 169;
        nArray[42] = 170;
        nArray[43] = 171;
        nArray[44] = 172;
        nArray[45] = 173;
        nArray[46] = 174;
        nArray[47] = 175;
        nArray[48] = 176;
        nArray[49] = 177;
        nArray[50] = 178;
        nArray[51] = 179;
        nArray[52] = 180;
        nArray[53] = 181;
        nArray[54] = 182;
        nArray[55] = 183;
        nArray[56] = 184;
        nArray[57] = 185;
        nArray[58] = 186;
        nArray[59] = 187;
        nArray[60] = 188;
        nArray[61] = 189;
        nArray[62] = 190;
        nArray[63] = 191;
        nArray[64] = 192;
        nArray[65] = 193;
        nArray[66] = 194;
        nArray[67] = 195;
        nArray[68] = 196;
        nArray[69] = 197;
        nArray[70] = 198;
        nArray[71] = 199;
        nArray[72] = 200;
        nArray[73] = 201;
        nArray[74] = 202;
        nArray[75] = 203;
        nArray[76] = 204;
        nArray[77] = 205;
        nArray[78] = 206;
        nArray[79] = 207;
        nArray[80] = 208;
        nArray[81] = 209;
        nArray[82] = 210;
        nArray[83] = 211;
        nArray[84] = 212;
        nArray[85] = 213;
        nArray[86] = 214;
        nArray[87] = 215;
        nArray[88] = 216;
        nArray[89] = 217;
        nArray[90] = 218;
        nArray[91] = 219;
        nArray[92] = 220;
        nArray[93] = 221;
        nArray[94] = 222;
        nArray[95] = 223;
        nArray[96] = 224;
        nArray[97] = 225;
        nArray[98] = 226;
        nArray[99] = 227;
        nArray[100] = 228;
        nArray[101] = 229;
        nArray[102] = 230;
        nArray[103] = 231;
        nArray[104] = 232;
        nArray[105] = 233;
        nArray[106] = 234;
        nArray[107] = 235;
        nArray[108] = 236;
        nArray[109] = 237;
        nArray[110] = 238;
        nArray[111] = 239;
        nArray[112] = 240;
        nArray[113] = 241;
        nArray[114] = 242;
        nArray[115] = 243;
        nArray[116] = 244;
        nArray[117] = 245;
        nArray[118] = 246;
        nArray[119] = 247;
        nArray[120] = 248;
        nArray[121] = 249;
        nArray[122] = 250;
        nArray[123] = 251;
        nArray[124] = 252;
        nArray[125] = 253;
        nArray[126] = 254;
        nArray[127] = 255;
        nArray[129] = 1;
        nArray[130] = 2;
        nArray[131] = 3;
        nArray[132] = 4;
        nArray[133] = 5;
        nArray[134] = 6;
        nArray[135] = 7;
        nArray[136] = 8;
        nArray[137] = 9;
        nArray[138] = 10;
        nArray[139] = 11;
        nArray[140] = 12;
        nArray[141] = 13;
        nArray[142] = 14;
        nArray[143] = 15;
        nArray[144] = 16;
        nArray[145] = 17;
        nArray[146] = 18;
        nArray[147] = 19;
        nArray[148] = 20;
        nArray[149] = 21;
        nArray[150] = 22;
        nArray[151] = 23;
        nArray[152] = 24;
        nArray[153] = 25;
        nArray[154] = 26;
        nArray[155] = 27;
        nArray[156] = 28;
        nArray[157] = 29;
        nArray[158] = 30;
        nArray[159] = 31;
        nArray[160] = 32;
        nArray[161] = 33;
        nArray[162] = 34;
        nArray[163] = 35;
        nArray[164] = 36;
        nArray[165] = 37;
        nArray[166] = 38;
        nArray[167] = 39;
        nArray[168] = 40;
        nArray[169] = 41;
        nArray[170] = 42;
        nArray[171] = 43;
        nArray[172] = 44;
        nArray[173] = 45;
        nArray[174] = 46;
        nArray[175] = 47;
        nArray[176] = 48;
        nArray[177] = 49;
        nArray[178] = 50;
        nArray[179] = 51;
        nArray[180] = 52;
        nArray[181] = 53;
        nArray[182] = 54;
        nArray[183] = 55;
        nArray[184] = 56;
        nArray[185] = 57;
        nArray[186] = 58;
        nArray[187] = 59;
        nArray[188] = 60;
        nArray[189] = 61;
        nArray[190] = 62;
        nArray[191] = 63;
        nArray[192] = 64;
        nArray[193] = 65;
        nArray[194] = 66;
        nArray[195] = 67;
        nArray[196] = 68;
        nArray[197] = 69;
        nArray[198] = 70;
        nArray[199] = 71;
        nArray[200] = 72;
        nArray[201] = 73;
        nArray[202] = 74;
        nArray[203] = 75;
        nArray[204] = 76;
        nArray[205] = 77;
        nArray[206] = 78;
        nArray[207] = 79;
        nArray[208] = 80;
        nArray[209] = 81;
        nArray[210] = 82;
        nArray[211] = 83;
        nArray[212] = 84;
        nArray[213] = 85;
        nArray[214] = 86;
        nArray[215] = 87;
        nArray[216] = 88;
        nArray[217] = 89;
        nArray[218] = 90;
        nArray[219] = 91;
        nArray[220] = 92;
        nArray[221] = 93;
        nArray[222] = 94;
        nArray[223] = 95;
        nArray[224] = 96;
        nArray[225] = 97;
        nArray[226] = 98;
        nArray[227] = 99;
        nArray[228] = 100;
        nArray[229] = 101;
        nArray[230] = 102;
        nArray[231] = 103;
        nArray[232] = 104;
        nArray[233] = 105;
        nArray[234] = 106;
        nArray[235] = 107;
        nArray[236] = 108;
        nArray[237] = 109;
        nArray[238] = 110;
        nArray[239] = 111;
        nArray[240] = 112;
        nArray[241] = 113;
        nArray[242] = 114;
        nArray[243] = 115;
        nArray[244] = 116;
        nArray[245] = 117;
        nArray[246] = 118;
        nArray[247] = 119;
        nArray[248] = 120;
        nArray[249] = 121;
        nArray[250] = 122;
        nArray[251] = 123;
        nArray[252] = 124;
        nArray[253] = 125;
        nArray[254] = 126;
        nArray[255] = 127;
        JaKkAMA = nArray;
    }
}
