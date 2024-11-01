//
// Created by zou on 2024/8/30.
//

#include "AudioPlayer.h"
#include "AndroidLog.h"
extern "C"{
#include "libavutil/time.h"
}


int AudioPlayer::Init(const std::shared_ptr<SkyFrameQueue> &audioFrameQueue, const std::shared_ptr<SkyClock>& skyClock) {
    m_audio_queue = audioFrameQueue;
    m_clock = skyClock;

    if(CreateSLEngine() != SL_RESULT_SUCCESS){
        ERROR("create opensl engine failed");
        return -1;
    }
    INFO("CreateSLEngine");

    if(CreateOutputMix() != SL_RESULT_SUCCESS){
        ERROR("create output mix failed");
        return -1;
    }
    INFO("CreateOutputMix");

    if(CreateAudioPlayer() != SL_RESULT_SUCCESS){
        ERROR("create audio player failed");
        return -1;
    }
    INFO("CreateAudioPlayer");

    return 0;

}

int AudioPlayer::Start() {
    if(!m_player_play)
        return -1;
    (*m_player_play)->SetPlayState(m_player_play, SL_PLAYSTATE_PLAYING);

    if(m_running)
        return 0;

    m_running = true;

    m_thread = std::thread(&AudioPlayer::Loop, this);

    return 0;
}

int AudioPlayer::Pause() {
    if(!m_player_play)
        return -1;
    (*m_player_play)->SetPlayState(m_player_play, SL_PLAYSTATE_PAUSED);

    return 0;
}

int AudioPlayer::Stop() {
    if(!m_player_play)
        return -1;
    (*m_player_play)->SetPlayState(m_player_play, SL_PLAYSTATE_STOPPED);


    return 0;
}

int AudioPlayer::Destroy() {
    if(m_buffers){
        free(m_buffers);
        m_buffers = 0;
    }

}

int AudioPlayer::CreateSLEngine() {
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
    return result;
}

int AudioPlayer::CreateOutputMix() {
    SLresult result = SL_RESULT_SUCCESS;
    const SLInterfaceID mids[1] = {SL_IID_VOLUME};
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

int AudioPlayer::CreateAudioPlayer() {
    SLresult result = SL_RESULT_SUCCESS;

    m_format = AV_SAMPLE_FMT_S16;
    m_bits_per_sample = SL_PCMSAMPLEFORMAT_FIXED_16;
//    m_samplerate = SL_SAMPLINGRATE_44_1 / 1000;
//TODO 获取音频设备的采样率，以此来设置OpenSL ES的采样率
    m_samplerate = SL_SAMPLINGRATE_48 / 1000;
    m_channels = 2;
    m_bytes_per_frame = m_channels * m_bits_per_sample / 8;
    m_frames_per_buffer = BUFFER_MILLI * m_samplerate * 1000 / 1000000;
    m_bytes_per_buffer = m_bytes_per_frame * m_frames_per_buffer;
    m_bytes_per_sec = av_samples_get_buffer_size(nullptr, m_channels, m_samplerate, m_format, 1);
    m_buffer_capacity = m_bytes_per_buffer * MAX_BUFFER_COUNTS;
    m_buffers = static_cast<uint8_t *>(malloc(m_buffer_capacity));

    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, MAX_BUFFER_COUNTS};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            static_cast<SLuint32>(m_channels),
            static_cast<SLuint32>(m_samplerate * 1000),
            static_cast<SLuint32>(m_bits_per_sample),
            static_cast<SLuint32>(m_bits_per_sample),
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };

    SLDataSource slDataSource = {&android_queue, &pcm};

    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, m_output_mix};
    SLDataSink sink = {&outputMix, nullptr};

    const SLInterfaceID ids[3] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_VOLUME, SL_IID_PLAY};
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

    result = (*m_player)->GetInterface(m_player, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &m_player_buffer_queue);
    if(result != SL_RESULT_SUCCESS){
        ERROR("opensl AudioPlayer get interface buffer queue failed(%d)", result);
        return result;
    }

    result = (*m_player_buffer_queue)->RegisterCallback(m_player_buffer_queue,
                                                        &AudioPlayer::AudioPlayerCallback, this);
    if(result != SL_RESULT_SUCCESS){
        ERROR("opensl AudioPlayer RegisterCallback failed(%d)", result);
        return result;
    }

    memset(m_buffers, 0, m_buffer_capacity);
    for(int i=0; i < MAX_BUFFER_COUNTS; ++i){
        result = (*m_player_buffer_queue)->Enqueue(m_player_buffer_queue, m_buffers + i * m_bytes_per_buffer, m_bytes_per_buffer);
        if(result != SL_RESULT_SUCCESS){
            ERROR("Enqueue failed(%d), %d", result, i);
            return result;
        }

    }

    return SL_RESULT_SUCCESS;
}

void AudioPlayer::AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context) {
    auto opensl_pLayer = static_cast<AudioPlayer*>(context);

    std::unique_lock<std::mutex> lck(opensl_pLayer->m_mutex);

    opensl_pLayer->m_condition.notify_one();
//    INFO("notify_one");

}

