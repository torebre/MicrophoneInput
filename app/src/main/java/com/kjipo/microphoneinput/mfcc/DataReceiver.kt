package com.kjipo.microphoneinput.mfcc

interface DataReceiver {


    fun handleData(dataPoints: FloatArray)


}