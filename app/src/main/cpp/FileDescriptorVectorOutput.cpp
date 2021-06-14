#ifndef ESSENTIA_FILEDESCRIPTOR_VECTOR_OUTPUT
#define ESSENTIA_FILEDESCRIPTOR_VECTOR_OUTPUT


#include "../../../../module/lib/x86_64/include/essentia/streaming/streamingalgorithm.h"
#include <unistd.h>

namespace essentia {
    namespace streaming {

        class FileDescriptorVectorOutput : public Algorithm {
        protected:
//            Sink <Real> _data;
            Sink<std::vector<Real>> _data;
            int fileDescriptor;

        public:
            FileDescriptorVectorOutput(int fileDescriptorInput) : Algorithm() {
                fileDescriptor = fileDescriptorInput;
                setName("FileDescriptorVectorOutput");
                declareInput(_data, 1, "data", "the incoming data to be stored in the output file");

//                declareParameters();
            }

            ~FileDescriptorVectorOutput() {
//                if (_stream != &std::cout) delete _stream;
            }

            void declareParameters() {

            }

            AlgorithmStatus process() {
                if (!_data.acquire(1)) {
                    return NO_INPUT;
                }

                writeToken(_data.firstToken());

                _data.release(1);

                return OK;
            }

            void writeToken(const std::vector<Real> value) {
//                LOGI("Got value: %f", value);

// TODO

                // TODO The bytes are in reversed order when they are written here
//                write(fileDescriptor, (const char *) &value, sizeof(Real));
            }

        };

    } // namespace streaming
} // namespace essentia

#endif //ESSENTIA_FILEDESCRIPTOR_VECTOR_OUTPUT
