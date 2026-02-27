/*
 * Decompiled with CFR 0.152.
 */
import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Event;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Image;
import java.net.URL;
import java.util.Hashtable;
import java.util.StringTokenizer;
import muhmu.hifi.device.MAD;

public class forward
extends mmjamma
implements Runnable {
    final static int KamajAk = 512;
    final static int kamajAk = 256;
    static public int KAmajAk;
    static public int kAmajAk;
    majakmk KaMAjAk;
    Image kaMAjAk;
    static public int KAMAjAk;
    static public int kAMAjAk;
    static public boolean KamAjAk;
    static public int kamAjAk;
    static public int KAmAjAk;
    static public boolean kAmAjAk;
    boolean KkAmAjA;
    boolean kkAmAjA;
    mmjjmka KKAmAjA;
    MAD kKAmAjA;
    majjmma KkamAjA;
    maajmmk kkamAjA;
    maajmmk KKamAjA;
    maajmmk kKamAjA;
    boolean KkAMAjA = false;
    static public kaaakka kkAMAjA;
    static public mmaamma KKAMAjA;
    static public kmjamka kKAMAjA;
    static public kmajkka KkaMAjA;
    boolean kkaMAjA;
    int KKaMAjA = -1;
    int kKaMAjA;
    String KkAmajA;
    String[] kkAmajA = new String[]{"init mute95", "init domina", "init saari", "init kukot", "init maku", "init watercube", "init feta", "init uppol", "loaded", "mod 1", "_000", "show mute95", "go 13", "_300 msg mute95 saviour", "_500 msg mute95 jmagic", "_700 msg mute95 jugi", "_900 msg mute95 anis", "_b00 msg mute95 carebear", "_d00 shutdown", "show domina", "go 16", "_f00 msg domina fade2black", "_1024", "clear24 0", "mod 2", "killmod 1", "_000 filmbox", "kill domina", "kill mute95", "go 7", "_000 show saari", "msg saari suh0", "_100 msg saari suh", "_600 msg saari suh", "__8 msg saari suh", "__8 msg saari suh", "__8 msg saari suh", "__8 msg saari suh", "__8 msg saari suh", "__8 msg saari suh", "go 13", "_700 show kukot", "_900 msg kukot suh", "__10 msg kukot suh", "__10 msg kukot suh", "__10 msg kukot suh", "_900 msg kukot suh2", "_a00 msg kukot suh1", "_b00 msg kukot suh0", "__04 msg kukot suh", "__04 msg kukot suh", "__04 msg kukot suh", "__10 msg kukot suh0", "__10 msg kukot suh0", "__04 msg kukot suh", "__04 msg kukot suh", "__04 msg kukot suh", "__10 msg kukot suh0", "__04 msg kukot suh1", "__04 msg kukot suh1", "__04 msg kukot suh1", "_c00 msg kukot suh0", "__10 msg kukot suh0", "__10 msg kukot suh0", "__04 msg kukot suh", "__04 msg kukot suh", "__04 msg kukot suh", "__10 msg kukot suh1", "__04 msg kukot suh2", "__04 msg kukot suh2", "__04 msg kukot suh2", "_d00 show maku", "go 16", "msg maku go 160.5", "msg maku speed -3.0", "_e00", "msg maku go 25.5", "msg maku speed 2", "_e20", "msg maku go 0", "msg maku speed 2.5", "_f00", "msg maku go 42.5", "msg maku speed -2", "_f20", "msg maku ksor", "msg maku go 55.5", "msg maku speed 4", "__8", "msg maku ksor", "__8", "msg maku ksor", "__4", "msg maku ksor", "__4", "msg maku ksor", "__4", "msg maku ksor", "_1000 show watercube", "go 19", "__04 msg watercube pum", "__04 msg watercube rok", "__04 msg watercube suh", "_1030 msg watercube pum", "_1100 msg watercube rok", "msg watercube pum", "__10 msg watercube suh0", "__18 msg watercube suh0", "__8 msg watercube suh0", "_1200 msg watercube suh1", "msg watercube pum", "msg watercube rok", "__10 msg watercube suh0", "msg watercube tex0", "__10 msg watercube suh1", "msg watercube tex1", "__10 msg watercube suh0", "msg watercube tex2", "msg feta 1", "_1300 show feta", "go 21", "_1520 msg feta blackfeta", "_1530 msg feta blackmuna", "_1600 show uppol", "reality", "_22222", "_88800", ""};
    Container KKAmajA;
    Hashtable kKAmajA = new Hashtable();
    Hashtable KkamajA = new Hashtable();
    int kkamajA;
    float KKamajA;
    float kKamajA;
    int KkAMajA;
    static public boolean kkAMajA;
    Graphics KKAMajA;
    Graphics kKAMajA;
    Component KkaMajA;
    int kkaMajA;
    int KKaMajA;
    int kKaMajA;
    int KkAmAJA;
    int kkAmAJA;
    int KKAmAJA = 31;
    Font kKAmAJA = new Font("Courier", 1, 15);
    boolean KkamAJA = true;
    static public forward kkamAJA;
    mmjjmma KKamAJA;
    majjkka kKamAJA;
    int KkAMAJA;
    maaakka kkAMAJA;
    float KKAMAJA;
    float kKAMAJA;
    float KkaMAJA;
    float kkaMAJA;
    boolean KKaMAJA;

    public static void main(String[] stringArray) {
        new forward().aMajAkK(stringArray, 512, 256);
    }

    public void init() {
        kkamAJA = this;
        this.setLayout(null);
        this.setBackground(Color.black);
        this.KkAmAjA = true;
        if (this.getParameter("1x1") != null) {
            this.KkAmAjA = false;
        }
        this.kkAmAjA = false;
        if (this.getParameter("nosound") != null) {
            this.kkAmAjA = true;
        }
    }

    public void start() {
        super.start();
    }

    public void stop() {
        super.stop();
        if (this.KKAmAjA != null) {
            this.KKAmAjA.dispose();
            this.KKAmAjA = null;
        }
        this.kKAmAjA.stop();
        this.kKAmAjA = null;
        this.KkamAjA = null;
    }

    public boolean keyDown(Event event, int n) {
        switch (event.key) {
            case 102: {
                this.KKaMAJA = !this.KKaMAJA;
                break;
            }
            case 115: {
                this.kkaMAjA = true;
            }
        }
        return true;
    }

    public boolean mouseMove(Event event, int n, int n2) {
        KAMAjAk = n;
        kAMAjAk = n2;
        KamAjAk = false;
        return true;
    }

    public boolean mouseUp(Event event, int n, int n2) {
        KamAjAk = false;
        KAMAjAk = n;
        kAMAjAk = n2;
        kAmAjAk = true;
        kamAjAk = n;
        KAmAjAk = n2;
        return true;
    }

    public boolean mouseDown(Event event, int n, int n2) {
        KamAjAk = true;
        KAMAjAk = n;
        kAMAjAk = n2;
        return true;
    }

    void KAMAjak() {
        MAD.component = this;
        this.KaMAjAk = new majakmk(this.KkAmAjA, this.kkAmAjA);
        this.KaMAjAk.reshape(0, 0, 512, 256);
        this.KaMAjAk.show();
        this.add(this.KaMAjAk);
        this.KaMAjAk.kKAMajA();
        this.KaMAjAk.KKaMajA();
        this.remove(this.KaMAjAk);
        this.kKAmAjA = this.KaMAjAk.akKaMAj;
        this.KaMAjAk = null;
        System.gc();
        System.out.println("tuner finito.");
    }

    void kamajak() {
        this.KKAmajA = this.getParent();
        this.KKAmAjA = new mmjjmka("Veni");
        this.KKAmAjA.kamaJAK(this);
        this.KKAmAjA.show();
    }

    void kaMajak() {
    }

    public void KaMAjak() {
        this.KkamAjA = new majjmma();
        this.kKAmAjA.init(this.KkamAjA, 22050, 4, 22050, MAD.component);
    }

    public void kAmAJak(maajmmk maajmmk2, int n) {
        this.kKAmAjA.boost = n;
        this.kKamAjA = null;
        this.kKamAjA = maajmmk2;
        this.kKamAjA.akKaMaJ(this.KkamAjA);
        if (!this.KkAMAjA) {
            this.kKAmAjA.start();
            this.KkAMAjA = true;
        }
    }

    public maajmmk kAmAjak(String string) {
        Object object;
        byte[] byArray = null;
        try {
            object = this.AmaJAkK(this.getDocumentBase(), string);
            byArray = this.AMajAkK((URL)object);
        }
        catch (Exception exception) {
            exception.printStackTrace();
            return null;
        }
        if (byArray == null) {
            return null;
        }
        object = majjmka.KAMAJAK(byArray);
        byArray = null;
        System.gc();
        return object;
    }

    void kaMAjak() {
        int n = 1;
        int n2 = 1;
        if (this.KkAmAjA) {
            n = 2;
            n2 = 2;
        }
        int n3 = 512;
        int n4 = 256;
        kkAMAjA = new kaaakka();
        kkAMAjA.kkamAjA(this, n3, n4, n, n2);
        KKAMAjA = forward.kkAMAjA.aMAjaKk;
        kKAMAjA = new kmjamka();
        kKAMAjA.kkamAjA(this, n3, n4, n, n2);
        KkaMAjA = forward.kKAMAjA.kAmaJAk;
        KAmajAk = forward.KKAMAjA.amAjakK;
        kAmajAk = forward.KKAMAjA.AMAjakK;
    }

    void kAmajak() {
        Graphics graphics = this.KAmAjak();
        int n = 13;
        long l = System.currentTimeMillis();
        int n2 = 0;
        while (n2 < n) {
            kkAMAjA.KKAMAjA(graphics, 0, 0);
            ++n2;
        }
        long l2 = System.currentTimeMillis();
        System.out.println("slowness " + (l2 - l) / (long)n);
    }

    void KamAjak() {
        if (this.kkaMAjA) {
            this.kkaMAjA = false;
            this.kAmajak();
        }
        if (this.KKaMAjA != -1) {
            if (this.kKamAjA == null) {
                return;
            }
            if (!this.kKamAjA.AKkaMaJ(this.KKaMAjA, (int)this.kkAMAJA.kaMaJAK())) {
                return;
            }
        }
        if (this.KkAmajA != null) {
            this.KaMajak(this.KkAmajA);
            this.KkAmajA = null;
        }
        boolean bl = false;
        while (!bl && this.kKaMAjA < this.kkAmajA.length) {
            String string;
            if ((string = this.kkAmajA[this.kKaMAjA++]).trim().length() == 0 || string.charAt(0) == '#') break;
            char c = (string = string.trim()).charAt(0);
            if (c == '_') {
                int n;
                int n2 = this.KKaMAjA;
                int n3 = 1;
                if (string.charAt(1) == '_') {
                    n3 = 2;
                }
                if ((n = string.indexOf(32)) == -1) {
                    this.KKaMAjA = Integer.parseInt(string.substring(n3, string.length()), 16);
                } else {
                    this.KKaMAjA = Integer.parseInt(string.substring(n3, n), 16);
                    this.KkAmajA = string.substring(n + 1, string.length());
                }
                if (n3 == 2) {
                    this.KKaMAjA = n2 + this.KKaMAjA;
                }
                bl = true;
                return;
            }
            this.KaMajak(string);
        }
    }

    void KaMajak(String string) {
        StringTokenizer stringTokenizer = new StringTokenizer(string);
        if (stringTokenizer.hasMoreTokens()) {
            String string2 = stringTokenizer.nextToken();
            if (string2.equals("init")) {
                this.kamAjak(stringTokenizer.nextToken());
                return;
            }
            if (string2.equals("show")) {
                this.kAMAJak(stringTokenizer.nextToken());
                return;
            }
            if (string2.equals("msg")) {
                String string3 = stringTokenizer.nextToken();
                String string4 = "";
                while (stringTokenizer.hasMoreTokens()) {
                    string4 = String.valueOf(string4) + stringTokenizer.nextToken() + " ";
                }
                string4 = string4.trim();
                this.Kamajak(string3, string4);
                return;
            }
            if (string2.equals("shutdown")) {
                this.kamajak();
                return;
            }
            if (string2.equals("post")) {
                this.KAmAJak(Integer.parseInt(stringTokenizer.nextToken()));
                return;
            }
            if (string2.equals("mod")) {
                int n = Integer.parseInt(stringTokenizer.nextToken());
                switch (n) {
                    case 1: {
                        this.kAmAJak(this.kkamAjA, 88);
                        return;
                    }
                    case 2: {
                        this.kAmAJak(this.KKamAjA, 128);
                        return;
                    }
                    default: {
                        return;
                    }
                }
            }
            if (string2.equals("go")) {
                Integer.parseInt(stringTokenizer.nextToken());
                return;
            }
            if (string2.equals("filmbox")) {
                this.kKamAJA = null;
                this.KKamAJA = null;
                return;
            }
            if (string2.equals("clear24")) {
                int n = Integer.parseInt(stringTokenizer.nextToken(), 16);
                kkAMAjA.KAMaJAK(n);
                return;
            }
            if (string2.equals("clear8")) {
                Integer.parseInt(stringTokenizer.nextToken(), 16);
                kKAMAjA.KkAMAjA();
                return;
            }
            if (string2.equals("kill")) {
                this.KAMajak(stringTokenizer.nextToken());
                return;
            }
            if (string2.equals("killmod")) {
                int n = Integer.parseInt(stringTokenizer.nextToken());
                if (n == 1) {
                    this.kkamAjA = null;
                    return;
                }
            } else {
                if (string2.equals("reality")) {
                    this.KKAmAjA.remove(this);
                    this.move(0, 0);
                    this.KKAmajA.add("Center", this);
                    this.KKAmajA.layout();
                    this.show();
                    this.KKAmAjA.dispose();
                    return;
                }
                if (string2.equals("loaded")) {
                    this.KkamAJA = false;
                    System.out.println("ml=" + this.kkAmAJA);
                    this.repaint(0L);
                }
            }
        }
    }

    void kamAjak(String string) {
        System.out.print(String.valueOf(this.kkamajA++));
        mmjjmma mmjjmma2 = (mmjjmma)this.kKAmajA.get(string);
        if (mmjjmma2 != null) {
            mmjjmma2.MajakkA(this);
            System.runFinalization();
            System.gc();
            return;
        }
        majjkka majjkka2 = (majjkka)this.KkamajA.get(string);
        if (majjkka2 != null) {
            majjkka2.AmAjAkk(this);
            System.runFinalization();
            System.gc();
            return;
        }
        System.out.println("muhmuscript error: no such routine " + string);
    }

    void Kamajak(String string, String string2) {
        mmjjmma mmjjmma2 = (mmjjmma)this.kKAmajA.get(string);
        if (mmjjmma2 != null) {
            mmjjmma2.MAJakkA(string2, this.kKAMAJA - this.kkaMAJA);
            return;
        }
        majjkka majjkka2 = (majjkka)this.KkamajA.get(string);
        if (majjkka2 != null) {
            majjkka2.AMaJaKK(string2, this.kKAMAJA - this.kkaMAJA);
            return;
        }
        System.out.println("muhmuscript error: no such routine " + string);
    }

    void kAMAJak(String string) {
        this.kkAMAJA.kAmAjaK(80L);
        this.kKAMAJA = (float)this.kkAMAJA.kamAjaK() / 1000.0f;
        this.KkaMAJA = this.kKAMAJA - 0.08f;
        this.kkaMAJA = this.kKAMAJA;
        mmjjmma mmjjmma2 = (mmjjmma)this.kKAmajA.get(string);
        if (mmjjmma2 != null) {
            mmjjmma2.MAjakkA();
            this.KKamAJA = mmjjmma2;
            this.kKamAJA = null;
            return;
        }
        majjkka majjkka2 = (majjkka)this.KkamajA.get(string);
        if (majjkka2 != null) {
            majjkka2.AMAjAkk();
            this.KKamAJA = null;
            this.kKamAJA = majjkka2;
            return;
        }
        System.out.println("muhmuscript error: no such routine " + string);
    }

    void KAMajak(String string) {
        mmjjmma mmjjmma2 = (mmjjmma)this.kKAmajA.get(string);
        if (mmjjmma2 != null) {
            mmjjmma2.mAJakkA();
            this.kKAmajA.remove(string);
            System.runFinalization();
            System.gc();
            return;
        }
        majjkka majjkka2 = (majjkka)this.KkamajA.get(string);
        if (majjkka2 != null) {
            majjkka2.aMaJaKK();
            this.KkamajA.remove(string);
            System.runFinalization();
            System.gc();
            return;
        }
        System.out.println("muhmuscript error: no such routine " + string);
    }

    void KAmAJak(int n) {
        this.kKamajA = 0.0f;
        this.KkAMajA = n;
        this.KKamajA = this.kKAMAJA;
    }

    void kAMAjak(mmjjmma mmjjmma2) {
        this.kKAmajA.put(mmjjmma2.majakkA(), mmjjmma2);
    }

    void KAmajak(majjkka majjkka2) {
        this.KkamajA.put(majjkka2.amAjAkk(), majjkka2);
    }

    public void kamAJak() {
        this.kAMAjak(new kmjjkmk());
        this.kAMAjak(new maajmka());
        this.kAMAjak(new kmaamka());
        this.kAMAjak(new kajjkka());
        this.kAMAjak(new kmajmka());
        this.kAMAjak(new kmjjmka());
        this.KAmajak(new kajakka());
        this.KAmajak(new mmaakmk());
    }

    public void run() {
        this.KAMAjak();
        System.gc();
        System.runFinalization();
        this.kaMAjak();
        this.kamAJak();
        this.kaMajak();
        this.KaMAjak();
        this.kkamAjA = this.kAmAjak("mods/kuninga.xm");
        this.kAMajak();
        this.KKamAjA = this.kAmAjak("mods/jarnomix.xm");
        this.kAMajak();
        this.kkAMAJA = new maaakka(10, 60);
        this.KamAJak(this, 0, 0, 512, 256);
        try {
            while (this.majAkka != null) {
                if (!kkAMajA) {
                    kkAMajA = true;
                    this.KKAMajA = this.KAmAjak();
                }
                this.KamAjak();
                Graphics graphics = this.KKAMajA;
                if (this.KKaMAJA) {
                    this.KaMaJAk(graphics);
                }
                this.kKAMAJA = (float)this.kkAMAJA.kamAjaK() / 1000.0f;
                this.KKAMAJA = this.kKAMAJA - this.KkaMAJA;
                if (this.KKamAJA != null) {
                    this.KKamAJA.maJakkA(KKAMAjA, this.kKAMAJA - this.kkaMAJA, this.KKAMAJA);
                    switch (this.KkAMajA) {
                        case 1: {
                            KKAMAjA.AMaJakK(mmaamma.aMAJAKK(255, 255, 255));
                            break;
                        }
                        case 2: {
                            KKAMAjA.aMajAKK(Math.max(0.0f, 0.93f - (this.kKAMAJA - this.KKamajA) * 2.0f));
                            break;
                        }
                    }
                    kkAMAjA.KKAMAjA(graphics, 0, 0);
                }
                if (this.kKamAJA != null && graphics != null) {
                    this.kKamAJA.amaJaKK(graphics, this.kKAMAJA - this.kkaMAJA, this.KKAMAJA);
                }
                this.KkaMAJA = this.kKAMAJA;
                this.kkAMAJA.KAmAjaK();
                ++this.KkAMAJA;
                Thread.sleep(2L);
            }
            return;
        }
        catch (Exception exception) {
            exception.printStackTrace();
            return;
        }
    }

    void KaMaJAk(Graphics graphics) {
        int n = 2;
        int n2 = 12;
        String string = String.valueOf((int)this.kkAMAJA.kaMaJAK());
        graphics.setColor(Color.black);
        graphics.drawString(string, n + 1, n2);
        graphics.drawString(string, n - 1, n2);
        graphics.drawString(string, n, n2 + 1);
        graphics.drawString(string, n, n2 - 1);
        graphics.setColor(Color.white);
        graphics.drawString(string, n, n2);
    }

    void KamAJak(Component component, int n, int n2, int n3, int n4) {
        this.KkaMajA = component;
        this.kkaMajA = n;
        this.KKaMajA = n2;
        this.kKaMajA = n3;
        this.KkAmAJA = n4;
        kkAMajA = false;
    }

    Graphics KAmAjak() {
        Graphics graphics = this.KkaMajA.getGraphics();
        return graphics.create(this.kkaMajA, this.KKaMajA, this.kKaMajA, this.KkAmAJA);
    }

    public void paint(Graphics graphics) {
        kkAMajA = false;
        if (this.KkamAJA) {
            String string = "FORWARD :: KOMPLEX";
            FontMetrics fontMetrics = this.getFontMetrics(this.kKAmAJA);
            int n = fontMetrics.stringWidth(string) + 14;
            int n2 = 20;
            int n3 = (512 - n) / 2;
            int n4 = (256 - n2) / 2;
            graphics.setFont(this.kKAmAJA);
            graphics.setColor(Color.black);
            graphics.fillRect(n3, n4, n, n2);
            graphics.setColor(Color.white);
            graphics.drawRect(n3, n4, n, n2);
            graphics.drawString(string, n3 + 7, n4 + 14);
            int n5 = this.kkAmAJA * (n - 1) / this.KKAmAJA;
            if (n5 > n - 1) {
                n5 = n - 1;
            }
            graphics.setXORMode(Color.black);
            graphics.fillRect(n3 + 1, n4 + 1, n5, n2 - 1);
            graphics.setPaintMode();
        }
    }

    public void kAMajak() {
        ++this.kkAmAJA;
        this.paint(this.getGraphics());
    }
}
