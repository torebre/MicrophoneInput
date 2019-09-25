#ifndef MICROPHONEINPUT_COLOURWHEEL_H
#define MICROPHONEINPUT_COLOURWHEEL_H


#include <android/bitmap.h>
#include <vector>
#include <map>

class ColourWheel {

private:

    typedef struct {
        int x;
        int y;
    } Pixel;

    int numberOfRows;
    int numberOfColumns;
    int currentHighlight = -1;

    typedef struct {
        double h;
        double s;
        double v;
    } Hsv;

    Hsv *hsvColourLookupTable;

    Pixel** segmentPixelList;

    typedef std::map<int, std::vector<Pixel>> PixelMap;

    PixelMap segmentPixelsMap;

    static uint32_t hsv(int hue, float sat, float val);


public:

    ~ColourWheel();

    void setup(int numberOfRows, int numberOfColumns);

    void renderColourWheel(AndroidBitmapInfo* androidBitmapInfo, void* pixels, int segmentToHighlight);

    void highlightSegmentInternal(int segmentToHighlight, uint32_t colour, void* pixels);



};


#endif //MICROPHONEINPUT_COLOURWHEEL_H
