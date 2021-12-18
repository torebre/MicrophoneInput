package com.kjipo.microphoneinput.mfcc

import android.content.Context
import android.graphics.Canvas
import android.util.AttributeSet
import android.view.View
import com.kjipo.microphoneinput.ColourWheelRenderer

class SpectrumGraph(spectrumGraphModel: SpectrumGraphModel, context: Context?, attributeSet: AttributeSet?) : View(context, attributeSet) {
    private val spectrumGraphModel: SpectrumGraphModel

    init {
        this.spectrumGraphModel = spectrumGraphModel
    }


    private val viewRectangle: IntArray = IntArray(4)


    private fun getBoxesForLineTracks() {
        val width = viewRectangle[2] - viewRectangle[0]
        val height = viewRectangle[3] - viewRectangle[1]

       val numberOfLines = spectrumGraphModel.getNumberOfLines()

        (0 until numberOfLines).map {
            spectrumGraphModel.getMaxAndMinForLine(it)
        }.reduce { pair1, pair2 ->
            Pair(if(pair1.first < pair2.first) pair1.first else pair2.first,
            if(pair1.second < pair2.second) pair2.second else pair1.second)
        }
    }




    override fun onDraw(canvas: Canvas?) {
//        super.onDraw(canvas)

        canvas?.apply {

            for(i in 0 until spectrumGraphModel.getNumberOfLines()) {
                val points = spectrumGraphModel.getLastNumberOfPointsForLine(i)

                for(point in points) {

                    // TODO
//                    drawLine()


                }



            }


        }


    }

}