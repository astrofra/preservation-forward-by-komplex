/*
 * Decompiled with CFR 0.152.
 */
import java.util.Enumeration;
import java.util.Vector;

public class SceneRenderer {
    public Vector MaJAKkA = new Vector(10);
    Triangle[] maJAKkA;
    DepthSorter MAJAKkA;
    static public boolean mAJAKkA = true;
    static public boolean MajAKkA = true;
    static public boolean majAKkA = true;
    static public boolean MAjAKkA = true;

    public void mAJAKkA() {
        int n = 0;
        Enumeration enumeration = this.MaJAKkA.elements();
        if (enumeration != null) {
            while (enumeration.hasMoreElements()) {
                MeshObject mmajmmk2 = (MeshObject)enumeration.nextElement();
                n += mmajmmk2.JAkkaMA.length;
            }
        }
        this.maJAKkA = new Triangle[n];
        this.MAJAKkA = new DepthSorter(this.maJAKkA);
    }

    public void MajAKkA(Camera kaajmmk2, SurfacePresenter mmajkmk2) {
        kmjamma.majaKKa = this.maJAKkA;
        kmjamma.MAjaKKa = 0;
        kaajmmk2.JAkKAMa = mmajkmk2.AKkamaJ;
        kaajmmk2.jAkKAMa = mmajkmk2.aKkamaJ;
        ViewFrustum kaajmka2 = new ViewFrustum();
        kaajmka2.kamaJak(kaajmmk2, null);
        Enumeration enumeration = this.MaJAKkA.elements();
        if (enumeration != null) {
            while (enumeration.hasMoreElements()) {
                MeshObject mmajmmk2 = (MeshObject)enumeration.nextElement();
                int n = kaajmka2.KAmaJak(mmajmmk2);
                if (n != -1) {
                    mmajmmk2.kkaMAJa(kaajmmk2, n);
                }
                if (!mmajmmk2.JaKkAma) continue;
                MeshObject mmajmmk3 = mmajmmk2.JAKkAma;
                mmajmmk3.jaKKaMA.mAjakKA(mmajmmk2.jaKKaMA);
                mmajmmk3.JakKaMA.aMajaKk(mmajmmk2.JakKaMA);
                mmajmmk3.jaKKaMA.mAJakKA *= -1.0f;
                Mat3f maajkka2 = mmajmmk3.JakKaMA;
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

    public void MAJAKkA(SurfacePresenter mmajkmk2) {
        if (MAjAKkA) {
            mmajkmk2.kKAMAjA(kmjamma.majaKKa, kmjamma.MAjaKKa);
        }
    }

    public void maJAKkA(MeshObject mmajmmk2) {
        this.MaJAKkA.addElement(mmajmmk2);
    }
}
