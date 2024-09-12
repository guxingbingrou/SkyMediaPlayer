//
// Created by zou on 2024/9/2.
//

#include "SkyPacketQueue.h"
#include "AndroidLog.h"

void SkyPacketQueue::Start() {
    m_running = true;
}

SkyPacketQueue::~SkyPacketQueue() {
    StopAndRelease();
}

void SkyPacketQueue::Pause() {
    m_running = false;
    m_condition_push.notify_one();
    m_condition_pop.notify_one();
}

void SkyPacketQueue::StopAndRelease() {
    m_running = false;
    m_condition_push.notify_one();
    m_condition_pop.notify_one();
    while (!m_queue.empty()){
        m_queue.pop();
    }
}

bool SkyPacketQueue::QueuePacket(AVPacket* packet) {
    if (!m_running)
        return false;
    std::unique_lock<std::mutex> lck(m_mutex);
    if (m_queue.size() == MAX_PACKETS) {
//        INFO("m_condition_push.wait(lck)");
        m_condition_push.wait(lck);
//        INFO("m_condition_push.wait(lck) done");
    }

    if (!m_running)
        return false;

//    INFO("m_queue.size() : %d, %x", m_queue.size(), this);
    if (m_queue.size() < MAX_PACKETS) {
        m_queue.push(*packet);
        av_packet_ref(&m_queue.back(), packet);
        m_condition_pop.notify_one();
//        INFO("success m_queue.size() : %d", m_queue.size());
        return true;
    }
//    INFO("failed");
    return false;
}

bool SkyPacketQueue::DequeuePacket(AVPacket* packet) {
    if (!m_running)
        return false;
    std::unique_lock<std::mutex> lck(m_mutex);
    if (m_queue.empty()) {
        m_condition_pop.wait(lck);
    }

    if (!m_running)
        return false;

    if (!m_queue.empty()) {
        *packet = m_queue.front();
        m_queue.pop();
        m_condition_push.notify_one();
//        INFO("m_condition_push.notify_one()");
        return true;
    }
    return false;
}
