#ifndef __FREETYPE_INTERFACE_H__
#define __FREETYPE_INTERFACE_H__

/**
 * @file freetype_interface.h
 * @brief Font library driver
 */

#include <map>
#include "grayscale_bitmap.h"

typedef std::pair<const char, obj_brightness> symbol_brightness_pair;
typedef std::map< const char, obj_brightness> brihgtness_map;

/**
 * @brief Prepare the font data needed to calculate image-to-symbol matches
 * @details Loads the given font file with the given font size and builds
 * the 'vocabulary' of average symbol pixelmap's brightness
 *
 * @param fontpath Path to font file that will be loaded, only monospaced,
 * scalable fonts will be accepted
 * @param fontSize Font size that will be used on symbol pixelmaps retrieval
 * @param invert Invert brightness values in vocabulary if true
 */
void setupFont(const std::string& fontpath, uint_fast16_t fontSize, bool invert);

/**
 * @brief Get font height in pixels
 * @details Baseline-to-baseline distance; actual symbol pixelmaps can have
 * larger height, refer to Freetype2 docs for more info
 */
uint_fast16_t getFontHeight();

/**
 * @brief Get font width in pixels
 * @details Maximum font advance width, refer to Freetype2 docs for more info
 */
uint_fast16_t getFontWidth();

/**
 * @brief Get average symbol brightness
 * @details Return the value calculated for the symbol's pixelmap during the
 * brightness vocabulary init
 *
 * @param symbol Printable ASCII symbol for which the brightness value will be
 * retrieved
 */
obj_brightness getSymbolBrightness(char symbol);
const brihgtness_map& getBrightnessVocabulary();

/**
 * @brief Find the symbol from the brightness vocabulary with the brightness
 * value closest to the requested brightness
 *
 * @param targetBrightness Target brightness value
 * @return Best matching printable ASCII symbol
 */
char symbolWithBrightnessClosestTo(obj_brightness targetBrightness);

const char FIRST_PRINTABLE_ASCII_SYMBOL = ' ';
const char LAST_PRINTABLE_ASCII_SYMBOL  = '~';

#endif // __FREETYPE_INTERFACE_H__
