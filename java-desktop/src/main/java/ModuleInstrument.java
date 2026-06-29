/*
 * Decompiled with CFR 0.152.
 */
public class ModuleInstrument {
    ModuleSample[] AkkamAJ = new ModuleSample[96];
    Envelope akkamAJ;
    Envelope AKkamAJ;
    int aKkamAJ;
    int AkKAmAJ;
    int akKAmAJ;
    int AKKAmAJ;

    public void routineRegistry(ModuleSample mmajmka2) {
        int n = 0;
        while (n < 96) {
            this.AkkamAJ[n] = mmajmka2;
            ++n;
        }
    }

    public void sceneRegistry(int n, ModuleSample mmajmka2) {
        this.AkkamAJ[n] = mmajmka2;
    }
}
