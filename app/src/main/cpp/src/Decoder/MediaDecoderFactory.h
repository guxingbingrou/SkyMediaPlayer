//
// Created by zou on 2024/8/29.
//

#ifndef SKYMEDIAPLAYER_MEDIADECODERFACTORY_H
#define SKYMEDIAPLAYER_MEDIADECODERFACTORY_H
#include <memory>
#include "SkyDecoder.h"

enum MediaDecoderType{
    MediaDecoderFFmpeg,
    MediaDecoderMediaCodec
};

class MediaDecoderFactory {
public:
    static SkyDecoder* CreateMediaDecoder(MediaDecoderType type=MediaDecoderFFmpeg);
};


#endif //SKYMEDIAPLAYER_MEDIADECODERFACTORY_H
