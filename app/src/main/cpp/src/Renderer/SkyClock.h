//
// Created by zou on 2024/9/23.
//

#ifndef SKYMEDIAPLAYER_SKYCLOCK_H
#define SKYMEDIAPLAYER_SKYCLOCK_H


class SkyClock {
public:
    SkyClock();
    void InitCLock();
    void SetClock(double pts);
    void SetClockAtTime(double pts, double time);
    double GetClock();


private:
    double m_pts_drift = 0;


};


#endif //SKYMEDIAPLAYER_SKYCLOCK_H
