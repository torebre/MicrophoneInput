#include "CallbackWriter.h"

//#include <jni.h>
//#include "../../../../module/lib/x86_64/include/essentia/streaming/streamingalgorithm.h"

#include <oboe/Oboe.h>
#include <essentia/pool.h>
#include <essentia/scheduler/network.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include <essentia/streaming/streamingalgorithm.h>
#include <essentia/streaming/algorithms/ringbufferinput.h>
#include <thread>
#include <essentia/scheduler/network.h>


CallbackWriter::CallbackWriter(JNIEnv *env) {
    this->env = env;
    this->clazz = env->FindClass(("com/kjipo/microphoneinput/mfcc/DataTransfer"));
    this->methodId = env->GetStaticMethodID(clazz, "storeFloat", "(F)V");
}


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
//                LOGI("Got value: %f", value);


    // TODO The bytes are in reversed order when they are written here
//                write(fileDescriptor, (const char *) &value, sizeof(Real));

// TODO For now just trying to call method and see if value is passed
    env->CallStaticVoidMethod(clazz, methodId, (float)value.at(0));



}
