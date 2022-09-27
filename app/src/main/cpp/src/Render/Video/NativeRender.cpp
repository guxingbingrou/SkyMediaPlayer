//
// Created by zou on 2022/9/20.
//

#include "NativeRender.h"
#include "utils/AndroidLog.h"
#include "libyuv/libyuv.h"

NativeRender::NativeRender() {

}

NativeRender::~NativeRender() {
    DestroyRender();
}

void NativeRender::InitRender(int targetWidth, int targetHeight, JNIEnv* env, jobject surface) {
    m_native_window = ANativeWindow_fromSurface(env, surface);

    if(!m_native_window){
        ERROR("native window is null");
        return;
    }

    int window_width = ANativeWindow_getWidth(m_native_window);
    int window_height = ANativeWindow_getHeight(m_native_window);

//    if(targetWidth <= window_width && targetHeight <= window_height){
        m_width = targetWidth;
        m_height = targetHeight;
//    }
//    else if(window_width < window_height * targetWidth / targetHeight){
//        m_width = window_width;
//        m_height = m_width * targetHeight / targetWidth;
//    }else{
//        m_height = window_height;
//        m_width = m_height * targetWidth / targetHeight;
//    }

    ANativeWindow_setBuffersGeometry(m_native_window, m_width, m_height, WINDOW_FORMAT_RGBA_8888);

    INFO("ANativeWindow_setBuffersGeometry: w x h: %d, %d", m_width, m_height);
}

void NativeRender::OnDecodedFrame(const FrameParams *frame) {
    if(!m_native_window || !frame){
        ERROR("native window or frame is null");
        return;
    }

    ANativeWindow_lock(m_native_window, &m_native_buffer, nullptr);

    uint8_t* dstBuffer = static_cast<uint8_t *>(m_native_buffer.bits);

    if(frame->format == IMAGE_FORMAT_I420){
        libyuv::I420ToABGR(frame->video_raw_data[0], frame->video_raw_linesize[0],
                frame->video_raw_data[1],  frame->video_raw_linesize[1],
                frame->video_raw_data[2], frame->video_raw_linesize[2],
                dstBuffer, m_native_buffer.stride*4, m_width, m_height);
    }else if(frame->format == IMAGE_FORMAT_J420){
        libyuv::J420ToABGR(frame->video_raw_data[0], frame->video_raw_linesize[0],
                           frame->video_raw_data[1],  frame->video_raw_linesize[1],
                           frame->video_raw_data[2], frame->video_raw_linesize[2],
                           dstBuffer, m_native_buffer.stride*4, m_width, m_height);
    }else if(frame->format == IMAGE_FORMAT_NV12){
        libyuv::NV12ToABGR(frame->video_raw_data[0], frame->video_raw_linesize[0],
                           frame->video_raw_data[1],  frame->video_raw_linesize[1],
                           dstBuffer, m_native_buffer.stride*4, m_width, m_height);
    }else if(frame->format == IMAGE_FORMAT_NV21){
        libyuv::NV21ToABGR(frame->video_raw_data[0], frame->video_raw_linesize[0],
                           frame->video_raw_data[1],  frame->video_raw_linesize[1],
                           dstBuffer, m_native_buffer.stride*4, m_width, m_height);
    }else if(frame->format == IMAGE_FORMAT_RGBA){
        libyuv::RGBAToARGB(frame->video_raw_data[0], frame->video_raw_linesize[0],
                           dstBuffer, m_native_buffer.stride*4, m_width, m_height);
    }else{
        ERROR("unknown format(%d)", frame->format);
    }



    ANativeWindow_unlockAndPost(m_native_window);
}


void NativeRender::DestroyRender() {

}

