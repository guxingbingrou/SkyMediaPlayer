package com.skystack.skymediaplayer;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.SurfaceHolder;
import android.widget.FrameLayout;

import com.skystack.skymediaplayer.databinding.ActivityVideoBinding;

public class VideoActivity extends AppCompatActivity implements MediaPlayerObserver {
    private ActivityVideoBinding binding;

    private final static String TAG = VideoActivity.class.getName();
    private MediaPlayer mediaPlayer = null;
    private SurfaceRenderView surfaceView = null;
    private int surfaceWidth = 0;
    private int surfaceHeight = 0;
    private boolean setUri = false;
    private boolean setSurface = false;
    private boolean running = false;

    private String videoPath;

    public static Intent newIntent(Context context, String videoPath){
        Intent intent = new Intent(context, VideoActivity.class);
        intent.putExtra("videoPath", videoPath);
        return intent;
    }

    public static void intentTo(Context context, String videoPath){
        context.startActivity(newIntent(context, videoPath));
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video);
        binding = ActivityVideoBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        init();
    }

    void init(){
        videoPath = getIntent().getStringExtra("videoPath");

        mediaPlayer = MediaPlayer.CreateMediaPlayer(MediaPlayer.MediaPlayerFFmpeg, this);

        FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.WRAP_CONTENT,
                FrameLayout.LayoutParams.WRAP_CONTENT,
                Gravity.CENTER);

        surfaceView = new SurfaceRenderView(this);
        surfaceView.setLayoutParams(layoutParams);
        binding.frameLayout.addView(surfaceView);

        SurfaceHolder surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                Log.i(TAG, "surfaceCreated");
                mediaPlayer.SetSurface(holder.getSurface());
                setSurface = true;


                mediaPlayer.SetSource(videoPath);
                setUri = true;
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
                Log.i(TAG, "surfaceChanged wxh: " + width + "x" + height);
                if(mediaPlayer !=null && width * height !=0){
                    tryStartMediaPlayer();
                }
            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                Log.i(TAG, "surfaceDestroyed");
                mediaPlayer.SetSurface(null);
                setSurface = false;
                if(mediaPlayer !=null){
                    mediaPlayer.Stop();
                    running = false;
                }

            }
        });

    }

    void tryStartMediaPlayer(){
        if(running)
            return;
        if(surfaceView !=null && surfaceWidth*surfaceHeight!=0 && setSurface && setUri && mediaPlayer != null){
            mediaPlayer.Start();
            running = true;
        }
    }

    @Override
    public void OnResolutionChanged(int width, int height) {
        if(width == surfaceWidth && height == surfaceHeight)
            return;

        Log.i(TAG, "OnResolutionChanged: " + width + "x" + height);
        surfaceWidth = width;
        surfaceHeight = height;
        if(surfaceView != null){
            surfaceView.setAspectRatio(surfaceWidth, surfaceHeight);
        }
    }
}