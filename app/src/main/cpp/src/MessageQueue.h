//
// Created by zou on 2024/11/5.
//

#ifndef SKYMEDIAPLAYER_MESSAGEQUEUE_H
#define SKYMEDIAPLAYER_MESSAGEQUEUE_H
#include <queue>
#include <mutex>
#include <condition_variable>

using Free_Obj = void(*)(void*);

constexpr static int MEDIA_NOP = 0; // interface test message
constexpr static int MEDIA_PREPARED = 1;
constexpr static int MEDIA_PLAYBACK_COMPLETE = 2;
constexpr static int MEDIA_BUFFERING_UPDATE = 3;
constexpr static int MEDIA_SEEK_COMPLETE = 4;
constexpr static int MEDIA_SET_VIDEO_SIZE = 5;
constexpr static int MEDIA_TIMED_TEXT = 99;
constexpr static int MEDIA_ERROR = 100;
constexpr static int MEDIA_INFO = 200;
constexpr static int MEDIA_SET_VIDEO_SAR = 10001;

struct Message{
    Message(int what, int arg1 = -1, int arg2 = -1, void* obj = nullptr, Free_Obj free_obj = nullptr)
        : m_what(what),
        m_arg1(arg1),
        m_arg2(arg2),
        m_obj(obj),
        m_free_obj(free_obj){}

    ~Message(){
        if(m_obj && m_free_obj){
            m_free_obj(m_obj);
        }
    }
    int m_what;
    int m_arg1;
    int m_arg2;
    void* m_obj;
    Free_Obj m_free_obj;
};

class MessageQueue {
public:
    MessageQueue();
    ~MessageQueue();

    bool Start();
    bool PushMessage(Message* message);
    bool GetMessage(Message** message);
    bool Abort();
private:
    std::queue<Message*> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_abort_quest = false;
};


#endif //SKYMEDIAPLAYER_MESSAGEQUEUE_H
