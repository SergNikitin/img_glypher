#include "comparison_thread.h"
#include "freetype_interface.h"

FrameWinner::FrameWinner(char symb, uint_fast8_t diff)
    : symbol(symb)
    , grayLvlDiff(diff) {
}

SymbolMatches::SymbolMatches(size_t framesQuantity, const std::string& set)
    : progress(0)
    , symbolSet(set) {
    frameWinners.reserve(framesQuantity);
}

SymbolMatches::SymbolMatches(const SymbolMatches& toCopy)
    : frameWinners(toCopy.frameWinners)
    , progress(toCopy.progress.load())
    , symbolSet(toCopy.symbolSet) {
}

static uint_fast8_t calculateGrayLevelDiff( const FrameSlider& imgPart,
                                            const GrayscaleBitmap& glyph) {
    if (imgPart.size() != glyph.rows * glyph.columns) {
        throw std::runtime_error("Sizes of image part and symbol glyph do not match");
    }

    size_t pixelCount = imgPart.size();
    size_t diffAcc = 0;

    for (size_t pixelNum = 0; pixelNum < pixelCount; ++pixelNum) {
        diffAcc += abs(static_cast<int>(imgPart.at(pixelNum))
                        - glyph.pixels->at(pixelNum));
    }

    return diffAcc / pixelCount;
}

static FrameWinner chooseMatchingSymbol(const FrameSlider& imgPart,
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

    return FrameWinner(bestMatch, minDiff);
}

#include <iostream>
#include <exception>

void processVocabularyPart(const FramedBitmap* map, SymbolMatches* matches) {
    try {
        const FrameSlider lastFrame = map->lastFrame();

        for (FrameSlider frame = map->firstFrame(); frame != lastFrame; frame.slide()) {
            FrameWinner winner = chooseMatchingSymbol(frame, matches->symbolSet);
            matches->frameWinners.push_back(winner);
            ++matches->progress;
        }

        FrameWinner winner = chooseMatchingSymbol(lastFrame, matches->symbolSet);
        matches->frameWinners.push_back(winner);
        ++matches->progress;

    }
    catch (std::exception& err) {
        std::cerr << err.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }
}
