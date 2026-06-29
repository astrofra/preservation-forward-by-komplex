/*
 * Decompiled with CFR 0.152.
 */
import java.io.DataInputStream;
import java.io.InputStream;
import java.net.URL;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.StringTokenizer;
import java.util.Vector;
import java.util.zip.GZIPInputStream;

public class AseSceneLoader
extends SceneRenderer {
    SceneRenderer aMajAKK;
    public kajakmk AmAJAKK;
    public kajakmk amAJAKK;
    Hashtable AMAJAKK;
    Hashtable aMAJAKK;
    Camera AmaJAKK = new Camera();
    Vec3f amaJAKK = new Vec3f();
    Vector AMaJAKK;

    public void kAMajaK(float f) {
        this.kAMAJaK(f, null);
    }

    public void kAMAJaK(float f, Camera kaajmmk2) {
        if (this.AmAJAKK == null || this.amAJAKK == null) {
            return;
        }
        this.AmAJAKK.KKAmAJA(f * 1000.0f);
        this.amAJAKK.KKAmAJA(f * 1000.0f);
        float[] fArray = this.AmAJAKK.akkAmAj;
        maaakmk[] maaakmkArray = this.amAJAKK.aKkAmAj;
        int n = fArray.length / 3 - 2;
        if (kaajmmk2 != null) {
            kaajmmk2.JAKkaMa.MaJaKka(fArray[0], fArray[1], fArray[2]);
            kaajmmk2.AkKAMaJ(new Vec3f(fArray[3], fArray[4], fArray[5]));
        }
        int n2 = 0;
        while (n2 < n) {
            MeshObject mmajmmk2 = (MeshObject)this.MaJAKkA.elementAt(n2);
            int n3 = (n2 + 2) * 3;
            mmajmmk2.jaKKaMA.MaJaKka(fArray[n3], fArray[n3 + 1], fArray[n3 + 2]);
            maaakmkArray[n2 + 2].kKamaJA(mmajmmk2.JakKaMA);
            mmajmmk2.JaKkama = f;
            ++n2;
        }
    }

    Vector KAMajaK(DataInputStream dataInputStream) throws Exception {
        Vector<float[]> vector = new Vector<float[]>();
        int n = 1;
        while (n > 0) {
            String string;
            String string2 = dataInputStream.readLine();
            StringTokenizer stringTokenizer = new StringTokenizer(string2 = string2.replace('\t', ' ').trim(), " ");
            String string3 = string = stringTokenizer.hasMoreTokens() ? stringTokenizer.nextToken() : "";
            if (string2.endsWith("{")) {
                ++n;
            }
            if (string2.endsWith("}")) {
                --n;
            }
            if (string.compareTo("*CONTROL_POS_SAMPLE") != 0) continue;
            float f = AseSceneLoader.kamajaK(stringTokenizer.nextToken());
            float f2 = AseSceneLoader.kamajaK(stringTokenizer.nextToken());
            float f3 = AseSceneLoader.kamajaK(stringTokenizer.nextToken());
            float f4 = AseSceneLoader.kamajaK(stringTokenizer.nextToken());
            float[] fArray = new float[]{f, f2, f3, f4};
            vector.addElement(fArray);
        }
        return vector;
    }

    Vector KAmaJaK(DataInputStream dataInputStream) throws Exception {
        Vector<float[]> vector = new Vector<float[]>();
        int n = 1;
        while (n > 0) {
            String string;
            String string2 = dataInputStream.readLine();
            StringTokenizer stringTokenizer = new StringTokenizer(string2 = string2.replace('\t', ' ').trim(), " ");
            String string3 = string = stringTokenizer.hasMoreTokens() ? stringTokenizer.nextToken() : "";
            if (string2.endsWith("{")) {
                ++n;
            }
            if (string2.endsWith("}")) {
                --n;
            }
            if (string.compareTo("*CONTROL_ROT_SAMPLE") != 0) continue;
            float f = AseSceneLoader.kamajaK(stringTokenizer.nextToken());
            float f2 = AseSceneLoader.kamajaK(stringTokenizer.nextToken());
            float f3 = AseSceneLoader.kamajaK(stringTokenizer.nextToken());
            float f4 = AseSceneLoader.kamajaK(stringTokenizer.nextToken());
            float f5 = AseSceneLoader.kamajaK(stringTokenizer.nextToken());
            float[] fArray = new float[]{f, f2, f3, f4, f5};
            vector.addElement(fArray);
        }
        return vector;
    }

    void KamaJaK(DataInputStream dataInputStream, MeshObject mmajmmk2) throws Exception {
        boolean bl = false;
        while (!bl) {
            String string;
            String string2 = dataInputStream.readLine();
            StringTokenizer stringTokenizer = new StringTokenizer(string2 = string2.replace('\t', ' ').trim(), " ");
            String string3 = string = stringTokenizer.hasMoreTokens() ? stringTokenizer.nextToken() : "";
            if (string.compareTo("}") == 0) {
                bl = true;
                return;
            }
            if (string.compareTo("*MESH_VERTEX") != 0) continue;
            int n = AseSceneLoader.KaMAJaK(stringTokenizer.nextToken());
            double d = AseSceneLoader.kamaJaK(stringTokenizer.nextToken());
            double d2 = AseSceneLoader.kamaJaK(stringTokenizer.nextToken());
            double d3 = AseSceneLoader.kamaJaK(stringTokenizer.nextToken());
            mmajmmk2.jAkkaMA[n] = new Vertex(d, d2, d3);
        }
    }

    void kaMajaK(DataInputStream dataInputStream, MeshObject mmajmmk2) throws Exception {
        boolean bl = false;
        while (!bl) {
            String string;
            String string2 = dataInputStream.readLine();
            if (string2 == null) {
                return;
            }
            StringTokenizer stringTokenizer = new StringTokenizer(string2 = string2.replace('\t', ' ').trim(), " ");
            String string3 = string = stringTokenizer.hasMoreTokens() ? stringTokenizer.nextToken() : "";
            if (string.compareTo("}") == 0) {
                bl = true;
                return;
            }
            if (string.compareTo("*MESH_TVERT") != 0) continue;
            int n = AseSceneLoader.KaMAJaK(stringTokenizer.nextToken());
            double d = AseSceneLoader.kamaJaK(stringTokenizer.nextToken());
            double d2 = AseSceneLoader.kamaJaK(stringTokenizer.nextToken());
            mmajmmk2.JaKKaMA[n] = new UvCoord(d, d2);
        }
    }

    void KaMajaK(DataInputStream dataInputStream, MeshObject mmajmmk2) throws Exception {
        boolean bl = false;
        while (!bl) {
            String string;
            String string2 = dataInputStream.readLine();
            if (string2 == null) {
                return;
            }
            string2 = string2.replace('\t', ' ').trim();
            StringTokenizer stringTokenizer = new StringTokenizer(string2 = string2.replace(':', ' '), " ");
            String string3 = string = stringTokenizer.hasMoreTokens() ? stringTokenizer.nextToken() : "";
            if (string.compareTo("}") == 0) {
                bl = true;
                return;
            }
            if (string.compareTo("*MESH_FACE") != 0) continue;
            int n = AseSceneLoader.KaMAJaK(stringTokenizer.nextToken());
            stringTokenizer.nextToken();
            int n2 = AseSceneLoader.KaMAJaK(stringTokenizer.nextToken());
            stringTokenizer.nextToken();
            int n3 = AseSceneLoader.KaMAJaK(stringTokenizer.nextToken());
            stringTokenizer.nextToken();
            int n4 = AseSceneLoader.KaMAJaK(stringTokenizer.nextToken());
            stringTokenizer.nextToken();
            mmajmmk2.JAkkaMA[n] = new Triangle(mmajmmk2, n2, n3, n4, n2, n3, n4);
        }
    }

    void kaMAJaK(DataInputStream dataInputStream, MeshObject mmajmmk2) throws Exception {
        boolean bl = false;
        while (!bl) {
            String string;
            String string2 = dataInputStream.readLine();
            if (string2 == null) {
                return;
            }
            StringTokenizer stringTokenizer = new StringTokenizer(string2 = string2.replace('\t', ' ').trim(), " ");
            String string3 = string = stringTokenizer.hasMoreTokens() ? stringTokenizer.nextToken() : "";
            if (string.compareTo("}") == 0) {
                bl = true;
                return;
            }
            if (string.compareTo("*MESH_TFACE") != 0) continue;
            int n = AseSceneLoader.KaMAJaK(stringTokenizer.nextToken());
            int n2 = AseSceneLoader.KaMAJaK(stringTokenizer.nextToken());
            int n3 = AseSceneLoader.KaMAJaK(stringTokenizer.nextToken());
            int n4 = AseSceneLoader.KaMAJaK(stringTokenizer.nextToken());
            Triangle kmaamma2 = mmajmmk2.JAkkaMA[n];
            kmaamma2.MAJAkKa = mmajmmk2.JaKKaMA[n2];
            kmaamma2.mAJAkKa = mmajmmk2.JaKKaMA[n3];
            kmaamma2.MajAkKa = mmajmmk2.JaKKaMA[n4];
        }
    }

    MeshObject KAmAJaK(DataInputStream dataInputStream) throws Exception {
        MeshObject mmajmmk2 = new MeshObject();
        int n = 1;
        while (n > 0) {
            String string;
            String string2 = dataInputStream.readLine();
            if (string2 == null) {
                return null;
            }
            StringTokenizer stringTokenizer = new StringTokenizer(string2.trim(), " ");
            String string3 = string = stringTokenizer.hasMoreTokens() ? stringTokenizer.nextToken() : "";
            if (string2.endsWith("{")) {
                ++n;
            }
            if (string2.endsWith("}")) {
                --n;
            }
            if (string.compareTo("*MESH_NUMVERTEX") == 0) {
                mmajmmk2.jAkkaMA = new Vertex[AseSceneLoader.KaMAJaK(stringTokenizer.nextToken())];
                mmajmmk2.JaKKaMA = new UvCoord[mmajmmk2.jAkkaMA.length];
                int n2 = 0;
                while (n2 < mmajmmk2.JaKKaMA.length) {
                    mmajmmk2.JaKKaMA[n2] = new UvCoord(0.0f, 0.0f);
                    ++n2;
                }
            }
            if (string.compareTo("*MESH_NUMFACES") == 0) {
                mmajmmk2.JAkkaMA = new Triangle[AseSceneLoader.KaMAJaK(stringTokenizer.nextToken())];
            }
            if (string.compareTo("*MESH_VERTEX_LIST") == 0) {
                this.KamaJaK(dataInputStream, mmajmmk2);
                --n;
            }
            if (string.compareTo("*MESH_FACE_LIST") == 0) {
                this.KaMajaK(dataInputStream, mmajmmk2);
                --n;
            }
            if (string.compareTo("*MESH_NUMTVERTEX") == 0) {
                mmajmmk2.JaKKaMA = new UvCoord[AseSceneLoader.KaMAJaK(stringTokenizer.nextToken())];
            }
            if (string.compareTo("*MESH_TVERTLIST") == 0) {
                this.kaMajaK(dataInputStream, mmajmmk2);
                --n;
            }
            if (string.compareTo("*MESH_TFACELIST") != 0) continue;
            this.kaMAJaK(dataInputStream, mmajmmk2);
            --n;
        }
        return mmajmmk2;
    }

    void KAMAJaK(DataInputStream dataInputStream, Object object) throws Exception {
        int n = 1;
        while (n > 0) {
            String string;
            String string2 = dataInputStream.readLine();
            if (string2 == null) {
                return;
            }
            StringTokenizer stringTokenizer = new StringTokenizer(string2 = string2.replace('\t', ' ').trim(), " ");
            String string3 = string = stringTokenizer.hasMoreTokens() ? stringTokenizer.nextToken() : "";
            if (string2.endsWith("{")) {
                ++n;
            }
            if (string2.endsWith("}")) {
                --n;
            }
            if (string.compareTo("*CONTROL_POS_TCB") == 0) {
                --n;
            }
            if (string.compareTo("*CONTROL_POS_TRACK") == 0) {
                this.AMAJAKK.put(object, this.KAMajaK(dataInputStream));
                --n;
            }
            if (string.compareTo("*CONTROL_ROT_TRACK") != 0) continue;
            this.aMAJAKK.put(object, this.KAmaJaK(dataInputStream));
            --n;
        }
    }

    Vec3f[] kamAJaK(DataInputStream dataInputStream) throws Exception {
        Vec3f[] mmajmmaArray = new Vec3f[1];
        boolean bl = false;
        while (!bl) {
            String string;
            String string2 = dataInputStream.readLine();
            if (string2 == null) {
                return null;
            }
            StringTokenizer stringTokenizer = new StringTokenizer(string2 = string2.replace('\t', ' ').trim(), " ");
            String string3 = string = stringTokenizer.hasMoreTokens() ? stringTokenizer.nextToken() : "";
            if (string.compareTo("}") == 0) {
                bl = true;
                break;
            }
            if (string.compareTo("*TM_POS") != 0) continue;
            double d = AseSceneLoader.kamaJaK(stringTokenizer.nextToken());
            double d2 = AseSceneLoader.kamaJaK(stringTokenizer.nextToken());
            double d3 = AseSceneLoader.kamaJaK(stringTokenizer.nextToken());
            mmajmmaArray[0] = new Vec3f(d, d2, d3);
        }
        return mmajmmaArray;
    }

    MeshObject kAmAJaK(DataInputStream dataInputStream, String[] stringArray) throws Exception {
        MeshObject mmajmmk2 = null;
        Vec3f[] mmajmmaArray = null;
        String string = null;
        int n = 1;
        boolean bl = false;
        while (n > 0) {
            String string2;
            String string3 = dataInputStream.readLine();
            if (string3 == null) {
                return null;
            }
            StringTokenizer stringTokenizer = new StringTokenizer(string3.trim(), " ");
            String string4 = string2 = stringTokenizer.hasMoreTokens() ? stringTokenizer.nextToken() : "";
            if (string3.endsWith("{")) {
                ++n;
            }
            if (string3.endsWith("}")) {
                --n;
            }
            if (bl) {
                string2 = "";
            }
            if (string2.compareTo("*MESH") == 0) {
                mmajmmk2 = this.KAmAJaK(dataInputStream);
                --n;
            }
            if (string2.compareTo("*NODE_TM") == 0) {
                mmajmmaArray = this.kamAJaK(dataInputStream);
                --n;
            }
            if (string2.compareTo("*NODE_NAME") == 0 && n == 1) {
                string = stringTokenizer.nextToken();
                if ((string = string.substring(1, string.length() - 1)).charAt(0) == '_') {
                    bl = true;
                } else if (stringArray != null) {
                    int n2 = 0;
                    while (n2 < stringArray.length) {
                        if (string.equalsIgnoreCase(stringArray[n2])) {
                            bl = true;
                        }
                        ++n2;
                    }
                }
            }
            if (string2.compareTo("*TM_ANIMATION") != 0) continue;
            this.KAMAJaK(dataInputStream, mmajmmk2);
            --n;
        }
        if (mmajmmk2 != null) {
            mmajmmk2.KKaMAJa((Vec3f)mmajmmaArray[0]);
            mmajmmk2.jaKKaMA.mAjakKA((Vec3f)mmajmmaArray[0]);
            mmajmmk2.kkaMaja(string);
        }
        return mmajmmk2;
    }

    Camera kAmaJaK(DataInputStream dataInputStream) throws Exception {
        Camera kaajmmk2 = new Camera();
        int n = 1;
        boolean bl = false;
        while (n > 0) {
            String string;
            String string2 = dataInputStream.readLine();
            if (string2 == null) {
                return null;
            }
            StringTokenizer stringTokenizer = new StringTokenizer(string2.trim(), " ");
            String string3 = string = stringTokenizer.hasMoreTokens() ? stringTokenizer.nextToken() : "";
            if (string2.endsWith("{")) {
                ++n;
            }
            if (string2.endsWith("}")) {
                --n;
            }
            if (string.compareTo("*TM_ANIMATION") != 0) continue;
            if (!bl) {
                this.KAMAJaK(dataInputStream, kaajmmk2);
                bl = true;
            } else {
                this.KAMAJaK(dataInputStream, this.amaJAKK);
            }
            --n;
        }
        return kaajmmk2;
    }

    public void KamAJaK(URL uRL) {
        this.KamajaK(uRL, null);
    }

    public void KamajaK(URL uRL, String[] stringArray) {
        Object object;
        Enumeration enumeration;
        Object object2;
        this.AmaJAKK = new Camera();
        this.amaJAKK = new Vec3f();
        this.AMAJAKK = new Hashtable();
        this.aMAJAKK = new Hashtable();
        this.AmAJAKK = null;
        this.amAJAKK = null;
        try {
            DataInputStream dataInputStream;
            InputStream inputStream = uRL.openConnection().getInputStream();
            String string = uRL.getFile().toLowerCase();
            if (string.endsWith("z")) {
                dataInputStream = new DataInputStream(new GZIPInputStream(inputStream));
            } else {
                dataInputStream = new DataInputStream(inputStream);
            }
            while ((string = dataInputStream.readLine()) != null) {
                MeshObject mmajmmk2;
                object = new StringTokenizer(string.trim(), " ");
                String string2 = ((StringTokenizer)object).hasMoreTokens() ? ((StringTokenizer)object).nextToken() : "";
                if (((StringTokenizer)object).hasMoreTokens() && string2.compareTo("*GEOMOBJECT") == 0 && (mmajmmk2 = this.kAmAJaK(dataInputStream, stringArray)) != null) {
                    this.maJAKkA(mmajmmk2);
                }
                if (!((StringTokenizer)object).hasMoreTokens() || string2.compareTo("*CAMERAOBJECT") != 0) continue;
                this.AmaJAKK = this.kAmaJaK(dataInputStream);
            }
        }
        catch (Exception exception) {
            exception.printStackTrace();
            return;
        }
        this.AmAJAKK = null;
        this.amAJAKK = null;
        object2 = null;
        int n = 0;
        enumeration = this.AMAJAKK.elements();
        while (enumeration.hasMoreElements()) {
            object = (Vector)enumeration.nextElement();
            if (object2 == null) {
                object2 = object;
            }
            if (((Vector)object).size() <= ((Vector)object2).size()) continue;
            object2 = object;
        }
        enumeration = this.aMAJAKK.elements();
        while (enumeration.hasMoreElements()) {
            object = (Vector)enumeration.nextElement();
            if (object2 == null) {
                object2 = object;
            }
            if (((Vector)object).size() <= ((Vector)object2).size()) continue;
            object2 = object;
        }
        kmjjmma[] kmjjmmaArray2;
        kmjjmma[] kmjjmmaArray;
        n = ((Vector)object2).size();
        int n2 = 2 + this.MaJAKkA.size();
        kmjjmmaArray2 = new kmjjmma[n];
        kmjjmmaArray = new kmjjmma[n];
        int n3 = 0;
        while (n3 < n) {
            float[] fArray = (float[])((Vector)object2).elementAt(n3);
            kmjjmmaArray2[n3] = new kmjjmma(fArray[0], 0.0f, 0.0f, 0.0f, new float[3 * n2]);
            kmjjmmaArray[n3] = new kmjjmma(fArray[0], 0.0f, 0.0f, 0.0f, new maaakmk[n2]);
            int n4 = 0;
            while (n4 < n2) {
                kmjjmmaArray[n3].maJAkkA[n4] = new maaakmk();
                ++n4;
            }
            ++n3;
        }
        int n5 = 0;
        while (n5 < n2) {
            Object object4;
            Vector vector;
            Vector vector2;
            float[] fArray = new float[3];
            maaakmk maaakmk2 = new maaakmk();
            switch (n5) {
                case 0: {
                    vector2 = (Vector)this.AMAJAKK.get(this.AmaJAKK);
                    vector = (Vector)this.aMAJAKK.get(this.AmaJAKK);
                    break;
                }
                case 1: {
                    vector2 = (Vector)this.AMAJAKK.get(this.amaJAKK);
                    vector = (Vector)this.aMAJAKK.get(this.amaJAKK);
                    break;
                }
                default: {
                    MeshObject mmajmmk3 = (MeshObject)this.MaJAKkA.elementAt(n5 - 2);
                    vector2 = (Vector)this.AMAJAKK.get(mmajmmk3);
                    vector = (Vector)this.aMAJAKK.get(mmajmmk3);
                    fArray[0] = mmajmmk3.jaKKaMA.maJakKA;
                    fArray[1] = mmajmmk3.jaKKaMA.MAJakKA;
                    fArray[2] = mmajmmk3.jaKKaMA.mAJakKA;
                    maaakmk2.KKAMaJA(mmajmmk3.JakKaMA);
                }
            }
            int n6 = 0;
            int n7 = 0;
            if (vector2 != null) {
                n7 = vector2.size();
            }
            n7 = Math.min(n7, n);
            n6 = 0;
            while (n6 < n7) {
                object4 = (float[])vector2.elementAt(n6);
                System.arraycopy(object4, 1, kmjjmmaArray2[n6].mAjakkA, n5 * 3, 3);
                System.arraycopy(object4, 1, fArray, 0, 3);
                ++n6;
            }
            while (n6 < n) {
                System.arraycopy(fArray, 0, kmjjmmaArray2[n6].mAjakkA, n5 * 3, 3);
                ++n6;
            }
            object4 = null;
            int n8 = 0;
            int n9 = 0;
            if (vector != null) {
                n9 = vector.size();
            }
            n9 = Math.min(n9, n);
            n8 = 0;
            while (n8 < n9) {
                float[] fArray2 = (float[])vector.elementAt(n8);
                maaakmk maaakmk3 = new maaakmk();
                maaakmk3.deltaSeconds(new Vec3f(fArray2[1], fArray2[2], fArray2[3]), fArray2[4]);
                if (object4 != null) {
                    maaakmk3.renderFrame((maaakmk)object4);
                }
                object4 = new maaakmk(maaakmk3);
                Vec3f mmajmma2 = new Vec3f();
                float f = maaakmk3.kkaMaJA(mmajmma2);
                kmjjmmaArray[n8].maJAkkA[n5] = new maaakmk(mmajmma2.maJakKA, mmajmma2.MAJakKA, mmajmma2.mAJakKA, f);
                maaakmk2 = kmjjmmaArray[n8].maJAkkA[n5];
                ++n8;
            }
            ++n5;
        }
        this.AmAJAKK = new kajakmk(kmjjmmaArray2, 2, 0);
        this.amAJAKK = new kajakmk(kmjjmmaArray, 2, 2);
    }

    public static float kamajaK(String string) {
        if (string.charAt(string.length() - 1) == ',') {
            string = string.substring(0, string.length() - 1);
        }
        return Float.valueOf(string).floatValue();
    }

    public static double kamaJaK(String string) {
        if (string.charAt(string.length() - 1) == ',') {
            string = string.substring(0, string.length() - 1);
        }
        return Double.valueOf(string);
    }

    public static int KaMAJaK(String string) {
        if (string.charAt(string.length() - 1) == ',') {
            string = string.substring(0, string.length() - 1);
        }
        return Integer.parseInt(string);
    }
}
