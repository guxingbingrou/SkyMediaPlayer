//
// Created by zou on 2024/9/23.
//

#include "SkyClock.h"
#include "AndroidLog.h"
extern "C"{
#include "libavutil/time.h"
}

void SkyClock::InitCLock() {
    m_pts_drift = 0;
}

void SkyClock::SetClockAtTime(double pts, double time) {
    m_pts_drift = pts - time;
}

void SkyClock::SetClock(double pts) {
    double time = av_gettime_relative() / 1000000.0;
    SetClockAtTime(pts, time);
}

double SkyClock::GetClock() {
    if(m_pts_drift == 0) return 0;
    double time = av_gettime_relative() / 1000000.0;
    return m_pts_drift + time;
}

SkyClock::SkyClock() {
    InitCLock();

}


