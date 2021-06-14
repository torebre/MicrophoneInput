#ifndef MICROPHONEINPUT_MFCCANALYSIS_H
#define MICROPHONEINPUT_MFCCANALYSIS_H


#include <essentia/streaming/streamingalgorithm.h>
#include <essentia/streaming/algorithms/ringbufferinput.h>
#include <oboe/AudioStreamCallback.h>


class MfccAnalysis : public oboe::AudioStreamCallback {

public:
    MfccAnalysis(const int writefd);

    void setupNetwork();

//    void startRecording();


private:

    int writefd;

    essentia::streaming::RingBufferInput *gen;

//    const char *outputFifo;

    const int frameSize = 1024;
    const int hopSize = 512;
    const int32_t sampleRate = 44100;


//    void setupFifo();


};


#endif //MICROPHONEINPUT_MFCCANALYSIS_H