void AudioPlayer::Loop() {

    //设置线程优先级为最高
    struct sched_param schedParam;
    int policy;
    pthread_t thread = pthread_self();

    if(pthread_getschedparam(thread, &policy, &schedParam) < 0){
        ERROR("pthread_getschedparam failed");
    }
    schedParam.sched_priority = sched_get_priority_max(policy);
    if(pthread_setschedparam(thread, policy, &schedParam) < 0){
        ERROR("pthread_setschedparam failed");
    }

    int failed_count = 5;

    uint8_t* buffer = nullptr;
    int buffer_index = 0;

    uint8_t* out_buffer = nullptr;
    int out_offset = 0;
    unsigned int out_size = 0;

    int len = 0;
    double current_read_clock;

    int64_t callback_time;

//    int frame_count = 0;

    while (m_running) {
        SLAndroidSimpleBufferQueueState slState = {0};

//        INFO("lck:%d,%d", slState.count, m_buffer_count);
        {
            std::unique_lock<std::mutex> lck(m_mutex);
            SLresult slRet = (*m_player_buffer_queue)->GetState(m_player_buffer_queue, &slState);
            if (slRet != SL_RESULT_SUCCESS) {
                ERROR("slBufferQueueItf->GetState() failed\n");
            }
            if(slState.count >= MAX_BUFFER_COUNTS){
                m_condition.wait(lck);
            }
        }


        callback_time = av_gettime_relative();


        int total_size = m_bytes_per_buffer;
        buffer = m_buffers + buffer_index * m_bytes_per_buffer;
        uint8_t* buffer_start = buffer;
        buffer_index = (buffer_index + 1) % MAX_BUFFER_COUNTS;

        while (total_size){
            if(out_offset >= out_size){ //out buffer中的数据已全部读取，重新从frame queue中获取
//                INFO("GetReadableFrame");
                const SkyFrame* audioFrame = m_audio_queue->GetReadableFrame();
//                INFO("GetReadableFrame done");
                if(audioFrame == nullptr || audioFrame->frame == nullptr){
                    INFO("GetReadableFrame failed");
                    if(--failed_count)
                        continue;
                    else{
                        ERROR("try get readable frame enough time");
                        break;
                    }
                }

                AVFrame* frame = audioFrame->frame;

                if(!m_swr_context){
                    InitSwr(frame);
                    INFO("init swr");
                }

                if(!isnan(frame->pts)){
                    current_read_clock = frame->pts * av_q2d(m_audio_timebase) + (double)frame->nb_samples / frame->sample_rate;
                }else{
                    current_read_clock = NAN;
                }

                int out_count = frame->nb_samples + 256;

                int min_size = av_samples_get_buffer_size(nullptr, m_channels,
                                                              out_count,
                                                              m_format, 1);

                av_fast_malloc(&out_buffer, &out_size, min_size);

                int len = swr_convert(m_swr_context, &out_buffer, out_count,
                                      const_cast<const uint8_t **>(frame->extended_data), frame->nb_samples);
                if(len < 0){
                    ERROR("swr_convert failed");
                    m_audio_queue->FlushReadableFrame();
                    return;
                }
                if(len == out_count){
                    WARNING( "audio buffer is probably too small\n");
                    if (swr_init(m_swr_context) < 0)
                        swr_free(&m_swr_context);
                    m_swr_context = nullptr;
                }

                out_size = len * m_bytes_per_frame;
                out_offset = 0;

                m_audio_queue->FlushReadableFrame();
            }

            len = out_size - out_offset;
            if(len > total_size){
                len = total_size;
            }
            memcpy(buffer, out_buffer + out_offset, len);
            buffer += len;
            total_size -= len;
            out_offset += len;

        }

        if(!isnan(current_read_clock)){
//            float latency = 2.0 * m_buffer_capacity / m_bytes_per_sec;
            float latency = 1.0 * m_buffer_capacity / m_bytes_per_sec;  //这个延迟是当前buffer被OpenES SL播放的延迟，也就是MAX_BUFFER_COUNTS个buffer的总播放时间
//            INFO("latency:%f, m_buffer_capacity:%d, m_bytes_per_sec:%d", latency, m_buffer_capacity, m_bytes_per_sec);
            m_clock->SetClockAtTime(current_read_clock - (double)(out_size - out_offset) / m_bytes_per_sec, callback_time/1000000.0 + latency);
        }

        (*m_player_buffer_queue)->Enqueue(m_player_buffer_queue, buffer_start, m_bytes_per_buffer);
//        INFO("Enqueue:%d", frame_count++);

    }

    if(out_buffer){
        av_freep(&out_buffer);
    }

}
int AudioPlayer::InitSwr(AVFrame* frame) {
        if(m_swr_context){
            swr_free(&m_swr_context);
            m_swr_context = nullptr;
        }
        m_swr_context = swr_alloc();

        av_opt_set_int(m_swr_context, "in_channel_layout", frame->channel_layout, 0);
        av_opt_set_int(m_swr_context, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);

        av_opt_set_int(m_swr_context, "in_sample_rate", frame->sample_rate, 0);
        av_opt_set_int(m_swr_context, "out_sample_rate", m_samplerate, 0);

        av_opt_set_sample_fmt(m_swr_context, "in_sample_fmt",
                              static_cast<AVSampleFormat>(frame->format), 0);
        av_opt_set_sample_fmt(m_swr_context, "out_sample_fmt", m_format,  0);

        swr_init(m_swr_context);


        return 0;
}

void AudioPlayer::SetTimebase(const AVRational &timebase) {
    m_audio_timebase = timebase;
    INFO("SetTimebase:%d,%d", m_audio_timebase.den, m_audio_timebase.num);
}
