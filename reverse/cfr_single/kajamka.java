/*
 * Decompiled with CFR 0.152.
 */
import java.io.DataInputStream;
import java.io.InputStream;
import java.net.URL;
import java.util.StringTokenizer;

public class kajamka {
    public static final mmajmmk kAMaJAk(URL uRL) {
        mmajmmk mmajmmk2 = new mmajmmk();
        try {
            InputStream inputStream = uRL.openConnection().getInputStream();
            DataInputStream dataInputStream = new DataInputStream(inputStream);
            boolean bl = false;
            try {
                String string;
                while ((string = dataInputStream.readLine()) != null) {
                    double d;
                    double d2;
                    int n;
                    int n2;
                    String string2;
                    StringTokenizer stringTokenizer = new StringTokenizer(string.trim(), " ");
                    String string3 = string2 = stringTokenizer.hasMoreTokens() ? stringTokenizer.nextToken() : "";
                    if (stringTokenizer.hasMoreTokens() && string2.startsWith("Vertices:") && !bl) {
                        bl = true;
                        n2 = Integer.parseInt(stringTokenizer.nextToken());
                        mmajmmk2.jAkkaMA = new mmjakka[n2];
                        mmajmmk2.JaKKaMA = new kmajkmk[n2];
                        n = 0;
                        while (n < n2) {
                            kmajkmk kmajkmk2;
                            mmjakka mmjakka2;
                            stringTokenizer = new StringTokenizer(dataInputStream.readLine().trim(), " ");
                            stringTokenizer.nextToken();
                            d2 = kajamka.KAMaJAk(stringTokenizer.nextToken());
                            stringTokenizer.nextToken();
                            d = kajamka.KAMaJAk(stringTokenizer.nextToken());
                            stringTokenizer.nextToken();
                            double d3 = kajamka.KAMaJAk(stringTokenizer.nextToken());
                            mmajmmk2.jAkkaMA[n] = mmjakka2 = new mmjakka(d2, d, d3);
                            mmajmmk2.JaKKaMA[n] = kmajkmk2 = new kmajkmk(0.0f, 0.0f);
                            ++n;
                        }
                    }
                    if (stringTokenizer.hasMoreTokens() && string2.startsWith("Vertices:") && bl) {
                        bl = true;
                        n2 = Integer.parseInt(stringTokenizer.nextToken());
                        n = 0;
                        while (n < n2) {
                            stringTokenizer = new StringTokenizer(dataInputStream.readLine().trim(), " ");
                            stringTokenizer.nextToken();
                            d2 = kajamka.KAMaJAk(stringTokenizer.nextToken());
                            stringTokenizer.nextToken();
                            d = kajamka.KAMaJAk(stringTokenizer.nextToken());
                            mmajmmk2.JaKKaMA[n].akKAMAJ = (float)d2;
                            mmajmmk2.JaKKaMA[n].AKKAMAJ = (float)d;
                            ++n;
                        }
                    }
                    if (!stringTokenizer.hasMoreTokens() || !string2.startsWith("Faces:")) continue;
                    n2 = Integer.parseInt(stringTokenizer.nextToken());
                    mmajmmk2.JAkkaMA = new kmaamma[n2];
                    n = 0;
                    while (n < n2) {
                        kmaamma kmaamma2;
                        stringTokenizer = new StringTokenizer(dataInputStream.readLine().trim(), " ");
                        stringTokenizer.nextToken();
                        int n3 = kajamka.kaMaJAk(stringTokenizer.nextToken());
                        stringTokenizer.nextToken();
                        int n4 = kajamka.kaMaJAk(stringTokenizer.nextToken());
                        stringTokenizer.nextToken();
                        int n5 = kajamka.kaMaJAk(stringTokenizer.nextToken());
                        mmajmmk2.JAkkaMA[n] = kmaamma2 = new kmaamma(mmajmmk2, n3, n4, n5, n3, n4, n5);
                        ++n;
                    }
                }
            }
            catch (Exception exception) {
                exception.printStackTrace();
            }
        }
        catch (Exception exception) {
            exception.printStackTrace();
            return null;
        }
        mmajmmk2.kkAMAJa();
        return mmajmmk2;
    }

    public static double KAMaJAk(String string) {
        if (string.charAt(string.length() - 1) == ',') {
            string = string.substring(0, string.length() - 1);
        }
        return Double.valueOf(string);
    }

    public static int kaMaJAk(String string) {
        if (string.charAt(string.length() - 1) == ',') {
            string = string.substring(0, string.length() - 1);
        }
        return Integer.parseInt(string);
    }
}
