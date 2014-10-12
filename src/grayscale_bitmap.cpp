#include <exception>
#include "grayscale_bitmap.h"

static const uint32_t FIXED_POINT_26_6_COEFF = 1<<6;
GrayscaleBitmap::GrayscaleBitmap(const FT_Face fontFace)
    : rows(fontFace->size->metrics.height / FIXED_POINT_26_6_COEFF)
    , columns(fontFace->size->metrics.max_advance / FIXED_POINT_26_6_COEFF)
    , pixels(new pixels_vector(rows*columns, 0))
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
                pixels->at(vectorPos) = ftBitmap.buffer[bufferPos];
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

    return MAX_GRAY_LEVELS - grayLevel;
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
            uint8_t* pixelDataStart = static_cast<uint8_t*>(surface->pixels)
                                        + pixelNum * bytesPerPixel;
            uint32_t rgbPixel = *reinterpret_cast<uint32_t*>(pixelDataStart);

            uint_fast8_t grayLevel = rgbPixelToGrayscale(rgbPixel, surface->format);
            pixels->at(pixelNum) = grayLevel;
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
    return FrameSlider(*this, frameWidth, frameHeight);
}

const FrameSlider FramedBitmap::lastFrame() const {
    size_t leftBorderCol    = columns   - columns % frameWidth  - frameWidth;
    size_t topBorderRow     = rows      - rows % frameHeight    - frameHeight;

    return FrameSlider(*this, frameWidth, frameHeight, leftBorderCol, topBorderRow);
}

void FramedBitmap::setFrameSize(size_t width, size_t height) {
    frameWidth = width;
    frameHeight = height;
}

size_t FramedBitmap::countFrames() const {
    return (columns / frameWidth) * (rows / frameHeight);
}

FrameSlider::FrameSlider(const FramedBitmap& _map,
                        const size_t _width, const size_t _height,
                        size_t _leftBorderCol, size_t _topBorderRow)
    : map(&_map)
    , width(_width)
    , height(_height)
    , leftBorderCol(_leftBorderCol)
    , topBorderRow(_topBorderRow) {

    if (    leftBorderCol + width > map->columns
        ||  topBorderRow + height > map->rows) {
        throw std::out_of_range("Specified frame is out of given bitmap borders");
    }
}

FrameSlider::~FrameSlider() {}

void FrameSlider::slide() {
    size_t newLeftBorder, newTopBorder;

    size_t colPosOfFrameToRight  = leftBorderCol + width;
    size_t rowPosOfFrameBelow    = topBorderRow  + height;

    if (colPosOfFrameToRight + width <= map->columns) {
        newLeftBorder = colPosOfFrameToRight;
        newTopBorder = topBorderRow;
    } else if (rowPosOfFrameBelow + height <= map->rows) {
        newLeftBorder = 0;
        newTopBorder = rowPosOfFrameBelow;
    } else {
        throw std::out_of_range("Can't slide out of bitmap range");
    }

    leftBorderCol = newLeftBorder;
    topBorderRow = newTopBorder;
}

gray_pixel FrameSlider::at(size_t pos) const {
    if (pos >= width * height) {
        throw std::out_of_range("Out of frame borders");
    }

    size_t mapRow = topBorderRow + pos / width;
    size_t mapColumn = leftBorderCol + pos % width;

    return map->pixels->at(mapRow * map->columns + mapColumn);
}

size_t FrameSlider::size() const {
    return width * height;
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
