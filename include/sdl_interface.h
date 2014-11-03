#ifndef __SDL_INTERFACE_H__
#define __SDL_INTERFACE_H__

/**
 * @file sdl_interface.h
 * @brief Image library driver
 */

#include <string>
#include "grayscale_bitmap.h"

/**
 * @brief Load pixel data from image file
 *
 * @param filepath File path of the image to load
 * @return Interface object with image data stored inside
 */
FramedBitmap loadGrayscaleImage(const std::string& filepath);

#endif // __SDL_INTERFACE_H__
