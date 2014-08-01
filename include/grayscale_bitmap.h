#ifndef __GRAYSCALE_BITMAP_H__
#define __GRAYSCALE_BITMAP_H__

#include <memory>
#include <vector>
#include <stdexcept>

#include "ft2build.h"
#include FT_IMAGE_H

#include "SDL.h"

typedef std::unique_ptr<std::vector<unsigned char>> unique_pixels_ptr;

class GrayscaleBitmap {
public:
     explicit GrayscaleBitmap(FT_Bitmap&);
     explicit GrayscaleBitmap(SDL_Surface&);

    GrayscaleBitmap(GrayscaleBitmap&);

    virtual ~GrayscaleBitmap();

    int16_t rows;
    int16_t columns;
    const unique_pixels_ptr pixels;
    uint8_t num_grays;

private:
    GrayscaleBitmap();
};

#endif
