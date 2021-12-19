package com.kjipo.microphoneinput.mfcc

import android.util.Log
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import java.lang.ref.WeakReference


class DataTransfer {

    companion object {

        var value = 1f
        var listener: WeakReference<DataReceiver>? = null


        fun setListener(listener: DataReceiver) {
            this.listener = WeakReference(listener)
        }


        @JvmStatic
        fun storeFloat(value: Float) {
            DataTransfer.value = value

            Log.i("DataTransfer", "Value is: ${DataTransfer.value}")
        }

        @JvmStatic
        fun storeFloatArray(values: FloatArray) {
            GlobalScope.launch(Dispatchers.Main) {
//                val valuesDescription = values.map {
//                it.toString()}.joinToString(",")
//                Log.i("DataTransfer", "Values: $valuesDescription")

                listener?.get()?.apply { handleData(values) }
            }
        }

    }

}

