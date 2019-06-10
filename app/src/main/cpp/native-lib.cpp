#include <jni.h>
#include <string>
#include <essentia/algorithmfactory.h>
#include <essentia/pool.h>

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
        JNIEnv *env, jclass, jstring pitchFifo, jstring pitchConfidenceFifo) {
    if (engine == nullptr) {
        engine = new MicrophoneInput(env->GetStringUTFChars(pitchFifo, nullptr),
                                     env->GetStringUTFChars(pitchConfidenceFifo, nullptr));
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


void
Java_com_kjipo_microphoneinput_MicrophoneRecording_initializeEssentia(JNIEnv *env, jclass clazz) {
    // Parameters
    LOGI("C++", "Initializing Essentia");

    essentia::init();

    essentia::Pool pool;


}


}