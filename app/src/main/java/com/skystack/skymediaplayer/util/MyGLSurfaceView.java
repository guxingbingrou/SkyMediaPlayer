package com.skystack.skymediaplayer.util;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;


/**
 * Time: 2022/10/12
 * Author: zou
 * Description:
 */
public class MyGLSurfaceView extends GLSurfaceView {
    private static final String TAG = "MyGLSurfaceView";
    private int mRatioWidth = 0;
    private int mRatioHegiht = 0;


    /**
     * Standard View constructor. In order to render something, you
     * must call {@link #setRenderer} to register a renderer.
     *
     * @param context
     */
    public MyGLSurfaceView(Context context) {
        super(context);
    }

    /**
     * Standard View constructor. In order to render something, you
     * must call {@link #setRenderer} to register a renderer.
     *
     * @param context
     * @param attrs
     */
    public MyGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);

        int width = MeasureSpec.getSize(widthMeasureSpec);
        int height = MeasureSpec.getSize(heightMeasureSpec);
        Log.i(TAG, "onMeasure  width x height: " + width + " x " + height);
        if(0 == mRatioWidth || 0 == mRatioHegiht){
            setMeasuredDimension(width, height);
        }else{
            if(width < height * mRatioWidth / mRatioHegiht){
                setMeasuredDimension(width, width * mRatioHegiht / mRatioWidth);
            }else{
                setMeasuredDimension(height * mRatioWidth / mRatioHegiht, height);
            }
        }
    }

    public void setAspectRadio(int width, int height){
        if(width * height <=0 ) return;
        Log.i(TAG, "setAspectRatio  width x height: " + width + " x " + height);
        mRatioHegiht = height;
        mRatioWidth = width;
        requestLayout();
    }


}
