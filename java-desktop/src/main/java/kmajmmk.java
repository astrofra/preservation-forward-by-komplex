/*
 * Decompiled with CFR 0.152.
 */
public class kmajmmk {
    int[] jAkkama;
    int JaKKama;
    maajmmk jaKKama;
    boolean JAKKama = false;
    int jAKKama;
    int JakKama;

    public void KkAMaJa(int n) {
        this.JaKKama = n;
    }

    public void kKamaJa(maajmmk maajmmk2) {
        this.jaKKama = maajmmk2;
    }

    public void KkaMaJa(byte[] byArray, int n, int n2) {
        this.jAkkama = new int[n2];
        int n3 = 0;
        while (n3 < n2) {
            this.jAkkama[n3] = byArray[n3 + n] & 0xFF;
            ++n3;
        }
    }

    public void KKamaJa(boolean bl) {
        this.JAKKama = bl;
    }

    public void KKAMaJa() {
        this.kkamaJa(0);
    }

    public void kkamaJa(int n) {
        this.jAKKama = n < 0 ? 0 : (n < this.jAkkama.length ? n : this.jAkkama.length - 1);
        this.JakKama = 0;
    }

    public void kKAMaJa(int n) {
        majamka majamka2 = this.jaKKama.JAkKama[this.jAkkama[this.jAKKama]];
        this.JakKama = n < 0 ? 0 : (n < majamka2.KamAJAk ? n : majamka2.KamAJAk - 1);
    }

    public void kkaMaJa() {
        if (!this.JAKKama) {
            ++this.jAKKama;
        }
        if (this.jAKKama < this.jAkkama.length) {
            this.kkamaJa(this.jAKKama);
            return;
        }
        if (this.JaKKama >= 0 && this.JaKKama < this.jAkkama.length) {
            this.kkamaJa(this.JaKKama);
            return;
        }
        this.kkamaJa(0);
    }

    public byte[] kkAMaJa() {
        majamka majamka2 = this.jaKKama.JAkKama[this.jAkkama[this.jAKKama]];
        byte[] byArray = majamka2.kaMAJak(this.JakKama);
        ++this.JakKama;
        if (this.JakKama == majamka2.KamAJAk) {
            this.kkaMaJa();
        }
        return byArray;
    }
}
