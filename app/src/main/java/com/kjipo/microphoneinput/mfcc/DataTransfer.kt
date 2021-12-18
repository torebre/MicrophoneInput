package com.kjipo.microphoneinput.mfcc

import android.util.Log
import kotlinx.coroutines.*


class DataTransfer {

    companion object {

        var value = 1f


        @JvmStatic
        fun storeFloat(value: Float) {
            DataTransfer.value = value

            Log.i("DataTransfer", "Value is: ${DataTransfer.value}")
        }

        @JvmStatic
        fun storeFloatArray(values: FloatArray) {
            GlobalScope.launch {
                val valuesDescription = values.map {
                it.toString()}.joinToString(",")
                Log.i("DataTransfer", "Values: $valuesDescription")
            }

        }

    }

}