package com.kjipo.microphoneinput

import android.content.Context
import android.graphics.Bitmap
import android.graphics.Canvas
import android.util.AttributeSet
import android.view.View

class ColourWheel(context: Context?, attributeSet: AttributeSet?) : View(context, attributeSet) {
    val bitmap = Bitmap.createBitmap(200, 200, Bitmap.Config.RGB_565, true)


    init {

        val pixels = IntArray(500, {
            801919
        })

        bitmap.setPixels(pixels, 0, 0, 0, 0, 0, 0)
    }

    override fun onDraw(canvas: Canvas?) {
        super.onDraw(canvas)

        canvas?.drawBitmap(bitmap, 0.0.toFloat(), 0.0.toFloat(), null)

    }


}
