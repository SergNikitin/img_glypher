#ifndef __FREETYPE_INTERFACE_H__
#define __FREETYPE_INTERFACE_H__

#include <map>
#include "grayscale_bitmap.h"

typedef std::map<unsigned char, GrayscaleBitmap> symbol_map;

void setFontFile(const std::string& newFilePath);
uint16_t getFontHeight();
uint16_t getFontWidth();
const GrayscaleBitmap& getVocabularyEntry(char symbol);
const symbol_map& getVocabulary();


#endif // __FREETYPE_INTERFACE_H__
