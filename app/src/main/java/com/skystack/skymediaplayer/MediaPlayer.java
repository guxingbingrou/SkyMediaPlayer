package com.skystack.skymediaplayer;

import android.view.Surface;

public class MediaPlayer {
    private long nativeMediaPlayer = 0;
    static public int MediaPlayerFFmpeg = 1;
    static public int MediaPlayerMediaCodec = 2;

    static public MediaPlayer CreateMediaPlayer(int type){
        return new MediaPlayer(type);
    }

    private MediaPlayer(int type){
        nativeMediaPlayer = nativeCreateMediaPlayer(type);
    }

    public boolean SetSource(String path){
        return nativeSetSource(nativeMediaPlayer, path);
    }

    public void SetSurface(Surface surface){
        nativeSetSurface(nativeMediaPlayer, surface);
    }

    public boolean Start(){
        return nativeStartMediaPlayer(nativeMediaPlayer);
    }

    public boolean Stop(){
        return nativeStopMediaPlayer(nativeMediaPlayer);
    }

    public boolean Destroy(){
        return nativeDestroyMediaPlayer(nativeMediaPlayer);
    }




    private native long nativeCreateMediaPlayer(int type);
    private native boolean nativeSetSource(long nativeMediaPlayer, String path);
    private native boolean nativeStartMediaPlayer(long nativeMediaPlayer);
    private native boolean nativeStopMediaPlayer(long nativeMediaPlayer);
    private native boolean nativeDestroyMediaPlayer(long nativeMediaPlayer);
    private native void nativeSetSurface(long nativeMediaPlayer, Surface surface);


}
