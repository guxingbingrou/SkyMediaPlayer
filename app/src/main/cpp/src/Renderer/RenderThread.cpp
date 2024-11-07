//
// Created by zou on 2024/8/29.
//

#include "RenderThread.h"
#include "AndroidLog.h"
extern "C"{
#include "libavutil/time.h"
}

void RenderThread::Init(ANativeWindow *native_window,
                        const std::shared_ptr<SkyFrameQueue> &audioFrameQueue,
                        const std::shared_ptr<SkyFrameQueue> &videoFrameQueue,
                        const std::shared_ptr<SkyFrameQueue> &subtitleFrameQueue) {
    m_audio_queue = audioFrameQueue;
    m_video_queue = videoFrameQueue;
    m_subtitle_queue = subtitleFrameQueue;

    m_video_clock = std::make_shared<SkyClock>();
    m_audio_clock = std::make_shared<SkyClock>();

    m_video_renderer = std::make_unique<VideoRenderer>();
    m_video_renderer->SetWindow(native_window);

    m_audio_player = std::make_unique<AudioPlayer>();
    m_audio_player->Init(audioFrameQueue, m_audio_clock);
    m_audio_player->SetTimebase(m_audio_timebase);


}

void RenderThread::SetTimebase(const AVMediaType& mediaType, const AVRational& timebase){
    if(mediaType == AVMEDIA_TYPE_AUDIO){
        m_audio_timebase = timebase;
        if(m_audio_player)
            m_audio_player->SetTimebase(timebase);
    }else if(mediaType == AVMEDIA_TYPE_VIDEO){
        m_video_timebase = timebase;
    }else if(mediaType == AVMEDIA_TYPE_SUBTITLE){
        m_subtitle_timebase = timebase;
    }
}

bool RenderThread::Start() {
    m_audio_player->Start();
    if(m_running){
        m_paused = false;
        return true;
    }

    m_running = true;
    m_thread = std::thread(&RenderThread::Loop, this);
    return false;
}

bool RenderThread::Pause() {
    if(!m_running) return false;
    m_audio_player->Pause();
    m_paused = true;
    return true;
}

bool RenderThread::Stop() {
    return false;
}

bool RenderThread::Release() {
    m_running = false;
    if(m_thread.joinable())
        m_thread.join();

    m_audio_player->Destroy();

    INFO("Release");
    return false;
}

void RenderThread::Loop() {
    if(!m_video_renderer->Init()){
        ERROR("video renderer init failed");
        return;
    }
    INFO("video render init success");

    double remaining_time = 0.0;
    SkyFrame lastFrame;


    double last_duration = 0.0;
    double delay = 0.0;
    double frame_timer = NAN;

    while (m_running){
        if(remaining_time > 0.0){
            av_usleep(remaining_time * 1000000.0);
        }
        remaining_time = REFRESH_RATE;

        if(lastFrame.frame == nullptr && m_video_queue->Empty()){
            continue;
        }

        do {
            if(m_video_queue->Empty()){
//                DEBUG("m_video_queue->Empty()");
                break;
            }else{
                const SkyFrame* skyFrame = m_video_queue->GetReadableFrame();
                if(skyFrame == nullptr || skyFrame->frame == nullptr){
                    WARNING("GetReadableFrame failed");
                    continue;
                }

                if(lastFrame.frame == nullptr){
                    lastFrame.frame = av_frame_alloc();
                    av_frame_ref(lastFrame.frame, skyFrame->frame);
                    m_video_queue->FlushReadableFrame();
//                    INFO("lastFrame == nullptr");
                    break;
                }

                if(m_paused)
                    break;

                AVFrame* avFrame = skyFrame->frame;
                AVFrame* lastAvFrame = lastFrame.frame;

                last_duration = avFrame->pts * av_q2d(m_video_timebase) - lastAvFrame->pts * av_q2d(m_video_timebase);
                if(isnan(last_duration) || last_duration <=0 || last_duration > MAX_DURATION){
                    last_duration = lastAvFrame->pts * av_q2d(m_video_timebase);
                }

//                INFO("avFrame->pts: %d, lastAvFrame->pts: %d, last_duration:%f, av_q2d(avFrame->time_base):%f",
//                     avFrame->pts, lastAvFrame->pts, last_duration, av_q2d(m_video_timebase));

                double diff, sync_threshold;
                diff = m_video_clock->GetClock() - m_audio_clock->GetClock();

                sync_threshold = FFMAX(SYNC_THRESHOLD_MIN, FFMIN(last_duration, SYNC_THRESHOLD_MAX));
                if(!isnan(diff) && fabs(diff) < MAX_DURATION){
                    if(diff <= -sync_threshold){   //音频过快
                        delay = FFMAX(0, last_duration + diff);
                    }else if(diff >= sync_threshold){ //视频过快
                        if(last_duration > SYNC_FRAMEDUP_THRESHOLD){  //视频间隔时间过长
                            delay = last_duration + diff;
                        }else{
                            delay = 2 * last_duration;
                        }
                    }else{
                        delay = last_duration;
                    }
                }

//                delay = last_duration;
//                INFO("diff:%f, delay:%f, last_duration:%f, sync_threshold:%f ", diff, delay, last_duration, sync_threshold);

                double time = av_gettime_relative() / 1000000.0;
                if(isnan(frame_timer) || frame_timer > time){
                    frame_timer = time;
                }

                if(time < frame_timer + delay){ //当前时间在上一帧播放时间内，可以播放
                    remaining_time = FFMIN(frame_timer + delay - time, remaining_time);
//                    INFO("display frame_timer: %f, time:%f, delay:%f", frame_timer, time, delay);
//                    INFO("remaining_time: %f, delay:%f", remaining_time, delay);
                    break;
                }else{  //当前时间已经超过上一帧播放区间，需要播放新帧
//                    INFO("wait frame_timer: %f, time:%f, delay:%f", frame_timer, time, delay);
                    frame_timer += delay;
                    if(delay >0 && time - frame_timer > SYNC_THRESHOLD_MAX){
                        frame_timer = time;
                    }

                    //更新视频时钟
                    m_video_clock->SetClock(avFrame->pts * av_q2d(m_video_timebase));

                    av_frame_unref(lastFrame.frame);
                    av_frame_ref(lastFrame.frame, skyFrame->frame);
                    m_video_queue->FlushReadableFrame();
                    continue;  //再次计算当前帧是否可播放或跳过播放

                }

            }

        } while (m_running);

        if(lastFrame.frame){
//            INFO("RenderPicture");
            m_video_renderer->RenderPicture(&lastFrame);
        }

    }

    m_video_renderer->Release();

    if(lastFrame.frame){
        av_frame_unref(lastFrame.frame);
        av_frame_free(&lastFrame.frame);
    }

}

int RenderThread::GetCurrentTime() {
    if(m_audio_player)
        return m_audio_player->GetCurrentTime(); //us
    else
        return m_video_clock->GetClock() * 1000000;

    return 0;
}
