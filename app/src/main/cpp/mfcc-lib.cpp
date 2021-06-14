#include <jni.h>
#include <string>
#include <essentia/algorithmfactory.h>
#include <essentia/pool.h>

#include "../../../../../../projects/oboe/src/common/OboeDebug.h"
#include "MicrophoneInput.h"


static MicrophoneInput *engine = nullptr;

extern "C" {


JNIEXPORT void JNICALL
Java_com_kjipo_microphoneinput_MicrophoneRecording_startRecording(
        JNIEnv * env , jclass ) {
if ( engine == nullptr ) {
LOGE(
        "Engine is null, you must call createEngine before calling this "
        "method") ;
return ;
}

engine->startRecording();

}


}