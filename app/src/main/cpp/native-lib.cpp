#include <jni.h>
#include <string>
#include <media/NdkMediaCodec.h>

int test(){
    AMediaCodec_createCodecByName("video/avc");
}