/*
 * Decompiled with CFR 0.152.
 * 
 * Could not load the following classes:
 *  com.ms.com.ComLib
 *  com.ms.directX.DSBufferDesc
 *  com.ms.directX.DSCaps
 *  com.ms.directX.DSCursors
 *  com.ms.directX.DirectSound
 *  com.ms.directX.DirectSoundBuffer
 *  com.ms.directX.DirectXConstants
 *  com.ms.directX.WaveFormatEx
 */
package muhmu.hifi.device;

import com.ms.com.ComLib;
import com.ms.directX.DSBufferDesc;
import com.ms.directX.DSCaps;
import com.ms.directX.DSCursors;
import com.ms.directX.DirectSound;
import com.ms.directX.DirectSoundBuffer;
import com.ms.directX.DirectXConstants;
import com.ms.directX.WaveFormatEx;
import muhmu.hifi.device.DeviceMSbase;
import muhmu.hifi.device.MAD;

public final class DeviceMS_IE4
extends DeviceMSbase
implements Runnable,
DirectXConstants {
    private DirectSound ds;
    private DirectSoundBuffer dsBuffer;
    private DirectSoundBuffer dsBuffer_primary;
    private DSCaps dsCaps;
    DSCursors dsc = new DSCursors();

    public void initDS() throws Exception {
        try {
            DSBufferDesc dSBufferDesc = null;
            int n = this.frequency;
            WaveFormatEx waveFormatEx = new WaveFormatEx();
            waveFormatEx.bitsPerSample = this.bit16 ? 16 : 8;
            waveFormatEx.channels = this.stereo ? 2 : 1;
            waveFormatEx.blockAlign = this.bytesPerSample;
            waveFormatEx.samplesPerSec = n;
            waveFormatEx.avgBytesPerSec = n * this.bytesPerSample;
            waveFormatEx.formatTag = 1;
            waveFormatEx.extra = null;
            waveFormatEx.size = 0;
            dSBufferDesc = new DSBufferDesc();
            dSBufferDesc.flags = 1;
            dSBufferDesc.bufferBytes = 0;
            try {
                this.dsBuffer_primary = this.ds.createSoundBuffer(dSBufferDesc, null);
                this.dsBuffer_primary.setFormat(waveFormatEx);
            }
            catch (Exception exception) {
                exception.printStackTrace();
                System.out.println("PRIMARY SOUNDBUFFER CREATION FAILED... going on anyway... power off");
            }
            this.dsCaps = new DSCaps();
            this.ds.getCaps(this.dsCaps);
            dSBufferDesc = new DSBufferDesc();
            dSBufferDesc.flags = 16384;
            dSBufferDesc.flags = 16384;
            dSBufferDesc.bufferBytes = this.buffer8.length;
            this.dsBuffer = this.ds.createSoundBuffer(dSBufferDesc, waveFormatEx);
        }
        catch (Exception exception) {
            exception.printStackTrace();
            throw exception;
        }
    }

    public DeviceMS_IE4() throws Exception {
        this.ds = new DirectSound();
        this.ds.setCooperativeLevel(MAD.component, 2);
    }

    public int playPos() {
        this.dsBuffer.getCurrentPosition(this.dsc);
        return this.dsc.play;
    }

    public void play() {
        if (this.dsBuffer_primary != null) {
            this.dsBuffer_primary.play(1);
        }
        this.dsBuffer.play(1);
    }

    public void write(int n) {
        this.dsBuffer.writeBuffer(n * this.bytesPerSample * this.buffer.length, this.buffer.length * this.bytesPerSample, this.buffer8, 0);
    }

    protected void finalize() {
        try {
            System.out.println("finalize ie4");
            if (this.dsBuffer != null) {
                ComLib.release((Object)this.dsBuffer);
            }
            if (this.dsBuffer_primary != null) {
                ComLib.release((Object)this.dsBuffer_primary);
            }
            if (this.ds != null) {
                ComLib.release((Object)this.ds);
            }
        }
        catch (Exception exception) {
            exception.printStackTrace();
        }
    }
}
