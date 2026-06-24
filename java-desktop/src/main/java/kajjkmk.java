/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Event;
import java.awt.Frame;

class kajjkmk
extends Frame {
    public kajjkmk(String string) {
        super(string);
    }

    public boolean handleEvent(Event event) {
        switch (event.id) {
            case 201: {
                this.dispose();
                System.exit(0);
                return true;
            }
        }
        return super.handleEvent(event);
    }
}
