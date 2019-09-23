#ifndef MICROPHONEINPUT_COLOURWHEEL_H
#define MICROPHONEINPUT_COLOURWHEEL_H


class ColourWheel {

private:

    typedef struct {
        int x;
        int y;
    } Pixel;

    Pixel** segmentPixelList;


public:

    ~ColourWheel();

    void setup(int numberOfRows, int numberOfColumns);



};


#endif //MICROPHONEINPUT_COLOURWHEEL_H
