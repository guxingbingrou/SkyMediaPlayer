package com.skystack.skymediaplayer.View;

import android.content.Context;
import android.content.res.Configuration;
import android.media.MediaPlayer;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.MediaController;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.skystack.skymediaplayer.MediaPlayer.IMediaPlayer;
import com.skystack.skymediaplayer.MediaPlayer.SkyMediaPlayer;

import java.io.IOException;

//模仿 VideoView
public class SkyVideoView extends FrameLayout implements MediaController.MediaPlayerControl{
    private final static String TAG = SkyVideoView.class.getName();
    // all possible internal states
    private static final int STATE_ERROR = -1;
    private static final int STATE_IDLE = 0;
    private static final int STATE_PREPARING = 1;
    private static final int STATE_PREPARED = 2;
    private static final int STATE_PLAYING = 3;
    private static final int STATE_PAUSED = 4;
    private static final int STATE_PLAYBACK_COMPLETED = 5;

    private int mCurrentState = STATE_IDLE;
    private int mTargetState = STATE_IDLE;

    private int mVideoWidth;
    private int mVideoHeight;

    private MediaController mMediaController = null;
    private IMediaPlayer mMediaPlayer = null;
    private Button mButton;
    private boolean m_fullscreen = false;
    private String mVideoPath = null;

    private int mSurfaceWidth = 0;
    private int mSurfaceHeight = 0;

    private int mScreenWidth = 0;
    private int mScreenHeight = 0;
    private int mOrientation = 1;

    private IRenderView mRenderView = null;
    private IRenderView.ISurfaceHolder mSurfaceHolder = null;

    private IMediaPlayer.OnCompletionListener mOnCompletionListener;
    private IMediaPlayer.OnPreparedListener mOnPreparedListener;
    private int mCurrentBufferPercentage;
    private IMediaPlayer.OnErrorListener mOnErrorListener;
    private IMediaPlayer.OnInfoListener mOnInfoListener;
    private int mSeekWhenPrepared;  // recording the seek position while preparing
    private boolean mCanPause;
    private boolean mCanSeekBack;
    private boolean mCanSeekForward;


    private OrientationCallback mOrientationCallback;

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

    public void SetOrientationCallback(OrientationCallback orientationCallback){
        mOrientationCallback = orientationCallback;
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
            Log.i(TAG, "onSurfaceChanged: " + width + " x " + height);
            mSurfaceWidth = width;
            mSurfaceHeight = height;

            if(mMediaPlayer !=null && width * height !=0 && (mTargetState == STATE_PLAYING)){
                if(mSeekWhenPrepared !=0){
                    seekTo(mSeekWhenPrepared);
                }
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
            if(mMediaController != null) mMediaController.hide();
            release(true);
        }
    };


    void initVideoView(Context context){
        mVideoWidth = 0;
        mVideoHeight = 0;


        FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.WRAP_CONTENT,
                FrameLayout.LayoutParams.WRAP_CONTENT,
                Gravity.CENTER);

        mRenderView = new SurfaceRenderView(context);

        View renderView = mRenderView.getView();
        renderView.setLayoutParams(layoutParams);
        addView(renderView);

        mRenderView.addRenderCallback(mRenderCallback);

