#ifndef __GRAYSCALE_BITMAP_H__
#define __GRAYSCALE_BITMAP_H__

#include <memory>
#include <vector>
#include <stdexcept>

extern "C" {
    #include "ft2build.h"
    #include FT_FREETYPE_H

    #include "SDL.h"
}

typedef unsigned char gray_pixel;
typedef std::vector<gray_pixel> pixels_vector;
typedef std::unique_ptr<pixels_vector> unique_pixels_ptr;

class GrayscaleBitmap {
public:
    explicit GrayscaleBitmap(const FT_Face);
    explicit GrayscaleBitmap(SDL_Surface*);

    GrayscaleBitmap(const GrayscaleBitmap&);

    virtual ~GrayscaleBitmap();

    const int16_t rows;
    const int16_t columns;
    const unique_pixels_ptr pixels;
    const uint8_t num_grays;

private:
    GrayscaleBitmap();
};

class FramedBitmap;
class FrameSlider;

class FramedBitmap : public GrayscaleBitmap {
public:
    FramedBitmap(const FramedBitmap&);
    FramedBitmap(SDL_Surface*);

    FrameSlider firstFrame(const int16_t width, const int16_t height);
    FrameSlider lastFrame( const int16_t width, const int16_t height);

private:
    FramedBitmap();
};

class FrameSlider {
public:
    FrameSlider(const FramedBitmap& _map,
                const int16_t _width, const int16_t _height,
                int16_t _leftBorderCol = 0, int16_t _topBorderRow = 0);

    virtual ~FrameSlider();

    void slide();
    gray_pixel at(int32_t pos) const;
    int32_t size() const;

    bool operator==(const FrameSlider&) const;
    bool operator!=(const FrameSlider&) const;

    bool newline;

private:
    const FramedBitmap* map;
    const int16_t width;
    const int16_t height;
    int16_t leftBorderCol;
    int16_t topBorderRow;


    FrameSlider();
};

#endif // __GRAYSCALE_BITMAP_H__
