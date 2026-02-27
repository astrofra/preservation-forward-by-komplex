/*
 * Decompiled with CFR 0.152.
 * 
 * Could not load the following classes:
 *  com.ms.awt.peer.ComponentPeerX
 *  com.ms.com.ComLib
 *  com.ms.directX.DSBufferDesc
 *  com.ms.directX.DSCursors
 *  com.ms.directX.WaveFormatEx
 *  com.ms.directX.dSound
 *  com.ms.directX.dSoundBuffer
 *  com.ms.directX.ddConstants
 */
package muhmu.hifi.device;

import com.ms.awt.peer.ComponentPeerX;
import com.ms.com.ComLib;
import com.ms.directX.DSBufferDesc;
import com.ms.directX.DSCursors;
import com.ms.directX.WaveFormatEx;
import com.ms.directX.dSound;
import com.ms.directX.dSoundBuffer;
import com.ms.directX.ddConstants;
import java.awt.peer.ComponentPeer;
import muhmu.hifi.device.DeviceMSbase;
import muhmu.hifi.device.MAD;

public final class DeviceMS_IE3
extends DeviceMSbase
implements Runnable,
ddConstants {
    private dSound ds;
    private dSoundBuffer dsBuffer;
    private dSoundBuffer dsBuffer_primary;
    DSCursors dsc = new DSCursors();

    public void initDS() throws Exception {
        ComponentPeer componentPeer = MAD.component.getPeer();
        ComponentPeerX componentPeerX = (ComponentPeerX)componentPeer;
        if (componentPeerX == null) {
            System.err.println("peer error in DeviceMS_IE3");
        }
        componentPeerX.gethwnd();
        try {
            int n = this.frequency;
            WaveFormatEx waveFormatEx = new WaveFormatEx();
            waveFormatEx.wBitsPerSample = this.bit16 ? 16 : 8;
            waveFormatEx.nChannels = this.stereo ? 2 : 1;
            waveFormatEx.nBlockAlign = this.bytesPerSample;
            waveFormatEx.nSamplesPerSec = n;
            waveFormatEx.nAvgBytesPerSec = n * this.bytesPerSample;
            waveFormatEx.extra = null;
            waveFormatEx.wFormatTag = 1;
            waveFormatEx.cbSize = 0;
            DSBufferDesc dSBufferDesc = new DSBufferDesc();
            dSBufferDesc.dwFlags = 1;
            dSBufferDesc.dwBufferBytes = 0;
            this.dsBuffer_primary = this.ds.CreateSoundBuffer(dSBufferDesc, null, null);
            this.dsBuffer_primary.SetFormat(waveFormatEx);
            dSBufferDesc = new DSBufferDesc();
            dSBufferDesc.dwFlags = 16384;
            dSBufferDesc.dwBufferBytes = this.buffer8.length;
            this.dsBuffer = this.ds.CreateSoundBuffer(dSBufferDesc, waveFormatEx, null);
        }
        catch (Exception exception) {
            exception.printStackTrace();
            throw exception;
        }
    }

    public DeviceMS_IE3() throws Exception {
        ComponentPeer componentPeer = MAD.component.getPeer();
        ComponentPeerX componentPeerX = (ComponentPeerX)componentPeer;
        if (componentPeerX == null) {
            System.err.println("peer error in DeviceMS_IE3");
        }
        int n = componentPeerX.gethwnd();
        this.ds = new dSound();
        this.ds.SetCooperativeLevel(n, 2);
    }

    public int playPos() {
        this.dsBuffer.GetCurrentPosition(this.dsc);
        return this.dsc.dwPlay;
    }

    public void play() {
        this.dsBuffer_primary.Play(0, 0, 1);
        this.dsBuffer.Play(0, 0, 1);
    }

    public void write(int n) {
        this.dsBuffer.WriteBuffer(n * this.bytesPerSample * this.buffer.length, this.buffer.length * this.bytesPerSample, this.buffer8, 0);
    }

    protected void finalize() {
        try {
            System.out.println("finalize ie3");
            ComLib.release((Object)this.dsBuffer);
            ComLib.release((Object)this.dsBuffer_primary);
            ComLib.release((Object)this.ds);
        }
        catch (Exception exception) {
            exception.printStackTrace();
        }
    }
}
