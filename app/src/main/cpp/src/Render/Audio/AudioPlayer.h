//
// Created by zou on 2022/9/22.
//

#ifndef SKYMEDIAPLAYER_AUDIOPLAYER_H
#define SKYMEDIAPLAYER_AUDIOPLAYER_H

#include "utils/PlayerFormats.h"
class AudioPlayer {
public:
    virtual ~AudioPlayer(){};
    virtual void InitAudioPlayer(int channel, int sample_rate, SampleFormat format) = 0;
    virtual void OnDecodedFrame(const PcmParams* frame) = 0;
    virtual void DestroyAudioPlayer() = 0;

protected:

};


#endif //SKYMEDIAPLAYER_AUDIOPLAYER_H
