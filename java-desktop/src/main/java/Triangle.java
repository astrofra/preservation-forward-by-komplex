/*
 * Decompiled with CFR 0.152.
 */
public final class Triangle
extends RenderPrimitive {
    public Vertex mAjakKa;
    public Vertex MaJAkKa;
    public Vertex maJAkKa;
    public UvCoord MAJAkKa;
    public UvCoord mAJAkKa;
    public UvCoord MajAkKa;

    public Triangle(MeshObject mmajmmk2, int n, int n2, int n3) {
        this.mAjakKa = mmajmmk2.jAkkaMA[n];
        this.MaJAkKa = mmajmmk2.jAkkaMA[n2];
        this.maJAkKa = mmajmmk2.jAkkaMA[n3];
    }

    public Triangle(MeshObject mmajmmk2, int n, int n2, int n3, int n4, int n5, int n6) {
        this.mAjakKa = mmajmmk2.jAkkaMA[n];
        this.MaJAkKa = mmajmmk2.jAkkaMA[n2];
        this.maJAkKa = mmajmmk2.jAkkaMA[n3];
        this.MAJAkKa = mmajmmk2.JaKKaMA[n4];
        this.mAJAkKa = mmajmmk2.JaKKaMA[n5];
        this.MajAkKa = mmajmmk2.JaKKaMA[n6];
    }

    public Triangle(Vertex mmjakka2, Vertex mmjakka3, Vertex mmjakka4) {
        this.mAjakKa = mmjakka2;
        this.MaJAkKa = mmjakka3;
        this.maJAkKa = mmjakka4;
    }

    public Triangle(Vertex mmjakka2, Vertex mmjakka3, Vertex mmjakka4, UvCoord kmajkmk2, UvCoord kmajkmk3, UvCoord kmajkmk4) {
        this.mAjakKa = mmjakka2;
        this.MaJAkKa = mmjakka3;
        this.maJAkKa = mmjakka4;
        this.MAJAkKa = kmajkmk2;
        this.mAJAkKa = kmajkmk3;
        this.MajAkKa = kmajkmk4;
    }

    public Triangle(Triangle kmaamma2) {
        this.mAjAkka = kmaamma2.mAjAkka;
        this.MaJaKKa = kmaamma2.MaJaKKa;
        this.maJaKKa = kmaamma2.maJaKKa;
    }

    public Triangle(Triangle kmaamma2, Vertex mmjakka2, Vertex mmjakka3, Vertex mmjakka4) {
        this.mAjAkka = kmaamma2.mAjAkka;
        this.MaJaKKa = kmaamma2.MaJaKKa;
        this.maJaKKa = kmaamma2.maJaKKa;
        this.mAjakKa = mmjakka2;
        this.MaJAkKa = mmjakka3;
        this.maJAkKa = mmjakka4;
        this.MAJAkKa = new UvCoord();
        this.mAJAkKa = new UvCoord();
        this.MajAkKa = new UvCoord();
    }

    public Triangle(Triangle kmaamma2, Vertex mmjakka2, Vertex mmjakka3, Vertex mmjakka4, UvCoord kmajkmk2, UvCoord kmajkmk3, UvCoord kmajkmk4) {
        this.mAjAkka = kmaamma2.mAjAkka;
        this.MaJaKKa = kmaamma2.MaJaKKa;
        this.maJaKKa = kmaamma2.maJaKKa;
        this.mAjakKa = mmjakka2;
        this.MaJAkKa = mmjakka3;
        this.maJAkKa = mmjakka4;
        this.MAJAkKa = kmajkmk2;
        this.mAJAkKa = kmajkmk3;
        this.MajAkKa = kmajkmk4;
    }

    public void AmaJAKK(Triangle kmaamma2) {
        this.mAjAkka = kmaamma2.mAjAkka;
        this.MaJaKKa = kmaamma2.MaJaKKa;
        this.maJaKKa = kmaamma2.maJaKKa;
    }

    public final void amaJAkK() {
        RenderPrimitive.majaKKa[RenderPrimitive.MAjaKKa++] = this;
    }

    public final void aMaJAkK() {
        Vec3f mmajmma2 = this.MaJAkKa.mAjAKKA(this.mAjakKa);
        Vec3f mmajmma3 = this.maJAkKa.mAjAKKA(this.mAjakKa);
        mmajmma2.majakKA(mmajmma3);
        mmajmma2.majaKKA();
        this.MAJaKKa = mmajmma2.maJakKA;
        this.mAJaKKa = mmajmma2.MAJakKA;
        this.MajaKKa = mmajmma2.mAJakKA;
    }

    public final void AMaJAkK() {
        this.mAjakKa.aMAJaKK += this.MAJaKKa;
        this.mAjakKa.AmaJaKK += this.mAJaKKa;
        this.mAjakKa.render += this.MajaKKa;
        this.MaJAkKa.aMAJaKK += this.MAJaKKa;
        this.MaJAkKa.AmaJaKK += this.mAJaKKa;
        this.MaJAkKa.render += this.MajaKKa;
        this.maJAkKa.aMAJaKK += this.MAJaKKa;
        this.maJAkKa.AmaJaKK += this.mAJaKKa;
        this.maJAkKa.render += this.MajaKKa;
    }
}
