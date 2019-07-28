package com.kjipo.microphoneinput

import android.content.Context
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Color
import android.util.AttributeSet
import android.view.View
import kotlin.math.abs
import kotlin.math.pow
import kotlin.math.sqrt

class ColourWheel(context: Context?, attributeSet: AttributeSet?) : View(context, attributeSet) {
    private val bitmapHeight = 500
    private val bitmapWidth = 500

    private val bitmap = Bitmap.createBitmap(bitmapWidth, bitmapHeight, Bitmap.Config.ARGB_8888, true)


    init {
        val pixels = createBitmapPixels(bitmapWidth / 2, bitmapHeight / 2, bitmapHeight, bitmapWidth)
        bitmap.setPixels(pixels, 0, bitmapWidth, 0, 0, bitmapWidth, bitmapHeight)
    }

    override fun onDraw(canvas: Canvas?) {
        super.onDraw(canvas)

        canvas?.drawBitmap(bitmap, 0.0.toFloat(), 0.0.toFloat(), null)

    }


    companion object {
        private const val NUMBER_OF_SEGMENTS = 100

        internal fun drawWheel(rows: Int, columns: Int): MutableMap<Int, MutableList<Pair<Int, Int>>> {
            val centerX = rows / 2
            val centerY = columns / 2

            val angleSegment = 2 * Math.PI / NUMBER_OF_SEGMENTS
            val segmentPixelsMap = mutableMapOf<Int, MutableList<Pair<Int, Int>>>()

            for (row in 0 until rows) {
                for (column in 0 until columns) {
                    val xDiff = column - centerX
                    val yDiff = row - centerY

                    var angle = Math.atan2(yDiff.toDouble(), xDiff.toDouble())
                    if (angle < 0) {
                        angle += 2 * Math.PI
                    }
                    val segment = angle.div(angleSegment).toInt()

                    if (!segmentPixelsMap.containsKey(segment)) {
                        segmentPixelsMap.putIfAbsent(segment, mutableListOf(Pair(row, column)))
                    } else {
                        segmentPixelsMap[segment]?.add(Pair(row, column))
                    }
                }
            }

            return segmentPixelsMap
        }


        internal fun transformMap(segmentPixelMap: MutableMap<Int, MutableList<Pair<Int, Int>>>): Pair<MutableList<IntArray>, MutableList<IntArray>> {
            val segments = segmentPixelMap.keys.sorted()

            val angleSegmentRows = mutableListOf<IntArray>()
            val angleSegmentColumns = mutableListOf<IntArray>()

            for (segment in segments) {
                val pixels = segmentPixelMap[segment] ?: continue
                val rows = IntArray(pixels.size, { position ->
                    pixels[position].first
                })
                angleSegmentRows.add(rows)

                val columns = IntArray(pixels.size, { position ->
                    pixels[position].second
                })
                angleSegmentColumns.add(columns)
            }

            return Pair(angleSegmentRows, angleSegmentColumns)
        }


        internal fun createBitmapPixels(centerX: Int, centerY: Int, bitmapHeight: Int, bitmapWidth: Int): IntArray {
            val segmentPixelList = transformMap(drawWheel(bitmapHeight, bitmapWidth))

            val rowIterator = segmentPixelList.first.iterator()
            val columnIterator = segmentPixelList.second.iterator()
            val pixelArray = IntArray(bitmapWidth * bitmapHeight) { 0 }
            val hueChange = 360 / segmentPixelList.first.size
            var hueValue = 0
            val background = Color.valueOf(0f, 0f, 0f, 0f).toArgb()

            while (rowIterator.hasNext()) {

                val row = rowIterator.next()
                val column = columnIterator.next()

                val xCoordItr = row.iterator()
                val yCoordItr = column.iterator()

                while (xCoordItr.hasNext()) {
                    val xCoord = xCoordItr.nextInt()
                    val yCoord = yCoordItr.nextInt()
                    val distanceToCenter = sqrt(abs(xCoord - centerX).toDouble().pow(2) + abs(yCoord - centerY).toDouble().pow(2))

                    // TODO Set proper cutoffs
                    pixelArray[xCoord * bitmapWidth + yCoord] = if (distanceToCenter > 100 || distanceToCenter < 20) {
                        background
                    } else {
                        Color.HSVToColor(255, floatArrayOf(hueValue.toFloat(), 1f, 1f))
                    }
                }
                hueValue += hueChange

            }

            return pixelArray
        }

    }


}
