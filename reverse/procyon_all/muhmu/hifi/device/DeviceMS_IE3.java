// 
// Decompiled by Procyon v0.6.0
// 

package muhmu.hifi.device;

import com.ms.com.ComLib;
import com.ms.com.IUnknown;
import com.ms.directX.DSBufferDesc;
import com.ms.directX.WaveFormatEx;
import com.ms.awt.peer.ComponentPeerX;
import com.ms.directX.DSCursors;
import com.ms.directX.dSoundBuffer;
import com.ms.directX.dSound;
import com.ms.directX.ddConstants;

public final class DeviceMS_IE3 extends DeviceMSbase implements Runnable, ddConstants
{
    private dSound ds;
    private dSoundBuffer dsBuffer;
    private dSoundBuffer dsBuffer_primary;
    DSCursors dsc;
    
    public void initDS() throws Exception {
        final ComponentPeerX componentPeerX = (ComponentPeerX)MAD.component.getPeer();
        if (componentPeerX == null) {
            System.err.println("peer error in DeviceMS_IE3");
        }
        componentPeerX.gethwnd();
        try {
            final int frequency = super.frequency;
            final WaveFormatEx waveFormatEx = new WaveFormatEx();
            if (super.bit16) {
                waveFormatEx.wBitsPerSample = 16;
            }
            else {
                waveFormatEx.wBitsPerSample = 8;
            }
            if (super.stereo) {
                waveFormatEx.nChannels = 2;
            }
            else {
                waveFormatEx.nChannels = 1;
            }
            waveFormatEx.nBlockAlign = super.bytesPerSample;
            waveFormatEx.nSamplesPerSec = frequency;
            waveFormatEx.nAvgBytesPerSec = frequency * super.bytesPerSample;
            waveFormatEx.extra = null;
            waveFormatEx.wFormatTag = 1;
            waveFormatEx.cbSize = 0;
            final DSBufferDesc dsBufferDesc = new DSBufferDesc();
            dsBufferDesc.dwFlags = 1;
            dsBufferDesc.dwBufferBytes = 0;
            (this.dsBuffer_primary = this.ds.CreateSoundBuffer(dsBufferDesc, (byte[])null, (IUnknown)null)).SetFormat(waveFormatEx);
            final DSBufferDesc dsBufferDesc2 = new DSBufferDesc();
            dsBufferDesc2.dwFlags = 16384;
            dsBufferDesc2.dwBufferBytes = super.buffer8.length;
            this.dsBuffer = this.ds.CreateSoundBuffer(dsBufferDesc2, waveFormatEx, (IUnknown)null);
        }
        catch (final Exception ex) {
            ex.printStackTrace();
            throw ex;
        }
    }
    
    public DeviceMS_IE3() throws Exception {
        this.dsc = new DSCursors();
        final ComponentPeerX componentPeerX = (ComponentPeerX)MAD.component.getPeer();
        if (componentPeerX == null) {
            System.err.println("peer error in DeviceMS_IE3");
        }
        (this.ds = new dSound()).SetCooperativeLevel(componentPeerX.gethwnd(), 2);
    }
    
    public int playPos() {
        this.dsBuffer.GetCurrentPosition(this.dsc);
        return this.dsc.dwPlay;
    }
    
    public void play() {
        this.dsBuffer_primary.Play(0, 0, 1);
        this.dsBuffer.Play(0, 0, 1);
    }
    
    public void write(final int n) {
        this.dsBuffer.WriteBuffer(n * super.bytesPerSample * super.buffer.length, super.buffer.length * super.bytesPerSample, super.buffer8, 0);
    }
    
    protected void finalize() {
        try {
            System.out.println("finalize ie3");
            ComLib.release((Object)this.dsBuffer);
            ComLib.release((Object)this.dsBuffer_primary);
            ComLib.release((Object)this.ds);
        }
        catch (final Exception ex) {
            ex.printStackTrace();
        }
    }
}
