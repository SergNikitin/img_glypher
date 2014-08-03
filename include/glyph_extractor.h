#ifndef __GLYPH_EXTRACTOR_H__
#define __GLYPH_EXTRACTOR_H__

#include "grayscale_bitmap.h"

void initGlyphExtractionModule();
void setFontFile(std::string newFilePath);
GrayscaleBitmap getBitmapForAsciiSymbol(char symbol);

#endif