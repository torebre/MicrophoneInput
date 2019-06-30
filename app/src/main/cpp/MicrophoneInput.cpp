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

#include <essentia/streaming/algorithms/vectoroutput.h>
#include <essentia/streaming/algorithms/diskwriter.h>
#include <essentia/streaming/algorithms/devnull.h>

#include <iostream>
#include <fstream>


using namespace oboe;


MicrophoneInput::MicrophoneInput(const char *pitchFifoInput, const char *pitchConfidenceFifoInput)
        : pitchFifo(pitchFifoInput), pitchConfidenceFifo(pitchConfidenceFifoInput) {

}

MicrophoneInput::~MicrophoneInput() {

}


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
    builder.setSampleRate(sampleRate);
    builder.setChannelCount(inputChannelCount);
    builder.setAudioApi(audioApi);
    builder.setFormat(format);
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setCallback(this);

    oboe::Result result = builder.openStream(&recordingStream);

    if (result == oboe::Result::OK && recordingStream) {
        assert(recordingStream->getChannelCount() == inputChannelCount);
        assert(recordingStream->getFormat() == oboe::AudioFormat::Float);
        assert(recordingStream->getSampleRate() == sampleRate);

        warnIfNotLowLatency(recordingStream);

        struct stat stat_p;
        if (stat(pitchFifo, &stat_p) == 0) {
            int returnCode = remove(pitchFifo);
            if (returnCode != 0) {
                LOGE("Failed to remove pipe file. Return code: %d", returnCode);
            }
        } else {
            LOGI("Pipe file does not already exist");
        }

        auto fifo = mkfifo(pitchFifo, 0777); //S_IFIFO | S_IRWXU);

        if (fifo != 0) {
            LOGE("Failed to create FIFO file. Return code: %d", fifo);
        } else {
            LOGI("Created FIFO file");
        }

        writefd = open(pitchFifo, O_RDWR);

        if (stat(pitchConfidenceFifo, &stat_p) == 0) {
            int returnCode = remove(pitchConfidenceFifo);
            if (returnCode != 0) {
                LOGE("Failed to remove pipe file. Return code: %d", returnCode);
            }
        } else {
            LOGI("Pipe file does not already exist");
        }

        auto confidenceFifo = mkfifo(pitchConfidenceFifo, 0777); //S_IFIFO | S_IRWXU);

        writeConfidenceFifo = open(pitchConfidenceFifo, O_RDWR);

        setupNetwork();


        network = new essentia::scheduler::Network(gen);


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

    if (recordingStream) {
        Result result = recordingStream->stop(0L);
        if (result != oboe::Result::OK) {
            LOGE("Error when stopping stream. %s", oboe::convertToText(result));
        }
    }

    // TODO Necessary to stop thread?

    network->clear();
    essentia::shutdown();

    auto statusCodePitchPipe = close(writefd);
    if (statusCodePitchPipe != 0) {
        LOGE("Problem closing pitch pipe: %d", statusCodePitchPipe);
    }

    auto statusCodeConfidencePipe = close(writeConfidenceFifo);
    if (statusCodeConfidencePipe != 0) {
        LOGE("Problem closing confidence pipe: %d", writeConfidenceFifo);
    }
}


void MicrophoneInput::runNetwork() {
    LOGI("Running network");
    network->run();
    LOGI("Network stopped");
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
                                                        "hopSize", hopSize,
                                                        "startFromZero", true);

    gen->output("signal") >> fc->input("signal");

    essentia::streaming::Algorithm *pitchYin = essentia::streaming::AlgorithmFactory::create(
            "PitchYin",
            "sampleRate", recordingStream->getSampleRate(),
            "frameSize", frameSize);

    fc->output("frame") >> pitchYin->input("signal");

    auto *outputWriter = new essentia::streaming::FileDescriptorOutputSingle2(
            writefd);

    essentia::streaming::connect(pitchYin->output("pitch"), outputWriter->input("data"));

    auto *confidenceWriter = new essentia::streaming::FileDescriptorOutputSingle2(
            writeConfidenceFifo);

    pitchYin->output("pitchConfidence") >> confidenceWriter->input("data");
}