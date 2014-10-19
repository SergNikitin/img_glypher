#ifndef __FREETYPE_INTERFACE_H__
#define __FREETYPE_INTERFACE_H__

#include <map>
#include "grayscale_bitmap.h"

typedef std::pair<const char, obj_brightness> symbol_brightness_pair;
typedef std::map< const char, obj_brightness> brihgtness_map;

void setFontFile(const std::string& newFilePath);
uint_fast16_t getFontHeight();
uint_fast16_t getFontWidth();
obj_brightness getSymbolBrightness(char symbol);
const brihgtness_map& getBrightnessVocabulary();

const char FIRST_PRINTABLE_ASCII_SYMBOL = ' ';
const char LAST_PRINTABLE_ASCII_SYMBOL  = '~';

#endif // __FREETYPE_INTERFACE_H__
