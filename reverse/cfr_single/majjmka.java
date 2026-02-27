/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Point;

public class majjmka {
    static int KaMajaK = 1;
    final static int[] kaMajaK = new int[]{1712, 1616, 1524, 1440, 1356, 1280, 1208, 1140, 1076, 1016, 960, 907, 856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480, 453, 428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226, 214, 202, 190, 180, 170, 160, 151, 143, 135, 127, 120, 113, 107, 101, 95, 90, 85, 80, 75, 71, 67, 63, 60, 56, 53, 50, 47, 45, 42, 40, 37, 35, 33, 31, 30, 28};

    /*
     * Enabled force condition propagation
     * Lifted jumps to return sites
     */
    public static maajmmk KAMAJAK(byte[] byArray) {
        if (majjmka.KaMAJAK(byArray)) {
            return majjmka.kaMAJAK(byArray);
        }
        if (majjmka.kAMAJAK(byArray)) {
            return majjmka.KamAJAK(byArray);
        }
        try {
            byte[] byArray2;
            kaaamma kaaamma2 = (kaaamma)Class.forName("muhmu.gl.ZipHoax").newInstance();
            kaaamma2.aMAjaKK(byArray);
            do {
                kaaamma2.AmAjaKK();
                if (kaaamma2.aMaJAKK()) {
                    return null;
                }
                if (kaaamma2.AMAjaKK()) return null;
                byArray2 = kaaamma2.amaJAKK();
                if (!majjmka.KaMAJAK(byArray2)) continue;
                return majjmka.kaMAJAK(byArray2);
            } while (!majjmka.kAMAJAK(byArray2));
            return majjmka.KamAJAK(byArray2);
        }
        catch (Exception exception) {
            exception.printStackTrace();
        }
        return null;
    }

    public static boolean KaMAJAK(byte[] byArray) {
        int n = 0;
        try {
            String string = new String(byArray, 0, n + 1080, 4);
            int n2 = 0;
            if (string.equals("M.K.")) {
                n2 = 4;
            }
            if (string.equals("M!K!")) {
                n2 = 4;
            }
            if (string.equals("FLT4")) {
                n2 = 4;
            }
            if (string.equals("OCTA")) {
                n2 = 8;
            }
            if (string.substring(1, 4).equals("CHN")) {
                n2 = string.charAt(0) - 48;
            }
            if (string.substring(2, 4).equals("CH")) {
                n2 = (string.charAt(0) - 48) * 10 + (string.charAt(1) - 48);
            }
            if (string.substring(0, 3).equals("TDZ")) {
                n2 = string.charAt(3) - 48;
            }
            return n2 > 0;
        }
        catch (Exception exception) {
            return false;
        }
    }

    public static boolean kAMAJAK(byte[] byArray) {
        try {
            String string = new String(byArray, 0, 0, 17);
            int n = kaaakmk.KkAmAja(byArray, 58);
            return string.equals("Extended Module: ") && n >= 260;
        }
        catch (Exception exception) {
            return false;
        }
    }

