#ifndef __FREETYPE_INTERFACE_H__
#define __FREETYPE_INTERFACE_H__

#include "grayscale_bitmap.h"

void setFontFile(const std::string& newFilePath);
uint16_t getFontHeight();
uint16_t getFontWidth();
GrayscaleBitmap getBitmapForAsciiSymbol(char symbol);

#endif // __FREETYPE_INTERFACE_H__
