package com.skystack.skymediaplayer.MediaPlayer;

/**
 * Time: 2022/9/22
 * Author: zou
 * Description:
 */
public interface VideoDecoderObserver {
    void OnDecoderReady(int width, int height);
    void OnDecoderOneFrame() ;
    void OnDecoderDone();
}
