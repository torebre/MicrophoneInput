#include "MicrophoneInput.h"
#include "../../../../../../projects/oboe/src/common/OboeDebug.h"

#include <sys/types.h>
#include <sys/stat.h>
//#include <fstream>
//#include <iostream>
//#include <stdio.h>
//#include <stdlib.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <limits.h>
//#include <sys/types.h>
//#include <sys/stat.h>

#include <essentia/algorithmfactory.h>
#include <essentia/streaming/algorithms/ringbufferinput.h>
#include <thread>
#include <essentia/pool.h>
#include <essentia/scheduler/network.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include <essentia/streaming/streamingalgorithm.h>
#include <essentia/streaming/algorithms/fileoutput.h>
//#include <essentia/algorithms/io/yamloutput.h>



using namespace oboe;
//using namespace essentia;

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
//    builder.setBufferCapacityInFrames(200); // TODO Just setting this value for testing
    builder.setChannelCount(inputChannelCount);
    builder.setAudioApi(audioApi);
    builder.setFormat(format);
//    builder.setFormat(AudioFormat::Float);
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setCallback(this);

    oboe::Result result = builder.openStream(&recordingStream);

    if (result == oboe::Result::OK && recordingStream) {
        assert(recordingStream->getChannelCount() == inputChannelCount);
        assert(recordingStream->getSampleRate() == sampleRate);
        assert(recordingStream->getFormat() == oboe::AudioFormat::I16);

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


// TODO Is it necessary to run this here?
    audioProcessor = std::thread(&MicrophoneInput::runNetwork, this);
//    network->run();



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


    write(writefd, static_cast<char *>(audioData), sizeof(uint8_t) * samplesToRead);

    uint8_t *castAudioData = static_cast<uint8_t *>(audioData);
    float convertedData[samplesToRead];
    for(int i = 0; i < samplesToRead; ++i) {
        convertedData[i] = castAudioData[i];

    }


    gen->add(&convertedData[0], samplesToRead);


//    for (auto channel = 0;
//         channel < maxOutputChannels;
//         ++channel) {
//        if ((!activeOutputChannels[channel]) || maxInputChannels == 0) {
//            bufferToFill.buffer->
//                    clear(channel, bufferToFill
//                    .startSample, bufferToFill.numSamples);
//        } else {
//            auto actualInputChannel = channel % maxInputChannels;
//
//            if (!activeInputChannels[channel]) {
//                bufferToFill.buffer->
//                        clear(channel, bufferToFill
//                        .startSample, bufferToFill.numSamples);
//            } else {
//                auto *inBuffer = bufferToFill.buffer->getReadPointer(actualInputChannel,
//                                                                     bufferToFill.startSample);
//                auto *outBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
//
//
//                auto audioBuffer = std::vector<Real>(static_cast<unsigned long>(bufferToFill.numSamples));
//
//
//                // TODO Is this needed?
////                if (audioBuffer.size() != frameSize) {
////                    gen->output(0).
////                            setAcquireSize(bufferToFill
////                                                   .numSamples);
////                    gen->output(0).
////                            setReleaseSize(bufferToFill
////                                                   .numSamples);
////
////                }
//
//                for (
//                        auto sample = 0;
//                        sample < bufferToFill.
//                                numSamples;
//                        ++sample) {
//                    outBuffer[sample] = inBuffer[sample];
//                    audioBuffer[sample] = inBuffer[sample];
//                }
//
//                gen->add(&audioBuffer[0], bufferToFill.numSamples);
//
//
//            }
//        }
//    }



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

//    gen = static_cast<RingBufferInput*>(streaming::AlgorithmFactory::create("RingBufferInput", "bufferSize", frameSize));
//    essentia::streaming::Algorithm *gen = factory.create("RingBufferInput",
//                                                        "bufferSize", 100000);


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
    essentia::streaming::Algorithm *pitchYinFft = essentia::streaming::AlgorithmFactory::create("PitchYinFFT",
                                                                            "frameSize", frameSize);
    spec->output("spectrum") >> pitchYinFft->input("spectrum");
    pitchYinFft->output("pitch") >> essentia::streaming::PoolConnector(pool, "pitch");
    pitchYinFft->output("pitchConfidence") >> essentia::streaming::PoolConnector(pool, "pitch_confidence");

//    spec->output("spectrum") >> essentia::streaming::PoolConnector(pool, "spectrum");


    essentia::streaming::Algorithm *pitchMelodia = essentia::streaming::AlgorithmFactory::create("PitchMelodia", "frameSize", frameSize,
                                                                             "hopSize", hopSize, "guessUnvoiced", true);
    gen->output("signal") >> pitchMelodia->input("signal");

    pitchMelodia->output("pitch") >> essentia::streaming::PoolConnector(pool, "melodia_pitch");
    pitchMelodia->output("pitchConfidence") >> essentia::streaming::PoolConnector(pool, "melodia_pitch_confidence");

}