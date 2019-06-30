#ifndef ESSENTIA_FILEDESCRIPTOROUTPUTSINGLE2_H
#define ESSENTIA_FILEDESCRIPTOROUTPUTSINGLE2_H


#include "../../../../module/lib/x86_64/include/essentia/streaming/streamingalgorithm.h"
#include <unistd.h>

namespace essentia {
    namespace streaming {

        class FileDescriptorOutputSingle2 : public Algorithm {
        protected:
            Sink <Real> _data;
            int fileDescriptor;

        public:
            FileDescriptorOutputSingle2(int fileDescriptorInput) : Algorithm() {
                fileDescriptor = fileDescriptorInput;
                setName("FileDescriptorOutputSingle2");
                declareInput(_data, 1, "data", "the incoming data to be stored in the output file");

//                declareParameters();
            }

            ~FileDescriptorOutputSingle2() {
//                if (_stream != &std::cout) delete _stream;
            }

            void declareParameters() {

            }

            AlgorithmStatus process() {
                if (!_data.acquire(1)) return NO_INPUT;

                writeToken(_data.firstToken());

                _data.release(1);

                return OK;
            }

            void writeToken(const Real value) {
//                LOGI("Got value: %f", value);

                // TODO The bytes are in reversed order when they are written here
                write(fileDescriptor, (const char *) &value, sizeof(Real));
            }
        };

    } // namespace streaming
} // namespace essentia

#endif //ESSENTIA_FILEDESCRIPTOROUTPUTSINGLE2_H
