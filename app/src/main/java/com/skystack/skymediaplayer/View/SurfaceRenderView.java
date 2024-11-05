package com.skystack.skymediaplayer.View;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import androidx.annotation.NonNull;

import com.skystack.skymediaplayer.MediaPlayer.IMediaPlayer;

import java.lang.ref.WeakReference;
import java.util.Set;
import java.util.concurrent.CopyOnWriteArraySet;

public class SurfaceRenderView extends SurfaceView implements IRenderView{
    static final String TAG = SurfaceRenderView.class.getName();

    private MeasureHelper mMeasureHelper = null;
    private SurfaceCallback mSurfaceCallback = null;


    public SurfaceRenderView(Context context) {
        super(context);
        initView(context);
    }

    public SurfaceRenderView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initView(context);
    }

    public SurfaceRenderView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initView(context);
    }

    public SurfaceRenderView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        initView(context);
    }

    private void initView(Context context){
        mMeasureHelper = new MeasureHelper();
        mSurfaceCallback = new SurfaceCallback(this);
        getHolder().addCallback(mSurfaceCallback);
//        getHolder().setType(SurfaceHolder.SURFACE_TYPE_NORMAL);
    }

    public void setAspectRatio(int width, int height){

    }

    @Override
    public View getView() {
        return this;
    }

    @Override
    public void setVideoSize(int width, int height) {
        if(width * height <= 0 ) return;
        Log.i(TAG, "setVideoSize  width x height: " + width + " x " + height);

        mMeasureHelper.setVideoSize(width, height);
        getHolder().setFixedSize(width, height);
        requestLayout();
    }

    @Override
    public void setVideoSampleAspectRatio(int num, int den) {
        if(num * den <= 0 ) return;
        Log.i(TAG, "setVideoSampleAspectRatio  num x den: " + num + " x " + den);

        mMeasureHelper.setVideoSampleAspectRatio(num, den);
        requestLayout();
    }

    @Override
    public void setVideoRotation(int degree) {
        Log.i(TAG, "setVideoRotation : " + degree);
    }

    @Override
    public void setAspectRatio(int aspectRatio) {
        Log.i(TAG, "setAspectRatio : " + aspectRatio);
        mMeasureHelper.setAspectRatio(aspectRatio);
        requestLayout();
    }

    @Override
    public void addRenderCallback(IRenderCallback renderCallback) {
        mSurfaceCallback.addRenderCallback(renderCallback);
    }

    @Override
    public void removeRenderCallback(IRenderCallback renderCallback) {
        mSurfaceCallback.removeRenderCallback(renderCallback);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        mMeasureHelper.doMeasure(widthMeasureSpec, heightMeasureSpec);
        setMeasuredDimension(mMeasureHelper.getMeasureWidth(), mMeasureHelper.getMeasureHeight());
    }

    private static final class InternalSurfaceHolder implements IRenderView.ISurfaceHolder{
        private SurfaceRenderView mSurfaceRenderView;
        private SurfaceHolder mSurfaceHolder;
        InternalSurfaceHolder(SurfaceRenderView surfaceRenderView, SurfaceHolder surfaceHolder){
            mSurfaceRenderView = surfaceRenderView;
            mSurfaceHolder = surfaceHolder;
        }

        @Override
        public void bindToMediaPlayer(IMediaPlayer mediaPlayer) {
            if(mediaPlayer != null){
                mediaPlayer.setDisplay(mSurfaceHolder);
            }
        }

        @NonNull
        @Override
        public IRenderView getRenderView() {
            return mSurfaceRenderView;
        }

        @NonNull
        @Override
        public SurfaceHolder getSurfaceHolder() {
            return mSurfaceHolder;
        }

        @NonNull
        @Override
        public Surface getSurface() {
            if(mSurfaceHolder == null)
                return null;
            return mSurfaceHolder.getSurface();
        }
    }

    private static final class SurfaceCallback implements SurfaceHolder.Callback{
        private SurfaceHolder mSurfaceHolder;
        private int mWidth;
        private int mHeight;
        private boolean mFormatChanged = false;
        private int mFormat;

        private WeakReference<SurfaceRenderView> mSurfaceRenderView;
        private Set<IRenderCallback> mRenderCallbackSet = new CopyOnWriteArraySet<>();

        public SurfaceCallback(SurfaceRenderView surfaceRenderView){
            mSurfaceRenderView = new WeakReference<SurfaceRenderView>(surfaceRenderView);
        }

        public void addRenderCallback(IRenderCallback renderCallback) {
            mRenderCallbackSet.add(renderCallback);

            ISurfaceHolder surfaceHolder = null;
            if(mSurfaceHolder != null){
                surfaceHolder = new InternalSurfaceHolder(mSurfaceRenderView.get(), mSurfaceHolder);
                renderCallback.onSurfaceCreated(surfaceHolder, mWidth, mHeight);
            }

            if(mFormatChanged){
                if(surfaceHolder == null){
                    surfaceHolder = new InternalSurfaceHolder(mSurfaceRenderView.get(), mSurfaceHolder);
                    renderCallback.onSurfaceChanged(surfaceHolder, mFormat, mWidth, mHeight);
                }
            }
        }

        public void removeRenderCallback(IRenderCallback renderCallback) {
            mRenderCallbackSet.remove(renderCallback);
        }

        @Override
        public void surfaceCreated(@NonNull SurfaceHolder holder) {
            mSurfaceHolder = holder;
            mFormatChanged = false;
            mFormat = 0;
            mWidth = 0;
            mHeight = 0;

            ISurfaceHolder surfaceHolder = new InternalSurfaceHolder(mSurfaceRenderView.get(), mSurfaceHolder);
            for(IRenderCallback renderCallback : mRenderCallbackSet){
                renderCallback.onSurfaceCreated(surfaceHolder, mWidth, mHeight);
            }

        }

        @Override
        public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
            mSurfaceHolder = holder;
            mFormatChanged = true;
            mFormat = format;
            mWidth = width;
            mHeight = height;

            ISurfaceHolder surfaceHolder = new InternalSurfaceHolder(mSurfaceRenderView.get(), mSurfaceHolder);
            for(IRenderCallback renderCallback : mRenderCallbackSet){
                renderCallback.onSurfaceChanged(surfaceHolder, mFormat, mWidth, mHeight);
            }
        }

        @Override
        public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
            mSurfaceHolder = null;
            mFormatChanged = false;
            mFormat = 0;
            mWidth = 0;
            mHeight = 0;

            ISurfaceHolder surfaceHolder = new InternalSurfaceHolder(mSurfaceRenderView.get(), mSurfaceHolder);
            for(IRenderCallback renderCallback : mRenderCallbackSet){
                renderCallback.onSurfaceDestroyed(surfaceHolder);
            }
        }
    }
}
