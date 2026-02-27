/*
 * Decompiled with CFR 0.152.
 */
import java.util.Enumeration;
import java.util.Vector;

public class kmaakma {
    public Vector MaJAKkA = new Vector(10);
    kmaamma[] maJAKkA;
    maaakma MAJAKkA;
    static public boolean mAJAKkA = true;
    static public boolean MajAKkA = true;
    static public boolean majAKkA = true;
    static public boolean MAjAKkA = true;

    public void mAJAKkA() {
        int n = 0;
        Enumeration enumeration = this.MaJAKkA.elements();
        if (enumeration != null) {
            while (enumeration.hasMoreElements()) {
                mmajmmk mmajmmk2 = (mmajmmk)enumeration.nextElement();
                n += mmajmmk2.JAkkaMA.length;
            }
        }
        this.maJAKkA = new kmaamma[n];
        this.MAJAKkA = new maaakma(this.maJAKkA);
    }

    public void MajAKkA(kaajmmk kaajmmk2, mmajkmk mmajkmk2) {
        kmjamma.majaKKa = this.maJAKkA;
        kmjamma.MAjaKKa = 0;
        kaajmmk2.JAkKAMa = mmajkmk2.AKkamaJ;
        kaajmmk2.jAkKAMa = mmajkmk2.aKkamaJ;
        kaajmka kaajmka2 = new kaajmka();
        kaajmka2.kamaJak(kaajmmk2, null);
        Enumeration enumeration = this.MaJAKkA.elements();
        if (enumeration != null) {
            while (enumeration.hasMoreElements()) {
                mmajmmk mmajmmk2 = (mmajmmk)enumeration.nextElement();
                int n = kaajmka2.KAmaJak(mmajmmk2);
                if (n != -1) {
                    mmajmmk2.kkaMAJa(kaajmmk2, n);
                }
                if (!mmajmmk2.JaKkAma) continue;
                mmajmmk mmajmmk3 = mmajmmk2.JAKkAma;
                mmajmmk3.jaKKaMA.mAjakKA(mmajmmk2.jaKKaMA);
                mmajmmk3.JakKaMA.aMajaKk(mmajmmk2.JakKaMA);
                mmajmmk3.jaKKaMA.mAJakKA *= -1.0f;
                maajkka maajkka2 = mmajmmk3.JakKaMA;
                maajkka2.AmAjAKK *= -1.0f;
                maajkka2.aMAjAKK *= -1.0f;
                maajkka2.AMajAKK *= -1.0f;
                int n2 = kmjamma.MAjaKKa;
                n = kaajmka2.KAmaJak(mmajmmk3);
                if (n != -1) {
                    mmajmmk3.kkaMAJa(kaajmmk2, n);
                }
                int n3 = n2;
                while (n3 < kmjamma.MAjaKKa) {
                    kmjamma.majaKKa[n3].majAkKa *= -1.0f;
                    ++n3;
                }
            }
        }
        if (MajAKkA) {
            this.MAJAKkA.MaJakkA(kmjamma.MAjaKKa);
        }
    }

    public void MAJAKkA(mmajkmk mmajkmk2) {
        if (MAjAKkA) {
            mmajkmk2.kKAMAjA(kmjamma.majaKKa, kmjamma.MAjaKKa);
        }
    }

    public void maJAKkA(mmajmmk mmajmmk2) {
        this.MaJAKkA.addElement(mmajmmk2);
    }
}
