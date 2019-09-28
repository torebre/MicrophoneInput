#include "ColourWheel.h"

#include <cmath>
#include <android/log.h>



ColourWheel::ColourWheel() {
    createLookupTable();
}


ColourWheel::~ColourWheel() {
    // TODO Check if it has been initialized
    delete segmentPixelList;
}


void ColourWheel::setup(int numberOfRows, int numberOfColumns) {

    __android_log_print(ANDROID_LOG_INFO,"Test23", "Setup. Height: %d. Width: %d", numberOfRows, numberOfColumns);

    this->numberOfRows = numberOfRows;
    this->numberOfColumns = numberOfColumns;

    int centerX = numberOfRows / 2;
    int centerY = numberOfColumns / 2;
    double angleSegment = 2 * M_PI / NUMBER_OF_SEGMENTS;

    for (int row = 0; row < numberOfRows; ++row) {
        for (int column = 0; column < numberOfColumns; ++column) {
            int xDiff = column - centerX;
            int yDiff = row - centerY;

            double angle = atan2(yDiff, xDiff);

            if (angle < 0) {
                angle += 2 * M_PI;
            }

            int segment = angle / angleSegment;
            auto pixelIterator = segmentPixelsMap.find(segment);

            if (pixelIterator == segmentPixelsMap.end()) {
                std::vector<Pixel> pixelVector = std::vector<Pixel>();
                pixelVector.push_back(Pixel{row, column});
                segmentPixelsMap[segment] = pixelVector;
            } else {
                pixelIterator->second.push_back(Pixel{row, column});
            }
        }
    }

    createPixelArray(numberOfRows, numberOfColumns);
}



void ColourWheel::createPixelArray(int height, int width) {
    auto mapSize = segmentPixelsMap.size();

    __android_log_print(ANDROID_LOG_INFO,"Test23", "Setting up pixel array. Height: %d. Width: %d. Size: %d", height, width, height * width);

    currentPixelArray = new uint32_t[height * width];

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            currentPixelArray[i * width + j] = 0;
        }
    }

    for (unsigned long i = 0; i < mapSize; ++i) {
        auto colour = colourLookupTable[i];

        for (auto const iterator : segmentPixelsMap[i]) {
            currentPixelArray[iterator.x * width + iterator.y] = colour;
        }
    }

//    currentPixelArray = segmentPixelColours;
}



void ColourWheel::createLookupTable() {
    double hue = 0;
    auto lookupTable = new ColourWheel::Hsv[NUMBER_OF_SEGMENTS];

    for (int i = 0; i < NUMBER_OF_SEGMENTS; ++i) {
        hue += HUE_CHANGE;
        lookupTable[i] = Hsv{(int) round(hue), 0.5, 1.0};
    }

    hsvColourLookupTable = lookupTable;

    __android_log_print(ANDROID_LOG_INFO,"Test23", "Setting up colour lookup table");

    colourLookupTable = createSegmentColours(hsvColourLookupTable);
}


uint32_t *ColourWheel::createSegmentColours(Hsv *hsvColourTable) {
    double hue = 0;
    auto argbLookupTable = new uint32_t[NUMBER_OF_SEGMENTS];

    for (int i = 0; i < NUMBER_OF_SEGMENTS; ++i) {
        hue += HUE_CHANGE;
        argbLookupTable[i] = hsvToArgb(hsvColourTable[i].h, hsvColourTable[i].s,
                                       hsvColourTable[i].v);
    }

    return argbLookupTable;
}

void ColourWheel::renderColourWheel(AndroidBitmapInfo* androidBitmapInfo, void* pixels, int segmentToHighlight) {
    if(androidBitmapInfo->height != numberOfRows || androidBitmapInfo->width != numberOfColumns) {
        __android_log_print(ANDROID_LOG_INFO,"Test23", "Setting up dimensions. Width: %d. Height: %d", +androidBitmapInfo->width, androidBitmapInfo->height);

        setup(androidBitmapInfo->height, androidBitmapInfo->width);
    }

    if (currentHighlight != -1) {
        auto originalColour = hsvColourLookupTable[segmentToHighlight];
        auto colour = hsvToArgb(originalColour.h, originalColour.s, originalColour.v);

        highlightSegmentInternal(currentHighlight, colour, &pixels);
    }

    currentHighlight = segmentToHighlight;

    if(currentHighlight == -1) {
        // Nothing to highlight
        return;
    }

    auto originalColour = hsvColourLookupTable[segmentToHighlight];
    auto colourToUse = hsvToArgb(originalColour.h, 1.0, originalColour.v);

    __android_log_print(ANDROID_LOG_INFO,"Test23", "Width: %d. Height: %d", +androidBitmapInfo->width, androidBitmapInfo->height);


    highlightSegmentInternal(segmentToHighlight, colourToUse, &pixels);
}



void ColourWheel::highlightSegmentInternal(int segmentToHighlight, uint32_t colour, void* pixels) {
    auto segment = segmentPixelsMap[segmentToHighlight];
    auto castPixels = (uint32_t*)pixels;


    int counter = 0;

    __android_log_print(ANDROID_LOG_INFO,"Test23", "Starting");

    for (auto const iterator : segment) {
//        currentPixelArray[4 * iterator.x * numberOfColumns + 4 * iterator.y] = colour->r;
//        currentPixelArray[4 * iterator.x * numberOfColumns + 4 * iterator.y + 1] = colour->g;
//        currentPixelArray[4 * iterator.x * numberOfColumns + 4 * iterator.y + 2] = colour->b;
//        currentPixelArray[4 * iterator.x * numberOfColumns + 4 * iterator.y + 3] = colour->a;


        castPixels[iterator.x * numberOfColumns + iterator.y] = colour;


//        __android_log_print(ANDROID_LOG_INFO,"Test23", "x: %d. y: %d. Number of columns: %d", iterator.x, iterator.y, numberOfColumns);

//        castPixels[iterator.x * numberOfColumns + iterator.y] = 0;


    }


    __android_log_print(ANDROID_LOG_INFO,"Test23", "Done");

}



// hue: 0-360°; sat: 0.f-1.f; val: 0.f-1.f
uint32_t ColourWheel::hsvToArgb(int hue, float sat, float val) {
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
