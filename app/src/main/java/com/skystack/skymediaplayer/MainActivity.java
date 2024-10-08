package com.skystack.skymediaplayer;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.content.ContentUris;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import android.util.Log;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.FrameLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.skystack.skymediaplayer.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity implements MediaPlayerObserver {

    // Used to load the 'SkyMediaPlayer' library on application startup.
    static {
        System.loadLibrary("SkyMediaPlayer");
    }

    private ActivityMainBinding binding;

    private final static String TAG = MainActivity.class.getName();

    private final static int RequestCodeFileOpen = 1;
    private final static int RequestCodePermissions = 2;
    static private final String[] PERMISSION = {Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.INTERNET};

    private MediaPlayer mediaPlayer = null;
    private SurfaceRenderView surfaceView = null;
    private int surfaceWidth = 0;
    private int surfaceHeight = 0;
    private boolean setUri = false;
    private boolean setSurface = false;
    private boolean running = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        mediaPlayer = MediaPlayer.CreateMediaPlayer(MediaPlayer.MediaPlayerFFmpeg, this);

        FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.WRAP_CONTENT,
                FrameLayout.LayoutParams.WRAP_CONTENT,
                Gravity.CENTER);

        surfaceView = new SurfaceRenderView(this);
//        surfaceView = binding.surfaceView;
        surfaceView.setLayoutParams(layoutParams);

        binding.frameLayout.addView(surfaceView);

        SurfaceHolder surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                Log.i(TAG, "surfaceCreated");
                mediaPlayer.SetSurface(holder.getSurface());
                setSurface = true;
                tryStartMediaPlayer();
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
            }
        });


        if (!allPermissionsGranted()) {
            ActivityCompat.requestPermissions(this, PERMISSION, RequestCodePermissions);
        }


    }

    void tryStartMediaPlayer(){
        if(running)
            return;
        if(surfaceView !=null && surfaceWidth*surfaceHeight!=0 && setSurface && setUri && mediaPlayer != null){
            mediaPlayer.Start();
            running = true;
        }
    }

    private boolean allPermissionsGranted() {
        boolean ret = true;
        for (String str : PERMISSION) {
            ret = ret && (ActivityCompat.checkSelfPermission(this, str) == PackageManager.PERMISSION_GRANTED);
        }
        return ret;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == RequestCodePermissions) {
            if (!allPermissionsGranted()) {
                Log.e(TAG, "request permissions denied");
                Toast.makeText(this, "request permissions denied", Toast.LENGTH_SHORT).show();
                finish();
            }
        }
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        switch (item.getItemId()){
            case R.id.open_file:
                Toast.makeText(this, "open file", Toast.LENGTH_SHORT).show();
                openFileManager();
                break;
            case R.id.setting:
                Toast.makeText(this, "setting", Toast.LENGTH_SHORT).show();
                break;
        }
        return super.onOptionsItemSelected(item);
    }

    private void openFileManager(){
        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.setType("*/*");
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        startActivityForResult(intent, RequestCodeFileOpen);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if(requestCode == RequestCodeFileOpen){
            if(resultCode == RESULT_OK){
                Uri uri = data.getData();
                String path = Utils.getPath(this, uri);
                Toast.makeText(this, "filePath: " + path, Toast.LENGTH_LONG).show();
                if(path != null){
                    mediaPlayer.SetSource(path);
                    setUri = true;
                    tryStartMediaPlayer();
                }
            }
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