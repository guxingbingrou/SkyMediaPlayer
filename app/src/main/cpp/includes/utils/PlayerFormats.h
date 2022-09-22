//
// Created by zou on 2022/9/16.
//

#ifndef SKYMEDIAPLAYER_PLAYERFORMATS_H
#define SKYMEDIAPLAYER_PLAYERFORMATS_H
#include <string>

enum ImageFormat{
    IMAGE_FORMAT_NONE,
    IMAGE_FORMAT_RGBA,
    IMAGE_FORMAT_I420,
    IMAGE_FORMAT_J420,
    IMAGE_FORMAT_NV12,
    IMAGE_FORMAT_NV21
};

struct PacketParams{
    uint8_t* data;
    size_t size;
    long timestamp;
};

struct FrameParams{
    uint8_t* video_raw_data[4];
    int video_raw_linesize[4];
    int width;
    int height;
    ImageFormat format;
    long timestamp;
};




#endif //SKYMEDIAPLAYER_PLAYERFORMATS_H