    public static maajmmk kaMAJAK(byte[] byArray) {
        int n;
        int n2;
        int n3;
        int n4;
        int n5;
        int n6 = 0;
        String string = new String(byArray, 0, n6 + 1080, 4);
        if (KaMajaK > 1) {
            System.out.println("sign=" + string);
        }
        int n7 = 0;
        if (string.equals("M.K.")) {
            n7 = 4;
        }
        if (string.equals("M!K!")) {
            n7 = 4;
        }
        if (string.equals("FLT4")) {
            n7 = 4;
        }
        if (string.equals("OCTA")) {
            n7 = 8;
        }
        if (string.substring(1, 4).equals("CHN")) {
            n7 = string.charAt(0) - 48;
        }
        if (string.substring(2, 4).equals("CH")) {
            n7 = (string.charAt(0) - 48) * 10 + (string.charAt(1) - 48);
        }
        if (string.substring(0, 3).equals("TDZ")) {
            n7 = string.charAt(3) - 48;
        }
        if (n7 <= 0) {
            if (KaMajaK > 1) {
                System.out.println("MOD LOADER ERROR: invalid signature");
            }
            return null;
        }
        String string2 = new String(byArray, 0, n6, 20);
        int n8 = kaaakmk.kKaMaJA(byArray, n6 + 950);
        int n9 = kaaakmk.kKaMaJA(byArray, n6 + 951);
        if (n9 >= n8) {
            n9 = 0;
        }
        kmajmmk kmajmmk2 = new kmajmmk();
        kmajmmk2.KkaMaJa(byArray, n6 + 952, n8);
        kmajmmk2.KkAMaJa(n9);
        int n10 = 0;
        int n11 = 0;
        while (n11 < 128) {
            int n12 = kaaakmk.kKaMaJA(byArray, n6 + 952 + n11);
            if (n12 > n10) {
                n10 = n12;
            }
            ++n11;
        }
        majamka[] majamkaArray = new majamka[++n10];
        if (KaMajaK > 1) {
            System.out.println("numChans=" + n7);
        }
        if (KaMajaK > 1) {
            System.out.println("numPatts=" + n10);
        }
        if (KaMajaK > 0) {
            System.out.println("loading MOD \"" + string2 + "\"");
        }
        n6 += 1084;
        byte[] byArray2 = new byte[31];
        int n13 = 0;
        while (n13 < n10) {
            majamkaArray[n13] = new majamka(64, n7);
            n5 = 0;
            while (n5 < 64) {
                int n14 = 0;
                while (n14 < n7) {
                    int n15 = (kaaakmk.kKaMaJA(byArray, n6) & 0xF) << 8 | kaaakmk.kKaMaJA(byArray, n6 + 1);
                    n4 = (kaaakmk.kKaMaJA(byArray, n6) & 0xF0) + (kaaakmk.kKaMaJA(byArray, n6 + 2) >> 4);
                    n3 = kaaakmk.kKaMaJA(byArray, n6 + 2) & 0xF;
                    n2 = kaaakmk.kKaMaJA(byArray, n6 + 3);
                    if (n4 > 0) {
                        byArray2[n4 - 1] = 1;
                    }
                    n = 0;
                    if (n15 > 0) {
                        n = 0;
                        while (n < 72) {
                            if (n15 >= kaMajaK[n]) break;
                            ++n;
                        }
                        n -= 7;
                    }
                    majamkaArray[n13].KAMAJak(n5, n14, n, n4, n3, n2, 0);
                    n6 += 4;
                    ++n14;
                }
                ++n5;
            }
            ++n13;
        }
        n5 = 31;
        mmajmka[] mmajmkaArray = new mmajmka[n5];
        mmjakmk[] mmjakmkArray = new mmjakmk[n5];
        n4 = 0;
        while (n4 < n5) {
            n3 = 20 + n4 * 30;
            n2 = 2 * kaaakmk.KkamAja(byArray, n3 + 22);
            n = 2 * kaaakmk.KkamAja(byArray, n3 + 26);
            int n16 = 2 * kaaakmk.KkamAja(byArray, n3 + 28);
            String string3 = new String(byArray, 0, n3, 21);
            if (KaMajaK > 1) {
                System.out.println("samplename=" + string3);
            }
            if (n2 == 0) {
                mmajmkaArray[n4] = null;
            } else {
                mmajmkaArray[n4] = new mmajmka();
                mmajmkaArray[n4].kAMajAK(byArray, n6, n2);
            }
            mmjakmkArray[n4] = new mmjakmk();
            mmjakmkArray[n4].KkamajA(mmajmkaArray[n4]);
            if (mmajmkaArray[n4] != null) {
                mmajmkaArray[n4].kAmAJAK(string3);
                mmajmkaArray[n4].KAMajAK(kaaakmk.kKaMaJA(byArray, n3 + 24));
                mmajmkaArray[n4].KaMajAK(kaaakmk.kKaMaJA(byArray, n3 + 25));
                if (n16 > 2) {
                    if (n > n2) {
                        n = n2;
                    }
                    if (n + n16 > n2) {
                        n16 = 20;
                    }
                    mmajmkaArray[n4].kaMajAK(1, n, n16);
                }
            }
            n6 += n2;
            ++n4;
        }
        maajmmk maajmmk2 = new maajmmk();
        maajmmk2.JakkAMa = string2;
        maajmmk2.JAKkAMa = 125;
        maajmmk2.jAKkAMa = 6;
        maajmmk2.jaKkAMa = n7;
        maajmmk2.jakKama = kmajmmk2;
        maajmmk2.JAkKama = majamkaArray;
        maajmmk2.jAkKama = mmajmkaArray;
        maajmmk2.JaKkAMa = mmjakmkArray;
        kmajmmk2.kKamaJa(maajmmk2);
        return maajmmk2;
    }

