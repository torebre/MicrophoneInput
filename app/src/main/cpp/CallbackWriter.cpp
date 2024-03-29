#include "CallbackWriter.h"

//#include <jni.h>
//#include "../../../../module/lib/x86_64/include/essentia/streaming/streamingalgorithm.h"

#include <oboe/Oboe.h>
#include "../../../../../../projects/oboe/src/common/OboeDebug.h"
#include <essentia/pool.h>
#include <essentia/scheduler/network.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include <essentia/streaming/streamingalgorithm.h>
#include <essentia/streaming/algorithms/ringbufferinput.h>
#include <thread>
#include <essentia/scheduler/network.h>


CallbackWriter::CallbackWriter(JavaVM *jvm, jclass* dataTransferClass) : Algorithm() {
    this->jvm = jvm;
    this->dataTransferClass = dataTransferClass;
//    attachToVmIfNecessary(jvm);

//    this->methodId = env->GetStaticMethodID(this->dataTransferClass, "storeFloat", "(F)V");

    setName("CallbackWriter");
    declareInput(_data, 1, "input", "a vector of real values");
}

//bool CallbackWriter::attachToVmIfNecessary(JavaVM *jvm) {
//    bool attachedThread = false;
//    auto getEnvResult = CallbackWriter::jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
//    if(getEnvResult == JNI_EDETACHED) {
//        if(jvm->AttachCurrentThread(&env, nullptr) == JNI_OK) {
//            attachedThread = true;
//        }
//        else {
//            LOGE("Did not attach thread");
//        }
//    }
//    else if(getEnvResult == JNI_EVERSION) {
//        LOGE("Unsupported JNI version");
//    }
//
//    return attachedThread;
//}


void CallbackWriter::declareParameters() {
    // TODO


}

essentia::streaming::AlgorithmStatus CallbackWriter::process() {
    if (!_data.acquire(1)) {
        return essentia::streaming::NO_INPUT;
    }

    writeToken(_data.firstToken());

    _data.release(1);

    return essentia::streaming::OK;
}

void CallbackWriter::writeToken(const std::vector<essentia::Real> value) {
//    LOGI("Got value: %lu", value.size());

    if (dataTransferClass == nullptr) {
        LOGE("Class not found");
    } else {
        LOGI("Class found");
    }

    if (methodId == nullptr) {
        LOGE("Method not found");
    } else {
        LOGI("Method found");
    }

    auto numberOfValues = value.size();

    // TODO The bytes are in reversed order when they are written here
//                write(fileDescriptor, (const char *) &value, sizeof(Real));

    JNIEnv *env;
    auto getEnvResult = CallbackWriter::jvm->GetEnv(reinterpret_cast<void **>(&env),
                                                    JNI_VERSION_1_6);
    jvm->AttachCurrentThread(&env, nullptr);

//    auto methodId = env->GetStaticMethodID(*dataTransferClass, "storeFloat", "(F)V");
    auto methodId = env->GetStaticMethodID(*dataTransferClass, "storeFloatArray", "([F)V");

    jfloatArray convertedValues = env->NewFloatArray(numberOfValues);
    jfloat tempValues[numberOfValues];

    std::string  valuesText("Values: ");
    for(int i = 0; i < numberOfValues; ++i) {
        tempValues[i] = value[i];
        valuesText = valuesText.append(std::to_string(value[i])).append(",");
    }
    // TODO Just here for debugging
    LOGI("%s", valuesText.c_str());

    env->SetFloatArrayRegion(convertedValues, 0, numberOfValues, tempValues);

// TODO For now just trying to call method and see if value is passed
//    env->CallStaticVoidMethod(*dataTransferClass, methodId, 5.0f); //(float)value.at(0));

    env->CallStaticVoidMethod(*dataTransferClass, methodId, convertedValues);

}
