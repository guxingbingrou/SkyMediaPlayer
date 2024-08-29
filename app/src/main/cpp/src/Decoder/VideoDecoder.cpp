//
// Created by zou on 2024/8/29.
//

#include "VideoDecoder.h"

std::shared_ptr <VideoDecoder> VideoDecoder::CreateVideoDecoder(VideoDecoderType type) {
    std::shared_ptr<VideoDecoder> videoDecoder;
    switch (type) {
        case VideoDecoderFFmpeg:
            break;
        case VideoDecoderMediaCodec:
            break;
    }
    return videoDecoder;
}
