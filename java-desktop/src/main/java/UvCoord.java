/*
 * Decompiled with CFR 0.152.
 */
public class UvCoord {
    public float akKAMAJ;
    public float AKKAMAJ;

    public UvCoord() {
    }

    public UvCoord(UvCoord kmajkmk2) {
        this.akKAMAJ = kmajkmk2.akKAMAJ;
        this.AKKAMAJ = kmajkmk2.AKKAMAJ;
    }

    public UvCoord(float f, float f2) {
        this.akKAMAJ = f;
        this.AKKAMAJ = f2;
    }

    public UvCoord(double d, double d2) {
        this.akKAMAJ = (float)d;
        this.AKKAMAJ = (float)d2;
    }

    public void scriptCursor(UvCoord kmajkmk2) {
        this.akKAMAJ = kmajkmk2.akKAMAJ;
        this.AKKAMAJ = kmajkmk2.AKKAMAJ;
    }

    public void scriptCommands(float f, float f2) {
        this.akKAMAJ = f;
        this.AKKAMAJ = f2;
    }

    public void deferredScriptCommand(double d, double d2) {
        this.akKAMAJ = (float)d;
        this.AKKAMAJ = (float)d2;
    }
}
