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

using namespace oboe;

void MicrophoneInput::create() {
    AudioStreamBuilder builder;
    builder.setDirection(Direction::Input);
    builder.setPerformanceMode(PerformanceMode::LowLatency);

    AudioStream *stream;
    Result r = builder.openStream(&stream);

    if(r != Result::OK) {
        LOGE("Error opening stream: %s", convertToText(r));
    }

    r = stream->requestStart();
    if(r != Result::OK) {
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
        if(stat(pipeFile, &stat_p) == 0) {
            int returnCode = remove(pipeFile);
            if(returnCode!= 0) {
                LOGE("Failed to remove pipe file. Return code: %d", returnCode);
            }
        }
        else {
            LOGI("Pipe file does not already exist");
        }

        auto fifo = mkfifo(pipeFile, 0777); //S_IFIFO | S_IRWXU);


        if(fifo != 0) {
            LOGE("Failed to create FIFO file. Return code: %d", fifo);
        }
        else {
            LOGI("Created FIFO file");
        }

        writefd = open(pipeFile, O_RDWR);

        recordingStream->requestStart();
    } else {
        LOGE("Failed to create recording stream. Error: %s",
             oboe::convertToText(result));
    }


}

void MicrophoneInput::stop() {
    LOGI("Stop called");

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

    LOGI("Test23");



    char *test ="Test data";
    write(writefd, test, strlen(test));

//    if(streamFile == nullptr) {
//
//        LOGI("Test24");
//
//
//
//        std::ofstream newFile(pipeFile);
//        newFile.open(pipeFile, std::ios::out | std::ios::binary);
//        streamFile = &newFile;
//    }

//    if(!streamFile->is_open()) {
//        LOGE("Pipe file not opened for writing");
//    }
//    else {

//        memset(static_cast<uint8_t *>(audioData), 0,
//               sizeof(int16_t) * inputChannelCount * numFrames);
//        LOGI("DATA VALUE FOUND %d",(int)sizeof(uint8_t) * inputChannelCount * numFrames);
//
//
//    if(streamFile == nullptr) {
//        LOGI("Stream file pointer is null");
//    }


//        streamFile->write(static_cast<char *>(audioData), sizeof(uint8_t) * inputChannelCount * numFrames);
//    streamFile->write(test, strlen(test));
//    streamFile->flush();
//    streamFile->write(test, strlen(test));
//
//    streamFile->flush();
//    streamFile->close();

//    streamFile = nullptr;


        LOGI("Wrote data");

//    }


//    if (oboeStream->getFormat() == oboe::AudioFormat::Float) {
//        memset(static_cast<uint8_t *>(audioData), 0,
//               sizeof(float) * inputChannelCount * numFrames);
//
//
//
////        oboe::ErrorOrValue<int32_t> status = playbackStream->write(audioData,numFrames,0);
////        int32_t framesRead = (!status) ? 0: status.value();
////        LOGI("FRAMES READ %d", framesRead);
//        LOGI("DATA VALUE FOUND %d",((uint8_t*)audioData)[0]);
//    } else {
//        memset(static_cast<uint8_t *>(audioData), 0,
//               sizeof(int16_t) * inputChannelCount * numFrames);
//        LOGI("DATA VALUE FOUND %d",((uint8_t*)audioData)[0]);
//    }

//    int32_t prevFrameRead = 0, framesRead = 0;
//    if (mProcessedFrameCount < mSystemStartupFrames) {
//        do {
//            // Drain the audio for the starting up period, half second for
//            // this sample.
//            prevFrameRead = framesRead;
//
//            oboe::ResultWithValue<int32_t> status =
//                    mRecordingStream->read(audioData, numFrames, 0);
//            framesRead = (!status) ? 0 : status.value();
//            if (framesRead == 0) break;
//
//        } while (framesRead);
//
//        framesRead = prevFrameRead;
//    } else {
//        oboe::ResultWithValue<int32_t> status =
//                mRecordingStream->read(audioData, numFrames, 0);
//        if (!status) {
//            LOGE("input stream read error: %s",
//                 oboe::convertToText(status.error()));
//            return oboe::DataCallbackResult ::Stop;
//        }
//        framesRead = status.value();
//    }
//
//    if (framesRead < numFrames) {
//        int32_t bytesPerFrame = mRecordingStream->getChannelCount() *
//                                oboeStream->getBytesPerSample();
//        uint8_t *padPos =
//                static_cast<uint8_t *>(audioData) + framesRead * bytesPerFrame;
//        memset(padPos, 0, static_cast<size_t>((numFrames - framesRead) * bytesPerFrame));
//    }
//
//    // add your audio processing here
//
//    mProcessedFrameCount += numFrames;

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
