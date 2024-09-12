//
// Created by zou on 2024/8/29.
//

#include "MediaDecoderFactory.h"
#include "FFMediaDecoder.h"
#include "MediaCodecDecoder.h"

SkyDecoder* MediaDecoderFactory::CreateMediaDecoder(MediaDecoderType type) {
    SkyDecoder* decoder = nullptr;
    switch (type) {
        case MediaDecoderFFmpeg:
            decoder = new FFMediaDecoder();
            break;
        case MediaDecoderMediaCodec:
            decoder = new MediaCodecDecoder();
            break;
    }
    return decoder;
}
