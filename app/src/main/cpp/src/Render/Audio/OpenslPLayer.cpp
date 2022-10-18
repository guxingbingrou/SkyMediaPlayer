//
// Created by zou on 2022/9/22.
//


#include "OpenslPLayer.h"
#include "utils/AndroidLog.h"
void OpenslPLayer::InitAudioPlayer(int channel, int sample_rate, SampleFormat format) {
    CreateSLEngine();

    CreateOutputMix();

    CreateAudioPlayer();

    (*m_player_play)->SetPlayState(m_player_play, SL_PLAYSTATE_PLAYING);

    m_thread = std::thread(AudioPlayerCallback, this->m_player_buffer_queue, this);

}

int OpenslPLayer::CreateSLEngine() {
    SLresult result = SL_RESULT_SUCCESS;

    result = slCreateEngine(&m_engine, 0, nullptr, 0, nullptr, nullptr);

    if(result != SL_RESULT_SUCCESS){
        ERROR("opensl create engine failed(%d)", result);
        return result;
    }

    result = (*m_engine)->Realize(m_engine, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS){
        ERROR("opensl Realize engine failed(%d)", result);
        return result;
    }

    result = (*m_engine)->GetInterface(m_engine, SL_IID_ENGINE, &m_engine_interface);
    if(result != SL_RESULT_SUCCESS){
        ERROR("opensl engine GetInterface failed(%d)", result);
        return result;
    }

    m_audio_frame_queue = std::make_shared<BufferQueue<PcmParams>>();

    return result;
}

int OpenslPLayer::CreateOutputMix() {
    SLresult result = SL_RESULT_SUCCESS;
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};

    result = (*m_engine_interface)->CreateOutputMix(m_engine_interface, &m_output_mix, 1,  mids, mreq);
    if(result != SL_RESULT_SUCCESS){
        ERROR("opensl create output mix failed(%d)", result);
        return result;
    }

    result = (*m_output_mix)->Realize(m_output_mix, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS){
        ERROR("opensl output mix Realize failed(%d)", result);
        return result;
    }

    return result;
}

int OpenslPLayer::CreateAudioPlayer() {
    SLresult result = SL_RESULT_SUCCESS;

    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            2,
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_32,
            SL_PCMSAMPLEFORMAT_FIXED_32,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };

    SLDataSource slDataSource = {&android_queue, &pcm};

    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, m_output_mix};
    SLDataSink sink = {&outputMix, nullptr};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    result = (*m_engine_interface)->CreateAudioPlayer(m_engine_interface, &m_player, &slDataSource, &sink, 3, ids, req);
    if(result != SL_RESULT_SUCCESS){
        ERROR("opensl CreateAudioPlayer failed(%d)", result);
        return result;
    }

    result = (*m_player)->Realize(m_player, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS){
        ERROR("opensl AudioPlayer Realize failed(%d)", result);
        return result;
    }

    result = (*m_player)->GetInterface(m_player, SL_IID_PLAY, &m_player_play);
    if(result != SL_RESULT_SUCCESS){
        ERROR("opensl AudioPlayer get interface play failed(%d)", result);
        return result;
    }

    result = (*m_player)->GetInterface(m_player, SL_IID_VOLUME, &m_player_volume);
    if(result != SL_RESULT_SUCCESS){
        ERROR("opensl AudioPlayer get interface volume failed(%d)", result);
        return result;
    }

    result = (*m_player)->GetInterface(m_player, SL_IID_BUFFERQUEUE, &m_player_buffer_queue);
    if(result != SL_RESULT_SUCCESS){
        ERROR("opensl AudioPlayer get interface buffer queue failed(%d)", result);
        return result;
    }

    result = (*m_player_buffer_queue)->RegisterCallback(m_player_buffer_queue,
                                           &OpenslPLayer::AudioPlayerCallback, this);
    if(result != SL_RESULT_SUCCESS){
        ERROR("opensl AudioPlayer RegisterCallback failed(%d)", result);
        return result;
    }

    return result;

}

void OpenslPLayer::AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context) {
    auto opensl_pLayer = static_cast<OpenslPLayer*>(context);


    PcmParams pcmParams;
    opensl_pLayer->GetBufferQueue()->WaitePopBuffer(pcmParams);
    if(pcmParams.size == 0){
        WARNING("pop no data");
        return;
    }

    auto buffer_queue_itf = opensl_pLayer->GetSLBufferQueueItf();
    if(buffer_queue_itf){
        SLresult result = (*buffer_queue_itf)->Enqueue(buffer_queue_itf, pcmParams.data, pcmParams.size);

        delete pcmParams.data;

        if(result != SL_RESULT_SUCCESS){
            ERROR("opensl AudioPlayer REnqueue buffer failed(%d)", result);
            return ;
        }
    }

}

void OpenslPLayer::DestroyAudioPlayer() {
    if(m_player_play){
        (*m_player_play)->SetPlayState(m_player_play, SL_PLAYSTATE_STOPPED);
        m_player_play = nullptr;
    }

    m_audio_frame_queue->Stop();

    if(m_player){
        (*m_player)->Destroy(m_player);
        m_player = nullptr;
    }

    if(m_output_mix){
        (*m_output_mix)->Destroy(m_output_mix);
        m_output_mix = nullptr;
    }

    if(m_engine){
        (*m_engine)->Destroy(m_engine);
        m_engine = nullptr;
    }
}

void OpenslPLayer::OnDecodedFrame(const PcmParams *frame) {
    PcmParams temp;
    temp.size = frame->size;
    temp.data = static_cast<uint8_t *>(malloc(temp.size));

    memcpy(temp.data, frame->data, temp.size);

    m_audio_frame_queue->WaitePushBuffer(temp);

}
