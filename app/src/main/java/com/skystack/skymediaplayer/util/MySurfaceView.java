package com.skystack.skymediaplayer.util;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceView;

/**
 * Time: 2022/9/22
 * Author: zou
 * Description:
 */
public class MySurfaceView extends SurfaceView {
    private int mRatioWidth = 0;
    private int mRatioHegiht = 0;
    static final String TAG = "MySurfaceView";
    public MySurfaceView(Context context) {
        super(context);
    }

    public MySurfaceView(Context context, AttributeSet attrs) {
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

    public void setAspectRatio(int width, int height){
        if(width * height <=0 ) return;
        Log.i(TAG, "setAspectRatio  width x height: " + width + " x " + height);
        mRatioHegiht = height;
        mRatioWidth = width;
        requestLayout();
    }
}
