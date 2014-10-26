#ifndef __GRAYSCALE_BITMAP_H__
#define __GRAYSCALE_BITMAP_H__

#include <memory>
#include <vector>

extern "C" {
    #include "ft2build.h"
    #include FT_FREETYPE_H

    #include "SDL.h"
}

const uint_fast8_t MAX_GRAY_LEVELS = 255;

typedef unsigned char obj_brightness;
typedef std::vector<obj_brightness> pixels_vector;
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
};

class FrameSlider;

class FramedBitmap : public GrayscaleBitmap {
public:
    explicit FramedBitmap(SDL_Surface*);
    FramedBitmap(const FramedBitmap&);

    FrameSlider       firstFrame()  const;
    const FrameSlider lastFrame()   const;

    void setFrameSize(size_t width, size_t height);
    size_t countFrames() const;

    size_t frameWidth;
    size_t frameHeight;
};

class FrameSlider {
public:
    FrameSlider(const FramedBitmap& _map,
                const size_t _width, const size_t _height,
                size_t _leftBorderCol = 0, size_t _topBorderRow = 0);

    void slide();
    obj_brightness at(size_t pos) const;
    size_t size() const;

    bool operator==(const FrameSlider&) const;
    bool operator!=(const FrameSlider&) const;

private:
    const FramedBitmap* map;
    const size_t width;
    const size_t height;
    size_t leftBorderCol;
    size_t topBorderRow;
};

#endif // __GRAYSCALE_BITMAP_H__
