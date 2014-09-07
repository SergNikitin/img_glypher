#include <exception>
#include "grayscale_bitmap.h"

typedef std::vector<gray_pixel> pixels_vector;

#define FIXED_POINT_26_6_COEFF 64
GrayscaleBitmap::GrayscaleBitmap(const FT_Face fontFace)
    : rows(fontFace->size->metrics.height / FIXED_POINT_26_6_COEFF)
    , columns(fontFace->size->metrics.max_advance / FIXED_POINT_26_6_COEFF)
    , pixels(new pixels_vector(rows*columns, 0))
    , num_grays(fontFace->glyph->bitmap.num_grays) {

    int16_t baselineRow         = fontFace->size->metrics.ascender
                                    / FIXED_POINT_26_6_COEFF;
    int16_t leftBearing         = fontFace->glyph->bitmap_left;
    int16_t fromTopToSymbol     = baselineRow - fontFace->glyph->bitmap_top;
    const FT_Bitmap* ftBitmap   = &fontFace->glyph->bitmap;

    for (int16_t symbolRow = 0; symbolRow < ftBitmap->rows; ++symbolRow) {
        for (int16_t symbolCol = 0; symbolCol < ftBitmap->width; ++symbolCol) {
            int16_t bitmapRow = fromTopToSymbol + symbolRow;
            int16_t bitmapCol = leftBearing + symbolCol;

            pixels->at(bitmapRow*columns + bitmapCol)
                = ftBitmap->buffer[symbolRow*ftBitmap->width + symbolCol];
        }
    }
}

static const uint8_t MAX_GRAY_LEVELS = 255;
static inline uint8_t rgbPixelToGrayscale(  uint32_t rgbPixel,
                                            const SDL_PixelFormat* fmt) {
    uint8_t r = ((rgbPixel & fmt->Rmask) >> fmt->Rshift) << fmt->Rloss;
    uint8_t g = ((rgbPixel & fmt->Gmask) >> fmt->Gshift) << fmt->Gloss;
    uint8_t b = ((rgbPixel & fmt->Bmask) >> fmt->Bshift) << fmt->Bloss;

    uint8_t grayLevel =   0.212671f * r
                        + 0.715160f * g
                        + 0.072169f * b;

    return MAX_GRAY_LEVELS - grayLevel;
}

GrayscaleBitmap::GrayscaleBitmap(SDL_Surface* surface)
    : rows(surface->h), columns(surface->w)
    , pixels(new pixels_vector(surface->h * surface->w, 0))
    , num_grays(MAX_GRAY_LEVELS) {

    static const uint32_t UNUSED_FLAGS = 0;
    SDL_Surface* surfaceCopy = SDL_ConvertSurfaceFormat(surface,
                                                        SDL_PIXELFORMAT_RGB888,
                                                        UNUSED_FLAGS);

    SDL_LockSurface(surfaceCopy);

    const uint8_t bytesPerPixel = surfaceCopy->format->BytesPerPixel;
    for (int16_t thisRow = 0; thisRow < rows; ++thisRow) {
        for (int16_t thisColumn = 0; thisColumn < columns; ++thisColumn) {
            uint32_t pixelNum = thisRow * columns + thisColumn;
            uint8_t* pixelDataStart = (uint8_t*)surfaceCopy->pixels
                                                + pixelNum * bytesPerPixel;
            uint32_t rgbPixel = *(uint32_t*)pixelDataStart;
            uint8_t grayLevel = rgbPixelToGrayscale(rgbPixel,
                                                    surfaceCopy->format);
            pixels->at(pixelNum) = grayLevel;
        }
    }

    SDL_UnlockSurface(surfaceCopy);
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

int32_t FrameSlider::size() const {
    return (int32_t)width * height;
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
