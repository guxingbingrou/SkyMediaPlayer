package com.skystack.skymediaplayer.MediaPlayer;

import android.Manifest;
import android.view.Surface;

import java.util.Observer;

/**
 * Time: 2022/9/21
 * Author: zou
 * Description:
 */
public class FFMediaPlayer implements SkyMediaPlayer {

    @Override
    public boolean InitMediaPLayer(String url, Surface surface, VideoDecoderObserver observer) {
        return NativeInitMediaPLayer(url, surface, observer);
    }

    @Override
    public boolean StartMediaPlayer() {
        return NativeStartMediaPlayer();
    }

    @Override
    public boolean StopMediaPlayer() {
        return NativeStopMediaPlayer();
    }

    @Override
    public boolean DestroyMediaPlayer() {
        return NativeDestroyMediaPlayer();
    }

    @Override
    public void OnSurfaceCreated() {
        NativeOnSurfaceCreated();
    }

    @Override
    public void OnSurfaceChanged(int w, int h) {
        NativeOnSurfaceChanged(w, h);
    }

    @Override
    public void OnDrawFrame() {
        NativeOnDrawFrame();
    }


    private native boolean NativeInitMediaPLayer(String url, Surface surface, VideoDecoderObserver observer);
    private native boolean NativeStartMediaPlayer();
    private native boolean NativeStopMediaPlayer();
    private native boolean NativeDestroyMediaPlayer();

    private native void NativeOnSurfaceCreated();
    private native void NativeOnSurfaceChanged(int w, int h);
    private native void NativeOnDrawFrame();
}
