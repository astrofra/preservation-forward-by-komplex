/*
 * Decompiled with CFR 0.152.
 */
class ScheduledSongPosition {
    public int KkAmAja;
    public long kkAmAja;

    ScheduledSongPosition(int n, long l) {
        this.KkAmAja = n;
        this.kkAmAja = l;
    }

    public String toString() {
        return "pos=0x" + Integer.toHexString(this.KkAmAja) + " time=" + this.kkAmAja;
    }
}
