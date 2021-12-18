#ifndef MICROPHONEINPUT_LOGGEROUTPUTVECTOR_H
#define MICROPHONEINPUT_LOGGEROUTPUTVECTOR_H


#include <jni.h>
#include "../../../../module/lib/x86_64/include/essentia/streaming/streamingalgorithm.h"


class LoggerOutputVector : public essentia::streaming::Algorithm {


public:
    LoggerOutputVector();

    void declareParameters();


protected:
    essentia::streaming::Sink<std::vector<essentia::Real>> _data;

private:

    void writeToken(const std::vector<essentia::Real> value);

    essentia::streaming::AlgorithmStatus process();

};


#endif //MICROPHONEINPUT_LOGGEROUTPUTVECTOR_H
