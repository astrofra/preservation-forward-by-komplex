/*
 * Decompiled with CFR 0.152.
 */
import muhmu.hifi.device.MAD;
import muhmu.hifi.device.Mixable;

public class mmjjkmk
implements Mixable {
    mmajmka kKamAja;
    int KkAMAja = 64;
    int kkAMAja = 64;
    int KKAMAja = 128;
    int kKAMAja = 128;
    int KkaMAja;
    int kkaMAja = -1;
    int KKaMAja;
    int kKaMAja;
    int KkAmaja;
    boolean kkAmaja = true;
    static final byte KKAmaja = 1;
    static final byte kKAmaja = 2;
    static final byte Kkamaja = 3;
    static final byte kkamaja = 4;
    static final byte KKamaja = 5;
    static final byte kKamaja = 6;
    static final byte KkAMaja = 7;
    static final byte kkAMaja = 8;
    static final byte KKAMaja = 9;
    static final byte kKAMaja = 10;
    static final byte KkaMaja = 11;
    byte kkaMaja;
    int KKaMaja;
    int kKaMaja;
    int KkAmAJa;
    int kkAmAJa;
    int KKAmAJa;
    int kKAmAJa;
    int KkamAJa;
    int[] kkamAJa = new int[3];
    int KKamAJa;
    int kKamAJa;
    int KkAMAJa;
    int kkAMAJa;
    int KKAMAJa;
    int kKAMAJa;
    int KkaMAJa;
    int kkaMAJa;
    int KKaMAJa;
    int kKaMAJa;
    int KkAmaJa;
    int kkAmaJa;
    int KKAmaJa;
    static final int[] kKAmaJa;
    static final int[] KkamaJa;
    static final int[] kkamaJa;

    public void kAMAjAk(boolean bl) {
        this.kkAmaja = bl;
    }

    public void KaMAjAk(int n) {
        this.KkAMAja = n <= 0 ? 0 : (n > 64 ? 64 : n);
        this.kkAMAja = this.KkAMAja;
    }

    public void kamAjAk(int n) {
        int n2 = this.KkAMAja + n;
        this.kkAMAja = n2 <= 0 ? 0 : (n2 > 64 ? 64 : n2);
    }

    public void kAmAjAk(int n) {
        int n2 = this.kkAMAja + n;
        this.kkAMAja = n2 <= 0 ? 0 : (n2 > 64 ? 64 : n2);
    }

    public void KKAmAjA(int n) {
        this.kkAMAja = n <= 0 ? 0 : (n > 64 ? 64 : n);
    }

    public void KamAjAk(int n) {
        this.kKaMAja = this.KKaMAja = n;
        this.KkAmaja = mmajmka.kAmajAK(this.kKaMAja);
    }

    public void kaMajAk(int n) {
        this.kKaMAja = this.KKaMAja + n;
        this.KkAmaja = mmajmka.kAmajAK(this.kKaMAja);
    }

    public void KAMajAk(int n) {
        this.kKaMAja += n;
        this.KkAmaja = mmajmka.kAmajAK(this.kKaMAja);
    }

    public void kAmajAk(int n) {
        this.KKAMAja = n <= 0 ? 0 : (n > 255 ? 255 : n);
        this.kKAMAja = this.KKAMAja;
    }

    public void kkAmAjA(int n) {
        this.kKAMAja = n <= 0 ? 0 : (n > 255 ? 255 : n);
    }

    public void kaMAjAk(int n) {
        int n2 = this.KKAMAja + n;
        this.kKAMAja = n2 <= 0 ? 0 : (n2 > 255 ? 255 : n2);
    }

    public void KAMAjAk(mmajmka mmajmka2, int n) {
        this.KkAmAjA(mmajmka2, n, true);
    }

    public void KkAmAjA(mmajmka mmajmka2, int n, boolean bl) {
        if (mmajmka2 == null) {
            this.KAmAjAk();
            return;
        }
        this.kKamAja = mmajmka2;
        this.KamAjAk(this.kKamAja.KamajAK(n));
        if (bl) {
            this.KaMAjAk(this.kKamAja.KamAjaK);
        }
        this.kkaMAja = 0;
        this.KKAMAja = this.kKamAja.kaMaJAK;
    }

    public void KaMajAk() {
        this.KAmajAk(true);
    }

    public void KAmajAk(boolean bl) {
        if (this.kKamAja == null) {
            return;
        }
        if (bl) {
            this.KaMAjAk(this.kKamAja.KamAjaK);
        }
        this.kkaMAja = 0;
        this.KKAMAja = this.kKamAja.kaMaJAK;
    }

    public void kAMajAk(int n, boolean bl) {
        if (this.kKamAja == null) {
            return;
        }
        this.KamAjAk(this.kKamAja.KamajAK(n));
        if (bl) {
            this.KaMAjAk(this.kKamAja.KamAjaK);
        }
        this.kkaMAja = 0;
        this.KKAMAja = this.kKamAja.kaMaJAK;
    }

    public void kamajAk() {
        if (this.kKamAja == null) {
            return;
        }
        this.KaMAjAk(this.kKamAja.KamAjaK);
    }

    public void KAmAjAk() {
        this.kkaMAja = -1;
    }

    public void KamajAk(int n) {
        if (this.kKamAja == null) {
            return;
        }
        if (n >= this.kKamAja.kaMAjaK) {
            this.kkaMAja = -1;
            return;
        }
        this.kkaMAja = n << 12;
    }

    public boolean mix(MAD mAD, int[] nArray, int n, int n2) {
        if (this.kkaMAja == -1) {
            return true;
        }
        if (!this.kkAmaja) {
            return true;
        }
        if (this.kKamAja.KaMAjaK.length == 0) {
            return true;
        }
        int n3 = this.kkAMAja * mAD.boost >> 6;
        int n4 = this.KkAmaja / mAD.frequency;
        this.kkaMAja = mAD.stereo ? this.kKamAja.kamajAK(nArray, n, n2, this.kkaMAja, n4, n3, this.KKAMAja) : this.kKamAja.kamajAK(nArray, n, n2, this.kkaMAja, n4, n3, 0);
        return true;
    }

    public void kAmAJAk(boolean bl) {
        int n = 0;
        int n2 = 0;
        int n3 = 0;
        if (bl) {
            int n4;
            if (this.kkaMaja == 5) {
                n4 = KkamaJa[this.kKaMAJa & 0x1F] * this.kkaMAJa >> 5;
                if ((this.kKaMAJa & 0x20) != 0) {
                    n4 = -n4;
                }
                n2 = n4;
                this.kKaMAJa += this.KKaMAJa;
            }
            if (this.kkaMaja == 11) {
                n4 = KkamaJa[this.KKAmaJa & 0x1F] * this.KkAmaJa >> 6;
                if ((this.KKAmaJa & 0x20) != 0) {
                    n4 = -n4;
                }
                n = n4;
                this.KKAmaJa += this.kkAmaJa;
            }
            this.kkaMaja = 0;
        } else {
            switch (this.kkaMaja) {
                case 10: {
                    this.KaMAjAk(this.KkAMAja + this.KKaMaja);
                }
                case 2: {
                    this.KKaMAja += this.KkAmAJa;
                    if (this.KkAmAJa < 0 ^ this.KKaMAja > this.kKAmAJa) {
                        this.KamAjAk(this.kKAmAJa);
                        break;
                    }
                    this.KamAjAk(this.KKaMAja);
                    break;
                }
                case 3: {
                    this.KKaMAja += this.kkAmAJa;
                    this.KamAjAk(this.KKaMAja);
                    break;
                }
                case 4: {
                    this.KKaMAja += this.KKAmAJa;
                    this.KamAjAk(this.KKaMAja);
                    break;
                }
                case 1: {
                    this.KaMAjAk(this.KkAMAja + this.KKaMaja);
                    break;
                }
                case 6: {
                    n2 += this.kkamAJa[this.KkamAJa++];
                    if (this.KkamAJa != 3) break;
                    this.KkamAJa = 0;
                    break;
                }
                case 7: {
                    this.kAmajAk(this.KKAMAja + this.kKaMaja);
                    break;
                }
                case 8: {
                    if (--this.KkaMAJa != 0) break;
                    this.KAmajAk(false);
                    this.kkaMaja = 0;
                    break;
                }
                case 9: {
                    this.KaMAjAk(this.KkAMAja + this.KKaMaja);
                }
                case 5: {
                    int n5 = KkamaJa[this.kKaMAJa & 0x1F] * this.kkaMAJa >> 5;
                    if ((this.kKaMAJa & 0x20) != 0) {
                        n5 = -n5;
                    }
                    n2 += n5;
                    this.kKaMAJa += this.KKaMAJa;
                    break;
                }
                case 11: {
                    int n6 = KkamaJa[this.KKAmaJa & 0x1F] * this.KkAmaJa >> 6;
                    if ((this.KKAmaJa & 0x20) != 0) {
                        n6 = -n6;
                    }
                    n = n6;
                    this.KKAmaJa += this.kkAmaJa;
                    break;
                }
            }
        }
        this.kaMajAk(n2);
        this.kamAjAk(n);
        this.kaMAjAk(n3);
    }

    static {
        int n;
        int n2;
        kKAmaJa = new int[768];
        KkamaJa = new int[32];
        kkamaJa = new int[256];
        int n3 = 0;
        while (n3 < 256) {
            mmjjkmk.kkamaJa[n3] = n2 = (int)(-Math.sin((double)n3 / 256.0 * Math.PI * 2.0) * 64.55);
            ++n3;
        }
        n2 = 0;
        while (n2 < 768) {
            mmjjkmk.kKAmaJa[n2] = n = (int)(8363.0 * Math.pow(2.0, (4608.0 - (double)n2) / 768.0));
            ++n2;
        }
        n = 0;
        while (n < 32) {
            int n4;
            mmjjkmk.KkamaJa[n] = n4 = (int)(Math.sin((double)n / 32.0 * Math.PI) * 255.0);
            ++n;
        }
    }
}
