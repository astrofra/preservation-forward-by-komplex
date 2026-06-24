/*
 * Decompiled with CFR 0.152.
 */
import java.net.URL;
import java.util.Hashtable;

class kaajkmk {
    kmjakka AKKamAJ = new kmjakka();
    Hashtable aKKamAJ = new Hashtable();
    private URL KkAkamAJ = kmjakka.kaMAjaK();

    public kmjakka getAppletContext() {
        return this.AKKamAJ;
    }

    public URL getCodeBase() {
        return this.KkAkamAJ;
    }

    public URL getDocumentBase() {
        return this.KkAkamAJ;
    }

    public void setBaseUrl(URL uRL) {
        if (uRL != null) {
            this.KkAkamAJ = uRL;
        }
    }

    public void KKAmajA(String string, String string2) {
        if (string == null || string2 == null) {
            return;
        }
        this.aKKamAJ.put(string, string2);
        if ("base".equalsIgnoreCase(string) || "basedir".equalsIgnoreCase(string)) {
            this.setBaseUrl(kmjakka.aMAJaK(string2));
        }
    }

    public String getParameter(String string) {
        return (String)this.aKKamAJ.get(string);
    }
}
