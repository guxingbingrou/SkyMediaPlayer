package com.skystack.skymediaplayer.MediaPlayer;

import android.view.Surface;
import android.view.SurfaceHolder;

public class SkyMediaPlayer implements IMediaPlayer{
    private long nativeMediaPlayer = 0;
    static public int MediaPlayerFFmpeg = 1;
    static public int MediaPlayerMediaCodec = 2;

    static public SkyMediaPlayer CreateMediaPlayer(int type, MediaPlayerObserver listener){
        return new SkyMediaPlayer(type, listener);
    }

    private SkyMediaPlayer(int type, MediaPlayerObserver listener){
        nativeMediaPlayer = nativeCreateMediaPlayer(type, listener);
    }

    @Override
    public void setDisplay(SurfaceHolder surfaceHolder){
        if(surfaceHolder != null){
            nativeSetSurface(nativeMediaPlayer, surfaceHolder.getSurface());
        }

    }

    @Override
    public void setDataSource(String path) {
        nativeSetSource(nativeMediaPlayer, path);
    }

    @Override
    public void prepareAsync() throws IllegalStateException {

    }

    @Override
    public void start() throws IllegalStateException {
        nativeStartMediaPlayer(nativeMediaPlayer);
    }

    @Override
    public void stop() throws IllegalStateException {
        nativeStopMediaPlayer(nativeMediaPlayer);
    }

    @Override
    public void pause() throws IllegalStateException {

    }

    @Override
    public void release() {
        nativeDestroyMediaPlayer(nativeMediaPlayer);
    }



    private native long nativeCreateMediaPlayer(int type, MediaPlayerObserver listener);
    private native boolean nativeSetSource(long nativeMediaPlayer, String path);
    private native boolean nativeStartMediaPlayer(long nativeMediaPlayer);
    private native boolean nativeStopMediaPlayer(long nativeMediaPlayer);
    private native boolean nativeDestroyMediaPlayer(long nativeMediaPlayer);
    private native void nativeSetSurface(long nativeMediaPlayer, Surface surface);
}
