//
// Created by zou on 2021/2/23.
//

#ifndef MEDIAENGINE_ANDROIDLOG_H
#define MEDIAENGINE_ANDROIDLOG_H

#include <android/log.h>

#define LOG_TAG "SkyMediaPlay"

#define PRINT_LOG
#define ANDROID_PLATFORM

#ifdef PRINT_LOG
    #ifdef ANDROID_PLATFORM
        #define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
        #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
        #define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
        #define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))

    #else
        #define LOGD(fmt, ...) printf(fmt"\n", ##__VA_ARGS__)
        #define LOGI(fmt, ...) printf(fmt"\n", ##__VA_ARGS__)
        #define LOGW(fmt, ...) printf(fmt"\n", ##__VA_ARGS__)
        #define LOGE(fmt, ...) printf(fmt"\n", ##__VA_ARGS__)
    #endif
#else
    #define LOGD(...);
    #define LOGI(...);
    #define LOGW(...);
    #define LOGE(...);
#endif

#define DEBUG(format,...) LOGD("%s:%s(%d), " format,__FILE__, __FUNCTION__,__LINE__, ##__VA_ARGS__)
#define INFO(format,...) LOGI("%s:%s(%d), " format,__FILE__, __FUNCTION__,__LINE__, ##__VA_ARGS__)
#define WARNING(format,...) LOGW("%s:%s(%d), " format,__FILE__, __FUNCTION__,__LINE__, ##__VA_ARGS__)
#define ERROR(format, ...) LOGE("%s:%s(%d), " format,__FILE__,__FUNCTION__, __LINE__, ##__VA_ARGS__)

#define RTC_CHECK(condition, format, ...) \
    (condition) ? static_cast<void>(0) : LOGE("%s:%s(%d), " format,__FILE__,__FUNCTION__, __LINE__, ##__VA_ARGS__)

#endif //MEDIAENGINE_ANDROIDLOG_H
