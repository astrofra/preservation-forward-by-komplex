// 
// Decompiled by Procyon v0.6.0
// 

package muhmu.hifi.device;

import com.ms.com.ComLib;
import com.ms.directX.DSBufferDesc;
import com.ms.directX.WaveFormatEx;
import com.ms.directX.DSCursors;
import com.ms.directX.DSCaps;
import com.ms.directX.DirectSoundBuffer;
import com.ms.directX.DirectSound;
import com.ms.directX.DirectXConstants;

public final class DeviceMS_IE4 extends DeviceMSbase implements Runnable, DirectXConstants
{
    private DirectSound ds;
    private DirectSoundBuffer dsBuffer;
    private DirectSoundBuffer dsBuffer_primary;
    private DSCaps dsCaps;
    DSCursors dsc;
    
    public void initDS() throws Exception {
        try {
            final int frequency = super.frequency;
            final WaveFormatEx format = new WaveFormatEx();
            if (super.bit16) {
                format.bitsPerSample = 16;
            }
            else {
                format.bitsPerSample = 8;
            }
            if (super.stereo) {
                format.channels = 2;
            }
            else {
                format.channels = 1;
            }
            format.blockAlign = super.bytesPerSample;
            format.samplesPerSec = frequency;
            format.avgBytesPerSec = frequency * super.bytesPerSample;
            format.formatTag = 1;
            format.extra = null;
            format.size = 0;
            final DSBufferDesc dsBufferDesc = new DSBufferDesc();
            dsBufferDesc.flags = 1;
            dsBufferDesc.bufferBytes = 0;
            try {
                (this.dsBuffer_primary = this.ds.createSoundBuffer(dsBufferDesc, (byte[])null)).setFormat(format);
            }
            catch (final Exception ex) {
                ex.printStackTrace();
                System.out.println("PRIMARY SOUNDBUFFER CREATION FAILED... going on anyway... power off");
            }
            this.dsCaps = new DSCaps();
            this.ds.getCaps(this.dsCaps);
            final DSBufferDesc dsBufferDesc2 = new DSBufferDesc();
            dsBufferDesc2.flags = 16384;
            dsBufferDesc2.flags = 16384;
            dsBufferDesc2.bufferBytes = super.buffer8.length;
            this.dsBuffer = this.ds.createSoundBuffer(dsBufferDesc2, format);
        }
        catch (final Exception ex2) {
            ex2.printStackTrace();
            throw ex2;
        }
    }
    
    public DeviceMS_IE4() throws Exception {
        this.dsc = new DSCursors();
        (this.ds = new DirectSound()).setCooperativeLevel(MAD.component, 2);
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
    
    public void write(final int n) {
        this.dsBuffer.writeBuffer(n * super.bytesPerSample * super.buffer.length, super.buffer.length * super.bytesPerSample, super.buffer8, 0);
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
        catch (final Exception ex) {
            ex.printStackTrace();
        }
    }
}
