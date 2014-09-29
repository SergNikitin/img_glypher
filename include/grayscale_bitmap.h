#ifndef __GRAYSCALE_BITMAP_H__
#define __GRAYSCALE_BITMAP_H__

#include <memory>
#include <vector>

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

    const size_t rows;
    const size_t columns;
    const unique_pixels_ptr pixels;
    const uint_fast8_t num_grays;

private:
    GrayscaleBitmap();
};

class FramedBitmap;
class FrameSlider;

class FramedBitmap : public GrayscaleBitmap {
public:
    FramedBitmap(const FramedBitmap&);
    FramedBitmap(SDL_Surface*);

    FrameSlider       firstFrame(const size_t width, const size_t height) const;
    const FrameSlider lastFrame( const size_t width, const size_t height) const;

private:
    FramedBitmap();
};

class FrameSlider {
public:
    FrameSlider(const FramedBitmap& _map,
                const size_t _width, const size_t _height,
                size_t _leftBorderCol = 0, size_t _topBorderRow = 0);

    virtual ~FrameSlider();

    void slide();
    gray_pixel at(size_t pos) const;
    size_t size() const;

    bool operator==(const FrameSlider&) const;
    bool operator!=(const FrameSlider&) const;

    bool newline;

private:
    const FramedBitmap* map;
    const size_t width;
    const size_t height;
    size_t leftBorderCol;
    size_t topBorderRow;


    FrameSlider();
};

#endif // __GRAYSCALE_BITMAP_H__
