#ifndef MICROPHONEINPUT_CALLBACKWRITER_H
#define MICROPHONEINPUT_CALLBACKWRITER_H


#include <jni.h>
#include "../../../../module/lib/x86_64/include/essentia/streaming/streamingalgorithm.h"


class CallbackWriter : public essentia::streaming::Algorithm {


public:
    CallbackWriter(JavaVM *jvm, jclass* dataTransferClass);

    void declareParameters();


protected:
    essentia::streaming::Sink<std::vector<essentia::Real>> _data;

private:
    jmethodID methodId;

    JavaVM *jvm;

//    JNIEnv *env;

    void writeToken(const std::vector<essentia::Real> value);

    essentia::streaming::AlgorithmStatus process();

//    bool attachToVmIfNecessary(JavaVM *jvm);

    jclass* dataTransferClass;
};


#endif //MICROPHONEINPUT_CALLBACKWRITER_H
