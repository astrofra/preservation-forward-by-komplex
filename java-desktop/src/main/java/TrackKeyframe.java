/*
 * Decompiled with CFR 0.152.
 */
public class TrackKeyframe {
    public float render;
    public float handleMessage;
    public float dispose;
    public float init;
    public float scriptName;
    public float onShow;
    public float[] mAjakkA;
    public int[] MaJAkkA;
    public Quaternionf[] maJAkkA;
    public Quaternionf[] MAJAkkA;
    public Quaternionf[] mAJAkkA;

    public TrackKeyframe(float f, float f2, float f3, float f4, float[] fArray) {
        this.render = f;
        this.handleMessage = f2;
        this.dispose = f3;
        this.init = f4;
        this.mAjakkA = fArray;
    }

    public TrackKeyframe(float f, TrackKeyframe kmjjmma2) {
        this.render = f;
        this.handleMessage = kmjjmma2.handleMessage;
        this.dispose = kmjjmma2.dispose;
        this.init = kmjjmma2.init;
        this.mAjakkA = kmjjmma2.mAjakkA;
        this.MaJAkkA = kmjjmma2.MaJAkkA;
        this.maJAkkA = kmjjmma2.maJAkkA;
    }

    public TrackKeyframe(TrackKeyframe kmjjmma2) {
        this.render = kmjjmma2.render;
        this.handleMessage = kmjjmma2.handleMessage;
        this.dispose = kmjjmma2.dispose;
        this.init = kmjjmma2.init;
        this.mAjakkA = kmjjmma2.mAjakkA;
        this.MaJAkkA = kmjjmma2.MaJAkkA;
        this.maJAkkA = kmjjmma2.maJAkkA;
    }

    public TrackKeyframe(float f, float f2, float f3, float f4, float f5) {
        this.render = f;
        this.handleMessage = f2;
        this.dispose = f3;
        this.init = f4;
        float[] fArray = new float[]{f5};
        this.mAjakkA = fArray;
    }

    public TrackKeyframe(float f, float f2, float f3, float f4, float f5, float f6) {
        this.render = f;
        this.handleMessage = f2;
        this.dispose = f3;
        this.init = f4;
        float[] fArray = new float[]{f5, f6};
        this.mAjakkA = fArray;
    }

    public TrackKeyframe(float f, float f2, float f3, float f4, float f5, float f6, float f7) {
        this.render = f;
        this.handleMessage = f2;
        this.dispose = f3;
        this.init = f4;
        float[] fArray = new float[]{f5, f6, f7};
        this.mAjakkA = fArray;
    }

    public TrackKeyframe(float f, float f2, float f3, float f4, float f5, float f6, float f7, float f8) {
        this.render = f;
        this.handleMessage = f2;
        this.dispose = f3;
        this.init = f4;
        float[] fArray = new float[]{f5, f6, f7, f8};
        this.mAjakkA = fArray;
    }

    public TrackKeyframe(float f, float f2, float f3, float f4, Quaternionf maaakmk2) {
        this.render = f;
        this.handleMessage = f2;
        this.dispose = f3;
        this.init = f4;
        Quaternionf[] maaakmkArray = new Quaternionf[]{maaakmk2};
        this.maJAkkA = maaakmkArray;
    }

    public TrackKeyframe(float f, float f2, float f3, float f4, Quaternionf[] maaakmkArray) {
        this.render = f;
        this.handleMessage = f2;
        this.dispose = f3;
        this.init = f4;
        this.maJAkkA = maaakmkArray;
    }
}
