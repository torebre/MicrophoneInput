#include <jni.h>
#include <string>

//#include <logging_macros.h>
#include "../../../../../../projects/oboe/src/common/OboeDebug.h"
#include "MicrophoneInput.h"


static MicrophoneInput *engine = nullptr;

extern "C" {


JNIEXPORT void JNICALL
Java_com_kjipo_microphoneinput_MicrophoneRecording_startRecording(
        JNIEnv *env, jclass) {
    if (engine == nullptr) {
        LOGE(
                "Engine is null, you must call createEngine before calling this "
                "method");
        return;
    }

    engine->startRecording();
}


JNIEXPORT bool JNICALL
Java_com_kjipo_microphoneinput_MicrophoneRecording_create(
        JNIEnv *env, jclass) {
    if (engine == nullptr) {
        engine = new MicrophoneInput();
    }

    return (engine != nullptr);
}

JNIEXPORT void JNICALL
Java_com_kjipo_microphoneinput_MicrophoneRecording_stop(
        JNIEnv *env, jclass) {
    if (engine == nullptr) {
        return;
    }

    engine->stop();

}


JNIEXPORT void JNICALL
Java_com_kjipo_microphoneinput_MicrophoneRecording_setRecordingDeviceId(
        JNIEnv *env, jclass, jint deviceId) {
    if (engine == nullptr) {
        LOGE(
                "Engine is null, you must call createEngine before calling this "
                "method");
        return;
    }

    engine->setRecordingDeviceId(deviceId);
}


}