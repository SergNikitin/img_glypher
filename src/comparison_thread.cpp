#include "comparison_thread.h"
#include "freetype_interface.h"

SymbolMatches::SymbolMatches(size_t framesQuantity, std::string& _symbolSet)
    : progress(new std::atomic_size_t(0))
    , symbolSet(_symbolSet) {
    frameWinners.reserve(framesQuantity);
}

static uint_fast8_t calculateGrayLevelDiff( const FrameSlider& imgPart,
                                            const GrayscaleBitmap& glyph) {
    if (imgPart.size() != glyph.rows * glyph.columns) {
        throw std::runtime_error("Sizes of image part and symbol glyph do not match");
    }

    size_t pixelCount = imgPart.size();
    size_t diffAcc = 0;

    for (size_t pixelNum = 0; pixelNum < pixelCount; ++pixelNum) {
        diffAcc += abs((int)imgPart.at(pixelNum) - glyph.pixels->at(pixelNum));
    }

    return diffAcc / pixelCount;
}

static frame_winner chooseMatchingSymbol(   const FrameSlider& imgPart,
                                            const std::string& symbolSet) {
    uint_fast8_t minDiff = 0xFF;
    char bestMatch = symbolSet.front();

    symbol_map vocabulary = getVocabulary();

    for (char symbol : symbolSet) {
        uint_fast8_t diff = calculateGrayLevelDiff(imgPart, vocabulary.at(symbol));

        if (diff < minDiff) {
            bestMatch = symbol;
            minDiff = diff;
        }
    }

    return frame_winner(bestMatch, minDiff);
}

#include <iostream>
#include <exception>

void processVocabularyPart( const FramedBitmap* map, SymbolMatches* matches) {
    try {
        FrameSlider lastFrame   = map->lastFrame();
        FrameSlider frame       = map->firstFrame();

        for (; frame != lastFrame; frame.slide()) {
            frame_winner winner = chooseMatchingSymbol(frame, matches->symbolSet);
            matches->frameWinners.push_back(winner);
            ++(*matches->progress);
        }
    }
    catch (std::exception& err) {
        std::cerr << err.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }
}
