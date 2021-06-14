#include <cstdint>
#include <oboe/AudioStream.h>
#include <essentia/scheduler/network.h>
#include <thread>
#include <essentia/streaming/algorithms/ringbufferinput.h>

#ifndef MICROPHONEINPUT_RECORDER_H
#define MICROPHONEINPUT_RECORDER_H

#endif //MICROPHONEINPUT_RECORDER_H


class Recorder : public oboe::AudioStreamCallback {

public:

    Recorder();

    ~Recorder();

    void create();

    void startRecording();

    void stop();

    void setRecordingDeviceId(int32_t deviceId);

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream,
                                          void *audioData, int32_t numFrames);

    void onErrorBeforeClose(oboe::AudioStream *oboeStream, oboe::Result error);

    void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error);


private:
    int32_t recordingDeviceId = oboe::kUnspecified;
    int32_t inputChannelCount = oboe::ChannelCount::Mono;
    oboe::AudioStream *recordingStream = nullptr;
    oboe::AudioApi audioApi = oboe::AudioApi::AAudio;
    oboe::AudioFormat format = oboe::AudioFormat::Float;

    const int frameSize = 1024;
    const int hopSize = 512;
    const int32_t sampleRate = 44100;

    void warnIfNotLowLatency(oboe::AudioStream *stream);

    essentia::streaming::RingBufferInput *gen;

    essentia::scheduler::Network *network = NULL;
    std::thread audioProcessor;

};