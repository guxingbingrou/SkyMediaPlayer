package com.skystack.skymediaplayer.MediaPlayer;

import android.view.Surface;
import android.view.SurfaceHolder;

public interface IMediaPlayer {
    void setDisplay(SurfaceHolder surfaceHolder);

    void setDataSource(String path);

    void prepareAsync() throws IllegalStateException;

    void start() throws IllegalStateException;

    void stop() throws IllegalStateException;

    void pause() throws IllegalStateException;

    void release();
}
