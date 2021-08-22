#ifndef MICROPHONEINPUT_CALLBACKWRITER_H
#define MICROPHONEINPUT_CALLBACKWRITER_H


#include <jni.h>
#include "../../../../module/lib/x86_64/include/essentia/streaming/streamingalgorithm.h"

//namespace essentia {
//    namespace streaming {

class CallbackWriter : public essentia::streaming::Algorithm {


public:
    CallbackWriter(JNIEnv *env);


protected:
    essentia::streaming::Sink <std::vector<essentia::Real>> _data;

private:
    JNIEnv *env;

    jclass clazz;
    jmethodID methodId;

    void writeToken(const std::vector<essentia::Real> value);

    essentia::streaming::AlgorithmStatus process();

    void declareParameters();
};

//    }
//}


#endif //MICROPHONEINPUT_CALLBACKWRITER_H
