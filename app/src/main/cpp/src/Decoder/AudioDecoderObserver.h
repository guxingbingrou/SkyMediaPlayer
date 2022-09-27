//
// Created by zou on 2022/9/22.
//

#ifndef SKYMEDIAPLAYER_AUDIODECODEROBSERVER_H
#define SKYMEDIAPLAYER_AUDIODECODEROBSERVER_H

#include "utils/PlayerFormats.h"
class AudioDecoderObserver {
public:
    virtual void OnDecoderReady(int channels, int sample_rate) = 0;
    virtual void OnDecoderOneFrame(PcmParams* frameParams) = 0;
    virtual void OnDecoderDone() = 0;
    virtual ~AudioDecoderObserver(){}
};


#endif //SKYMEDIAPLAYER_AUDIODECODEROBSERVER_H
