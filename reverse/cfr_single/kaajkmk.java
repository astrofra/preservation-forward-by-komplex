/*
 * Decompiled with CFR 0.152.
 */
import java.applet.AppletContext;
import java.applet.AppletStub;
import java.net.URL;
import java.util.Hashtable;

class kaajkmk
implements AppletStub {
    kmjakka AKKamAJ;
    Hashtable aKKamAJ = new Hashtable();

    kaajkmk() {
        this.AKKamAJ = new kmjakka();
    }

    public void appletResize(int n, int n2) {
    }

    public AppletContext getAppletContext() {
        return this.AKKamAJ;
    }

    public URL getCodeBase() {
        return kmjakka.kaMAjaK();
    }

    public URL getDocumentBase() {
        return kmjakka.KAMAjaK();
    }

    public boolean isActive() {
        return true;
    }

    public void KKAmajA(String string, String string2) {
        this.aKKamAJ.put(string, string2);
    }

    public String getParameter(String string) {
        return (String)this.aKKamAJ.get(string);
    }
}
