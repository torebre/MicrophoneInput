#include <jni.h>
#include <string>
#include <essentia/algorithmfactory.h>
#include <essentia/pool.h>

#include "../../../../../../projects/oboe/src/common/OboeDebug.h"
#include "MicrophoneInput.h"
#include "MfccAnalysis.h"


static MfccAnalysis *mfccAnalysis = nullptr;

extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_kjipo_microphoneinput_mfcc_MfccLibrary_create(
        JNIEnv *env, jclass) {
    if (mfccAnalysis == nullptr) {

        LOGI("C++", "Initializing Essentia");
        essentia::init();

        mfccAnalysis = new MfccAnalysis(env);
    }

    return (mfccAnalysis != nullptr);
}



JNIEXPORT void JNICALL
Java_com_kjipo_microphoneinput_mfcc_MfccLibrary_setRecordingDeviceId(
        JNIEnv *env, jclass, jint deviceId) {
    if (mfccAnalysis == nullptr) {
        LOGE("MfccAnalysis not created");
        return;
    }

    mfccAnalysis->setRecordingDeviceId(deviceId);
}


JNIEXPORT void JNICALL
Java_com_kjipo_microphoneinput_mfcc_MfccLibrary_start(
        JNIEnv *env, jclass) {
    LOGI("Calling Mfcc start");

    if (mfccAnalysis == nullptr) {
        LOGE("MfccAnalysis not created");
    }

    mfccAnalysis->startRecording();
}

JNIEXPORT void JNICALL
Java_com_kjipo_microphoneinput_mfcc_MfccLibrary_stop(
        JNIEnv *env, jclass) {

    // TODO


}


JNIEXPORT void JNICALL
Java_com_kjipo_microphoneinput_mfcc_MfccLibrary_testCallback(
        JNIEnv *env, jclass) {
    jclass clazz = env->FindClass(("com/kjipo/microphoneinput/mfcc/DataTransfer"));
    jmethodID methodId = env->GetStaticMethodID(clazz, "storeFloat", "(F)V");

    if (methodId == NULL) {
        LOGE("C++", "Method not found");
    } else {
        env->CallStaticVoidMethod(clazz, methodId, (float)4.0);
    }
}

}