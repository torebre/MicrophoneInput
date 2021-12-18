#ifndef MICROPHONEINPUT_DATATRANSFER_H
#define MICROPHONEINPUT_DATATRANSFER_H

//#include "../../../../module/lib/x86_64/include/essentia/streaming/streamingalgorithm.h"

#include <queue>
#include <vector>


class DataTransfer {

public:
    DataTransfer();



private:
    std::queue<float> *melQueue;
    std::queue<float> *coeffQueue;

    int melQueueCount;
    int coeffQueueCount;

    void addToMelQueue(std::vector<float> *values);


};


#endif //MICROPHONEINPUT_DATATRANSFER_H
