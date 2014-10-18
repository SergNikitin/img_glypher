#include <iostream>
#include <exception>

#include "comparison_thread.h"
#include "freetype_interface.h"

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

static gray_pixel averageFrameBrightness(const FrameSlider& imgPart) {
    uint64_t acc = 0;
    size_t frameSize = imgPart.size();
    for (size_t pixelNum = 0; pixelNum < frameSize; ++pixelNum) {
        acc += imgPart.at(pixelNum);
    }

    return acc / frameSize;
}

static char bestBrightnessMatch(gray_pixel frameBrightness) {
    brihgtness_map brightnessMap = getBrightnessMap();

    gray_pixel leastBrightnessDiff = MAX_GRAY_LEVELS;
    char bestMatch = brightnessMap.begin()->first;

    for (auto& entry : brightnessMap) {
        gray_pixel brightnessDiff = abs(static_cast<int>(frameBrightness)
                                        - entry.second);
        if (brightnessDiff < leastBrightnessDiff) {
            leastBrightnessDiff = brightnessDiff;
            bestMatch = entry.first;
        }
    }

    return bestMatch;
}

static char chooseMatchingSymbol(const FrameSlider& imgPart) {
    // uint_fast8_t minDiff = MAX_GRAY_LEVELS;


    // for (char symbol : symbolSet) {
        // uint_fast8_t diff = calculateGrayLevelDiff(imgPart, vocabulary.at(symbol));
//
        // if (diff < minDiff) {
            // bestMatch = symbol;
            // minDiff = diff;
        // }
    // }

    gray_pixel frameBrightness = averageFrameBrightness(imgPart);

    // std::cout << (int)frameBrightness << std::endl;
    // return FrameWinner(bestMatch, minDiff);
    return bestBrightnessMatch(frameBrightness);
}

void processVocabularyPart(const FramedBitmap* map, SymbolMatches* matches) {
    try {
        const FrameSlider lastFrame = map->lastFrame();

        for (FrameSlider frame = map->firstFrame(); frame != lastFrame; frame.slide()) {
            char match = chooseMatchingSymbol(frame);
            matches->frameWinners.push_back(match);
            ++matches->progress;
        }

        gray_pixel match = chooseMatchingSymbol(lastFrame);
        matches->frameWinners.push_back(match);
        ++matches->progress;
    }
    catch (std::exception& err) {
        std::cerr << err.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }
}
