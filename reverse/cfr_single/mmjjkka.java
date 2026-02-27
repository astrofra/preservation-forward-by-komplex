/*
 * Decompiled with CFR 0.152.
 */
public class mmjjkka
extends mmjjkmk {
    mmjakmk MAjAkKa;
    mmjjmmk mAjAkKa = new mmjjmmk();
    mmjjmmk AmAjAkK = new mmjjmmk();
    int amAjAkK;
    int AMAjAkK;
    int aMAjAkK;
    boolean AmajAkK = false;

    mmjjkka() {
    }

    public void amajaKK(mmjakmk mmjakmk2, int n) {
        this.MAjAkKa = mmjakmk2;
        if (this.MAjAkKa == null) {
            this.mAjAkKa.KKAMAja(null);
            this.AmAjAkK.KKAMAja(null);
            this.KAmAjAk();
            return;
        }
        this.mAjAkKa.KKAMAja(this.MAjAkKa.akkamAJ);
        this.AmAjAkK.KKAMAja(this.MAjAkKa.AKkamAJ);
        this.AMajaKK();
        int n2 = n < 0 ? 0 : (n > 95 ? 95 : n);
        mmajmka mmajmka2 = this.MAjAkKa.AkkamAJ[n2];
        super.KAMAjAk(mmajmka2, n);
    }

    public void kamajAk() {
        if (this.MAjAkKa == null) {
            return;
        }
        super.kamajAk();
    }

    public void KAmAjAk() {
        if (this.MAjAkKa != null && this.mAjAkKa.JakkAMA) {
            this.mAjAkKa.KkAMAja();
            return;
        }
        super.KAmAjAk();
    }

    void AMajaKK() {
        if (this.MAjAkKa.AkKAmAJ > 0) {
            this.AmajAkK = true;
            this.amAjAkK = 0;
            this.AMAjAkK = this.MAjAkKa.akKAmAJ == 0 ? this.MAjAkKa.AkKAmAJ : 0;
            this.AmajaKK();
            return;
        }
        this.AmajAkK = false;
    }

    int AmajaKK() {
        if (this.MAjAkKa.akKAmAJ != 0) {
            this.AMAjAkK += this.MAjAkKa.AkKAmAJ / this.MAjAkKa.akKAmAJ;
            if (this.AMAjAkK > this.MAjAkKa.AkKAmAJ) {
                this.AMAjAkK = this.MAjAkKa.AkKAmAJ;
            }
        }
        int n = 0;
        switch (this.MAjAkKa.AKKAmAJ) {
            case 3: {
                n = (64 - (this.amAjAkK >> 1) & 0x7F) - 64;
                break;
            }
            case 2: {
                n = (64 + (this.amAjAkK >> 1) & 0x7F) - 64;
                break;
            }
            case 1: {
                n = (this.amAjAkK & 0x80) != 0 ? 64 : -64;
                break;
            }
            case 0: {
                n = mmjjkmk.kkamaJa[this.amAjAkK & 0xFF];
                break;
            }
        }
        this.amAjAkK += this.MAjAkKa.aKkamAJ >> 2;
        return n * this.AMAjAkK >> 10;
    }

    public void aMajaKK() {
        byte by;
        if (this.mAjAkKa.JakkAMA) {
            by = this.mAjAkKa.kKAMAja();
            this.KKAmAjA(by * this.kkAMAja >> 6);
        }
        if (this.AmAjAkK.JakkAMA) {
            by = this.AmAjAkK.kKAMAja();
            int n = this.KKAMAja < 128 ? this.KKAMAja : 255 - this.KKAMAja;
            n = (by - 32) * n >> 5;
            this.kkAmAjA(this.kKAMAja + n);
        }
        if (this.AmajAkK) {
            this.aMAjAkK = this.AmajaKK();
            this.KAMajAk(this.aMAjAkK);
        }
    }
}