    public static maajmmk KamAJAK(byte[] byArray) {
        int n;
        int n2;
        int n3;
        int n4;
        int n5;
        int n6;
        int n7;
        int n8;
        int n9;
        int n10;
        String string = new String(byArray, 0, 0, 17);
        String string2 = new String(byArray, 0, 17, 20);
        String string3 = new String(byArray, 0, 38, 20);
        int n11 = kaaakmk.KkAmAja(byArray, 58);
        int n12 = kaaakmk.kKamAja(byArray, 60);
        int n13 = kaaakmk.KkAmAja(byArray, 64);
        int n14 = kaaakmk.KkAmAja(byArray, 66);
        int n15 = kaaakmk.KkAmAja(byArray, 68);
        int n16 = kaaakmk.KkAmAja(byArray, 70);
        int n17 = kaaakmk.KkAmAja(byArray, 72);
        int n18 = kaaakmk.KkAmAja(byArray, 74);
        int n19 = kaaakmk.KkAmAja(byArray, 76);
        int n20 = kaaakmk.KkAmAja(byArray, 78);
        if (!string.equals("Extended Module: ") || n11 < 260) {
            if (KaMajaK > 1) {
                System.out.println("ERROR LOADING XM: Invalid module");
            }
            return null;
        }
        maajmmk maajmmk2 = new maajmmk();
        maajmmk2.jAKkAMa = n19;
        maajmmk2.JAKkAMa = n20;
        kmajmmk kmajmmk2 = new kmajmmk();
        kmajmmk2.KkAMaJa(n14);
        if (KaMajaK > 1) {
            System.out.println("initializing song.");
        }
        int n21 = n16;
        int n22 = 0;
        int n23 = 0;
        while (n23 < n13) {
            int n24 = kaaakmk.kKaMaJA(byArray, 80 + n23);
            if (n24 > n22) {
                n22 = n24;
            }
            ++n23;
        }
        if (n22 >= n16) {
            n16 = n22 + 1;
        }
        kmajmmk2.KkaMaJa(byArray, 80, n13);
        majamka[] majamkaArray = new majamka[n16];
        majamka majamka2 = new majamka(64, n15);
        int n25 = 0;
        int n26 = n12 + 60;
        if (KaMajaK > 1) {
            System.out.println("signature=" + string);
        }
        if (KaMajaK > 1) {
            System.out.println("name=" + string2);
        }
        if (KaMajaK > 1) {
            System.out.println("trackerName=" + string3);
        }
        if (KaMajaK > 1) {
            System.out.println("version=" + n11);
        }
        if (KaMajaK > 1) {
            System.out.println("headerSize=" + n12);
        }
        if (KaMajaK > 1) {
            System.out.println("songlen=" + n13);
        }
        if (KaMajaK > 1) {
            System.out.println("restart=" + n14);
        }
        if (KaMajaK > 1) {
            System.out.println("numChans=" + n15);
        }
        if (KaMajaK > 1) {
            System.out.println("numPatts=" + n16);
        }
        if (KaMajaK > 1) {
            System.out.println("numInsts=" + n17);
        }
        if (KaMajaK > 1) {
            System.out.println("flags=" + n18);
        }
        if (KaMajaK > 1) {
            System.out.println("speed=" + n19);
        }
        if (KaMajaK > 1) {
            System.out.println("tempo=" + n20);
        }
        if (KaMajaK > 0) {
            System.out.println("loading XM \"" + string2 + "\"");
        }
        maajmmk2.JakkAMa = string2;
        int n27 = n17;
        int n28 = 0;
        while (n28 < n16) {
            if (n28 < n21) {
                int n29 = kaaakmk.kKamAja(byArray, n26);
                kaaakmk.kKaMaJA(byArray, n26 + 4);
                int n30 = kaaakmk.KkAmAja(byArray, n26 + 5);
                n10 = kaaakmk.KkAmAja(byArray, n26 + 7);
                majamkaArray[n28] = new majamka(n30, n15);
                n9 = n26 + n29;
                n8 = 0;
                while (n8 < n30) {
                    n7 = 0;
                    while (n7 < n15) {
                        n6 = 0;
                        n5 = 0;
                        n4 = 0;
                        n3 = 0;
                        n2 = 0;
                        if ((n = byArray[n9++]) < 0) {
                            if ((n & 1) != 0) {
                                n6 = byArray[n9++];
                            }
                        } else {
                            n6 = n;
                            n = 127;
                        }
                        if ((n & 2) != 0) {
                            n5 = byArray[n9++];
                        }
                        if ((n & 4) != 0) {
                            n2 = byArray[n9++];
                        }
                        if ((n & 8) != 0) {
                            n4 = byArray[n9++];
                        }
                        if ((n & 0x10) != 0) {
                            n3 = byArray[n9++];
                        }
                        majamkaArray[n28].KAMAJak(n8, n7, n6, n5, n4, n3, n2);
                        ++n7;
                    }
                    ++n8;
                }
                n26 += n29;
                n26 += n10;
            } else {
                majamkaArray[n28] = majamka2;
            }
            ++n28;
        }
        mmjakmk[] mmjakmkArray = new mmjakmk[n27];
        mmajmka[] mmajmkaArray = new mmajmka[n17 * 20];
        n10 = 0;
        while (n10 < n17 * 20) {
            mmajmkaArray[n10] = new mmajmka();
            ++n10;
        }
        n9 = 0;
        n8 = 0;
        n7 = 0;
        n = 0;
        n6 = 0;
        n5 = 0;
        while (n5 < n17) {
            int n31;
            int n32;
            int n33;
            int n34;
            int n35;
            int n36;
            int n37;
            int n38;
            byte by;
            byte by2;
            int n39;
            int n40;
            byte by3;
            int n41;
            int n42;
            n4 = n25;
            if (n5 < n27) {
                mmjakmkArray[n5] = new mmjakmk();
                mmjakmk mmjakmk2 = mmjakmkArray[n5];
                n7 = kaaakmk.kKamAja(byArray, n26);
                String string4 = new String(byArray, 0, n26 + 4, 22);
                kaaakmk.kKaMaJA(byArray, n26 + 26);
                n8 = kaaakmk.KkAmAja(byArray, n26 + 27);
                if (KaMajaK > 1) {
                    System.out.println("samplename=" + string4);
                }
                n26 += 29;
                n6 = n7 - 29;
                if (n8 != 0) {
                    n42 = 0;
                    boolean bl = false;
                    n41 = 0;
                    by3 = byArray[n26 + 204];
                    if ((by3 & 1) != 0) {
                        n42 = 1;
                    }
                    if ((by3 & 4) != 0) {
                        bl = true;
                    }
                    if ((by3 & 2) != 0) {
                        n41 = 1;
                    }
                    n40 = byArray[n26 + 196];
                    n39 = byArray[n26 + 198];
                    by2 = byArray[n26 + 199];
                    by = byArray[n26 + 200];
                    Point[] pointArray = new Point[n40];
                    n38 = 0;
                    while (n38 < n40) {
                        n37 = kaaakmk.KkAmAja(byArray, n26 + 100 + n38 * 4);
                        n36 = kaaakmk.KkAmAja(byArray, n26 + 100 + n38 * 4 + 2);
                        pointArray[n38] = new Point(n37, n36);
                        ++n38;
                    }
                    mmjakmk2.akkamAJ = new kajamma(pointArray, n39, (int)by2, (int)by);
                    mmjakmk2.akkamAJ.majAKKa = n42;
                    mmjakmk2.akkamAJ.MAjAKKa = bl;
                    mmjakmk2.akkamAJ.mAjAKKa = n41;
                    n42 = 0;
                    bl = false;
                    n41 = 0;
                    by3 = byArray[n26 + 205];
                    if ((by3 & 1) != 0) {
                        n42 = 1;
                    }
                    if ((by3 & 4) != 0) {
                        bl = true;
                    }
                    if ((by3 & 2) != 0) {
                        n41 = 1;
                    }
                    n37 = byArray[n26 + 197];
                    n36 = byArray[n26 + 201];
                    n35 = byArray[n26 + 202];
                    n34 = byArray[n26 + 203];
                    pointArray = new Point[n37];
                    n33 = 0;
                    while (n33 < n37) {
                        n32 = kaaakmk.KkAmAja(byArray, n26 + 148 + n33 * 4);
                        n31 = kaaakmk.KkAmAja(byArray, n26 + 148 + n33 * 4 + 2);
                        pointArray[n33] = new Point(n32, n31);
                        ++n33;
                    }
                    mmjakmk2.AKkamAJ = new kajamma(pointArray, n36, n35, n34);
                    mmjakmk2.AKkamAJ.majAKKa = n42;
                    mmjakmk2.AKkamAJ.MAjAKKa = bl;
                    mmjakmk2.AKkamAJ.mAjAKKa = n41;
                    n32 = kaaakmk.KkAmAja(byArray, n26 + 210);
                    mmjakmk2.akkamAJ.amAjakK(n32);
                    n31 = kaaakmk.kKaMaJA(byArray, n26 + 206);
                    int n43 = kaaakmk.kKaMaJA(byArray, n26 + 207);
                    int n44 = kaaakmk.kKaMaJA(byArray, n26 + 208);
                    int n45 = kaaakmk.kKaMaJA(byArray, n26 + 209);
                    mmjakmk2.AKKAmAJ = n31;
                    mmjakmk2.akKAmAJ = n43;
                    mmjakmk2.AkKAmAJ = (n44 & 0xF) << 4;
                    mmjakmk2.aKkamAJ = (n45 & 0x3F) << 2;
                    if (n44 != 0) {
                        if (KaMajaK > 1) {
                            System.out.println("dep=" + n44);
                        }
                        if (KaMajaK > 1) {
                            System.out.println("swe=" + n43);
                        }
                        if (KaMajaK > 1) {
                            System.out.println("spe=" + n45);
                        }
                        if (KaMajaK > 1) {
                            System.out.println("typ=" + n31);
                        }
                    }
                    n = kaaakmk.kKamAja(byArray, n26);
                    int n46 = 0;
                    while (n46 < 96) {
                        int n47 = kaaakmk.kKaMaJA(byArray, n26 + 4 + n46);
                        mmjakmk2.AkkamAJ[n46] = mmajmkaArray[n47 + n4];
                        if (n47 >= n9) {
                            n9 = n47 + 1;
                        }
                        ++n46;
                    }
                    n26 += 214;
                    n6 -= 214;
                } else {
                    n9 = 0;
                }
            } else {
                n6 = 0;
            }
            n3 = (n26 += n6) + n8 * n;
            n2 = 0;
            n42 = 0;
            while (n42 < n8) {
                String string5 = new String(byArray, 0, n26 + 18, 22);
                n41 = kaaakmk.kKamAja(byArray, n26);
                mmajmkaArray[n25].kAmAJAK(string5);
                by3 = (byte)(kaaakmk.kKaMaJA(byArray, n26 + 14) & 3);
                n40 = kaaakmk.kKamAja(byArray, n26 + 4);
                n39 = kaaakmk.kKamAja(byArray, n26 + 8);
                by2 = byArray[n26 + 16];
                by = byArray[n26 + 12];
                byte by4 = byArray[n26 + 13];
                mmajmkaArray[n25].KAmAJAK(by2);
                mmajmkaArray[n25].KaMajAK(by);
                mmajmkaArray[n25].KAMajAK(by4);
                n38 = byArray[n26 + 15];
                n37 = n38 & 0xFF;
                mmajmkaArray[n25].KAmajAK(n37);
                n36 = 0;
                n34 = n35 = n3 + n2;
                n33 = n41;
                n32 = n35;
                if ((kaaakmk.kKaMaJA(byArray, n26 + 14) & 0x10) != 0) {
                    n33 /= 2;
                    n40 /= 2;
                    n39 /= 2;
                    n31 = 0;
                    while (n31 < n41 / 2) {
                        byArray[n34] = (byte)((n36 += kaaakmk.KKAmAja(byArray, n35)) >>> 8);
                        n35 += 2;
                        ++n34;
                        ++n31;
                    }
                } else {
                    n31 = 0;
                    while (n31 < n41) {
                        n36 += byArray[n35];
                        n36 = (byte)n36;
                        byArray[n35] = (byte)n36;
                        ++n35;
                        ++n31;
                    }
                }
                mmajmkaArray[n25].kAMajAK(byArray, n32, n33);
                mmajmkaArray[n25].kaMajAK(by3, n40, n39);
                n2 += n41;
                ++n25;
                n26 += n;
                ++n42;
            }
            n26 += n2;
            ++n5;
        }
        maajmmk2.jaKkAMa = n15;
        maajmmk2.jakKama = kmajmmk2;
        maajmmk2.JAkKama = majamkaArray;
        maajmmk2.jAkKama = mmajmkaArray;
        maajmmk2.JaKkAMa = mmjakmkArray;
        kmajmmk2.kKamaJa(maajmmk2);
        return maajmmk2;
    }
}
