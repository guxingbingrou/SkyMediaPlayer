//
// Created by zou on 2024/9/2.
//

#include "SkyPacketQueue.h"
#include "AndroidLog.h"

void SkyPacketQueue::Start() {
    m_flush_pkt.data = reinterpret_cast<uint8_t *>(&m_flush_pkt);
    m_running = true;
    m_serial = 0;
}

SkyPacketQueue::~SkyPacketQueue() {
    StopAndRelease();
}

void SkyPacketQueue::StopAndRelease() {
    std::unique_lock<std::mutex> lck(m_mutex);
    m_running = false;
    m_condition_pop.notify_one();
    while (!m_queue.empty()) {
        av_packet_unref(&m_queue.front().packet);
        m_queue.pop();
    }
}

bool SkyPacketQueue::QueuePacket(AVPacket *packet) {
    if (!m_running)
        return false;
    std::unique_lock<std::mutex> lck(m_mutex);

    if (!m_running)
        return false;

    m_queue.push(SkyPacket{m_serial, *packet});

    av_packet_ref(&m_queue.back().packet, packet);
    if (packet == &m_flush_pkt)
        ++m_serial;
    m_queue.back().serial = m_serial;

    m_condition_pop.notify_one();
    return true;

}

bool SkyPacketQueue::DequeuePacket(AVPacket *packet, int *serial) {
    if (!m_running)
        return false;
    std::unique_lock<std::mutex> lck(m_mutex);
    if (m_queue.empty()) {
        m_condition_pop.wait(lck);
    }

    if (!m_running)
        return false;

    if (!m_queue.empty()) {
        *packet = m_queue.front().packet;
        *serial = m_queue.front().serial;
        m_queue.pop();
        return true;
    }
    return false;
}

void SkyPacketQueue::Clear() {

    std::unique_lock<std::mutex> lck(m_mutex);
    while (!m_queue.empty()) {
        av_packet_unref(&m_queue.front().packet);
        m_queue.pop();
    }

}

void SkyPacketQueue::FlushPacket() {
    QueuePacket(&m_flush_pkt);
}

bool SkyPacketQueue::IsFlushPacket(AVPacket *packet) {
    if (!packet)
        return false;
    return packet->data == m_flush_pkt.data;
}
