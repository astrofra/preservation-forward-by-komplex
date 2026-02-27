/*
 * Decompiled with CFR 0.152.
 */
public class maajmmk
implements maajmma {
    public kmajmmk jakKama;
    public majamka[] JAkKama;
    public mmajmka[] jAkKama;
    public mmjakmk[] JaKkAMa;
    public int jaKkAMa;
    public int JAKkAMa;
    public int jAKkAMa;
    public String JakkAMa;
    majamka jakkAMa;
    int JAkkAMa;
    int jAkkAMa;
    int JaKKAMa;
    public mmjjkka[] jaKKAMa;
    majjmma JAKKAMa;
    kajjmmk jAKKAMa;
    boolean JakKAMa = false;
    kmjjkka jakKAMa = new kmjjkka();

    public void akKaMaJ(majjmma majjmma2) {
        this.JAKKAMa = majjmma2;
        this.JAKKAMa.mAJAkkA();
        this.JAKKAMa.MajAkkA(this);
        this.JAkkAMa = this.jAKkAMa;
        this.jAkkAMa = this.JAKkAMa;
        this.JAKKAMa.majAkkA(maajmmk.aKKaMaJ(this.jAkkAMa));
        this.jaKKAMa = new mmjjkka[this.jaKkAMa];
        int n = 0;
        while (n < this.jaKkAMa) {
            mmjjkka mmjjkka2;
            this.jaKKAMa[n] = mmjjkka2 = new mmjjkka();
            this.JAKKAMa.maJAkkA(n, mmjjkka2);
            ++n;
        }
        this.jakKama.KKAMaJa();
        this.JaKKAMa = 0;
    }

    public void kKaMaJa(kajjmmk kajjmmk2) {
        this.jAKKAMa = kajjmmk2;
    }

    static final float aKKaMaJ(int n) {
        float f = 1.0f / ((float)n / 125.0f) / 50.0f;
        return f;
    }

    public void MaJakKa(majjmma majjmma2) {
        int n;
        if (this.JaKKAMa-- == 0) {
            n = 0;
            while (n < this.jaKkAMa) {
                this.jaKKAMa[n].kAmAJAk(true);
                ++n;
            }
            int n2 = this.jakKama.jAKKama;
            int n3 = this.jakKama.JakKama;
            if (this.jAKKAMa != null) {
                this.jAKKAMa.kkaMAja((n2 << 8) + n3, majjmma2.maJaKKA);
            }
            this.KKaMaJa((n2 << 8) + n3, majjmma2.maJaKKA);
            byte[] byArray = this.jakKama.kkAMaJa();
            this.AkkaMaJ(byArray);
            this.JaKKAMa = this.JAkkAMa - 1;
        } else {
            n = 0;
            while (n < this.jaKkAMa) {
                this.jaKKAMa[n].kAmAJAk(false);
                ++n;
            }
        }
        n = 0;
        while (n < this.jaKkAMa) {
            this.jaKKAMa[n].aMajaKK();
            ++n;
        }
    }

    mmjakmk akkaMaJ(int n) {
        if (n < 0 || n >= this.JaKkAMa.length) {
            return null;
        }
        return this.JaKkAMa[n];
    }

    void AkkaMaJ(byte[] byArray) {
        boolean bl = false;
        int n = 0;
        boolean bl2 = false;
        int n2 = 0;
        int n3 = 0;
        int n4 = 0;
        while (n4 < this.jaKkAMa) {
            int n5;
            mmjjkka mmjjkka2 = this.jaKKAMa[n4];
            byte by = byArray[n3 + 2];
            int n6 = byArray[n3] & 0xFF;
            if (n6 != 0 && n6 <= 96) {
                if (by != 3) {
                    n5 = byArray[n3 + 1];
                    if (n5 == 0) {
                        mmjjkka2.kAMajAk(n6, false);
                    } else {
                        mmjakmk mmjakmk2 = this.akkaMaJ(n5 - 1);
                        mmjjkka2.amajaKK(mmjakmk2, n6);
                        mmjjkka2.kKaMAJa = 0;
                        mmjjkka2.KKAmaJa = 0;
                    }
                    if (by == 9) {
                        mmjjkka2.KamajAk((byArray[n3 + 3] & 0xFF) << 8);
                    }
                } else if (byArray[n3 + 1] != 0) {
                    mmjjkka2.kamajAk();
                }
            } else {
                n5 = byArray[n3 + 1];
                if (n5 > 0) {
                    mmjjkka2.kamajAk();
                    mmjjkka2.kKaMAJa = 0;
                    mmjjkka2.KKAmaJa = 0;
                }
                if (n6 > 96) {
                    mmjjkka2.KAmAjAk();
                }
            }
            if ((n5 = byArray[n3 + 4]) != 0) {
                if ((n5 &= 0xFF) <= 80 && n5 >= 16) {
                    mmjjkka2.KaMAjAk(n5 - 16);
                } else {
                    int n7 = n5 & 0xF;
                    switch ((n5 & 0xF0) >> 4) {
                        case 6: {
                            mmjjkka2.KaMAjAk(mmjjkka2.KkAMAja - n7 * 2);
                            break;
                        }
                        case 7: {
                            mmjjkka2.KaMAjAk(mmjjkka2.KkAMAja + n7 * 2);
                            break;
                        }
                        case 8: {
                            mmjjkka2.KaMAjAk(mmjjkka2.KkAMAja - n7);
                            break;
                        }
                        case 9: {
                            mmjjkka2.KaMAjAk(mmjjkka2.KkAMAja + n7);
                            break;
                        }
                        case 12: {
                            mmjjkka2.kAmajAk(n7 << 4);
                            break;
                        }
                        case 13: {
                            if (n7 == 0) break;
                            mmjjkka2.kkaMaja = 7;
                            mmjjkka2.kKaMaja = -n7;
                            break;
                        }
                        case 14: {
                            if (n7 == 0) break;
                            mmjjkka2.kkaMaja = 7;
                            mmjjkka2.kKaMaja = n7;
                            break;
                        }
                        default: {
                            if (!this.JakKAMa) break;
                            System.out.print("unsup volcmd " + ((n5 & 0xF0) >> 4) + " ");
                            break;
                        }
                    }
                }
            }
            int n8 = byArray[n3 + 3] & 0xFF;
            block9 : switch (by) {
                case 0: {
                    if (n8 == 0) break;
                    mmjjkka2.kkaMaja = 6;
                    mmjjkka2.kkamAJa[0] = 0;
                    mmjjkka2.kkamAJa[1] = -(n8 & 0xF) * 64;
                    mmjjkka2.kkamAJa[2] = -((n8 & 0xF0) >> 4) * 64;
                    mmjjkka2.KkamAJa = 0;
                    break;
                }
                case 1: {
                    mmjjkka2.kkaMaja = 3;
                    if (n8 == 0) break;
                    mmjjkka2.kkAmAJa = -n8 * 4;
                    break;
                }
                case 2: {
                    mmjjkka2.kkaMaja = 4;
                    if (n8 == 0) break;
                    mmjjkka2.KKAmAJa = n8 * 4;
                    break;
                }
                case 3: {
                    if (mmjjkka2.kKamAja == null) break;
                    mmjjkka2.kkaMaja = 2;
                    if (n8 != 0) {
                        mmjjkka2.KkAmAJa = byArray[n3 + 1] != 0 ? n8 << 2 : n8 << 1;
                    }
                    if (n6 != 0) {
                        mmjjkka2.kKAmAJa = mmjjkka2.kKamAja.KamajAK(n6);
                    }
                    if (!(mmjjkka2.kKAmAJa < mmjjkka2.KKaMAja ^ mmjjkka2.KkAmAJa < 0)) break;
                    mmjjkka2.KkAmAJa = -mmjjkka2.KkAmAJa;
                    break;
                }
                case 4: {
                    mmjjkka2.kkaMaja = 5;
                    if ((n8 & 0xF) != 0) {
                        mmjjkka2.kkaMAJa = n8 & 0xF;
                    }
                    if ((n8 & 0xF0) == 0) break;
                    mmjjkka2.KKaMAJa = (n8 & 0xF0) >> 4;
                    break;
                }
                case 5: {
                    mmjjkka2.kkaMaja = 10;
                    if ((n8 & 0xF) != 0) {
                        mmjjkka2.KKaMaja = -(n8 & 0xF);
                    }
                    if ((n8 & 0xF0) == 0) break;
                    mmjjkka2.KKaMaja = n8 >> 4;
                    break;
                }
                case 6: {
                    mmjjkka2.kkaMaja = 9;
                    if ((n8 & 0xF) != 0) {
                        mmjjkka2.KKaMaja = -(n8 & 0xF);
                    }
                    if ((n8 & 0xF0) == 0) break;
                    mmjjkka2.KKaMaja = n8 >> 4;
                    break;
                }
                case 7: {
                    mmjjkka2.kkaMaja = 11;
                    if ((n8 & 0xF) != 0) {
                        mmjjkka2.KkAmaJa = n8 & 0xF;
                    }
                    if ((n8 & 0xF0) == 0) break;
                    mmjjkka2.kkAmaJa = (n8 & 0xF0) >> 4;
                    break;
                }
                case 8: {
                    mmjjkka2.kAmajAk(n8);
                    break;
                }
                case 10: {
                    mmjjkka2.kkaMaja = 1;
                    if ((n8 & 0xF) != 0) {
                        mmjjkka2.KKaMaja = -(n8 & 0xF);
                    }
                    if ((n8 & 0xF0) == 0) break;
                    mmjjkka2.KKaMaja = n8 >> 4;
                    break;
                }
                case 11: {
                    bl2 = true;
                    n2 = n8;
                    break;
                }
                case 12: {
                    mmjjkka2.KaMAjAk(n8);
                    break;
                }
                case 13: {
                    bl = true;
                    n = n8;
                    break;
                }
                case 14: {
                    int n9 = n8 & 0xF;
                    switch ((n8 & 0xF0) >> 4) {
                        case 1: {
                            if (n9 != 0) {
                                mmjjkka2.KKamAJa = n9;
                            }
                            mmjjkka2.KamAjAk(mmjjkka2.KKaMAja - mmjjkka2.KKamAJa * 4);
                            break block9;
                        }
                        case 2: {
                            if (n9 != 0) {
                                mmjjkka2.kKamAJa = n9;
                            }
                            mmjjkka2.KamAjAk(mmjjkka2.KKaMAja + mmjjkka2.kKamAJa * 4);
                            break block9;
                        }
                        case 9: {
                            if (n9 != 0) {
                                mmjjkka2.kkaMaja = 8;
                                mmjjkka2.KkaMAJa = n9;
                                break block9;
                            }
                            mmjjkka2.KAmajAk(false);
                            break block9;
                        }
                        case 10: {
                            if (n9 != 0) {
                                mmjjkka2.KkAMAJa = n9;
                            }
                            mmjjkka2.KaMAjAk(mmjjkka2.KkAMAja + mmjjkka2.KkAMAJa);
                            break block9;
                        }
                        case 11: {
                            if (n9 != 0) {
                                mmjjkka2.kkAMAJa = n9;
                            }
                            mmjjkka2.KaMAjAk(mmjjkka2.KkAMAja - mmjjkka2.kkAMAJa);
                            break block9;
                        }
                    }
                    if (!this.JakKAMa) break;
                    System.out.print("unsup cmd E" + ((n8 & 0xF0) >> 4) + " ");
                    break;
                }
                case 15: {
                    if (n8 < 32) {
                        this.JAkkAMa = n8;
                        break;
                    }
                    this.jAkkAMa = n8;
                    this.JAKKAMa.majAkkA(maajmmk.aKKaMaJ(this.jAkkAMa));
                    break;
                }
                case 33: {
                    int n9 = n8 & 0xF;
                    switch ((n8 & 0xF0) >> 4) {
                        case 1: {
                            if (n9 != 0) {
                                mmjjkka2.KKAMAJa = n9;
                            }
                            mmjjkka2.KamAjAk(mmjjkka2.KKaMAja - mmjjkka2.KKAMAJa);
                            break block9;
                        }
                        case 2: {
                            if (n9 != 0) {
                                mmjjkka2.kKAMAJa = n9;
                            }
                            mmjjkka2.KamAjAk(mmjjkka2.KKaMAja + mmjjkka2.kKAMAJa);
                            break block9;
                        }
                    }
                    if (!this.JakKAMa) break;
                    System.out.print("unsup cmd X" + ((n8 & 0xF0) >> 4) + " ");
                    break;
                }
                default: {
                    if (!this.JakKAMa) break;
                    System.out.print("unsup cmd " + by + " ");
                }
                case 9: 
            }
            ++n4;
            n3 += 5;
        }
        if (bl) {
            this.jakKama.kkaMaJa();
            this.jakKama.kKAMaJa(n);
            return;
        }
        if (bl2) {
            this.jakKama.kkamaJa(n2);
        }
    }

    public void KKaMaJa(int n, long l) {
        this.jakKAMa.AmAJaKK(n, l);
    }

    public int AKKaMaJ(int n) {
        return this.jakKAMa.aMAJaKK(n);
    }

    public boolean aKkaMaJ(int n) {
        return this.jakKAMa.AmaJaKK(n, 0);
    }

    public boolean AKkaMaJ(int n, int n2) {
        return this.jakKAMa.AmaJaKK(n, n2);
    }

    public void AkKaMaJ() {
        this.jakKAMa.amAJaKK();
    }
}
