package com.kjipo.microphoneinput

import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel

class ColourWheelModel: ViewModel() {
    val inputData = MutableLiveData<Pair<Double, Double>>()

    fun setCurrentData(updatedData: Pair<Double, Double>) {
        inputData.value = updatedData
    }


}