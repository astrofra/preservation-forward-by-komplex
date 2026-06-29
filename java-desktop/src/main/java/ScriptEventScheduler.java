/*
 * Decompiled with CFR 0.152.
 */
public class ScriptEventScheduler
extends Thread {
    int jAkKAMA;
    ScheduledScriptEvent[] JaKkaMA;
    ScriptEventListener jaKkaMA;
    int JAKkaMA;
    int jAKkaMA;
    long JakkaMA = -1L;

    public ScriptEventScheduler(int n, ScriptEventListener kajakma2) {
        super("Muhmu Event Pipe");
        this.JaKkaMA = new ScheduledScriptEvent[n];
        this.jaKkaMA = kajakma2;
    }

    public void KKaMAja(int n, String string) {
        this.JaKkaMA[this.jAkKAMA++] = new ScheduledScriptEvent(n, string);
    }

    public void kkaMAja(int n, long l) {
        ScheduledScriptEvent kmaakmk2;
        float f;
        while (true) {
            if (this.JAKkaMA >= this.jAkKAMA) {
                return;
            }
            f = n;
            kmaakmk2 = this.JaKkaMA[this.JAKkaMA];
            if (f != kmaakmk2.majAkKa) break;
            kmaakmk2.JaKKamA = l;
            if (this.JAKkaMA == this.jAKkaMA) {
                this.kKaMAja();
            }
            ++this.JAKkaMA;
        }
        if (f > kmaakmk2.majAkKa) {
            return;
        }
    }

    synchronized void kKaMAja() {
        this.notify();
    }

    public void start() {
        new DepthSorter(this.JaKkaMA).MaJakkA(this.jAkKAMA);
        this.jAKkaMA = 0;
        super.start();
    }

    public synchronized void run() {
        try {
            this.jAKkaMA = 0;
            while (this.jAKkaMA < this.jAkKAMA) {
                long l;
                ScheduledScriptEvent kmaakmk2 = this.JaKkaMA[this.jAKkaMA];
                if (kmaakmk2.JaKKamA == 0L) {
                    this.wait();
                }
                if ((l = kmaakmk2.JaKKamA - System.currentTimeMillis()) > 0L) {
                    Thread.sleep(l);
                }
                if (this.jaKkaMA != null) {
                    this.jaKkaMA.MaJaKkA((int)kmaakmk2.majAkKa, kmaakmk2.jaKKamA);
                } else {
                    System.out.println(kmaakmk2.jaKKamA);
                }
                ++this.jAKkaMA;
            }
        }
        catch (Exception exception) {
            exception.printStackTrace();
        }
        System.out.println("muhmupipe/muhmuscript finished. (c) saviour.");
    }
}
