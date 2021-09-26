#include <jni.h>
#include <string>
#include <essentia/algorithmfactory.h>
#include <essentia/pool.h>

#include "../../../../../../projects/oboe/src/common/OboeDebug.h"
#include "MicrophoneInput.h"
#include "MfccAnalysis.h"


static MfccAnalysis *mfccAnalysis = nullptr;
static JavaVM *javaVm;
static jclass dataTransferClass;

extern "C" {

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
    javaVm = vm;

    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    auto tempClass = env->FindClass(("com/kjipo/microphoneinput/mfcc/DataTransfer"));
    dataTransferClass = (jclass)env->NewGlobalRef(tempClass);



//    if (c == nullptr) return JNI_ERR;

//    static const JNINativeMethod methods[] = {
//            {"nativeFoo", "()V", reinterpret_cast<void*>(nativeFoo)},
//            {"nativeBar", "(Ljava/lang/String;I)Z", reinterpret_cast<void*>(nativeBar)},
//    };
//    int rc = env->RegisterNatives(c, methods, sizeof(methods)/sizeof(JNINativeMethod));
//    if (rc != JNI_OK) return rc;


    return JNI_VERSION_1_6;
}

JNIEXPORT jboolean JNICALL
Java_com_kjipo_microphoneinput_mfcc_MfccLibrary_create(
        JNIEnv *env, jclass) {
    if (mfccAnalysis == nullptr) {

        LOGI("Initializing Essentia");
        essentia::init();

        mfccAnalysis = new MfccAnalysis(javaVm, &dataTransferClass);
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
    if (mfccAnalysis == nullptr) {
        return;
    }
    mfccAnalysis->stop();
}


JNIEXPORT void JNICALL
Java_com_kjipo_microphoneinput_mfcc_MfccLibrary_testCallback(
        JNIEnv *env, jclass) {
    jclass clazz = env->FindClass(("com/kjipo/microphoneinput/mfcc/DataTransfer"));
    jmethodID methodId = env->GetStaticMethodID(clazz, "storeFloat", "(F)V");

    LOGI("Test callback");

    if (methodId == nullptr) {
        LOGE("C++", "Method not found");
    } else {
        env->CallStaticVoidMethod(clazz, methodId, (float) 4.0);
    }
}

}