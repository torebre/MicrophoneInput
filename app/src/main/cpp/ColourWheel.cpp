#include "ColourWheel.h"

#include <cmath>


ColourWheel::~ColourWheel() {
    // TODO Check if it has been initialized
    delete segmentPixelList;
}


void ColourWheel::setup(int numberOfRows, int numberOfColumns) {
    // TODO






}

void ColourWheel::renderColourWheel(AndroidBitmapInfo* androidBitmapInfo, void* pixels, int segmentToHighlight) {

//    int y;
//    for(y = 0; y < androidBitmapInfo->height; ++y) {
//        int x;
//        for(x = 0; x < androidBitmapInfo->width; ++x) {
//
//
//        }
//
//        pixels = (char*)pixels + androidBitmapInfo->stride;
//    }


    if (currentHighlight != -1) {
        auto originalColour = hsvColourLookupTable[segmentToHighlight];
        auto colour = hsv(originalColour.h, originalColour.s, originalColour.v);

        highlightSegmentInternal(currentHighlight, colour, pixels);
    }

    currentHighlight = segmentToHighlight;

    auto originalColour = hsvColourLookupTable[segmentToHighlight];
    auto colourToUse = hsv(originalColour.h, 1.0, originalColour.v);

    highlightSegmentInternal(segmentToHighlight, colourToUse, &pixels);
}



void ColourWheel::highlightSegmentInternal(int segmentToHighlight, uint32_t colour, void* pixels) {
    auto segment = segmentPixelsMap[segmentToHighlight];
    uint32_t* castPixels = (uint32_t*)pixels;


    for (auto const iterator : segment) {
//        currentPixelArray[4 * iterator.x * numberOfColumns + 4 * iterator.y] = colour->r;
//        currentPixelArray[4 * iterator.x * numberOfColumns + 4 * iterator.y + 1] = colour->g;
//        currentPixelArray[4 * iterator.x * numberOfColumns + 4 * iterator.y + 2] = colour->b;
//        currentPixelArray[4 * iterator.x * numberOfColumns + 4 * iterator.y + 3] = colour->a;


        castPixels[4 * iterator.x * numberOfColumns + 4 * iterator.y] = colour;


    }
}



// hue: 0-360°; sat: 0.f-1.f; val: 0.f-1.f
uint32_t ColourWheel::hsv(int hue, float sat, float val) {
    hue %= 360;
    while (hue < 0) hue += 360;

    if (sat < 0.f) sat = 0.f;
    if (sat > 1.f) sat = 1.f;

    if (val < 0.f) val = 0.f;
    if (val > 1.f) val = 1.f;

    int h = hue / 60;
    float f = float(hue) / 60 - h;
    float p = val * (1.f - sat);
    float q = val * (1.f - sat * f);
    float t = val * (1.f - sat * (1 - f));

    uint32_t colour;

    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha = 255;

    switch (h) {
        default:
        case 0:
        case 6:
            red = val * 255;
            green = t * 255;
            blue = p * 255;
            break;

        case 1:
            red = q * 255;
            green = val * 255;
            blue = p * 255;
            break;

        case 2:
            red = p * 255;
            green = val * 255;
            blue = t * 255;
            break;

        case 3:
            red = p * 255;
            green = q * 255;
            blue = val * 255;
            break;

        case 4:
            red = t * 255;
            green = p * 255;
            blue = val * 255;
            break;

        case 5:
            red = val * 255;
            green = p * 255;
            blue = q * 255;
            break;
    }

    colour = red;
    colour = colour << 8;
    colour += green;
    colour = colour << 8;
    colour += blue;
    colour = colour << 8;
    colour += alpha;

    return colour;
}
