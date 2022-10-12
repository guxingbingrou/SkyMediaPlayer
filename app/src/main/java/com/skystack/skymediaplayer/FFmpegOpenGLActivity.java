package com.skystack.skymediaplayer;

import androidx.appcompat.app.AppCompatActivity;

import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;

import com.skystack.skymediaplayer.MediaPlayer.FFMediaPlayer;
import com.skystack.skymediaplayer.MediaPlayer.VideoDecoderObserver;
import com.skystack.skymediaplayer.util.MyGLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class FFmpegOpenGLActivity extends AppCompatActivity implements GLSurfaceView.Renderer{
    MyGLSurfaceView glSurfaceView;
    FFMediaPlayer ffMediaPlayer;
    private String mVideoPath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/sky/one_piece.mp4";
    static final String TAG = "FFmpegOpenGLActivity";
    Observer observer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_f_fmpeg_open_g_l);

        glSurfaceView = findViewById(R.id.glsurfaceView);
        glSurfaceView.setEGLContextClientVersion(3);
        glSurfaceView.setRenderer(this);
        glSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);

        ffMediaPlayer = new FFMediaPlayer();
        observer = new Observer();
        ffMediaPlayer.InitMediaPLayer(mVideoPath, null, observer);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if(ffMediaPlayer != null){
            ffMediaPlayer.DestroyMediaPlayer();
            ffMediaPlayer = null;
        }
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.i(TAG, "onSurfaceCreated ");
        ffMediaPlayer.OnSurfaceCreated();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.i(TAG, "surfaceChanged  width x height: " + width + " x " + height);
        ffMediaPlayer.StartMediaPlayer();

        ffMediaPlayer.OnSurfaceChanged(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
//        Log.i(TAG, "onDrawFrame ");
        ffMediaPlayer.OnDrawFrame();
    }

    class Observer implements VideoDecoderObserver {

        @Override
        public void OnDecoderReady(int width, int height) {
            if(width * height != 0){
                glSurfaceView.setAspectRadio(width, height);
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