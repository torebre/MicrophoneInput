package com.kjipo.microphoneinput

import android.graphics.Bitmap

object ColourWheelRenderer {

    init {
        System.loadLibrary("colourwheel-lib")
    }


    external fun setup(numberOfRows: Int, numberOfColumns: Int)

    external fun renderColourWheel(bitmap: Bitmap, segmentToHighLight: Int)



}