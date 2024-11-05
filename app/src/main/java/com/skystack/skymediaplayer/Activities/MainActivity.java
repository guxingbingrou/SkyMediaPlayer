package com.skystack.skymediaplayer.Activities;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

import com.skystack.skymediaplayer.R;
import com.skystack.skymediaplayer.Utils;
import com.skystack.skymediaplayer.databinding.ActivityMainBinding;
import com.skystack.skymediaplayer.databinding.ToolbarBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'SkyMediaPlayer' library on application startup.
    static {
        System.loadLibrary("SkyMediaPlayer");
    }

    private ActivityMainBinding binding;

    private final static String TAG = MainActivity.class.getName();

    private final static int RequestCodeFileOpen = 1;
    private final static int RequestCodePermissions = 2;
    static private final String[] PERMISSION = {Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.INTERNET};




    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        if (!allPermissionsGranted()) {
            ActivityCompat.requestPermissions(this, PERMISSION, RequestCodePermissions);
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
                    VideoActivity.intentTo(this, path);
                }
            }
        }
    }

}