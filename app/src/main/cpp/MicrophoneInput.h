#ifndef MICROPHONEINPUT_MICROPHONEINPUT_H
#define MICROPHONEINPUT_MICROPHONEINPUT_H

#include <oboe/Oboe.h>
//#include <fstream>
//#include <iostream>
#include <essentia/pool.h>
#include <essentia/scheduler/network.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include <essentia/streaming/streamingalgorithm.h>
#include <essentia/streaming/algorithms/ringbufferinput.h>
#include <thread>
#include <essentia/scheduler/network.h>


class MicrophoneInput : public oboe::AudioStreamCallback {

public:
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

    //    int32_t sampleRate = 48000;
    int32_t inputChannelCount = oboe::ChannelCount::Mono;
    oboe::AudioStream *recordingStream = nullptr;
    oboe::AudioApi audioApi = oboe::AudioApi::AAudio;
    oboe::AudioFormat format = oboe::AudioFormat::Float;

    // TODO Take this as input from Java side
    const char *pipeFile = "/data/data/com.kjipo.microphoneinput/record_pipe";

    int writefd;


    void warnIfNotLowLatency(oboe::AudioStream *stream);

    essentia::streaming::RingBufferInput *gen;
    essentia::Pool pool;

    const int frameSize = 1024;
    const int hopSize = 512;

    essentia::scheduler::Network *network = NULL;
    std::thread audioProcessor;

//    uint framesize = 4096;
//    uint hopsize = 2048;
//    uint zeropadding = 0;

    void runNetwork();


};



#endif //MICROPHONEINPUT_MICROPHONEINPUT_H