        mButton = new Button(getContext());
        FrameLayout.LayoutParams button_layout = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.WRAP_CONTENT,
                FrameLayout.LayoutParams.WRAP_CONTENT,
                Gravity.BOTTOM | Gravity.RIGHT);

        addView(mButton, button_layout);
        mButton.setEnabled(true);
        m_fullscreen = false;
        mButton.setText("全屏");

        mButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                m_fullscreen = !m_fullscreen;
                if(m_fullscreen){
                    mButton.setText("退出全屏");
                    if(mVideoWidth < mVideoHeight){  //竖屏
                        if(mOrientationCallback != null){
                            mOrientationCallback.ChangeOrientation(false);
                        }

                    }else{
                        if(mOrientationCallback != null){
                            mOrientationCallback.ChangeOrientation(true);
                        }

                    }

                }else{
                    mButton.setText("全屏");
                    if(mOrientationCallback != null){
                        mOrientationCallback.ChangeOrientation(false);
                    }
//                    if(mRenderView != null) {
//                        mRenderView.setVideoSize(mVideoWidth, mVideoHeight);
//                    }
                }


            }
        });

        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();

        mCurrentState = STATE_IDLE;
        mTargetState = STATE_IDLE;

    }

    public void setVideoPath(String videoPath){
        mVideoPath = videoPath;
        mSeekWhenPrepared = 0;
        openVideo();
        requestLayout();
        invalidate();
    }

    public void stopPlayback() {
        if (mMediaPlayer != null) {
            mMediaPlayer.stop();
            mMediaPlayer.release();
            mMediaPlayer = null;
            mCurrentState = STATE_IDLE;
            mTargetState  = STATE_IDLE;
        }
    }

    private void openVideo(){
        Log.i(TAG, "openVideo");
        if(mVideoPath == null || mSurfaceHolder == null){
            return;
        }

        release(false);

        try {
            mMediaPlayer = SkyMediaPlayer.CreateMediaPlayer(SkyMediaPlayer.MediaPlayerFFmpeg);

            mMediaPlayer.setOnPreparedListener(mPreparedListener);
            mMediaPlayer.setOnVideoSizeChangedListener(mSizeChangedListener);
            mMediaPlayer.setOnCompletionListener(mCompletionListener);
            mMediaPlayer.setOnInfoListener(mInfoListener);
            mMediaPlayer.setOnErrorListener(mErrorListener);
            mMediaPlayer.setOnBufferingUpdateListener(mBufferingUpdateListener);
            mMediaPlayer.setOnSeekCompleteListener(mSeekCompleteListener);
            mMediaPlayer.setOnTimedTextListener(mTimedTextListener);

            mCurrentBufferPercentage = 0;
            mMediaPlayer.setDataSource(mVideoPath);
            bindSurfaceHolder(mMediaPlayer, mSurfaceHolder);
            mMediaPlayer.setScreenOnWhilePlaying(true);
            mMediaPlayer.prepareAsync();
            mCurrentState = STATE_PREPARING;
            attachMediaController();
        }catch (IOException exception){
            Log.e(TAG, "unable to open url: " + mVideoPath);
            mCurrentState = STATE_ERROR;
            mTargetState = STATE_ERROR;
            mErrorListener.onError(mMediaPlayer, MediaPlayer.MEDIA_ERROR_UNKNOWN, 0);
        }catch (IllegalArgumentException illegalArgumentException){
            Log.e(TAG, "unable to open url: " + mVideoPath);
            mCurrentState = STATE_ERROR;
            mTargetState = STATE_ERROR;
            mErrorListener.onError(mMediaPlayer, MediaPlayer.MEDIA_ERROR_UNKNOWN, 0);
        }finally {

        }
    }

    public void setMediaController(MediaController mediaController){
        if(mMediaController != null){
            mMediaController.hide();
        }
        mMediaController = mediaController;
        attachMediaController();
    }

    private void attachMediaController() {
        if (mMediaPlayer != null && mMediaController != null) {
            mMediaController.setMediaPlayer(this);
            View anchorView = this.getParent() instanceof View ?
                    (View)this.getParent() : this;
            mMediaController.setAnchorView(anchorView);
            mMediaController.setEnabled(isInPlaybackState());
        }
    }

    IMediaPlayer.OnVideoSizeChangedListener mSizeChangedListener = new IMediaPlayer.OnVideoSizeChangedListener() {
        @Override
        public void onVideoSizeChanged(IMediaPlayer mp, int width, int height) {
            Log.i(TAG, "onVideoSizeChanged: " + width + " x " + height);
            if(width == mSurfaceWidth && height == mSurfaceHeight)
                return;

            Log.i(TAG, "onVideoSizeChanged: " + width + "x" + height);
            mVideoWidth = width;
            mVideoHeight = height;
//            mVideoWidth = 1796;
//            mVideoHeight = 1008;
//            if(mRenderView != null){
//                mRenderView.setVideoSize(mVideoWidth, mVideoHeight);
//            }

            UpdateScreenSize();
        }
    };

    IMediaPlayer.OnPreparedListener mPreparedListener = new IMediaPlayer.OnPreparedListener() {
        @Override
        public void onPrepared(IMediaPlayer mp) {
            Log.i(TAG, "onPrepared");
            mCurrentState = STATE_PREPARED;

            if(mOnPreparedListener != null){
                mOnPreparedListener.onPrepared(mMediaPlayer);
            }

            mCanPause = mCanSeekBack = mCanSeekForward = true;

            if(mMediaController != null){
                mMediaController.setEnabled(true);
            }

            mVideoWidth = mp.getVideoWidth();
            mVideoHeight = mp.getVideoHeight();
//
//            mVideoWidth = 1796;
//            mVideoHeight = 1008;

            int seekToPosition = mSeekWhenPrepared;  // mSeekWhenPrepared may be changed after seekTo() call
            if(seekToPosition != 0)
                seekTo(seekToPosition);

            if(mVideoWidth * mVideoHeight != 0){
                if(mRenderView != null){
//                    mRenderView.setVideoSize(mVideoWidth, mVideoHeight);
                    UpdateScreenSize();
                    if(mVideoWidth == mSurfaceWidth && mVideoHeight == mSurfaceHeight){
                        // We didn't actually change the size (it was already at the size
                        // we need), so we won't get a "surface changed" callback, so
                        // start the video here instead of in the callback.
                        if(mTargetState == STATE_PLAYING){
                            start();
                            if(mMediaController != null){
                                mMediaController.show();
                            }
                        }else if(!isPlaying() && (seekToPosition !=0 || getCurrentPosition() > 0)){
                            if(mMediaController != null){
                                // Show the media controls when we're paused into a video and make 'em stick.
                                mMediaController.show(0);
                            }
                        }
                    }
                }

            }else {
                if(mTargetState == STATE_PLAYING){
                    start();
                }
            }
        }
    };

    IMediaPlayer.OnCompletionListener mCompletionListener = new IMediaPlayer.OnCompletionListener() {
        @Override
        public void onCompletion(IMediaPlayer mp) {
            mCurrentState = STATE_PLAYBACK_COMPLETED;
            mTargetState = STATE_PLAYBACK_COMPLETED;
            if(mMediaController != null){
                mMediaController.hide();
            }
            if(mOnCompletionListener != null){
                mOnCompletionListener.onCompletion(mMediaPlayer);
            }
        }
    };

    IMediaPlayer.OnInfoListener mInfoListener = new IMediaPlayer.OnInfoListener() {
        @Override
        public boolean onInfo(IMediaPlayer mp, int what, int extra) {
            if(mOnInfoListener != null){
                mOnInfoListener.onInfo(mp, what, extra);
            }
            return true;
        }
    };

    IMediaPlayer.OnErrorListener mErrorListener = new IMediaPlayer.OnErrorListener() {
        @Override
        public boolean onError(IMediaPlayer mp, int what, int extra) {
            mCurrentState = STATE_ERROR;
            mTargetState = STATE_ERROR;
            if(mMediaController != null){
                mMediaController.hide();
            }

            if(mOnErrorListener != null){
                mOnErrorListener.onError(mMediaPlayer, what, extra);
                return true;
            }
            return true;

        }
    };

    IMediaPlayer.OnBufferingUpdateListener mBufferingUpdateListener = new IMediaPlayer.OnBufferingUpdateListener() {
        @Override
        public void onBufferingUpdate(IMediaPlayer mp, int percent) {
            mCurrentBufferPercentage = percent;
        }
    };

    IMediaPlayer.OnSeekCompleteListener mSeekCompleteListener = new IMediaPlayer.OnSeekCompleteListener() {
        @Override
        public void onSeekComplete(IMediaPlayer mp) {

        }
    };

    IMediaPlayer.OnTimedTextListener mTimedTextListener = new IMediaPlayer.OnTimedTextListener() {
        @Override
        public void onTimedText(IMediaPlayer mp, String text) {

        }
    };


    void release(boolean cleartargetstate){
        Log.i(TAG, "release");
        if(mMediaPlayer != null){
            mMediaPlayer.reset();
            mMediaPlayer.release();
            mMediaPlayer = null;
            mCurrentState = STATE_IDLE;
        }
        if(cleartargetstate)
            mTargetState = STATE_IDLE;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if(event.getAction() == MotionEvent.ACTION_DOWN
            && isInPlaybackState() && mMediaController != null){
            toggleMediaControlsVisiblity();
        }
        return super.onTouchEvent(event);
    }

    @Override
    public boolean onTrackballEvent(MotionEvent event) {
        if(event.getAction() == MotionEvent.ACTION_DOWN
                && isInPlaybackState() && mMediaController != null){
            toggleMediaControlsVisiblity();
        }
        return super.onTrackballEvent(event);
    }


    private void toggleMediaControlsVisiblity() {
        if (mMediaController.isShowing()) {
            mMediaController.hide();
        } else {
            mMediaController.show();
        }
    }

    //implements MediaController.MediaPlayerControl
    @Override
    public void start() {
        Log.i(TAG, "start");
        if(isInPlaybackState()){
            mMediaPlayer.start();
            mCurrentState = STATE_PLAYING;
        }
        mTargetState = STATE_PLAYING;
    }

    @Override
    public void pause() {
        if(isInPlaybackState()){
            if(mMediaPlayer.isPlaying()){
                mMediaPlayer.pause();
                mCurrentState = STATE_PAUSED;
            }
        }
        mTargetState = STATE_PAUSED;
    }

    @Override
    public int getDuration() {
        if(isInPlaybackState()){
            return mMediaPlayer.getDuration();
        }
        return 0;
    }

    @Override
    public int getCurrentPosition() {
        if(isInPlaybackState()){
            return mMediaPlayer.getCurrentPosition();
        }
        return 0;
    }

    @Override
    public void seekTo(int pos) {
        if(isInPlaybackState()){
            mMediaPlayer.seekTo(pos);
            mSeekWhenPrepared = 0;
        }else{
            mSeekWhenPrepared = pos;
        }
    }

    @Override
    public boolean isPlaying() {
        return isInPlaybackState() && mMediaPlayer.isPlaying();
    }

    @Override
    public int getBufferPercentage() {
        if(mMediaPlayer != null){
            return mCurrentBufferPercentage;
        }
        return 0;
    }

    private boolean isInPlaybackState() {
        return (mMediaPlayer != null &&
                mCurrentState != STATE_ERROR &&
                mCurrentState != STATE_IDLE &&
                mCurrentState != STATE_PREPARING);
    }

    @Override
    public boolean canPause() {
        return mCanPause;
    }

    @Override
    public boolean canSeekBackward() {
        return mCanSeekBack;
    }

    @Override
    public boolean canSeekForward() {
        return mCanSeekForward;
    }

    @Override
    public int getAudioSessionId() {
        return 0;
    }


    public void SetScreenSize(int width, int height) {
        mScreenHeight = height;
        mScreenWidth = width;
        if(width > height){
            mScreenWidth = height;
            mScreenHeight = width;
        }
    }

    private void UpdateScreenSize(){
        Log.i(TAG, "fullscreen: " + m_fullscreen);
        if(m_fullscreen){
            if(mRenderView != null) {
                int targetWidth = 0;
                int targetHeight = 0;

                int screenW = mScreenWidth;
                int screenH = mScreenHeight;
                if(mOrientation == Configuration.ORIENTATION_LANDSCAPE){
                    screenW = mScreenHeight;
                    screenH = mScreenWidth;
                }


                if(screenW * mVideoHeight > mVideoWidth * screenH){
                    targetHeight = screenH;
                    targetWidth = mVideoWidth * screenH / mVideoHeight;
                }else{
                    targetWidth = screenW;
                    targetHeight = screenW * mVideoHeight / mVideoWidth;
                }

//                Log.i(TAG, "targetWidth: " + targetWidth + "   targetHeight: " + targetHeight);
//                Log.i(TAG, "mScreenWidth: " + screenW + "   mScreenHeight: " + screenH);

//                if(landscape){
//                    mRenderView.setVideoSize(targetHeight, targetWidth);
//                }else{
//                    mRenderView.setVideoSize(targetWidth, targetHeight);
//                }

                mRenderView.setVideoSize(targetWidth, targetHeight);
            }
        }else{
            if(mRenderView != null) {
                mRenderView.setVideoSize(mVideoWidth, mVideoHeight);
            }
        }
    }

    public void OnOrientationChanged(int orientation){
        Log.i(TAG, "OnOrientationChanged");
        mOrientation = orientation;
        UpdateScreenSize();

    }

    public interface OrientationCallback{
        void ChangeOrientation(boolean landscape);
    }

}
