#include <iostream>
#include <exception>

#include "image_processor.h"
#include "freetype_interface.h"

ImageToTextResult::ImageToTextResult(size_t framesQuantity)
    : done(false) {
    frameMatches.reserve(framesQuantity);
}

ImageToTextResult::ImageToTextResult(const ImageToTextResult& toCopy)
    : frameMatches(toCopy.frameMatches)
    , done(toCopy.done.load()) {
}

static obj_brightness averageFrameBrightness(const FrameSlider& imgPart) {
    uint64_t acc = 0;
    size_t frameSize = imgPart.size();
    for (size_t pixelNum = 0; pixelNum < frameSize; ++pixelNum) {
        acc += imgPart.at(pixelNum);
    }

    return acc / frameSize;
}

static char symbolWithBrightnessClosestTo(obj_brightness targetBrightness) {
    const brihgtness_map vocabulary = getBrightnessVocabulary();

    obj_brightness leastBrDiff = MAX_GRAY_LEVELS;
    char bestMatch = vocabulary.begin()->first;

    for (const symbol_brightness_pair& entry : vocabulary) {
        obj_brightness brDiff = abs(static_cast<int>(targetBrightness)
                                    - entry.second);
        if (brDiff < leastBrDiff) {
            leastBrDiff = brDiff;
            bestMatch = entry.first;
        }
    }

    return bestMatch;
}

static char matchFrameToSymbol(const FrameSlider& imgPart) {
    obj_brightness frameBrightness = averageFrameBrightness(imgPart);

    return symbolWithBrightnessClosestTo(frameBrightness);
}

void processImagePart(  FrameSlider& start, const FrameSlider& end,
                        ImageToTextResult& result) {
    try {
        for (FrameSlider frame = start; frame != end; frame.slide()) {
            char match = matchFrameToSymbol(frame);
            result.frameMatches.push_back(match);
        }

        char match = matchFrameToSymbol(end);
        result.frameMatches.push_back(match);
        result.done = true;
    }
    catch (std::exception& err) {
        std::cerr << err.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }
}
