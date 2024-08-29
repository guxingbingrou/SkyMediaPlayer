package com.skystack.skymediaplayer;

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



    private native long nativeCreateMediaPlayer(int type);
    private native boolean nativeSetSource(long nativeMediaPlayer, String path);
    private native boolean nativeStartMediaPlayer(long nativeMediaPlayer);
    private native boolean nativeStopMediaPlayer(long nativeMediaPlayer);
    private native boolean nativeDestroyMediaPlayer(long nativeMediaPlayer);
    private native void nativeOnSurfaceCreated(long nativeMediaPlayer);
    private native void nativeOnSurfaceChanged(long nativeMediaPlayer, int width, int height);
    private native void nativeOnDrawFrame(long nativeMediaPlayer);


}
