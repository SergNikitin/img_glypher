#ifndef __GRAYSCALE_BITMAP_H__
#define __GRAYSCALE_BITMAP_H__

#include <memory>
#include <vector>

extern "C" {
    #include "ft2build.h"
    #include FT_FREETYPE_H

    #include "SDL.h"
}

typedef uint_fast8_t obj_brightness;
typedef std::vector<obj_brightness> pixels_vector;
typedef std::unique_ptr<pixels_vector> unique_pixels_ptr;

/**
 * Brightness level that corresponds to a white-colored pixel
 */
const obj_brightness MAX_GRAY_LEVELS = 255;

/**
 * @brief Grayscale bitmap that stores pixel data
 * @details Interface object that is used in other modules, essentially
 * a wrapper over the different implemetations of the same thing provided by
 * font and image libraries; pixels are stored as grayscale, with the stored
 * value as their gray level, or brightness (the closer the value is to 0,
 * the closer the pixel's color to black)
 *
 * @see MAX_GRAY_LEVELS
 */
class GrayscaleBitmap {
public:
    /**
     * @brief Place FreeType2 font image data into an interface object
     *
     * @param  Pointer to valid FreeType2 FT_FaceRec object
     */
    explicit GrayscaleBitmap(const FT_Face);

    /**
     * @brief Place SDL_Surface image data into an interface object
     *
     * @param  Pointer to a valid SDL_Surface object, not const due to the ways
     * SDL handles conversion of colored images to grayscale
     */
    explicit GrayscaleBitmap(SDL_Surface*);

    GrayscaleBitmap(const GrayscaleBitmap&);

    virtual ~GrayscaleBitmap();

    const size_t rows;              /**< Number of pixel rows in bitmap */
    const size_t columns;           /**< Number of pixel columns in bitmap */
    const unique_pixels_ptr pixels; /**< Pointer to vector containig pixel data */
    const uint_fast8_t num_grays;   /**< Stored gray pixel format */
};

class FrameSlider;

/**
 * @brief Grayscale bitmap with interfaces for easy pixel data navigation
 * @details Provides the ability to navigate bitmap frame-by-frame in the
 * iterator-like fashion, where frame is a bitmap part of a given size
 */
class FramedBitmap : public GrayscaleBitmap {
public:
    /**
     * @brief Place SDL_Surface image data into an interface object
     * @see GrayscaleBitmap(SDL_Surface*)
     */
    explicit FramedBitmap(SDL_Surface*);
    FramedBitmap(const FramedBitmap&);

    /**
     * @brief Get frame slider with access to the first bitmap frame
     */
    FrameSlider       firstFrame()  const;
    /**
     * @brief Get frame slider with access to the last bitmap frame
     */
    const FrameSlider lastFrame()   const;

    /**
     * @brief Set frame size
     *
     * @param width frame width in pixels
     * @param height frame height in pixels
     */
    void setFrameSize(size_t width, size_t height);
    /**
     * @brief Get how much frames with the current size there are
     * in the bitmap
     */
    size_t countFrames() const;

    size_t frameWidth;  /**< frame width in pixels */
    size_t frameHeight; /**< frame height in pixels */
};

/**
 * @brief Iterator-like class for FramedBitmap navigation
 * @see FramedBitmap
 */
class FrameSlider {
public:
    /**
     * @brief Create an iterator-like object for bitmap
     *
     * @param map bitmap which data will be accessed via slider
     * @param leftBorderCol absolute horizontal slider position inside the bitmap
     * @param topBorderRow absolute vertical slider position inside the bitmap
     */
    FrameSlider(const FramedBitmap& map,
                size_t leftBorderCol = 0, size_t topBorderRow = 0);

    /**
     * @brief Advance slider for one frame
     * @details Frame advancement is horizontal; if slider reached right
     * border of the bitmap, the next slide will bring him one frame down and to
     * the left border of the bitmap
     */
    void slide();

    /**
     * @brief Get bitmap pixel data through slider
     * @details Allows to access frame like an image smaller then the bitmap,
     * allows not to worry about the absolute pixel coordinates
     *
     * @param pos requested pixel position, absolute inside the frame
     */
    obj_brightness at(size_t pos) const;

    /**
     * @brief Get frame size in pixels
     */
    size_t size() const;

    bool operator==(const FrameSlider&) const;
    bool operator!=(const FrameSlider&) const;

private:
    const FramedBitmap* map;    /**< pointer to bitmap with the pixel data */
    size_t leftBorderCol;       /**< absolute horizontal slider position
                                    inside the bitmap */
    size_t topBorderRow;        /**< absolute vertical slider position
                                    inside the bitmap */
};

#endif // __GRAYSCALE_BITMAP_H__
