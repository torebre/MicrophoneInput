package com.kjipo.microphoneinput.mfcc

import android.util.Log

class DataTransfer {

    companion object {

        var value = 1f

        @JvmStatic
        fun storeFloat(value: Float) {
            DataTransfer.value = value

            Log.i("DataTransfer", "Value is: ${DataTransfer.value}")
        }


        @JvmStatic
        fun storeFloatArray(value: FloatArray) {
            Log.i("DataTransfer", "Storing float array. Number of values: ${value.size}")
        }

    }

}