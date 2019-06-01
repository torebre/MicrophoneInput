#ifndef MICROPHONEINPUT_MICROPHONEINPUT_H
#define MICROPHONEINPUT_MICROPHONEINPUT_H

#include <oboe/Oboe.h>
//#include <fstream>
//#include <iostream>



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


private:
    int32_t recordingDeviceId = oboe::kUnspecified;
    // TODO Set sample rate not using constant here
    int32_t sampleRate = 48000;
    int32_t inputChannelCount = oboe::ChannelCount::Mono;
    oboe::AudioStream *recordingStream = nullptr;
    oboe::AudioApi audioApi = oboe::AudioApi::AAudio;
    oboe::AudioFormat format = oboe::AudioFormat::I16;

    const char *pipeFile = "/data/data/com.kjipo.microphoneinput/record_pipe";


//    std::ofstream *streamFile = nullptr;

    int writefd;


    void warnIfNotLowLatency(oboe::AudioStream *stream);



};



#endif //MICROPHONEINPUT_MICROPHONEINPUT_H