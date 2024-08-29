//
// Created by zou on 2024/8/29.
//

#ifndef SKYMEDIAPLAYER_VIDEODECODER_H
#define SKYMEDIAPLAYER_VIDEODECODER_H
#include <memory>
enum VideoDecoderType{
    VideoDecoderFFmpeg,
    VideoDecoderMediaCodec
};

class VideoDecoder {
public:
    static std::shared_ptr<VideoDecoder> CreateVideoDecoder(VideoDecoderType type);
};


#endif //SKYMEDIAPLAYER_VIDEODECODER_H
