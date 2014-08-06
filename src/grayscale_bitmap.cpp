#include <stdexcept>
#include "grayscale_bitmap.h"

typedef std::vector<gray_pixel> pixels_vector;

GrayscaleBitmap::GrayscaleBitmap(const FT_Bitmap& ftBitmap)
    : rows(ftBitmap.rows), columns(ftBitmap.width)
    , pixels(new pixels_vector(ftBitmap.buffer, ftBitmap.buffer + rows*columns))
    , num_grays(ftBitmap.num_grays) {

    if (ftBitmap.pixel_mode != FT_PIXEL_MODE_GRAY) {
        throw std::runtime_error("Given Freetype bitmap is not 8-bit grayscale");
    }
}

static inline uint8_t convert24BitRgbPixelToGrayscale(const uint8_t* rgbPixel) {
    uint8_t r = *(rgbPixel + 0);
    uint8_t g = *(rgbPixel + 1);
    uint8_t b = *(rgbPixel + 2);
    uint8_t grayLevel =   0.212671f * r
                        + 0.715160f * g
                        + 0.072169f * b;
    return grayLevel;
}

static const uint8_t MAX_GRAY_LEVELS = 255;
GrayscaleBitmap::GrayscaleBitmap(SDL_Surface* surface)
    : rows(surface->h), columns(surface->w)
    , pixels(new pixels_vector(surface->h * surface->w, 0))
    , num_grays(MAX_GRAY_LEVELS) {

    static const uint32_t UNUSED_FLAGS = 0;
    SDL_Surface* surfaceCopy = SDL_ConvertSurface(surface,  surface->format,
                                                            surface->flags);
    surfaceCopy = SDL_ConvertSurfaceFormat( surfaceCopy,
                                            SDL_PIXELFORMAT_RGB888,
                                            UNUSED_FLAGS);

    static const uint8_t RGB_PIXEL_SIZE = 3;
    for (int16_t thisRow = 0; thisRow < rows; ++thisRow) {
        for (int16_t thisColumn = 0; thisColumn < columns; ++thisColumn) {
            uint8_t* rgbPixel = (uint8_t*)surfaceCopy->pixels
                                        + thisRow * rows
                                        + thisColumn * RGB_PIXEL_SIZE;
            (*pixels)[thisRow*rows + thisColumn]
                = convert24BitRgbPixelToGrayscale(rgbPixel);
        }
    }

    SDL_FreeSurface(surfaceCopy);
}

GrayscaleBitmap::GrayscaleBitmap(const GrayscaleBitmap& toCopy)
    : rows(toCopy.rows), columns(toCopy.columns)
    , pixels(new pixels_vector(toCopy.pixels->begin(), toCopy.pixels->end()))
    , num_grays(toCopy.num_grays) {}

GrayscaleBitmap::~GrayscaleBitmap() {}


FramedBitmap::FramedBitmap(SDL_Surface* surface) : GrayscaleBitmap(surface) {}
FramedBitmap::FramedBitmap(const FramedBitmap& map) : GrayscaleBitmap(map) {}

FrameSlider FramedBitmap::firstFrame(const int16_t width, const int16_t height) {
    return FrameSlider(*this, width, height);
}

FrameSlider FramedBitmap::lastFrame(const int16_t width, const int16_t height) {
    int16_t leftBorderCol = columns - width - columns % width;
    int16_t topBorderRow = rows - height - rows % height;

    return FrameSlider(*this, width, height, leftBorderCol, topBorderRow);
}


FrameSlider::FrameSlider(const FramedBitmap& _map,
                        const int16_t _width, const int16_t _height,
                        int16_t _leftBorderCol, int16_t _topBorderRow)
    : newline(false), map(&_map), width(_width), height(_height)
    , leftBorderCol(_leftBorderCol), topBorderRow(_topBorderRow) {

    if (    width >= map->columns
        ||  height >= map->rows
        ||  leftBorderCol + width >= map->columns
        ||  topBorderRow + height >= map->rows) {
        throw std::out_of_range("Specified frame is out of given bitmap borders");
    }
}

FrameSlider::~FrameSlider() {}

void FrameSlider::slide() {
    int16_t newLeftBorder, newTopBorder;

    int16_t frameToRightColPos = leftBorderCol + width;
    int16_t frameBelowRowPos = topBorderRow + height;

    if (frameToRightColPos + width < map->columns) {
        newline = false;
        newLeftBorder = frameToRightColPos;
        newTopBorder = topBorderRow;
    }
    else if (frameBelowRowPos + height < map->rows) {
        newline = true;
        newLeftBorder = 0;
        newTopBorder = frameBelowRowPos;
    }
    else {
        throw std::out_of_range("Can't slide out of bitmap range");
    }

    leftBorderCol = newLeftBorder;
    topBorderRow = newTopBorder;
}

gray_pixel FrameSlider::at(int32_t pos) const {
    if (pos >= width * height) {
        throw std::out_of_range("Out of frame borders");
    }

    int16_t mapRow = topBorderRow + pos / width;
    int16_t mapColumn = leftBorderCol + pos % width;

    return map->pixels->at(mapRow * map->columns + mapColumn);
}

bool FrameSlider::operator==(const FrameSlider& toCompare) const {
    return     map == toCompare.map
            && width == toCompare.width
            && height == toCompare.height
            && leftBorderCol == toCompare.leftBorderCol
            && topBorderRow == toCompare.topBorderRow;
}

bool FrameSlider::operator!=(const FrameSlider& toCompare) const {
    return !operator==(toCompare);
}
