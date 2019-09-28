#include <jni.h>

#include <android/log.h>
#include <android/bitmap.h>
#include "ColourWheel.h"

#define  LOG_TAG    "colourwheel-lib"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


static ColourWheel *colourWheel = nullptr;

extern "C" {

JNIEXPORT void JNICALL
Java_com_kjipo_microphoneinput_ColourWheelRenderer_setup(JNIEnv *env, jobject obj,
                                                         int numberOfRows,
                                                         int numberOfColumns) {
    if (colourWheel == nullptr) {
        colourWheel = new ColourWheel();
    }

    colourWheel->setup(numberOfRows, numberOfColumns);

}


JNIEXPORT void JNICALL
Java_com_kjipo_microphoneinput_ColourWheelRenderer_renderColourWheel(JNIEnv *env, jobject obj,
                                                                     jobject bitmap,
                                                                     int highlightSegment) {
    AndroidBitmapInfo info;
    void *pixels;
    int returnCode;

    if ((returnCode = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", returnCode);
        return;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("Bitmap format is not ARGB_8888");
        return;
    }

    if ((returnCode = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", returnCode);
    }

    // TODO


    colourWheel->renderColourWheel(&info, pixels, highlightSegment);

    LOGI("Test23. Returning");


    AndroidBitmap_unlockPixels(env, bitmap);

    LOGI("Test23. Returning2");


}


}
