//
// Created by zou on 2022/9/22.
//

#ifndef SKYMEDIAPLAYER_OPENSLPLAYER_H
#define SKYMEDIAPLAYER_OPENSLPLAYER_H
#include "AudioPlayer.h"
#include "utils/BufferQueue.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <thread>


class OpenslPLayer : public AudioPlayer{
public:
    void InitAudioPlayer(int channel=2, int sample_rate=44100, SampleFormat format = AUDIO_FORMAT_S32) override ;
    void OnDecodedFrame(const PcmParams* frame) override;
    void DestroyAudioPlayer() override;

    inline std::shared_ptr<BufferQueue<PcmParams>> GetBufferQueue(){return m_audio_frame_queue;}
    inline SLPlayItf GetPLayItf(){return m_player_play;}
    inline SLAndroidSimpleBufferQueueItf GetSLBufferQueueItf(){return m_player_buffer_queue;}
private:
    int CreateSLEngine();
    int CreateOutputMix();
    int CreateAudioPlayer();

    static void AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void* context);

    SLObjectItf m_engine = nullptr;
    SLEngineItf m_engine_interface = nullptr;
    SLObjectItf m_output_mix = nullptr;
    SLObjectItf m_player = nullptr;
    SLPlayItf m_player_play = nullptr;
    SLVolumeItf m_player_volume = nullptr;
    SLAndroidSimpleBufferQueueItf m_player_buffer_queue = nullptr;

    std::shared_ptr<BufferQueue<PcmParams>> m_audio_frame_queue;

    std::thread m_thread;


};


#endif //SKYMEDIAPLAYER_OPENSLPLAYER_H
