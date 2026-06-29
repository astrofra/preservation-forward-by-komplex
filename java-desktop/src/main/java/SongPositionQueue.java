/*
 * Decompiled with CFR 0.152.
 */
import java.util.Enumeration;
import java.util.Vector;

class SongPositionQueue {
    Vector amajAkK = new Vector(100);

    synchronized void AmAJaKK(int n, long l) {
        Vector vector = this.amajAkK;
        synchronized (vector) {
            ScheduledSongPosition maaamka2 = new ScheduledSongPosition(n, l);
            this.amajAkK.addElement(maaamka2);
            long l2 = System.currentTimeMillis();
            Enumeration enumeration = this.amajAkK.elements();
            if (enumeration != null) {
                while (enumeration.hasMoreElements()) {
                    ScheduledSongPosition maaamka3 = (ScheduledSongPosition)enumeration.nextElement();
                    if (maaamka3.kkAmAja > l2) continue;
                    this.amajAkK.removeElement(maaamka3);
                }
            }
        }
        this.notify();
    }

    int aMAJaKK(int n) {
        ScheduledSongPosition maaamka2 = this.AMAJaKK(n);
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
                ScheduledSongPosition maaamka2 = (ScheduledSongPosition)enumeration.nextElement();
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

    synchronized ScheduledSongPosition AMAJaKK(int var1_1) {
        synchronized (this.amajAkK) {
            Enumeration enumeration = this.amajAkK.elements();
            while (enumeration != null && enumeration.hasMoreElements()) {
                ScheduledSongPosition maaamka2 = (ScheduledSongPosition)enumeration.nextElement();
                if (maaamka2.KkAmAja >= var1_1) {
                    return maaamka2;
                }
            }
        }
        while (true) {
            try {
                this.wait();
            }
            catch (Exception exception) {
                return null;
            }
            Enumeration enumeration = this.amajAkK.elements();
            while (enumeration != null && enumeration.hasMoreElements()) {
                ScheduledSongPosition maaamka2 = (ScheduledSongPosition)enumeration.nextElement();
                if (maaamka2.KkAmAja >= var1_1) {
                    return maaamka2;
                }
            }
        }
    }

    public void amAJaKK() {
        Vector vector = this.amajAkK;
        synchronized (vector) {
            Enumeration enumeration = this.amajAkK.elements();
            if (enumeration != null) {
                while (enumeration.hasMoreElements()) {
                    ScheduledSongPosition maaamka2 = (ScheduledSongPosition)enumeration.nextElement();
                    System.out.println(maaamka2);
                }
            }
            return;
        }
    }

    SongPositionQueue() {
    }
}
