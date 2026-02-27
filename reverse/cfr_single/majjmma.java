/*
 * Decompiled with CFR 0.152.
 */
import java.util.Enumeration;
import java.util.Hashtable;
import muhmu.hifi.device.MAD;
import muhmu.hifi.device.Mixable;

public class majjmma
implements Mixable {
    Hashtable MajAkkA = new Hashtable(32);
    int majAkkA = -1;
    maajmma MAjAkkA;
    float mAjAkkA;
    float MaJaKKA = 1.0f;
    public long maJaKKA;

    public majjmma() {
        this.mAjakkA(3.0f);
    }

    public synchronized void mAJAkkA() {
        this.MajAkkA.clear();
    }

    public void MajAkkA(maajmma maajmma2) {
        this.MAjAkkA = maajmma2;
    }

    void mAjakkA(float f) {
        this.MaJaKKA = 1.0f / f;
    }

    void majAkkA(float f) {
        this.MaJaKKA = f;
    }

    synchronized void maJAkkA(int n, Mixable mixable) {
        this.MajAkkA.put(new Integer(n), mixable);
    }

    synchronized void MAJAkkA(Mixable mixable) {
        this.MajAkkA.put(new Integer(this.majAkkA), mixable);
        --this.majAkkA;
        if (this.majAkkA == Integer.MIN_VALUE) {
            this.majAkkA = -1;
        }
    }

    public synchronized boolean mix(MAD mAD, int[] nArray, int n, int n2) {
        int n3 = (int)this.mAjAkkA;
        if (n3 >= nArray.length) {
            this.MaJAkkA(mAD, nArray, 0, nArray.length);
        } else {
            int n4 = 0;
            while (n4 < nArray.length) {
                if (n3 > nArray.length) {
                    this.MaJAkkA(mAD, nArray, n4, nArray.length);
                    n4 = nArray.length;
                    continue;
                }
                this.MaJAkkA(mAD, nArray, n4, n3);
                n4 = n3;
                this.maJaKKA = mAD.bufferStartTime + (long)(n4 * 1000 / mAD.frequency);
                if (this.MAjAkkA != null) {
                    this.MAjAkkA.MaJakKa(this);
                }
                this.mAjAkkA += this.MaJaKKA * (float)mAD.frequency;
                n3 = (int)this.mAjAkkA;
            }
        }
        this.mAjAkkA -= (float)nArray.length;
        return true;
    }

    public synchronized void MaJAkkA(MAD mAD, int[] nArray, int n, int n2) {
        Enumeration enumeration = this.MajAkkA.elements();
        if (enumeration == null) {
            return;
        }
        Enumeration enumeration2 = this.MajAkkA.keys();
        while (enumeration.hasMoreElements()) {
            Mixable mixable = (Mixable)enumeration.nextElement();
            Object k = enumeration2.nextElement();
            boolean bl = mixable.mix(mAD, nArray, n, n2);
            if (bl) continue;
            this.MajAkkA.remove(k);
        }
    }
}
