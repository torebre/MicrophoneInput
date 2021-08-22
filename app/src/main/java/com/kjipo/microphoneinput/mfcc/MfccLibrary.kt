package com.kjipo.microphoneinput.mfcc

class MfccLibrary {



    companion object {

        @JvmStatic
        external fun create(): Boolean

        @JvmStatic
        external fun start()

        @JvmStatic
        external fun stop()

        @JvmStatic
        external fun testCallback()

        init {
            System.loadLibrary("mfcc-lib")
        }
    }


}