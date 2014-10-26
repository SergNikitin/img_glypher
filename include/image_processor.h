#ifndef __IMAGE_PROCESSOR_H__
#define __IMAGE_PROCESSOR_H__

#include <memory>
#include <vector>
#include <atomic>

#include "grayscale_bitmap.h"

class ImageToTextResult {
public:
    ImageToTextResult(size_t framesQuantity);
    ImageToTextResult(const ImageToTextResult&);

    std::vector<char>   frameMatches;
    std::atomic_bool    done;
};

void processImagePart(  FrameSlider &start, const FrameSlider& end,
                        ImageToTextResult& result);

#endif // __IMAGE_PROCESSOR_H__
