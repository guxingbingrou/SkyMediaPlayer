//
// Created by zou on 2024/8/30.
//

#ifndef SKYMEDIAPLAYER_AUDIOPLAYER_H
#define SKYMEDIAPLAYER_AUDIOPLAYER_H
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <thread>
#include <condition_variable>
#include <mutex>
extern "C"{
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
};
#include "SkyFrameQueue.h"
#include "SkyClock.h"

class AudioPlayer {
public:
    int Init(const std::shared_ptr<SkyFrameQueue>& audioFrameQueue, const std::shared_ptr<SkyClock>& skyClock);
    int Start();
    int Pause();
    int Stop();
    int Destroy();
    void SetTimebase(const AVRational& timebase);

private:
    int CreateSLEngine();
    int CreateOutputMix();
    int CreateAudioPlayer();
    int InitSwr(AVFrame* frame);

    static void AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void* context);
    void Loop();
    SwrContext* m_swr_context = nullptr;
    AVRational m_audio_timebase;

    SLObjectItf m_engine = nullptr;
    SLEngineItf m_engine_interface = nullptr;
    SLObjectItf m_output_mix = nullptr;
    SLObjectItf m_player = nullptr;
    SLPlayItf m_player_play = nullptr;
    SLVolumeItf m_player_volume = nullptr;
    SLAndroidSimpleBufferQueueItf m_player_buffer_queue = nullptr;

    std::thread m_thread;
    bool m_running = false;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::shared_ptr<SkyFrameQueue> m_audio_queue;
    std::shared_ptr<SkyClock> m_clock;

    uint8_t* m_buffers = nullptr;
    int m_bits_per_sample = SL_PCMSAMPLEFORMAT_FIXED_16;
    int m_bytes_per_frame = 0;
    int m_frames_per_buffer = 0;
    int m_bytes_per_buffer = 0;
    int m_buffer_capacity = 0;
    int m_bytes_per_sec = 0;
    int m_channels = 2;
    int m_samplerate = SL_SAMPLINGRATE_44_1;

    AVSampleFormat m_format = AV_SAMPLE_FMT_S16;

    static constexpr int BUFFER_MILLI = 10;    //每个buffer持续时间 单位ms
    static constexpr int MAX_BUFFER_COUNTS = 255;
};


#endif //SKYMEDIAPLAYER_AUDIOPLAYER_H
