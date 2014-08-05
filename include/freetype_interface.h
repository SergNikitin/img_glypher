#ifndef __FREETYPE_INTERFACE_H__
#define __FREETYPE_INTERFACE_H__

#include "grayscale_bitmap.h"

void setFontFile(const std::string& newFilePath);
GrayscaleBitmap getBitmapForAsciiSymbol(char symbol);

#endif // __FREETYPE_INTERFACE_H__
