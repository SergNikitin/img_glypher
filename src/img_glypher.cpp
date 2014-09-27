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
    uint8_t minDiff = 0xFF;
    char bestMatch = ' ';

    symbol_map vocabulary = getVocabulary();

    for (   symbol_map::const_iterator symbolIter = vocabulary.begin();
            symbolIter != vocabulary.end(); ++symbolIter) {
        uint8_t diff = calculateGrayLevelDiff(imgPart, symbolIter->second);

        if (diff < minDiff) {
            bestMatch = symbolIter->first;
            minDiff = diff;
        }
    }

    return bestMatch;
}

void imageToText(const std::string& imgPath, const std::string& fontPath) {
    std::ofstream outfile("test.txt");

    setFontFile(fontPath);
    const FramedBitmap map = loadGrayscaleImage(imgPath);

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
    catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }

    return 0;
}
