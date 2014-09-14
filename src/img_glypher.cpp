#include <iostream>
#include <fstream>
#include <cstdlib>
#include <exception>

#include "grayscale_bitmap.h"
#include "freetype_interface.h"
#include "sdl_interface.h"

uint8_t calculateGrayLevelDiff( const FrameSlider& imgPart,
                                const GrayscaleBitmap& glyph) {
    if (imgPart.size() != (int32_t)glyph.rows * glyph.columns) {
        std::cout << imgPart.size() << " " << (int32_t)glyph.rows * glyph.columns
                    << std::endl;
        throw std::runtime_error("Sizes of image part and symbol glyph do not match");
    }

    int32_t pixelCount = imgPart.size();
    int32_t diffAcc = 0;

    for (int32_t pixelNum = 0; pixelNum < pixelCount; ++pixelNum) {
        diffAcc += abs((int)imgPart.at(pixelNum) - glyph.pixels->at(pixelNum));
    }

    return diffAcc / pixelCount;
}

char chooseMatchingSymbol(const FrameSlider& imgPart) {
    static const char FIRST_ASCII_SYMBOL    = ' ';
    static const char LAST_ASCII_SYMBOL     = '~';

    uint8_t minGrayLevelDiff = 0xFF;
    char bestMatch = FIRST_ASCII_SYMBOL;

    for (char symbol = FIRST_ASCII_SYMBOL; symbol <= LAST_ASCII_SYMBOL; ++symbol) {
        GrayscaleBitmap symbolGlyph = getBitmapForAsciiSymbol(symbol);
        uint8_t thisSymbolDiff = calculateGrayLevelDiff(imgPart, symbolGlyph);

        if (thisSymbolDiff < minGrayLevelDiff) {
            bestMatch = symbol;
            minGrayLevelDiff = thisSymbolDiff;
        }
    }

    return bestMatch;
}

void imageToText(std::string imgPath, std::string imgFont) {
    std::ofstream outfile("test.txt");

    setFontFile(imgFont);
    FramedBitmap map = loadGrayscaleImage(imgPath);

    FrameSlider lastFrame   = map.lastFrame(getFontWidth(), getFontHeight());
    FrameSlider frame       = map.firstFrame(getFontWidth(), getFontHeight());

    for (; frame != lastFrame; frame.slide()) {
        if (frame.newline) {
            outfile << '\n';
        }

        outfile << chooseMatchingSymbol(frame);
    }

    outfile << chooseMatchingSymbol(lastFrame);

    outfile.close();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "incorrect number of arguements: " << argc << std::endl;
        return 1;
    }

    std::string imageFilePath(argv[1]);
    std::string fontFilePath(argv[2]);

    try {
        imageToText(imageFilePath, fontFilePath);
    }
    catch (const std::exception& error) {
        std::cerr << error.what() << std::endl;
    }

    return 0;
}
