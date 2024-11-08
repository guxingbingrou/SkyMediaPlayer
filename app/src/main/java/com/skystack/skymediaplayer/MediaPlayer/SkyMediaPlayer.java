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

    static public int MediaPlayerFFmpeg = 1;
    static public int MediaPlayerMediaCodec = 2;

    private int mVideoWidth = 0;
    private int mVideoHeight = 0;
    private EventHandler mEventHandler;


    static public SkyMediaPlayer CreateMediaPlayer(int type){
        return new SkyMediaPlayer(type);
    }

    private SkyMediaPlayer(int type){
        nativeInit(type);

        Looper looper;
        if((looper = Looper.myLooper()) != null){
            mEventHandler = new EventHandler(this, looper);
        }else if((looper = Looper.getMainLooper()) != null){
            mEventHandler = new EventHandler(this, looper);
        }else{
            mEventHandler = null;
        }

        nativeSetup(this);
    }

    @Override
    public void setDisplay(SurfaceHolder surfaceHolder){
        Surface surface = null;
        if(surfaceHolder != null){
            surface = surfaceHolder.getSurface();
        }

        nativeSetSurface(surface);
    }

    @Override
    public void setDataSource(String path) {
        nativeSetSource(path);
    }

    @Override
    public void prepareAsync() throws IllegalStateException {
        nativePrepareAsync();
    }

    private  boolean m_is_playing = true;
    @Override
    public void start() throws IllegalStateException {
        m_is_playing = true;
        nativeStartMediaPlayer();
    }

    @Override
    public void stop() throws IllegalStateException {
        nativeStopMediaPlayer();
    }

    @Override
    public void pause() throws IllegalStateException {
        m_is_playing = false;
        Log.i(TAG, "pause");
        nativePause();
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
        return m_is_playing;
    }

    @Override
    public void seekTo(int msec) throws IllegalStateException {
        nativeSeekTo(msec);
    }

    @Override
    public int getCurrentPosition() {
        return nativeGetCurrentPosition();
    }

    @Override
    public int getDuration() {
        return nativeGetDuration();
    }

    @Override
    public void release() {
        nativeDestroyMediaPlayer();
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
            if(mediaPlayer == null){
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

    private void postEventFromNative(int what, int arg1, int arg2, Object obj){
        Log.i(TAG, "postEventFromNative: " + what);
        if(mEventHandler != null){
            Message message =mEventHandler.obtainMessage(what, arg1, arg2, obj);
            mEventHandler.sendMessage(message);
        }
    }


    private native void nativeInit(int type);
    private native void nativeSetup(Object obj);
    private native boolean nativeSetSource(String path);
    private native void nativeSetSurface(Surface surface);
    private native void nativePrepareAsync();
    private native boolean nativeStartMediaPlayer();
    private native boolean nativePause();
    private native boolean nativeStopMediaPlayer();
    private native boolean nativeDestroyMediaPlayer();

    private native int nativeGetDuration();
    private native int nativeGetCurrentPosition();
    private native void nativeSeekTo(int msec);

}
