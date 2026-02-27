/*
 * Decompiled with CFR 0.152.
 */
import java.util.Enumeration;
import java.util.Vector;

class kmjjkka {
    Vector amajAkK = new Vector(100);

    synchronized void AmAJaKK(int n, long l) {
        Vector vector = this.amajAkK;
        synchronized (vector) {
            maaamka maaamka2 = new maaamka(n, l);
            this.amajAkK.addElement(maaamka2);
            long l2 = System.currentTimeMillis();
            Enumeration enumeration = this.amajAkK.elements();
            if (enumeration != null) {
                while (enumeration.hasMoreElements()) {
                    maaamka maaamka3 = (maaamka)enumeration.nextElement();
                    if (maaamka3.kkAmAja > l2) continue;
                    this.amajAkK.removeElement(maaamka3);
                }
            }
        }
        this.notify();
    }

    int aMAJaKK(int n) {
        maaamka maaamka2 = this.AMAJaKK(n);
        long l = maaamka2.kkAmAja - System.currentTimeMillis();
        if (l > 0L) {
            try {
                Thread.sleep(l);
            }
            catch (Exception exception) {
                return maaamka2.KkAmAja;
            }
        }
        return maaamka2.KkAmAja;
    }

    /*
     * Enabled force condition propagation
     * Lifted jumps to return sites
     */
    boolean AmaJaKK(int n, int n2) {
        long l = System.currentTimeMillis();
        l += (long)n2;
        Vector vector = this.amajAkK;
        synchronized (vector) {
            Enumeration enumeration = this.amajAkK.elements();
            if (enumeration == null) return false;
            int n3 = 0;
            while (enumeration.hasMoreElements()) {
                maaamka maaamka2 = (maaamka)enumeration.nextElement();
                if (n3 == 0 && maaamka2.KkAmAja > n) {
                    return true;
                }
                if (maaamka2.KkAmAja >= n && maaamka2.kkAmAja <= l) {
                    return true;
                }
                ++n3;
            }
            return false;
        }
    }

    /*
     * Unable to fully structure code
     */
    synchronized maaamka AMAJaKK(int var1_1) {
        var3_2 = this.amajAkK;
        synchronized (var3_2) {
            var5_3 = this.amajAkK.elements();
            if (var5_3 != null) {
                while (var5_3.hasMoreElements()) {
                    var6_4 = (maaamka)var5_3.nextElement();
                    if (var6_4.KkAmAja < var1_1) continue;
                    var2_5 = var6_4;
                    var4_7 = null;
                    return var2_5;
                }
            }
        }
        block6: while (true) {
            try {
                this.wait();
            }
            catch (Exception v1) {
                return null;
            }
            var2_6 = this.amajAkK.elements();
            if (var2_6 == null) continue;
            do {
                if (var2_6.hasMoreElements()) ** break;
                continue block6;
                var3_2 = (maaamka)var2_6.nextElement();
            } while (var3_2.KkAmAja < var1_1);
            break;
        }
        return var3_2;
    }

    public void amAJaKK() {
        Vector vector = this.amajAkK;
        synchronized (vector) {
            Enumeration enumeration = this.amajAkK.elements();
            if (enumeration != null) {
                while (enumeration.hasMoreElements()) {
                    maaamka maaamka2 = (maaamka)enumeration.nextElement();
                    System.out.println(maaamka2);
                }
            }
            return;
        }
    }

    kmjjkka() {
    }
}
