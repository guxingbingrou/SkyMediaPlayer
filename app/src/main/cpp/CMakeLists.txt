# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("skymediaplayer")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

INCLUDE_DIRECTORIES(
        ${PROJECT_SOURCE_DIR}/includes
        ${PROJECT_SOURCE_DIR}/includes/ffmpeg
        ${PROJECT_SOURCE_DIR}/includes/libyuv
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_BINARY_DIR}
)
set(LIBRARY_DIR ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI})
find_library(log-lib log)

add_library(libyuv SHARED IMPORTED)
set_target_properties(libyuv PROPERTIES IMPORTED_LOCATION
        ${LIBRARY_DIR}/libyuv/libyuv.so)

set(FFMPEG_DIR ${LIBRARY_DIR}/ffmpeg)

add_library(avcodec SHARED IMPORTED)
set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_DIR}/libavcodec.so)

#add_library(avdevice SHARED IMPORTED)
#set_target_properties(avdevice PROPERTIES IMPORTED_LOCATION
#        ${FFMPEG_DIR}/libavdevice.so)

add_library(avfilter SHARED IMPORTED)
set_target_properties(avfilter PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_DIR}/libavfilter.so)

add_library(avformat SHARED IMPORTED)
set_target_properties(avformat PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_DIR}/libavformat.so)

add_library(avutil SHARED IMPORTED)
set_target_properties(avutil PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_DIR}/libavutil.so)

add_library(swscale SHARED IMPORTED)
set_target_properties(swscale PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_DIR}/libswscale.so)

add_library(swresample SHARED IMPORTED)
set_target_properties(swresample PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_DIR}/libswresample.so)

add_library(x264 SHARED IMPORTED)
set_target_properties(x264 PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_DIR}/libx264.so)

add_library(fdk-aac SHARED IMPORTED)
set_target_properties(fdk-aac PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_DIR}/libfdk-aac.so)

set(FFMPEG
        avcodec
        #        avdevice
        avfilter
        avformat
        avutil
        swscale
        swresample
        fdk-aac
        x264)

add_library(SkyMediaEngine
        SHARED
        src/Decoder/DemuxThread.cpp
        src/Decoder/FFMediaDecoder.cpp
        src/Decoder/MediaCodecDecoder.cpp
        src/Decoder/MediaDecoderFactory.cpp
        src/Renderer/RenderThread.cpp
        src/Renderer/AudioPlayer.cpp
        src/Renderer/VideoRenderer.cpp
        src/Renderer/GLUtils.cpp
        src/Decoder/SkyPacketQueue.cpp
        src/SkyFrameQueue.cpp
        src/SkyMediaPlayer.cpp
        src/Renderer/SkyClock.cpp
        src/MessageQueue.cpp

        )

target_link_libraries(SkyMediaEngine
        ${log-lib}
        ${FFMPEG}
        libyuv
        android
        OpenSLES
        GLESv3
        EGL)



add_library( # Sets the name of the library.
        SkyMediaPlayer

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        src/jni/jni_onload.cc
        src/jni/jvm.cc
        src/jni/native-lib.cpp
        src/jni/MediaPlayerProxy.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.



# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        SkyMediaPlayer

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        SkyMediaEngine
        ${FFMPEG}
        libyuv
        android
        OpenSLES
        GLESv3
        EGL)