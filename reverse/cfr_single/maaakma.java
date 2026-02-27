/*
 * Decompiled with CFR 0.152.
 */
public final class maaakma {
    public maaamma[] mAjAKkA;

    public maaakma(int n) {
        this(n, false);
    }

    public maaakma(int n, boolean bl) {
        this.mAjAKkA = new maaamma[n];
        if (bl) {
            int n2 = 0;
            while (n2 < n) {
                this.mAjAKkA[n2] = new majamma();
                this.mAjAKkA[n2].majAkKa = 100.0f;
                ++n2;
            }
        }
    }

    public maaakma(maaamma[] maaammaArray) {
        this.mAjAKkA = maaammaArray;
    }

    public final void MaJakkA(int n) {
        if (n < 2) {
            return;
        }
        this.MAjAKkA(0, n - 1);
    }

    final void MAjAKkA(int n, int n2) {
        maaamma maaamma2;
        maaamma[] maaammaArray = this.mAjAKkA;
        int n3 = n2 - n;
        if (n3 > 4) {
            int n4 = this.majAKkA(n, n2);
            if (n4 != n2) {
                maaamma2 = maaammaArray[n4];
                maaammaArray[n4] = maaammaArray[n2];
                maaammaArray[n2] = maaamma2;
            }
        } else if (n3 == 1) {
            if (maaammaArray[n].majAkKa > maaammaArray[n2].majAkKa) {
                maaamma maaamma3 = maaammaArray[n];
                maaammaArray[n] = maaammaArray[n2];
                maaammaArray[n2] = maaamma3;
            }
            return;
        }
        float f = maaammaArray[n2].majAkKa;
        int n5 = n - 1;
        int n6 = n2;
        while (true) {
            if (maaammaArray[++n5].majAkKa < f) {
                continue;
            }
            while (maaammaArray[--n6].majAkKa > f && n6 > 0) {
            }
            if (n5 >= n6) break;
            maaamma2 = maaammaArray[n5];
            maaammaArray[n5] = maaammaArray[n6];
            maaammaArray[n6] = maaamma2;
        }
        maaamma2 = maaammaArray[n5];
        maaammaArray[n5] = maaammaArray[n2];
        maaammaArray[n2] = maaamma2;
        if (n5 - 1 > n) {
            this.MAjAKkA(n, n5 - 1);
        }
        if (n2 > n5 + 1) {
            this.MAjAKkA(n5 + 1, n2);
        }
    }

    final void mAjAKkA(int n, int n2) {
        maaamma maaamma2 = this.mAjAKkA[n];
        this.mAjAKkA[n] = this.mAjAKkA[n2];
        this.mAjAKkA[n2] = maaamma2;
    }

    final int majAKkA(int n, int n2) {
        int n3 = n + n2 >> 1;
        float f = this.mAjAKkA[n].majAkKa;
        float f2 = this.mAjAKkA[n2].majAkKa;
        float f3 = this.mAjAKkA[n3].majAkKa;
        if (f2 > f) {
            if (f3 > f2) {
                return n2;
            }
            if (f3 > f) {
                return n3;
            }
            return n;
        }
        if (f3 > f) {
            return n;
        }
        if (f3 > f2) {
            return n3;
        }
        return n2;
    }
}
