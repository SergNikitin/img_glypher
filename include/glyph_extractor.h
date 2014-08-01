#ifndef __GLYPH_EXTRACTOR_H__
#define __GLYPH_EXTRACTOR_H__

#include "grayscale_bitmap.h"

GrayscaleBitmap getBitmapForAsciiSymbol(uint8_t symbolCode);
void setFontFile(std::string filepath);

#endif
