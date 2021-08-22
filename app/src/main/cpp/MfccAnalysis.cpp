#include "MfccAnalysis.h"


#include <oboe/Oboe.h>
#include <essentia/pool.h>
#include <essentia/scheduler/network.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include <essentia/streaming/streamingalgorithm.h>
#include <essentia/streaming/algorithms/ringbufferinput.h>
#include <thread>
#include <essentia/scheduler/network.h>

#include "../../../../../../projects/oboe/src/common/OboeDebug.h"

#include <iostream>
#include <fstream>
#include <essentia/algorithmfactory.h>

#include "CallbackWriter.h"


using namespace oboe;


MfccAnalysis::MfccAnalysis(JNIEnv *env) {
    this->env = env;

    setupNetwork();
}

void MfccAnalysis::create() {
    AudioStreamBuilder builder;
    builder.setDirection(Direction::Input);
    builder.setPerformanceMode(PerformanceMode::LowLatency);

    AudioStream *stream;
    Result r = builder.openStream(&stream);

    if (r != Result::OK) {
        LOGE("Error opening stream: %s", convertToText(r));
    }

    r = stream->requestStart();
    if (r != Result::OK) {
        LOGE("Error starting stream: %s", convertToText(r));
    }


}

void MfccAnalysis::startRecording() {
    LOGI("Starting recording");

    AudioStreamBuilder builder;

    builder.setDeviceId(recordingDeviceId)
    ->setDirection(oboe::Direction::Input)
    ->setSampleRate(sampleRate)
    ->setChannelCount(inputChannelCount)
    ->setAudioApi(audioApi)
    ->setFormat(format)
    ->setSharingMode(oboe::SharingMode::Exclusive)
    ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
    ->setCallback(this);

    oboe::Result result = builder.openStream(&recordingStream);

    if (result == oboe::Result::OK && recordingStream) {
        assert(recordingStream->getChannelCount() == inputChannelCount);
        assert(recordingStream->getFormat() == oboe::AudioFormat::Float);
        assert(recordingStream->getSampleRate() == sampleRate);

        warnIfNotLowLatency(recordingStream);

        setupNetwork();
        network = new essentia::scheduler::Network(gen);


// TODO Is it necessary to run this here?
        audioProcessor = std::thread(&MfccAnalysis::runNetwork, this);

        recordingStream->requestStart();
    } else {
        LOGE("Failed to create recording stream. Error: %s",
             oboe::convertToText(result));
    }

}


void MfccAnalysis::runNetwork() {
    LOGI("Running network");
    network->run();
    LOGI("Network stopped");
}


void MfccAnalysis::setRecordingDeviceId(int32_t deviceId) {
    recordingDeviceId = deviceId;
}


void MfccAnalysis::warnIfNotLowLatency(oboe::AudioStream *stream) {
    if (stream->getPerformanceMode() != oboe::PerformanceMode::LowLatency) {
        LOGW(
                "Stream is NOT low latency."
                "Check your requested format, sample rate and channel count");
    }
}

void MfccAnalysis::setupNetwork() {
    essentia::streaming::AlgorithmFactory &factory = essentia::streaming::AlgorithmFactory::instance();

    auto gen = new essentia::streaming::RingBufferInput();
    gen->declareParameters();
    gen->configure();

    gen->output(0).setAcquireSize(frameSize);
    gen->output(0).setReleaseSize(frameSize);

    essentia::streaming::Algorithm *fc = factory.create("FrameCutter",
                                                        "frameSize", frameSize,
                                                        "hopSize", hopSize,
                                                        "startFromZero", true);
    essentia::streaming::Algorithm *w = factory.create("Windowing",
                                                       "type", "blackmanharris62");
    essentia::streaming::Algorithm *spec = factory.create("Spectrum");
    essentia::streaming::Algorithm *mfcc = factory.create("MFCC");


    spec->output("spectrum") >> mfcc->input("spectrum");
    mfcc->output("bands")
            >> essentia::streaming::NOWHERE;

// TODO Write to a buffer

//    auto *outputWriter = new essentia::streaming::FileDescriptorVectorOutput(writefd);

//auto *outputWriter = new essentia::streaming::CallbackWriter(env);

//    essentia::streaming::connect(mfcc->output("mfcc"), outputWriter->input("data"));

}




oboe::DataCallbackResult MfccAnalysis::onAudioReady(
        oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    assert(oboeStream == recordingStream);

//    const int samplesToRead = inputChannelCount * numFrames * oboeStream->getBytesPerFrame();
    const int samplesToRead = numFrames; //inputChannelCount * numFrames * oboeStream->getBytesPerFrame();
//    LOGI("DATA VALUE FOUND %d", (int) sizeof(uint8_t) * samplesToRead);

//    auto *castAudioData = static_cast<uint8_t *>(audioData);
    auto *castAudioData = static_cast<float *>(audioData);

    gen->add(&castAudioData[0], samplesToRead);

    return oboe::DataCallbackResult::Continue;
}

/**
 * Oboe notifies the application for "about to close the stream".
 *
 * @param oboeStream: the stream to close
 * @param error: oboe's reason for closing the stream
 */
void MfccAnalysis::onErrorBeforeClose(oboe::AudioStream *oboeStream,
                                         oboe::Result error) {
    LOGE("%s stream Error before close: %s",
         oboe::convertToText(oboeStream->getDirection()),
         oboe::convertToText(error));
}

/**
 * Oboe notifies application that "the stream is closed"
 *
 * @param oboeStream
 * @param error
 */
void MfccAnalysis::onErrorAfterClose(oboe::AudioStream *oboeStream,
                                        oboe::Result error) {
    LOGE("%s stream Error after close: %s",
         oboe::convertToText(oboeStream->getDirection()),
         oboe::convertToText(error));
}
