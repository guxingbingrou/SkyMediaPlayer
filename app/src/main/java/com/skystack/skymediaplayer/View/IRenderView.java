package com.skystack.skymediaplayer.View;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.View;

import androidx.annotation.NonNull;

import com.skystack.skymediaplayer.MediaPlayer.IMediaPlayer;

public interface IRenderView {
    View getView();

    void setVideoSize(int width, int height);

    void setVideoSampleAspectRatio(int num, int den);

    void setVideoRotation(int degree);

    void setAspectRatio(int aspectRatio);

    void addRenderCallback(IRenderCallback renderCallback);

    void removeRenderCallback(IRenderCallback renderCallback);


    interface ISurfaceHolder{
        void bindToMediaPlayer(IMediaPlayer mediaPlayer);

        @NonNull
        IRenderView getRenderView();

        @NonNull
        SurfaceHolder getSurfaceHolder();

        @NonNull
        Surface getSurface();
    }

    interface IRenderCallback{
        void onSurfaceCreated(@NonNull ISurfaceHolder surfaceHolder, int width, int height);

        void onSurfaceChanged(@NonNull ISurfaceHolder surfaceHolder, int format, int width, int height);

        void onSurfaceDestroyed(@NonNull ISurfaceHolder surfaceHolder);
    }
}


