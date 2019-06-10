#ifndef MICROPHONEINPUT_MICROPHONEINPUT_H
#define MICROPHONEINPUT_MICROPHONEINPUT_H

#include <oboe/Oboe.h>
#include <essentia/pool.h>
#include <essentia/scheduler/network.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include <essentia/streaming/streamingalgorithm.h>
#include <essentia/streaming/algorithms/ringbufferinput.h>
#include <thread>
#include <essentia/scheduler/network.h>


#include <iostream>
#include <fstream>


class MicrophoneInput : public oboe::AudioStreamCallback {

public:
    MicrophoneInput(const char* pitchFifo, const char* pitchConfidenceFifo);

    ~MicrophoneInput();

    void create();
    void startRecording();
    void stop();
    void setRecordingDeviceId(int32_t deviceId);

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream,
                                          void *audioData, int32_t numFrames);
    void onErrorBeforeClose(oboe::AudioStream *oboeStream, oboe::Result error);
    void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error);

    void setupNetwork();


private:
    int32_t recordingDeviceId = oboe::kUnspecified;
    int32_t inputChannelCount = oboe::ChannelCount::Mono;
    oboe::AudioStream *recordingStream = nullptr;
    oboe::AudioApi audioApi = oboe::AudioApi::AAudio;
    oboe::AudioFormat format = oboe::AudioFormat::Float;

    const char* pitchFifo;
    const char* pitchConfidenceFifo;

    int writefd;
    int writeConfidenceFifo;

    std::ofstream outputFile;


    void warnIfNotLowLatency(oboe::AudioStream *stream);

    essentia::streaming::RingBufferInput *gen;
    essentia::Pool pool;

    const int frameSize = 1024;
    const int hopSize = 512;
//    const int windowSize = 1024;
    const int32_t sampleRate = 44100;

    essentia::scheduler::Network *network = NULL;
    std::thread audioProcessor;

    void runNetwork();


};



#endif //MICROPHONEINPUT_MICROPHONEINPUT_H