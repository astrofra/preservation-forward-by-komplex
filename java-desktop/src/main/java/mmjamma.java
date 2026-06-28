/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Dimension;
import java.awt.Insets;
import java.awt.Panel;
import java.io.DataInputStream;
import java.net.URL;
import java.util.Vector;

public abstract class mmjamma
extends Panel {
    boolean mAJAkka = false;
    public boolean MajAkka;
    public Thread majAkka;
    mmjamma MAjAkka;
    private final kaajkmk KAjAkka = new kaajkmk();

    public mmjamma() {
        mmaakma.MAjaKkA = this;
    }

    public boolean mAjAkKa() {
        return this.MajAkka;
    }

    public void AMAjAkK() {
        this.MajAkka = !this.MajAkka;
    }

    public void amAJAkK() {
        this.aMAjAkK(null);
    }

    public void aMAjAkK(String[] stringArray) {
        this.aMajAkK(stringArray, 320, 240);
    }

    public void aMajAkK(String[] stringArray, int n, int n2) {
        this.MAjAkka = this;
        this.KAjAkka.setBaseUrl(kmjakka.kaMAjaK());
        if (stringArray != null) {
            int n3 = 0;
            while (n3 < stringArray.length - 1) {
                this.KAjAkka.KKAmajA(stringArray[n3], stringArray[n3 + 1]);
                n3 += 2;
            }
        }
        ForwardLaunchConfiguration forwardLaunchConfiguration = ForwardLaunchConfiguration.fromParameters(this);
        ForwardHostFrame forwardHostFrame = new ForwardHostFrame(ForwardLaunchConfiguration.APPLICATION_TITLE, forwardLaunchConfiguration);
        forwardHostFrame.attach(this.MAjAkka);
        this.MAjAkka.mAJAkka = true;
        this.MAjAkka.init();
        forwardHostFrame.setVisible(true);
        this.MAjAkka.start();
    }

    public kmjakka getAppletContext() {
        return this.KAjAkka.getAppletContext();
    }

    public URL getCodeBase() {
        return this.KAjAkka.getCodeBase();
    }

    public URL getDocumentBase() {
        return this.KAjAkka.getDocumentBase();
    }

    public void showStatus(String string) {
        this.getAppletContext().showStatus(string);
    }

    public void finalize() {
        this.MAjAkka.stop();
    }

    public void init() {
    }

    public void start() {
        if (this instanceof Runnable && this.majAkka == null) {
            Runnable runnable = (Runnable)this;
            this.majAkka = new Thread(runnable, "Muhmu Desktop runner - " + this.getClass().getSimpleName());
            this.majAkka.start();
        }
    }

    public void stop() {
        Thread thread = this.majAkka;
        this.majAkka = null;
        if (thread != null) {
            thread.interrupt();
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
            return null;
        }
        int n5 = 0;
        byte[] byArray = new byte[n2];
        int n6 = 0;
        while (n5 < n2) {
            byte[] byArray2 = vector.elementAt(n6++);
            if (n2 - n5 >= n) {
                System.arraycopy(byArray2, 0, byArray, n5, n);
                n5 += n;
                continue;
            }
            System.arraycopy(byArray2, 0, byArray, n5, n2 - n5);
            n5 = n2;
        }
        vector.removeAllElements();
        return byArray;
    }

    public URL AMAJAkK(String string) {
        return this.AmaJAkK(this.getCodeBase(), string);
    }

    public URL amAjAkK(String string) {
        return this.AmaJAkK(this.getDocumentBase(), string);
    }

    public URL AmaJAkK(URL uRL, String string) {
        if (uRL == null || string == null || string.isEmpty()) {
            return null;
        }
        if (this.mAJAkka) {
            string = string.replace('\\', '/');
            if (string.charAt(0) == '/') {
                if (uRL.getFile().length() > 2 && uRL.getFile().charAt(2) == ':') {
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
            if (string.length() > 1 && string.charAt(1) == ':') {
                try {
                    return new URL("file:///" + string);
                }
                catch (Exception exception) {
                    exception.printStackTrace();
                    return null;
                }
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
        String string3 = this.KAjAkka.getParameter(string);
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
