//
// Created by zou on 2024/9/4.
//

#include "SkyFrameQueue.h"
#include "AndroidLog.h"

void SkyFrameQueue::Init() {
    m_running = false;
    m_read_pos = 0;
    m_write_pos = 0;
    m_live_size = MAX_FRAMES;
    for(auto& frame : m_frame_queue){
        if(!frame.frame){
            frame.frame =  av_frame_alloc();
            if(!frame.frame){
                ERROR("allocate frame failed");
                return;
            }
        }
    }
}


void SkyFrameQueue::Start() {
    m_running = true;
}

void SkyFrameQueue::Pause() {
    m_running = false;
    m_condition_read.notify_one();
    m_condition_write.notify_one();
}

void SkyFrameQueue::Release() {
    m_running = false;
    m_condition_read.notify_one();
    m_condition_write.notify_one();
    for(auto& frame : m_frame_queue){
        if(frame.frame){
            av_frame_unref(frame.frame);
            av_frame_free(&(frame.frame));
            frame.frame = nullptr;
        }
    }
}

SkyFrame *SkyFrameQueue::GetWriteableFrame() {
    std::unique_lock<std::mutex> lck(m_mutex);
    if(m_live_size == 0 && m_running){
        m_condition_write.wait(lck);
    }

    if(!m_running){
        return nullptr;
    }

    return &m_frame_queue[m_write_pos];
}

void SkyFrameQueue::FlushWriteableFrame() {
    std::unique_lock<std::mutex> lck(m_mutex);
    m_write_pos = (++m_write_pos) % MAX_FRAMES;
    --m_live_size;
    m_condition_read.notify_one();

}

const SkyFrame *SkyFrameQueue::GetReadableFrame() {
    std::unique_lock<std::mutex> lck(m_mutex);
    if(m_live_size == MAX_FRAMES && m_running){
        m_condition_read.wait(lck);
    }

    if(!m_running){
        return nullptr;
    }

    return &m_frame_queue[m_read_pos];
}

void SkyFrameQueue::FlushReadableFrame() {
    std::unique_lock<std::mutex> lck(m_mutex);
    av_frame_unref(m_frame_queue[m_read_pos].frame);
    m_read_pos = (++m_read_pos) % MAX_FRAMES;
    ++m_live_size;
    m_condition_write.notify_one();

}

