#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdexcept>

#include "grayscale_bitmap.h"
#include "freetype_interface.h"
#include "sdl_interface.h"

uint8_t calculateGrayLevelDiff( const FrameSlider& imgPart,
                                const GrayscaleBitmap& glyph) {
    if ((int32_t)glyph.rows * glyph.columns != imgPart.size()) {
        std::cout << glyph.rows << 'x' << glyph.columns << " vs " << imgPart.size()
                << std::endl;
        throw std::runtime_error("Sizes of image part and symbol glyph do not match");
    }

    int32_t pixelCount = imgPart.size();
    int32_t diffAcc = 0;

    for (int32_t pixelNum = 0; pixelNum < pixelCount; ++pixelNum) {
        diffAcc += abs(imgPart.at(pixelNum) - glyph.pixels->at(pixelNum));
    }

    return diffAcc / pixelCount;
}

char chooseMatchingSymbol(const FrameSlider& imgPart) {
    static const char LAST_ASCII_SYMBOL     = static_cast<char>(126);
    static const char FIRST_ASCII_SYMBOL    = static_cast<char>(33);

    uint8_t minGrayLevelDiff = 0xFF;
    char bestMatch = ' ';

    for (char symbol = FIRST_ASCII_SYMBOL; symbol <= LAST_ASCII_SYMBOL; ++symbol) {
        GrayscaleBitmap map = getBitmapForAsciiSymbol(symbol);
        uint8_t thisSymbolDiff = calculateGrayLevelDiff(imgPart, map);

        if (thisSymbolDiff < minGrayLevelDiff) {
            bestMatch = symbol;
            minGrayLevelDiff = thisSymbolDiff;
        }
    }

    return bestMatch;
}

void imageToText(std::string image, std::string font) {
    std::ofstream outfile("test.txt");

    setFontFile(font);
    FramedBitmap map = loadGrayscaleImage(image);

    FrameSlider lastFrame = map.lastFrame(9, 16);
    FrameSlider frame = map.firstFrame(9, 16);

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
