//
// Created by zou on 2024/9/20.
//

#ifndef SKYMEDIAPLAYER_NATIVEOBSERVER_H
#define SKYMEDIAPLAYER_NATIVEOBSERVER_H

#include "MediaPLayerObserver.h"
#include "jvm.h"
class NativeObserver : public MediaPlayerObserver{
public:
    NativeObserver(jobject jobserver);
    void OnSizeChanged(int width, int height) override;


private:
    jobject m_java_observer = nullptr;
    jmethodID m_methodOnResolutionChanged = nullptr;
};


#endif //SKYMEDIAPLAYER_NATIVEOBSERVER_H
