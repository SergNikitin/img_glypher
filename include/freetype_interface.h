#ifndef __FREETYPE_INTERFACE_H__
#define __FREETYPE_INTERFACE_H__

#include <map>
#include "grayscale_bitmap.h"

typedef std::pair<unsigned char, gray_pixel> symbol_brightness;
typedef std::map<unsigned char, gray_pixel> brihgtness_map;

void setFontFile(const std::string& newFilePath);
uint_fast16_t getFontHeight();
uint_fast16_t getFontWidth();
gray_pixel getSymbolBrightness(char symbol);
const brihgtness_map& getBrightnessMap();

const char FIRST_PRINTABLE_ASCII_SYMBOL = ' ';
const char LAST_PRINTABLE_ASCII_SYMBOL  = '~';

#endif // __FREETYPE_INTERFACE_H__
