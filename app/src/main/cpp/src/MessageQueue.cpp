//
// Created by zou on 2024/11/5.
//

#include "MessageQueue.h"

MessageQueue::MessageQueue() {

}

MessageQueue::~MessageQueue() {

}

bool MessageQueue::PushMessage(Message *message) {
    std::unique_lock<std::mutex> lck(m_mutex);

    m_queue.push(message);
    m_condition.notify_one();

    return true;
}

bool MessageQueue::GetMessage(Message** message) {
    if(m_queue.empty()){
        std::unique_lock<std::mutex> lck(m_mutex);
        while (m_queue.empty() && !m_abort_quest){
            m_condition.wait(lck);
        }
    }

    if(m_queue.empty())
        return false;

    *message = m_queue.front();
    m_queue.pop();

    return true;
}

bool MessageQueue::Abort() {
    m_abort_quest = true;
    m_condition.notify_all();
    return true;
}

bool MessageQueue::Start() {
    m_abort_quest = false;
    return true;
}


