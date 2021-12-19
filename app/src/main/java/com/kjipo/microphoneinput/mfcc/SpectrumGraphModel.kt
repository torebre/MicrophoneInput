package com.kjipo.microphoneinput.mfcc

import android.util.Log
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel

class SpectrumGraphModel : ViewModel(), DataReceiver {
    val lineData = MutableLiveData<FloatArray>()

    // TODO Get proper number of lines from somewhere
    val numberOfLines = 2

    init {
//        lineData.value = Array(numberOfLines) {
//            mutableListOf()
//        }

        DataTransfer.setListener(this)
    }

//    fun getPointsForLineNumber(lineNumber: Int): List<Float> {
//        lineData.value.let {
//            return if(it == null) {
//                emptyList()
//            }
//            else {
//                it[lineNumber]
//            }
//        }
//    }


    fun getMinAndMaxForLine(lineNumber: Int): Pair<Float, Float> {
        // TODO

        return Pair(-600.0f, 600.0f)
    }

    override fun handleData(dataPoints: FloatArray) {
        Log.i("SpectrumGraphModel", "Handling data")

//        lineData.value?.let {
//            for (i in 0 until 2) {
//               it[i].add(dataPoints[i])
//            }
//        }

//        lineData.value?.let {
//            dataPoints.forEachIndexed { index, value ->
//                it[index].add(value)
//            }
//        }


        lineData.value = dataPoints




    }


}