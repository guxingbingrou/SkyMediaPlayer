//
// Created by zou on 2022/9/14.
//

#ifndef SKYMEDIAPLAYER_DECODERBASE_H
#define SKYMEDIAPLAYER_DECODERBASE_H

#include "utils/PlayerFormats.h"


class DecoderBase {
public:
    virtual bool InitDecoder() = 0;
    virtual bool DestroyDecoder() = 0;

    virtual bool OnPacketAvailable(PacketParams* packetParams) = 0;


    virtual ~DecoderBase(){};
};


#endif //SKYMEDIAPLAYER_DEMUXINGDECODERBASE_H
