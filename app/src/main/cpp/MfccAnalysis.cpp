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

#include "FileDescriptorVectorOutput.cpp"




MfccAnalysis::MfccAnalysis(const int writefd) {
    this->writefd = writefd;
}

//void MfccAnalysis::startRecording() {
//    LOGI("Starting recording");
//
//    AudioStreamBuilder builder;
//
//    builder.setDeviceId(recordingDeviceId);
//    builder.setDirection(oboe::Direction::Input);
//    builder.setSampleRate(sampleRate);
//    builder.setChannelCount(inputChannelCount);
//    builder.setAudioApi(audioApi);
//    builder.setFormat(format);
//    builder.setSharingMode(oboe::SharingMode::Exclusive);
//    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
//    builder.setCallback(this);
//
//    oboe::Result result = builder.openStream(&recordingStream);
//
//    if (result == oboe::Result::OK && recordingStream) {
//        assert(recordingStream->getChannelCount() == inputChannelCount);
//        assert(recordingStream->getFormat() == oboe::AudioFormat::Float);
//        assert(recordingStream->getSampleRate() == sampleRate);
//
//        warnIfNotLowLatency(recordingStream);
//
////        setupNetwork();
//
//
//        network = new essentia::scheduler::Network(gen);
//
//
//// TODO Is it necessary to run this here?
//        audioProcessor = std::thread(&MicrophoneInput::runNetwork, this);
//
//        recordingStream->requestStart();
//    } else {
//        LOGE("Failed to create recording stream. Error: %s",
//             oboe::convertToText(result));
//    }
//
//}
//
//void MfccAnalysis::setupFifo() {
//    struct stat stat_p;
//    if (stat(outputFifo, &stat_p) == 0) {
//        int returnCode = remove(outputFifo);
//        if (returnCode != 0) {
//            LOGE("Failed to remove pipe file. Return code: %d", returnCode);
//        }
//    } else {
//        LOGI("Pipe file does not already exist");
//    }
//
//    auto fifo = mkfifo(outputFifo, 0777); //S_IFIFO | S_IRWXU);
//
//    if (fifo != 0) {
//        LOGE("Failed to create FIFO file. Return code: %d", fifo);
//    } else {
//        LOGI("Created FIFO file");
//    }
//
//    writefd = open(outputFifo, O_RDWR);
//}


void MfccAnalysis::setupNetwork() {
    essentia::streaming::AlgorithmFactory &factory = essentia::streaming::AlgorithmFactory::instance();

    auto gen = new essentia::streaming::RingBufferInput();
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


    essentia::streaming::Algorithm *w = factory.create("Windowing",
                                                       "type", "blackmanharris62");

    essentia::streaming::Algorithm *spec = factory.create("Spectrum");

//    gen->output("signal") >> fc->input("signal");

    essentia::streaming::Algorithm *mfcc = factory.create("MFCC");


// Spectrum -> MFCC -> Pool
    spec->output("spectrum") >> mfcc->input("spectrum");

    mfcc->output("bands")
            >> essentia::streaming::NOWHERE;                   // we don't want the mel bands
//    mfcc->output("mfcc")      >>  PC(pool, "lowlevel.mfcc"); // store only the mfcc coeffs

//    essentia::streaming::Algorithm *pitchYin = essentia::streaming::AlgorithmFactory::create(
//            "PitchYin",
//            "sampleRate", recordingStream->getSampleRate(),
//            "frameSize", frameSize);
//
//    fc->output("frame") >> pitchYin->input("signal");

    auto *outputWriter = new essentia::streaming::FileDescriptorVectorOutput(writefd);

    essentia::streaming::connect(mfcc->output("mfcc"), outputWriter->input("data"));

}

