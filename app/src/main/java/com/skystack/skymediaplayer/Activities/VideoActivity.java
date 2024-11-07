package com.skystack.skymediaplayer.Activities;

import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.widget.MediaController;

import com.skystack.skymediaplayer.R;
import com.skystack.skymediaplayer.View.SkyVideoView;
import com.skystack.skymediaplayer.databinding.ActivityVideoBinding;

public class VideoActivity extends AppCompatActivity {
    private ActivityVideoBinding binding;

    private final static String TAG = VideoActivity.class.getName();

    private String mVideoPath;

    private MediaController mMediaController;
    private SkyVideoView mVideoView;

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

        mVideoView = binding.videoView;

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

}