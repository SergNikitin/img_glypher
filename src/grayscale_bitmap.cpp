#include <exception>
#include "grayscale_bitmap.h"

static const uint32_t FIXED_POINT_26_6_COEFF = 1<<6;
GrayscaleBitmap::GrayscaleBitmap(const FT_Face fontFace)
    : rows(fontFace->size->metrics.height / FIXED_POINT_26_6_COEFF)
    , columns(fontFace->size->metrics.max_advance / FIXED_POINT_26_6_COEFF)
    , pixels(new pixels_vector(rows*columns, MAX_GRAY_LEVELS))
    , num_grays(fontFace->glyph->bitmap.num_grays) {

    size_t baselineRow         = fontFace->size->metrics.ascender
                                    / FIXED_POINT_26_6_COEFF;
    size_t leftBearing         = fontFace->glyph->bitmap_left;
    size_t fromTopToSymbol     = baselineRow - fontFace->glyph->bitmap_top;
    const FT_Bitmap& ftBitmap  = fontFace->glyph->bitmap;
    const size_t vectorSize    = pixels->size();

    size_t symbolRows = static_cast<size_t>(ftBitmap.rows);
    size_t symbolCols = static_cast<size_t>(ftBitmap.width);

    for (size_t symbolRow = 0; symbolRow < symbolRows; ++symbolRow) {
        for (size_t symbolCol = 0; symbolCol < symbolCols; ++symbolCol) {
            size_t bitmapRow = fromTopToSymbol + symbolRow;
            size_t bitmapCol = leftBearing + symbolCol;

            size_t vectorPos = bitmapRow*columns + bitmapCol;
            // to not crash on symbols that are bigger then their box borders
            if (vectorPos < vectorSize) {
                size_t bufferPos = symbolRow*ftBitmap.width + symbolCol;
                pixels->at(vectorPos) = MAX_GRAY_LEVELS
                                        - ftBitmap.buffer[bufferPos];
            }
        }
    }
}

#define COLOR_BYTE(color, fullPixel, pixFormat)                         \
    (((fullPixel & pixFormat->color##mask) >> pixFormat->color##shift)  \
        << pixFormat->color##loss)

static inline uint_fast8_t rgbPixelToGrayscale( uint32_t rgbPixel,
                                                const SDL_PixelFormat* fmt) {
    uint_fast8_t r = COLOR_BYTE(R, rgbPixel, fmt);
    uint_fast8_t g = COLOR_BYTE(G, rgbPixel, fmt);
    uint_fast8_t b = COLOR_BYTE(B, rgbPixel, fmt);

    uint_fast8_t grayLevel =      0.212671f * r
                                + 0.715160f * g
                                + 0.072169f * b;

    return grayLevel;
}

GrayscaleBitmap::GrayscaleBitmap(SDL_Surface* surface)
    : rows(surface->h)
    , columns(surface->w)
    , pixels(new pixels_vector(surface->h * surface->w, 0))
    , num_grays(MAX_GRAY_LEVELS) {

    const uint_fast8_t bytesPerPixel = surface->format->BytesPerPixel;
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < columns; ++col) {
            size_t pixelNum = row * columns + col;
            uint8_t* pixelDataStart = reinterpret_cast<uint8_t*>(surface->pixels)
                                        + pixelNum * bytesPerPixel;
            uint32_t rgbPixel = *reinterpret_cast<uint32_t*>(pixelDataStart);

            pixels->at(pixelNum) = rgbPixelToGrayscale(rgbPixel, surface->format);
        }
    }
}

GrayscaleBitmap::GrayscaleBitmap(const GrayscaleBitmap& toCopy)
    : rows(toCopy.rows)
    , columns(toCopy.columns)
    , pixels(new pixels_vector(*toCopy.pixels))
    , num_grays(toCopy.num_grays) {}

GrayscaleBitmap::~GrayscaleBitmap() {}


FramedBitmap::FramedBitmap(SDL_Surface* surface)
    : GrayscaleBitmap(surface)
    , frameWidth(1)
    , frameHeight(1) {}
FramedBitmap::FramedBitmap(const FramedBitmap& toCopy)
    : GrayscaleBitmap(toCopy)
    , frameWidth(toCopy.frameWidth)
    , frameHeight(toCopy.frameHeight) {}

FrameSlider FramedBitmap::firstFrame() const {
    return FrameSlider(*this);
}

const FrameSlider FramedBitmap::lastFrame() const {
    size_t leftBorderCol    = columns   - columns % frameWidth  - frameWidth;
    size_t topBorderRow     = rows      - rows % frameHeight    - frameHeight;

    return FrameSlider(*this, leftBorderCol, topBorderRow);
}

void FramedBitmap::setFrameSize(size_t width, size_t height) {
    frameWidth = width;
    frameHeight = height;
}

size_t FramedBitmap::countFrames() const {
    return (columns / frameWidth) * (rows / frameHeight);
}

FrameSlider::FrameSlider(const FramedBitmap& _map,
                        size_t _leftBorderCol, size_t _topBorderRow)
    : map(&_map)
    , leftBorderCol(_leftBorderCol)
    , topBorderRow(_topBorderRow) {

    if (    leftBorderCol + map->frameWidth  > map->columns
        ||  topBorderRow  + map->frameHeight > map->rows) {
        throw std::out_of_range("Specified frame is out of given bitmap borders");
    }
}

void FrameSlider::slide() {
    size_t newLeftBorder, newTopBorder;

    size_t colPosOfFrameToRight  = leftBorderCol + map->frameWidth;
    size_t rowPosOfFrameBelow    = topBorderRow  + map->frameHeight;

    if (colPosOfFrameToRight + map->frameWidth <= map->columns) {
        newLeftBorder = colPosOfFrameToRight;
        newTopBorder = topBorderRow;
    } else if (rowPosOfFrameBelow + map->frameHeight <= map->rows) {
        newLeftBorder = 0;
        newTopBorder = rowPosOfFrameBelow;
    } else {
        throw std::out_of_range("Can't slide out of bitmap range");
    }

    leftBorderCol = newLeftBorder;
    topBorderRow = newTopBorder;
}

obj_brightness FrameSlider::at(size_t pos) const {
    if (pos >= map->frameWidth * map->frameHeight) {
        throw std::out_of_range("Out of frame borders");
    }

    size_t mapRow    = topBorderRow  + pos / map->frameWidth;
    size_t mapColumn = leftBorderCol + pos % map->frameWidth;

    return map->pixels->at(mapRow * map->columns + mapColumn);
}

size_t FrameSlider::size() const {
    return map->frameWidth * map->frameHeight;
}

bool FrameSlider::operator==(const FrameSlider& toCompare) const {
    return     map == toCompare.map
            && map->frameWidth == toCompare.map->frameWidth
            && map->frameHeight == toCompare.map->frameHeight
            && leftBorderCol == toCompare.leftBorderCol
            && topBorderRow == toCompare.topBorderRow;
}

bool FrameSlider::operator!=(const FrameSlider& toCompare) const {
    return !operator==(toCompare);
}
