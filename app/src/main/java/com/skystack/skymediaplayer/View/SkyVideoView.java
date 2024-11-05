package com.skystack.skymediaplayer.View;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.MediaController;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.skystack.skymediaplayer.MediaPlayer.IMediaPlayer;
import com.skystack.skymediaplayer.MediaPlayer.SkyMediaPlayer;
import com.skystack.skymediaplayer.MediaPlayer.MediaPlayerObserver;

public class SkyVideoView extends FrameLayout implements MediaController.MediaPlayerControl, MediaPlayerObserver {
    private final static String TAG = SkyVideoView.class.getName();
    private MediaController mMediaController = null;
    private IMediaPlayer mMediaPlayer = null;
    private String mVideoPath = null;

    private int mSurfaceWidth = 0;
    private int mSurfaceHeight = 0;
    private IRenderView mRenderView = null;
    private IRenderView.ISurfaceHolder mSurfaceHolder = null;


    public SkyVideoView(@NonNull Context context) {
        super(context);
        initVideoView(context);
    }

    public SkyVideoView(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        initVideoView(context);
    }

    public SkyVideoView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initVideoView(context);
    }

    public SkyVideoView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        initVideoView(context);
    }

    private void bindSurfaceHolder(IMediaPlayer mediaPlayer, IRenderView.ISurfaceHolder surfaceHolder){
        if(mediaPlayer == null)
            return;
        if(surfaceHolder == null){
            mediaPlayer.setDisplay(null);
        }

        surfaceHolder.bindToMediaPlayer(mediaPlayer);
    }

    IRenderView.IRenderCallback mRenderCallback = new IRenderView.IRenderCallback() {
        @Override
        public void onSurfaceCreated(@NonNull IRenderView.ISurfaceHolder surfaceHolder, int width, int height) {
            if(surfaceHolder.getRenderView() != mRenderView){
                Log.e(TAG, "onSurfaceCreated: unmatched render");
                return;
            }
            mSurfaceHolder = surfaceHolder;
            if(mMediaPlayer != null){
                bindSurfaceHolder(mMediaPlayer, surfaceHolder);
            }else{
                openVideo();
            }
        }

        @Override
        public void onSurfaceChanged(@NonNull IRenderView.ISurfaceHolder surfaceHolder, int format, int width, int height) {
            if(surfaceHolder.getRenderView() != mRenderView){
                Log.e(TAG, "onSurfaceChanged: unmatched render");
                return;
            }
            mSurfaceWidth = width;
            mSurfaceHeight = height;

            if(mMediaPlayer !=null && width * height !=0){
                start();
            }
        }

        @Override
        public void onSurfaceDestroyed(@NonNull IRenderView.ISurfaceHolder surfaceHolder) {
            if(surfaceHolder.getRenderView() != mRenderView){
                Log.e(TAG, "onSurfaceDestroyed: unmatched render");
                return;
            }

            mSurfaceHolder = null;
            release();
        }
    };


    void initVideoView(Context context){
        FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.WRAP_CONTENT,
                FrameLayout.LayoutParams.WRAP_CONTENT,
                Gravity.CENTER);

        mRenderView = new SurfaceRenderView(context);

        View renderView = mRenderView.getView();
        renderView.setLayoutParams(layoutParams);
        addView(renderView);
        mRenderView.addRenderCallback(mRenderCallback);

    }

    private void openVideo(){
        Log.i(TAG, "openVideo");
        if(mVideoPath == null || mSurfaceHolder == null){
            return;
        }

        release();

        mMediaPlayer = SkyMediaPlayer.CreateMediaPlayer(SkyMediaPlayer.MediaPlayerFFmpeg, this);

        mMediaPlayer.setDataSource(mVideoPath);

        bindSurfaceHolder(mMediaPlayer, mSurfaceHolder);

        mMediaPlayer.prepareAsync();


    }

    public void setMediaController(MediaController mediaController){
        if(mMediaController != null){
            mMediaController.hide();
        }
        mMediaController = mediaController;

//        if(mMediaPlayer != null && mMediaController != null){
            mMediaController.setMediaPlayer(this);

            View anchorView = this.getParent() instanceof View ? (View) this.getParent() : this;

            mMediaController.setAnchorView(anchorView);
            mMediaController.setEnabled(true);
//            mMediaController.show();
//        }
    }

    public void setVideoPath(String videoPath){
        mVideoPath = videoPath;
        openVideo();

//        requestLayout();
    }

    void release(){
        if(mMediaPlayer != null){
            mMediaPlayer.setDisplay(null);
            mMediaPlayer.release();
            mMediaPlayer = null;
        }
    }

//implements MediaController.MediaPlayerControl
    @Override
    public void start() {
        if(mMediaPlayer != null){
            mMediaPlayer.start();
        }
    }

    @Override
    public void pause() {

    }

    @Override
    public int getDuration() {
        return 5;
    }

    @Override
    public int getCurrentPosition() {
        return 0;
    }

    @Override
    public void seekTo(int pos) {

    }

    @Override
    public boolean isPlaying() {
        return true;
    }

    @Override
    public int getBufferPercentage() {
        return 100;
    }

    @Override
    public boolean canPause() {
        return true;
    }

    @Override
    public boolean canSeekBackward() {
        return true;
    }

    @Override
    public boolean canSeekForward() {
        return true;
    }

    @Override
    public int getAudioSessionId() {
        return 0;
    }

    @Override
    public void OnResolutionChanged(int width, int height) {
        if(width == mSurfaceWidth && height == mSurfaceHeight)
            return;

        Log.i(TAG, "OnResolutionChanged: " + width + "x" + height);
        mSurfaceWidth = width;
        mSurfaceHeight = height;
        if(mRenderView != null){
            mRenderView.setVideoSize(mSurfaceWidth, mSurfaceHeight);
        }
    }
}
