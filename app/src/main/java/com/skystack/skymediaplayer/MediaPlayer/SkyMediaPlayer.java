package com.skystack.skymediaplayer.MediaPlayer;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;

import androidx.annotation.NonNull;

import java.lang.ref.WeakReference;
import java.util.logging.LogRecord;

public class SkyMediaPlayer extends AbstractMediaPlayer{
    private static final String TAG = SkyMediaPlayer.class.getName();

    private static final int MEDIA_NOP = 0; // interface test message
    private static final int MEDIA_PREPARED = 1;
    private static final int MEDIA_PLAYBACK_COMPLETE = 2;
    private static final int MEDIA_BUFFERING_UPDATE = 3;
    private static final int MEDIA_SEEK_COMPLETE = 4;
    private static final int MEDIA_SET_VIDEO_SIZE = 5;
    private static final int MEDIA_TIMED_TEXT = 99;
    private static final int MEDIA_ERROR = 100;
    private static final int MEDIA_INFO = 200;

    protected static final int MEDIA_SET_VIDEO_SAR = 10001;

    private long mNativeMediaPlayer = 0;
    static public int MediaPlayerFFmpeg = 1;
    static public int MediaPlayerMediaCodec = 2;

    private int mVideoWidth = 0;
    private int mVideoHeight = 0;
    private EventHandler mEventHandler;

    static public SkyMediaPlayer CreateMediaPlayer(int type, MediaPlayerObserver listener){
        return new SkyMediaPlayer(type, listener);
    }

    private SkyMediaPlayer(int type, MediaPlayerObserver listener){
        mNativeMediaPlayer = nativeCreateMediaPlayer(type, listener);

        Looper looper;
        if((looper = Looper.myLooper()) != null){
            mEventHandler = new EventHandler(this, looper);
        }else if((looper = Looper.getMainLooper()) != null){
            mEventHandler = new EventHandler(this, looper);
        }else{
            mEventHandler = null;
        }


    }

    @Override
    public void setDisplay(SurfaceHolder surfaceHolder){
        Surface surface = null;
        if(surfaceHolder != null){
            surface = surfaceHolder.getSurface();
        }

        nativeSetSurface(mNativeMediaPlayer, surface);

    }

    @Override
    public void setDataSource(String path) {
        nativeSetSource(mNativeMediaPlayer, path);
    }

    @Override
    public void prepareAsync() throws IllegalStateException {

    }

    @Override
    public void start() throws IllegalStateException {
        nativeStartMediaPlayer(mNativeMediaPlayer);
    }

    @Override
    public void stop() throws IllegalStateException {
        nativeStopMediaPlayer(mNativeMediaPlayer);
    }

    @Override
    public void pause() throws IllegalStateException {

    }

    @Override
    public void setScreenOnWhilePlaying(boolean screenOn) {

    }

    @Override
    public int getVideoWidth() {
        return mVideoWidth;
    }

    @Override
    public int getVideoHeight() {
        return mVideoHeight;
    }

    @Override
    public boolean isPlaying() {
        return true;
    }

    @Override
    public void seekTo(long msec) throws IllegalStateException {

    }

    @Override
    public int getCurrentPosition() {
        return 0;
    }

    @Override
    public int getDuration() {
        return 0;
    }

    @Override
    public void release() {
        nativeDestroyMediaPlayer(mNativeMediaPlayer);
        resetListeners();
    }

    @Override
    public void reset() {
        if(mEventHandler != null){
            mEventHandler.removeCallbacksAndMessages(null);
        }

        mVideoWidth = mVideoHeight = 0;
    }

    @Override
    public void setVolume(float leftVolume, float rightVolume) {

    }

    @Override
    public int getAudioSessionId() {
        return 0;
    }

    private static class EventHandler extends Handler {
        private final WeakReference<SkyMediaPlayer> mWeakPlayer;

        public EventHandler(SkyMediaPlayer mediaPlayer, Looper looper){
            super(looper);
            mWeakPlayer = new WeakReference<SkyMediaPlayer>(mediaPlayer);
        }

        @Override
        public void handleMessage(@NonNull Message msg) {
            SkyMediaPlayer mediaPlayer = mWeakPlayer.get();
            if(mediaPlayer == null || mediaPlayer.mNativeMediaPlayer == 0){
                Log.w(TAG, "mediaPlayer is null" );
                return;
            }

            switch (msg.what){
                case MEDIA_PREPARED:
                    mediaPlayer.notifyOnPrepared();
                    break;
                case MEDIA_PLAYBACK_COMPLETE:
                    mediaPlayer.notifyOnCompletion();
                    break;
                case MEDIA_BUFFERING_UPDATE:
                    int bufferPosition = msg.arg1;
                    if(bufferPosition < 0) bufferPosition = 0;

                    int percent = 0;
                    int duration = mediaPlayer.getDuration();
                    if(duration > 0){
                        percent = bufferPosition * 100 / duration;
                    }
                    if(percent > 100)
                        percent = 100;

                    mediaPlayer.notifyOnBufferingUpdate(percent);
                    break;
                case MEDIA_SEEK_COMPLETE:
                    mediaPlayer.notifyOnSeekComplete();
                    break;
                case MEDIA_SET_VIDEO_SIZE:
                    mediaPlayer.mVideoWidth = msg.arg1;
                    mediaPlayer.mVideoHeight = msg.arg2;
                    mediaPlayer.notifyOnVideoSizeChanged(mediaPlayer.mVideoWidth, mediaPlayer.mVideoHeight);
                    break;
                case MEDIA_ERROR:
                    if(!mediaPlayer.notifyOnError(msg.arg1, msg.arg2)){
                        mediaPlayer.notifyOnCompletion();
                    }
                    break;
                case MEDIA_INFO:
                    mediaPlayer.notifyOnInfo(msg.arg1, msg.arg2);
                    break;
                case MEDIA_TIMED_TEXT:
                    mediaPlayer.notifyOnTimedText((String) msg.obj);
                    break;
                case MEDIA_SET_VIDEO_SAR:
                    break;
                default:
                    Log.e(TAG, "unknown message type: " + msg.what);
                    break;
            }


        }
    }

    private static void postEventFromNative(Object weakThiz, int what, int arg1, int arg2, Object obj){
        if(weakThiz == null)
            return;

        SkyMediaPlayer mediaPlayer = (SkyMediaPlayer) ((WeakReference)weakThiz).get();
        if(mediaPlayer == null)
            return;

        if(mediaPlayer.mEventHandler != null){
            Message message = mediaPlayer.mEventHandler.obtainMessage(what, arg1, arg2, obj);
            mediaPlayer.mEventHandler.sendMessage(message);
        }
    }


    private native long nativeCreateMediaPlayer(int type, MediaPlayerObserver listener);
    private native boolean nativeSetSource(long nativeMediaPlayer, String path);
    private native boolean nativeStartMediaPlayer(long nativeMediaPlayer);
    private native boolean nativeStopMediaPlayer(long nativeMediaPlayer);
    private native boolean nativeDestroyMediaPlayer(long nativeMediaPlayer);
    private native void nativeSetSurface(long nativeMediaPlayer, Surface surface);
}
