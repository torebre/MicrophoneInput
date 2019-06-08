#include "MicrophoneInput.h"
#include "../../../../../../projects/oboe/src/common/OboeDebug.h"
#include "FileDescriptorOutputSingle.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <limits.h>

#include <essentia/algorithmfactory.h>
#include <essentia/streaming/algorithms/ringbufferinput.h>
#include <thread>
#include <essentia/pool.h>
#include <essentia/scheduler/network.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include <essentia/streaming/streamingalgorithm.h>
#include <essentia/streaming/algorithms/fileoutput.h>
//#include <essentia/streaming/algorithms/filedescriptoroutputsingle.h>

#include <essentia/streaming/algorithms/vectoroutput.h>
#include <essentia/streaming/algorithms/diskwriter.h>
#include <essentia/streaming/algorithms/devnull.h>


using namespace oboe;


void MicrophoneInput::create() {
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

void MicrophoneInput::startRecording() {
    LOGI("Starting recording");

    AudioStreamBuilder builder;

    builder.setDeviceId(recordingDeviceId);
    builder.setDirection(oboe::Direction::Input);
//    builder.setSampleRate(sampleRate);
//    builder.setBufferCapacityInFrames(200); // TODO Just setting this value for testing
    builder.setChannelCount(inputChannelCount);
    builder.setAudioApi(audioApi);
    builder.setFormat(format);
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setCallback(this);

    oboe::Result result = builder.openStream(&recordingStream);

    if (result == oboe::Result::OK && recordingStream) {
        assert(recordingStream->getChannelCount() == inputChannelCount);
//        assert(recordingStream->getSampleRate() == sampleRate);
        assert(recordingStream->getFormat() == oboe::AudioFormat::Float);

        warnIfNotLowLatency(recordingStream);

        struct stat stat_p;
        if (stat(pipeFile, &stat_p) == 0) {
            int returnCode = remove(pipeFile);
            if (returnCode != 0) {
                LOGE("Failed to remove pipe file. Return code: %d", returnCode);
            }
        } else {
            LOGI("Pipe file does not already exist");
        }

        auto fifo = mkfifo(pipeFile, 0777); //S_IFIFO | S_IRWXU);


        if (fifo != 0) {
            LOGE("Failed to create FIFO file. Return code: %d", fifo);
        } else {
            LOGI("Created FIFO file");
        }

        writefd = open(pipeFile, O_RDWR);

        setupNetwork();


        network = new essentia::scheduler::Network(gen);
//        network->runPrepare();


// TODO Is it necessary to run this here?
        audioProcessor = std::thread(&MicrophoneInput::runNetwork, this);

        recordingStream->requestStart();
    } else {
        LOGE("Failed to create recording stream. Error: %s",
             oboe::convertToText(result));
    }


}

void MicrophoneInput::stop() {
    LOGI("Stop called");

}


void MicrophoneInput::runNetwork() {
    LOGI("Running network");

    network->run();
}

void MicrophoneInput::setRecordingDeviceId(int32_t deviceId) {
    recordingDeviceId = deviceId;
}


void MicrophoneInput::warnIfNotLowLatency(oboe::AudioStream *stream) {
    if (stream->getPerformanceMode() != oboe::PerformanceMode::LowLatency) {
        LOGW(
                "Stream is NOT low latency."
                "Check your requested format, sample rate and channel count");
    }
}


oboe::DataCallbackResult MicrophoneInput::onAudioReady(
        oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    assert(oboeStream == recordingStream);

    const int samplesToRead = inputChannelCount * numFrames * oboeStream->getBytesPerFrame();
    LOGI("DATA VALUE FOUND %d", (int) sizeof(uint8_t) * samplesToRead);


//    write(writefd, static_cast<char *>(audioData), sizeof(uint8_t) * samplesToRead);

    uint8_t *castAudioData = static_cast<uint8_t *>(audioData);
    float convertedData[samplesToRead];
    for (int i = 0; i < samplesToRead; ++i) {
        convertedData[i] = castAudioData[i];
    }

    gen->add(&convertedData[0], samplesToRead);

//    network->runStep();
//    while(network->runStep()) {
//        // Do nothing
//    }
//    network->printBufferFillState();


    LOGI("Wrote data");

    return oboe::DataCallbackResult::Continue;
}

/**
 * Oboe notifies the application for "about to close the stream".
 *
 * @param oboeStream: the stream to close
 * @param error: oboe's reason for closing the stream
 */
void MicrophoneInput::onErrorBeforeClose(oboe::AudioStream *oboeStream,
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
void MicrophoneInput::onErrorAfterClose(oboe::AudioStream *oboeStream,
                                        oboe::Result error) {
    LOGE("%s stream Error after close: %s",
         oboe::convertToText(oboeStream->getDirection()),
         oboe::convertToText(error));
}


void MicrophoneInput::setupNetwork() {
    essentia::streaming::AlgorithmFactory &factory = essentia::streaming::AlgorithmFactory::instance();

    gen = new essentia::streaming::RingBufferInput();
    gen->declareParameters();
    gen->configure();


//    gen->setParameters(parameterMap);
//        gen->_bufferSize = frameSize;

    gen->output(0).setAcquireSize(frameSize);
    gen->output(0).setReleaseSize(frameSize);

    essentia::streaming::Algorithm *fc = factory.create("FrameCutter",
                                                        "frameSize", frameSize,
                                                        "hopSize", hopSize);

    essentia::streaming::Algorithm *w = factory.create("Windowing",
                                                       "type", "hann");
    essentia::streaming::Algorithm *spec = factory.create("Spectrum");

    gen->output("signal") >> fc->input("signal");

    fc->output("frame") >> w->input("frame");
    w->output("frame") >> spec->input("frame");
    essentia::streaming::Algorithm *pitchYinFft = essentia::streaming::AlgorithmFactory::create(
            "PitchYinFFT",
            "frameSize", frameSize,
            "sampleRate", recordingStream->getSampleRate());
    spec->output("spectrum") >> pitchYinFft->input("spectrum");
    pitchYinFft->output("pitch") >> essentia::streaming::PoolConnector(pool, "pitch");
//    pitchYinFft->output("pitchConfidence")
//            >> essentia::streaming::PoolConnector(pool, "pitch_confidence");

    // TODO Temporarily piping pitch confidence to nowhere
    pitchYinFft->output("pitchConfidence") >> essentia::streaming::NOWHERE;


    essentia::streaming::FileDescriptorOutputSingle2 *outputWriter = new essentia::streaming::FileDescriptorOutputSingle2(
            writefd);

    essentia::streaming::connect(pitchYinFft->output("pitch"), outputWriter->input("data"));

}