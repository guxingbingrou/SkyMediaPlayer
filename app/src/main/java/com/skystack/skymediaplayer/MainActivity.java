package com.skystack.skymediaplayer;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import com.skystack.skymediaplayer.databinding.ActivityMainBinding;
import com.skystack.skymediaplayer.util.CommonUtils;

import java.io.IOException;

public class MainActivity extends AppCompatActivity {
    static private final String TAG = "SkyMediaPlayer";
    private ActivityMainBinding binding;
    static private final String[] PERMISSION = {Manifest.permission.WRITE_EXTERNAL_STORAGE};

    static {
        System.loadLibrary("native-lib");
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        CommonUtils.copyAssetsDirToSDCard(this, "sky", "/sdcard");
        if (!allPermissonsGranted()) {
            ActivityCompat.requestPermissions(this, PERMISSION, 1);
        }
    }

    private boolean allPermissonsGranted() {
        boolean ret = true;
        for (String str : PERMISSION) {
            ret = ret && (ActivityCompat.checkSelfPermission(this, str) == PackageManager.PERMISSION_GRANTED);
        }
        return ret;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 1) {
            if (!allPermissonsGranted()) {
                Log.e(TAG, "request permissions denied");
                finish();
            }
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main_menu, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        int id = item.getItemId();
        switch (id){
            case R.id.test1:
                Log.i(TAG, "haha");
                startActivity(new Intent(MainActivity.this, FFmpegNativeActivity.class));
                break;
            case R.id.test2:
                Log.i(TAG,"huhu");
                break;
        }
        return super.onOptionsItemSelected(item);
    }

}