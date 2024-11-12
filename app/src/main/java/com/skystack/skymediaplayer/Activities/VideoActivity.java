package com.skystack.skymediaplayer.Activities;

import androidx.annotation.NonNull;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.widget.MediaController;

import com.skystack.skymediaplayer.R;
import com.skystack.skymediaplayer.View.SkyVideoView;
import com.skystack.skymediaplayer.databinding.ActivityVideoBinding;

public class VideoActivity extends AppCompatActivity implements SkyVideoView.OrientationCallback{
    private ActivityVideoBinding binding;

    private final static String TAG = VideoActivity.class.getName();

    private String mVideoPath;

    private MediaController mMediaController;
    private SkyVideoView mVideoView;

    private int mOrientation = Configuration.ORIENTATION_PORTRAIT;

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

        DisplayMetrics displayMetrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
        Log.i(TAG, "wxh: " + displayMetrics.widthPixels + " x " + displayMetrics.heightPixels);

        mVideoView = binding.videoView;
        mVideoView.SetOrientationCallback(this);
        mVideoView.SetScreenSize(displayMetrics.widthPixels, displayMetrics.heightPixels);

        Toolbar toolbar = binding.toolbar;
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        actionBar.hide();

        mMediaController = new MediaController(this);
        mVideoView.setMediaController(mMediaController);

        mVideoPath = getIntent().getStringExtra("videoPath");
        mVideoView.setVideoPath(mVideoPath);

        mVideoView.start();
    }

    @Override
    public void ChangeOrientation(boolean landscape) {
        if(landscape)
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        else
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
    }

    @Override
    public void onConfigurationChanged(@NonNull Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        if(newConfig.orientation != mOrientation){
            mOrientation = newConfig.orientation;
            if(mVideoView != null){
                mVideoView.OnOrientationChanged(mOrientation);
            }
        }

    }
}