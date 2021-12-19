package com.kjipo.microphoneinput.mfcc

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.util.AttributeSet
import android.util.Log
import android.view.View

class SpectrumGraph(context: Context?, attributeSet: AttributeSet?) : View(context, attributeSet) {

    companion object {
        const val XDIFF_BETWEEN_POINTS = 1
        const val YDIFF_BETWEEN_SLOTS = 200
        const val CHANNEL_HEIGHT = 100
    }

    //    private val spectrumGraphModel: SpectrumGraphModel
    // TODO Use proper number of columns
    // TODO Data should be stored in model not here
    private var currentData = Array<MutableList<Float>>(13) {
        mutableListOf()
    }

    // TODO Hardcoded limits while developing
    private val minMaxValues = Pair(-600.0f, 600.0f)

    private val pointPaint = Paint().apply {
        color = Color.RED
        // TODO Does this setting do anything in this context?
        style = Paint.Style.FILL

    }

//    init {
//        this.spectrumGraphModel = spectrumGraphModel
//    }


//    private val viewRectangle: IntArray = IntArray(4)


//    private fun getBoxesForLineTracks() {
//        val width = viewRectangle[2] - viewRectangle[0]
//        val height = viewRectangle[3] - viewRectangle[1]
//
//       val numberOfLines = spectrumGraphModel.numberOfLines
//
//        (0 until numberOfLines).map {
//            spectrumGraphModel.getMaxAndMinForLine(it)
//        }.reduce { pair1, pair2 ->
//            Pair(if(pair1.first < pair2.first) pair1.first else pair2.first,
//            if(pair1.second < pair2.second) pair2.second else pair1.second)
//        }
//    }

    fun updateData(currentData: FloatArray) {
        Log.i("SpectrumGraph", "Updating data")


        // TODO This is still not completely thread-safe
        currentData.forEachIndexed { index, value ->
            this.currentData[index].add(value)
        }

        invalidate()
    }


    override fun onDraw(canvas: Canvas?) {
//        super.onDraw(canvas)

        canvas?.let {
            drawInternal(it)
        }
    }


    private fun drawInternal(canvas: Canvas) {
        currentData.let { currentData ->

            Log.i("SpectrumGraph", "Drawing lines")

            // TODO Use proper number of lines
            for (i in currentData.indices) {
                val points = currentData[i]
                val (min, max) = minMaxValues

                var xCurrent = 0.0f
                val yCurrent = YDIFF_BETWEEN_SLOTS + i * YDIFF_BETWEEN_SLOTS

                for (point in points) {
                    val scaledPoint = (point - min) / (max - min)

                    if (scaledPoint < min || scaledPoint > max) {
                        // Point is outside max or min
                        continue
                    }

//                    Log.i(
//                        "SpectrumGraph",
//                        "Drawing point: $xCurrent, ${yCurrent + scaledPoint * CHANNEL_HEIGHT}"
//                    )

                    canvas.drawPoint(xCurrent, yCurrent + scaledPoint * CHANNEL_HEIGHT, pointPaint)

                    xCurrent += XDIFF_BETWEEN_POINTS
                }
            }
        }

    }

}