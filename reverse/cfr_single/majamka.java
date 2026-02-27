/*
 * Decompiled with CFR 0.152.
 */
public class majamka {
    public int KamAJAk;
    int kamAJAk;
    byte[][] KAmAJAk;
    final static int kAmAJAk = 0;
    final static int KaMajAk = 1;
    final static int kaMajAk = 2;
    final static int KAMajAk = 3;
    final static int kAMajAk = 4;

    majamka(int n, int n2) {
        this.KamAJAk = n;
        this.kamAJAk = n2;
        this.KAmAJAk = new byte[n][n2 * 5];
    }

    void KAMAJak(int n, int n2, int n3, int n4, int n5, int n6, int n7) {
        byte[] byArray = this.KAmAJAk[n];
        int n8 = n2 * 5;
        byArray[n8] = (byte)n3;
        byArray[n8 + 1] = (byte)n4;
        byArray[n8 + 2] = (byte)n5;
        byArray[n8 + 3] = (byte)n6;
        byArray[n8 + 4] = (byte)n7;
    }

    byte[] kaMAJak(int n) {
        return this.KAmAJAk[n];
    }
}
