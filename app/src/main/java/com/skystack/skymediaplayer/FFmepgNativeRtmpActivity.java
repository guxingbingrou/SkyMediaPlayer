package com.skystack.skymediaplayer;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.SurfaceHolder;

import com.skystack.skymediaplayer.MediaPlayer.FFMediaPlayer;
import com.skystack.skymediaplayer.MediaPlayer.VideoDecoderObserver;
import com.skystack.skymediaplayer.util.MySurfaceView;

public class FFmepgNativeRtmpActivity extends AppCompatActivity {
    MySurfaceView surfaceView;
    FFMediaPlayer ffMediaPlayer;

    private String mVideoPath = "rtmp://119.91.25.164/live/livestream";
    static final String TAG = "FFmpegNativeActivity";
    Observer observer;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_f_fmepg_native_rtmp);

        surfaceView = findViewById(R.id.surfaceView);

        SurfaceHolder surfaceHolder = surfaceView.getHolder();

        surfaceHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                ffMediaPlayer = new FFMediaPlayer();
                observer = new Observer();
                ffMediaPlayer.InitMediaPLayer(mVideoPath, holder.getSurface(), observer);
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
                Log.i(TAG, "surfaceChanged  width x height: " + width + " x " + height);
                ffMediaPlayer.StartMediaPlayer();
            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                ffMediaPlayer.DestroyMediaPlayer();
                ffMediaPlayer = null;
            }
        });
    }
    

    class Observer implements VideoDecoderObserver {

        @Override
        public void OnDecoderReady(int width, int height) {
            if(width * height != 0){
                surfaceView.setAspectRatio(width, height);
            }
        }

        @Override
        public void OnDecoderOneFrame() {

        }

        @Override
        public void OnDecoderDone() {

        }
    }
}