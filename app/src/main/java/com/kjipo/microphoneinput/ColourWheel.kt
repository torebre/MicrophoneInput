package com.kjipo.microphoneinput

import android.content.Context
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Color
import android.util.AttributeSet
import android.util.Log
import android.view.View
import kotlin.math.*

class ColourWheel(context: Context?, attributeSet: AttributeSet?) : View(context, attributeSet) {
    private var currentHighlight = -1
    private var pixels: IntArray
    private var segmentPixelList: Pair<MutableList<IntArray>, MutableList<IntArray>>

    private val bitmapHeight = 100
    private val bitmapWidth = 100

    private val bitmap = Bitmap.createBitmap(bitmapWidth, bitmapHeight, Bitmap.Config.ARGB_8888, true)


    init {
        segmentPixelList = transformMap(drawWheel(bitmapHeight, bitmapWidth))
        pixels = createBitmapPixels(bitmapWidth / 2, bitmapHeight / 2, bitmapHeight, bitmapWidth, segmentPixelList)
    }

    override fun onDraw(canvas: Canvas?) {
//        super.onDraw(canvas)

        Log.i("Test24", "About to render colour wheel")
        ColourWheelRenderer.renderColourWheel(bitmap, currentHighlight)

        Log.i("Test24", "Drawing bitmap")

        canvas?.drawBitmap(bitmap, 0f, 0f, null)


    }

    internal fun updateHighlight(updatedData: Pair<Double, Double>) {
        if (updatedData.second < 0.9) {
//            updateBitmapPixels(bitmapWidth / 2, bitmapHeight / 2, pixels, bitmapWidth, segmentPixelList, -1, currentHighlight)
            currentHighlight = -1

            return
        }

        // TODO Make proper computation
        val octaves = abs(log2(updatedData.first / 440))
        val decimalPlaces = octaves - floor(octaves)
        val previousHighlight = currentHighlight
        currentHighlight = (decimalPlaces * NUMBER_OF_SEGMENTS).roundToInt() - 1

//        updateBitmapPixels(bitmapWidth / 2, bitmapHeight / 2, pixels, bitmapWidth, segmentPixelList, currentHighlight, previousHighlight)

//        Log.i("ColourWheel", "Current highlight: $currentHighlight")

//        bitmap.setPixels(pixels, 0, bitmapWidth, 0, 0, bitmapWidth, bitmapHeight)
        invalidate()



    }


    companion object {
        private const val NUMBER_OF_SEGMENTS = 100

        private val background = Color.valueOf(0f, 0f, 0f, 0f).toArgb()


        internal fun drawWheel(rows: Int, columns: Int): MutableMap<Int, MutableList<Pair<Int, Int>>> {
            val centerX = rows / 2
            val centerY = columns / 2

            val angleSegment = 2 * Math.PI / NUMBER_OF_SEGMENTS
            val segmentPixelsMap = mutableMapOf<Int, MutableList<Pair<Int, Int>>>()

            for (row in 0 until rows) {
                for (column in 0 until columns) {
                    val xDiff = column - centerX
                    val yDiff = row - centerY

                    var angle = atan2(yDiff.toDouble(), xDiff.toDouble())
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


        internal fun createBitmapPixels(centerX: Int, centerY: Int, bitmapHeight: Int, bitmapWidth: Int, segmentPixelList: Pair<MutableList<IntArray>, MutableList<IntArray>>): IntArray {
            val rowIterator = segmentPixelList.first.iterator()
            val columnIterator = segmentPixelList.second.iterator()
            val pixelArray = IntArray(bitmapWidth * bitmapHeight) { 0 }
            val hueChange = 360 / segmentPixelList.first.size
            var hueValue = 0


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
                    pixelArray[xCoord * bitmapWidth + yCoord] = if (distanceToCenter > 500 || distanceToCenter < 20) {
                        background
                    } else {
                        Color.HSVToColor(255, floatArrayOf(hueValue.toFloat(), 1f, 0.7f))
                    }
                }
                hueValue += hueChange

            }

            return pixelArray
        }


        internal fun updateBitmapPixels(centerX: Int, centerY: Int, pixelArray: IntArray, bitmapWidth: Int, segmentPixelList: Pair<MutableList<IntArray>, MutableList<IntArray>>, currentHighlight: Int, previousHighlight: Int) {
            if (previousHighlight != -1) {
                val hueChange = 360f / segmentPixelList.first.size
                val hueValue = hueChange * previousHighlight
                setHighlight(centerX, centerY, hueValue, bitmapWidth, previousHighlight, pixelArray, segmentPixelList, 0.7f)
            }

            if (currentHighlight != -1) {
                val hueChange = 360f / segmentPixelList.first.size
                val hueValue = hueChange * currentHighlight
                setHighlight(centerX, centerY, hueValue, bitmapWidth, currentHighlight, pixelArray, segmentPixelList, 1.0f)
            }

        }

        private fun setHighlight(centerX: Int, centerY: Int, hueValue: Float, bitmapWidth: Int, rowToHighlight: Int, pixelArray: IntArray, segmentPixelList: Pair<MutableList<IntArray>, MutableList<IntArray>>, value: Float) {
            val xCoordItr = segmentPixelList.first[rowToHighlight].iterator()
            val yCoordItr = segmentPixelList.second[rowToHighlight].iterator()


            while (xCoordItr.hasNext()) {
                val xCoord = xCoordItr.nextInt()
                val yCoord = yCoordItr.nextInt()
                val distanceToCenter = sqrt(abs(xCoord - centerX).toDouble().pow(2) + abs(yCoord - centerY).toDouble().pow(2))

                // TODO Set proper cutoffs
                pixelArray[xCoord * bitmapWidth + yCoord] = if (distanceToCenter > 500 || distanceToCenter < 20) {
                    background
                } else {
                    Color.HSVToColor(255, floatArrayOf(hueValue, 1f, value))
                }
            }


        }

    }


}
