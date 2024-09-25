//
// Created by zou on 2024/9/3.
//

#include "MediaCodecDecoder.h"

bool MediaCodecDecoder::Init(SkyMediaPlayer* mediaPlayer, AVStream* stream,
                             const std::shared_ptr<SkyPacketQueue>& packetQueue,
                             const std::shared_ptr<SkyFrameQueue>& frameQueue) {
    m_packet_queue = packetQueue;
    m_frame_queue = frameQueue;
    return false;
}

bool MediaCodecDecoder::Start() {
    return false;
}

bool MediaCodecDecoder::Stop() {
    return false;
}

bool MediaCodecDecoder::Release() {
    return false;
}
