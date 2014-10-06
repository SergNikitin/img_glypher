#ifndef __FREETYPE_INTERFACE_H__
#define __FREETYPE_INTERFACE_H__

#include <map>
#include "grayscale_bitmap.h"

typedef std::map<unsigned char, GrayscaleBitmap> symbol_map;

void setFontFile(const std::string& newFilePath);
uint_fast16_t getFontHeight();
uint_fast16_t getFontWidth();
const GrayscaleBitmap& getVocabularyEntry(char symbol);
const symbol_map& getVocabulary();

const char FIRST_PRINTABLE_ASCII_SYMBOL = ' ';
const char LAST_PRINTABLE_ASCII_SYMBOL  = '~';


#endif // __FREETYPE_INTERFACE_H__
