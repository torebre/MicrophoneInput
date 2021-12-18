#include "LoggerOutputVector.h"

#include <oboe/Oboe.h>
#include "../../../../../../projects/oboe/src/common/OboeDebug.h"
#include <essentia/streaming/streamingalgorithm.h>


LoggerOutputVector::LoggerOutputVector() : Algorithm() {
    setName("CallbackWriter");
    declareInput(_data, 1, "input", "a vector of real values");
}


essentia::streaming::AlgorithmStatus LoggerOutputVector::process() {
    if (!_data.acquire(1)) {
        return essentia::streaming::NO_INPUT;
    }

    writeToken(_data.firstToken());

    _data.release(1);

    return essentia::streaming::OK;
}


void LoggerOutputVector::declareParameters() {
    // TODO

}


void LoggerOutputVector::writeToken(const std::vector<essentia::Real> value) {
    auto numberOfValues = value.size();

    std::string  valuesText("Values: ");
    for(int i = 0; i < numberOfValues; ++i) {
        valuesText = valuesText.append(std::to_string(value[i])).append(",");
    }
    LOGI("%s", valuesText.c_str());
}

