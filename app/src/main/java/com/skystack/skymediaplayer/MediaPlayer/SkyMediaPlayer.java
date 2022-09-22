package com.skystack.skymediaplayer.MediaPlayer;

import android.view.Surface;

/**
 * Time: 2022/9/21
 * Author: zou
 * Description:
 */

public interface SkyMediaPlayer {
    boolean InitMediaPLayer(String url, Surface surface, VideoDecoderObserver observer);
    boolean StartMediaPlayer();
    boolean StopMediaPlayer();
    boolean DestroyMediaPlayer();

    /** MediaPlayerType:
     * 0-ffmpeg
     * 1-mediacodec
     */
    int mMediaPlayerType = 0;

}
