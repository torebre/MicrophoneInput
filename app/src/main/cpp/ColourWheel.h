#ifndef MICROPHONEINPUT_COLOURWHEEL_H
#define MICROPHONEINPUT_COLOURWHEEL_H


#include <android/bitmap.h>
#include <vector>
#include <map>

class ColourWheel {

private:

    static const unsigned int NUMBER_OF_SEGMENTS = 100;
    static constexpr double HUE_CHANGE = 360.0 / NUMBER_OF_SEGMENTS;

    typedef struct {
        int x;
        int y;
    } Pixel;

    int numberOfRows;
    int numberOfColumns;
    int currentHighlight = -1;

    typedef struct {
        int h;
        float s;
        float v;
    } Hsv;

    Hsv *hsvColourLookupTable;
    uint32_t *colourLookupTable;

    Pixel** segmentPixelList;

    typedef std::map<int, std::vector<Pixel>> PixelMap;

    PixelMap segmentPixelsMap;

    uint32_t *currentPixelArray;

    static uint32_t hsvToArgb(int hue, float sat, float val);

    void createLookupTable();

    uint32_t *createSegmentColours(Hsv *hsvColourTable);

    void createPixelArray(int height, int width);



public:

    ColourWheel();

    ~ColourWheel();

    void setup(int numberOfRows, int numberOfColumns);

    void renderColourWheel(AndroidBitmapInfo* androidBitmapInfo, void* pixels, int segmentToHighlight);

    void highlightSegmentInternal(int segmentToHighlight, uint32_t colour, void* pixels);



};


#endif //MICROPHONEINPUT_COLOURWHEEL_H
