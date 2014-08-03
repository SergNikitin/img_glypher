#include "grayscale_bitmap.h"

typedef std::vector<unsigned char> pixels_vector;

GrayscaleBitmap::GrayscaleBitmap(FT_Bitmap& ftBitmap)
    : rows(ftBitmap.rows), columns(ftBitmap.width)
    , pixels(new pixels_vector(ftBitmap.buffer, ftBitmap.buffer + rows*columns))
    , num_grays(ftBitmap.num_grays) {

    if (ftBitmap.pixel_mode != FT_PIXEL_MODE_GRAY) {
        throw std::runtime_error("Given Freetype bitmap is not 8-bit grayscale");
    }
}

static inline uint8_t convert24BitRgbPixelToGrayscale(uint8_t* rgbPixel) {
    uint8_t r = *(rgbPixel + 0);
    uint8_t g = *(rgbPixel + 1);
    uint8_t b = *(rgbPixel + 2);
    uint8_t grayLevel =   0.212671f * r
                        + 0.715160f * g
                        + 0.072169f * b;
    return grayLevel;
}

static const uint8_t MAX_GRAY_LEVELS = 255;
GrayscaleBitmap::GrayscaleBitmap(SDL_Surface& givenSurface)
    : rows(givenSurface.h), columns(givenSurface.w)
    , pixels(new pixels_vector(givenSurface.h * givenSurface.w, 0))
    , num_grays(MAX_GRAY_LEVELS) {

    static const uint32_t UNUSED_FLAGS = 0;
    SDL_Surface* surface = SDL_ConvertSurfaceFormat(&givenSurface,
                                                    SDL_PIXELFORMAT_RGB888,
                                                    UNUSED_FLAGS);

    static const uint8_t RGB_PIXEL_SIZE = 3;
    for (int16_t thisRow = 0; thisRow < rows; ++thisRow) {
        for (int16_t thisColumn = 0; thisColumn < columns; ++columns) {
            uint8_t* rgbPixel = (uint8_t*)surface->pixels
                                        + thisRow * rows
                                        + thisColumn * RGB_PIXEL_SIZE;
            (*pixels)[thisRow*rows + thisColumn]
                = convert24BitRgbPixelToGrayscale(rgbPixel);
        }
    }
}

GrayscaleBitmap::GrayscaleBitmap(GrayscaleBitmap& toCopy)
    : rows(toCopy.rows), columns(toCopy.columns)
    , pixels(new pixels_vector(toCopy.pixels->begin(), toCopy.pixels->end()))
    , num_grays(toCopy.num_grays) {}

GrayscaleBitmap::~GrayscaleBitmap() {}
