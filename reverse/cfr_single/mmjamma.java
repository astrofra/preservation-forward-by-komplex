/*
 * Decompiled with CFR 0.152.
 */
import java.applet.Applet;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Insets;
import java.io.DataInputStream;
import java.net.URL;
import java.util.Vector;

public abstract class mmjamma
extends Applet {
    boolean mAJAkka = false;
    public boolean MajAkka;
    public Thread majAkka;
    mmjamma MAjAkka;

    public mmjamma() {
        mmaakma.MAjaKkA = this;
    }

    public boolean mAjAkKa() {
        return this.MajAkka;
    }

    public void AMAjAkK() {
        if (this.MajAkka) {
            this.majAkka.resume();
            this.MajAkka = false;
            return;
        }
        this.majAkka.suspend();
        this.MajAkka = true;
    }

    public void amAJAkK() {
        this.aMAjAkK(null);
    }

    public void aMAjAkK(String[] stringArray) {
        this.aMajAkK(stringArray, 320, 240);
    }

    public void aMajAkK(String[] stringArray, int n, int n2) {
        kaajkmk kaajkmk2 = new kaajkmk();
        this.MAjAkka = this;
        this.MAjAkka.setStub(kaajkmk2);
        if (stringArray != null) {
            int n3 = 0;
            while (n3 < stringArray.length - 1) {
                kaajkmk2.KKAmajA(stringArray[n3], stringArray[n3 + 1]);
                n3 += 2;
            }
        }
        kajjkmk kajjkmk2 = null;
        kajjkmk2 = new kajjkmk(this.getClass().getName());
        kajjkmk2.show();
        ((Component)kajjkmk2).hide();
        Insets insets = kajjkmk2.insets();
        int n4 = insets.left + insets.right + n;
        int n5 = insets.top + insets.bottom + n2;
        Dimension dimension = this.getToolkit().getScreenSize();
        int n6 = (dimension.width - n) / 2;
        int n7 = (dimension.height - n2) / 2;
        ((Component)kajjkmk2).reshape(n6, n7, n4, n5);
        kajjkmk2.add("Center", this.MAjAkka);
        this.MAjAkka.mAJAkka = true;
        this.MAjAkka.init();
        kajjkmk2.show();
        this.MAjAkka.start();
    }

    public void finalize() {
        this.MAjAkka.stop();
    }

    public void start() {
        if (this instanceof Runnable) {
            Runnable runnable = (Runnable)((Object)this);
            if (this.majAkka == null) {
                this.majAkka = new Thread(runnable, "Muhmu BaseApplet runner - " + runnable);
                this.majAkka.start();
            }
        }
    }

    public void stop() {
        if (this instanceof Runnable && this.majAkka != null) {
            this.majAkka.stop();
            this.majAkka = null;
            System.out.println("baseapplet: runner stopped");
        }
    }

    public byte[] AMajAkK(URL uRL) {
        Vector<byte[]> vector = new Vector<byte[]>(10);
        int n = 65536;
        int n2 = 0;
        try {
            DataInputStream dataInputStream = new DataInputStream(uRL.openConnection().getInputStream());
            int n3 = 0;
            block2: while (n3 != -1) {
                byte[] byArray = new byte[n];
                int n4 = 0;
                vector.addElement(byArray);
                while (n4 < n) {
                    n3 = dataInputStream.read(byArray, n4, n - n4);
                    if (n3 < 0) continue block2;
                    n4 += n3;
                    n2 += n3;
                }
            }
        }
        catch (Exception exception) {
            vector.removeAllElements();
            vector = null;
            return null;
        }
        int n5 = 0;
        byte[] byArray = new byte[n2];
        int n6 = 0;
        while (n5 < n2) {
            byte[] byArray2 = (byte[])vector.elementAt(n6++);
            if (n2 - n5 >= n) {
                System.arraycopy(byArray2, 0, byArray, n5, n);
                n5 += n;
                continue;
            }
            System.arraycopy(byArray2, 0, byArray, n5, n2 - n5);
            n5 = n2;
        }
        vector.removeAllElements();
        vector = null;
        return byArray;
    }

    public URL AMAJAkK(String string) {
        return this.AmaJAkK(this.getCodeBase(), string);
    }

    public URL amAjAkK(String string) {
        return this.AmaJAkK(this.getDocumentBase(), string);
    }

    public URL AmaJAkK(URL uRL, String string) {
        if (this.mAJAkka) {
            if ((string = string.replace('\\', '/')).charAt(0) == '/') {
                if (uRL.getFile().charAt(2) == ':') {
                    try {
                        return new URL("file:///" + uRL.getFile().substring(0, 3) + string);
                    }
                    catch (Exception exception) {
                        exception.printStackTrace();
                        return null;
                    }
                }
                try {
                    return new URL("file:///" + string);
                }
                catch (Exception exception) {
                    exception.printStackTrace();
                    return null;
                }
            }
            if (string.charAt(1) == ':') {
                try {
                    return new URL("file:///" + string);
                }
                catch (Exception exception) {
                    exception.printStackTrace();
                    return null;
                }
            }
            try {
                return new URL(uRL, string);
            }
            catch (Exception exception) {
                exception.printStackTrace();
                return null;
            }
        }
        try {
            return new URL(uRL, string);
        }
        catch (Exception exception) {
            exception.printStackTrace();
            return null;
        }
    }

    public String getParameter(String string) {
        return this.AmAJAkK(string, null);
    }

    public String AmAJAkK(String string, String string2) {
        String string3 = null;
        string3 = super.getParameter(string);
        if (string3 == null) {
            string3 = string2;
        }
        return string3;
    }

    public int aMAJAkK(String string, int n) {
        try {
            return Integer.parseInt(this.AmAJAkK(string, null));
        }
        catch (Exception exception) {
            return n;
        }
    }

    public long AmAjAkK(String string, long l) {
        try {
            return Long.parseLong(this.AmAJAkK(string, null));
        }
        catch (Exception exception) {
            return l;
        }
    }

    public float AmajAkK(String string, float f) {
        try {
            return Float.valueOf(this.AmAJAkK(string, null)).floatValue();
        }
        catch (Exception exception) {
            return f;
        }
    }

    public double amajAkK(String string, double d) {
        try {
            return Double.valueOf(this.AmAJAkK(string, null));
        }
        catch (Exception exception) {
            return d;
        }
    }
}
