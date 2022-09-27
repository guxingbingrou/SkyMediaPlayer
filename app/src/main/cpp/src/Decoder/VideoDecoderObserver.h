//
// Created by zou on 2022/9/22.
//

#ifndef SKYMEDIAPLAYER_VIDEODECODEROBSERVER_H
#define SKYMEDIAPLAYER_VIDEODECODEROBSERVER_H

#include "utils/PlayerFormats.h"
class VideoDecoderObserver {
public:
    virtual void OnDecoderReady(int width, int height) = 0;
    virtual void OnDecoderOneFrame(FrameParams* frameParams) = 0;
    virtual void OnDecoderDone() = 0;
    virtual ~VideoDecoderObserver(){}
};


#endif //SKYMEDIAPLAYER_VIDEODECODEROBSERVER_H
