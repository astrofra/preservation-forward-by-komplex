/*
 * Decompiled with CFR 0.152.
 */
package muhmu.hifi.device;

import java.awt.Component;
import muhmu.hifi.device.MAD;
import muhmu.hifi.device.Mixable;

public abstract class DeviceMSbase
extends MAD
implements Runnable {
    Mixable mixable;
    int len;
    Thread mixer;
    int splitAm;
    int[] buffer;
    int[] bufferEmpty;
    byte[] buffer8;
    boolean bit16;
    int bytesPerSample;
    int format;
    int lag = 40;

    public boolean init(Mixable mixable, int n, int n2, int n3, Component component) {
        this.mixable = mixable;
        MAD.component = component;
        this.splitAm = 10;
        this.len = n / this.splitAm;
        this.setFormat(n2);
        this.setFrequency(n3);
        this.initMixingBuffers();
        try {
            this.initDS();
            return true;
        }
        catch (Exception exception) {
            exception.printStackTrace();
            return false;
        }
    }

    void initMixingBuffers() {
        this.buffer = new int[this.len];
        this.bufferEmpty = new int[this.len];
        this.buffer8 = new byte[this.len * this.bytesPerSample * this.splitAm];
    }

    void setFormat(int n) {
        this.format = n;
        switch (n) {
            case 1: {
                this.stereo = false;
                this.bit16 = false;
                this.bytesPerSample = 1;
                return;
            }
            case 2: {
                this.stereo = false;
                this.bit16 = true;
                this.bytesPerSample = 2;
                return;
            }
            case 3: {
                this.stereo = true;
                this.bit16 = false;
                this.bytesPerSample = 2;
                return;
            }
            case 4: {
                this.stereo = true;
                this.bit16 = true;
                this.bytesPerSample = 4;
                return;
            }
        }
    }

    void setFrequency(int n) {
        this.frequency = n;
    }

    public void start() {
        if (this.mixer == null) {
            this.mixer = new Thread((Runnable)this, "Muhmu DirectSound device");
            this.mixer.start();
            this.mixer.setPriority(10);
        }
    }

    public void stop() {
        if (this.mixer != null) {
            this.mixer.stop();
            this.mixer = null;
        }
    }

    void clearBuffer() {
        System.arraycopy(this.bufferEmpty, 0, this.buffer, 0, this.buffer.length);
    }

    /*
     * Exception decompiling
     */
    void hoxBuff(int[] var1_1, int var2_2, byte[] var3_3, int var4_4, int var5_5) {
        /*
         * This method has failed to decompile.  When submitting a bug report, please provide this stack trace, and (if you hold appropriate legal rights) the relevant class file.
         * 
         * org.benf.cfr.reader.util.ConfusedCFRException: First case is not immediately after switch.
         *     at org.benf.cfr.reader.bytecode.analysis.opgraph.op3rewriters.SwitchReplacer.examineSwitchContiguity(SwitchReplacer.java:442)
         *     at org.benf.cfr.reader.bytecode.analysis.opgraph.op3rewriters.SwitchReplacer.replaceRawSwitches(SwitchReplacer.java:94)
         *     at org.benf.cfr.reader.bytecode.CodeAnalyser.getAnalysisInner(CodeAnalyser.java:517)
         *     at org.benf.cfr.reader.bytecode.CodeAnalyser.getAnalysisOrWrapFail(CodeAnalyser.java:278)
         *     at org.benf.cfr.reader.bytecode.CodeAnalyser.getAnalysis(CodeAnalyser.java:201)
         *     at org.benf.cfr.reader.entities.attributes.AttributeCode.analyse(AttributeCode.java:94)
         *     at org.benf.cfr.reader.entities.Method.analyse(Method.java:531)
         *     at org.benf.cfr.reader.entities.ClassFile.analyseMid(ClassFile.java:1055)
         *     at org.benf.cfr.reader.entities.ClassFile.analyseTop(ClassFile.java:942)
         *     at org.benf.cfr.reader.Driver.doClass(Driver.java:84)
         *     at org.benf.cfr.reader.CfrDriverImpl.analyse(CfrDriverImpl.java:78)
         *     at org.benf.cfr.reader.Main.main(Main.java:54)
         */
        throw new IllegalStateException("Decompilation failed");
    }

    public void run() {
        try {
            this.play();
            while (this.mixer != null) {
                int n = 0;
                while (n < this.splitAm) {
                    int n2 = n * this.len;
                    int n3 = (n + 1) * this.len;
                    int n4 = this.playPos();
                    this.bufferStartTime = (long)((this.buffer8.length + n2 * this.bytesPerSample - n4) / this.bytesPerSample * 1000 / this.frequency + this.lag) + System.currentTimeMillis();
                    this.clearBuffer();
                    this.mixable.mix(this, this.buffer, 0, this.buffer.length);
                    this.hoxBuff(this.buffer, 0, this.buffer8, n * this.buffer.length * this.bytesPerSample, this.buffer.length);
                    while ((n4 = this.playPos()) <= n3 * this.bytesPerSample && n4 >= n2 * this.bytesPerSample) {
                        int n5 = (n3 - n4 / this.bytesPerSample) * 1000 / this.frequency;
                        if ((n5 += 20) < 20) {
                            n5 = 20;
                        }
                        Thread.sleep(n5);
                    }
                    this.write(n);
                    ++n;
                }
            }
            return;
        }
        catch (Exception exception) {
            exception.printStackTrace();
            return;
        }
    }

    public abstract void initDS() throws Exception;

    public abstract void write(int var1);

    public abstract int playPos();

    public abstract void play();
}
