#ifndef MICROPHONEINPUT_MFCCANALYSIS_H
#define MICROPHONEINPUT_MFCCANALYSIS_H


#include <essentia/streaming/streamingalgorithm.h>
#include <essentia/streaming/algorithms/ringbufferinput.h>
#include <essentia/pool.h>
#include <oboe/AudioStreamCallback.h>
#include <jni.h>
#include <thread>


class MfccAnalysis : public oboe::AudioStreamCallback {

public:
    MfccAnalysis(JavaVM* jvm, jclass* dataTransferClass);

    void create();

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream,
                                          void *audioData, int32_t numFrames);

    void onErrorBeforeClose(oboe::AudioStream *oboeStream, oboe::Result error);

    void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error);

    void setupNetwork();

    void setupAndRunNetwork();

    void startRecording();

    void setRecordingDeviceId(int32_t deviceId);

    void stop();


private:
    int32_t recordingDeviceId = oboe::kUnspecified;
    int32_t inputChannelCount = oboe::ChannelCount::Mono;
    oboe::AudioStream *recordingStream = nullptr;
    oboe::AudioApi audioApi = oboe::AudioApi::AAudio;
    oboe::AudioFormat format = oboe::AudioFormat::Float;

    void warnIfNotLowLatency(oboe::AudioStream *stream);

    essentia::streaming::RingBufferInput *gen;
    essentia::Pool pool;

    JavaVM* jvm;

    const int frameSize = 1024;
    const int hopSize = 512;
    const int32_t sampleRate = 44100;

    essentia::scheduler::Network *network = NULL;
    std::thread audioProcessor;

    jclass* dataTransferClass;
};


#endif //MICROPHONEINPUT_MFCCANALYSIS_H
