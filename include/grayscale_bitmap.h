#ifndef __GRAYSCALE_BITMAP_H__
#define __GRAYSCALE_BITMAP_H__

#include <memory>
#include <vector>
#include <stdexcept>

#include "ft2build.h"
#include FT_IMAGE_H

#include "SDL.h"

typedef unsigned char gray_pixel;
typedef std::unique_ptr<std::vector<gray_pixel>> unique_pixels_ptr;

class GrayscaleBitmap {
public:
    explicit GrayscaleBitmap(const FT_Bitmap&);
    explicit GrayscaleBitmap(SDL_Surface&);

    GrayscaleBitmap(const GrayscaleBitmap&);

    virtual ~GrayscaleBitmap();

    int16_t rows;
    int16_t columns;
    const unique_pixels_ptr pixels;
    uint8_t num_grays;

private:
    GrayscaleBitmap();
};

class FramedBitmap;
class FrameSlider;

class FramedBitmap : public GrayscaleBitmap {
public:
    FrameSlider firstFrame(const int16_t width, const int16_t height);
    FrameSlider lastFrame( const int16_t width, const int16_t height);
};

class FrameSlider {
public:
    FrameSlider(const FramedBitmap& _map,
                const int16_t _width, const int16_t _height,
                int16_t _leftBorderCol = 0, int16_t _topBorderRow = 0);

    virtual ~FrameSlider();

    void slide();
    gray_pixel at(int16_t pos) const;

    bool operator==(const FrameSlider&) const;
    bool operator!=(const FrameSlider&) const;

private:
    const FramedBitmap* map;
    const int16_t width;
    const int16_t height;
    int16_t leftBorderCol;
    int16_t topBorderRow;

    FrameSlider();
};

#endif
