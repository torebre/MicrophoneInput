package com.kjipo.microphoneinput

import org.junit.Test

class ColourWheelTest {


    @Test
    fun setupColourWheelTest() {
        val colourWheel = ColourWheel.drawWheel(200, 200)
        val segmentPixelData = ColourWheel.transformMap(colourWheel)

        println("Rows:")
        segmentPixelData.first.forEach { rows ->
            println(rows.joinToString(","))
        }

        println("Columns:")
        segmentPixelData.second.forEach { columns ->
            println(columns.joinToString(","))
        }

//        println("Rows: ${segmentPixelData.first}")
//        println("Columns: ${segmentPixelData.second}")

    }


//    @Test
//    fun createBitmapPixelsTest() {
//        val colourWheel = ColourWheel.drawWheel(200, 200)
//        val segmentPixelData = ColourWheel.transformMap(colourWheel)
//
//        val pixels = ColourWheel.createBitmapPixels(200, 200, 400, 400, segmentPixelData, -1)
//
//        println("Pixels: $pixels")
//    }


}