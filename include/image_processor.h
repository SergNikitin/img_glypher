#ifndef __IMAGE_PROCESSOR_H__
#define __IMAGE_PROCESSOR_H__

/**
 * @file image_processor.h
 * @brief Multithreaded image processing module
 */

#include <memory>
#include <vector>
#include <atomic>

#include "grayscale_bitmap.h"

/**
 * @brief Image to symbols conversion result storage
 * @details Multiple threads can be assigned with objects of this type
 * to store partial image processing results in them and to organize feedback
 * to the main thread
 */
class ImageToTextResult {
public:
    /**
     * @brief Make the thread result container
     *
     * @param framesQuantity space will be allocated for this number of symbols
     */
    ImageToTextResult(size_t framesQuantity);
    ImageToTextResult(const ImageToTextResult&);

    std::vector<char>   frameMatches;   /**< symbols that were matched to image
                                            frames */
    std::atomic_bool    done;           /**< indicates to the main thread that
                                            processing of the image part was
                                            completed */
};

/**
 * @brief Find symbol matches for frames in the given frame range
 * @details Entry point for threads spawned by the main thread, frame range is
 * inclusive
 *
 * @param start first frame to find symbol match for
 * @param end last frame to find symbol match for
 * @param result storage for symbol matches
 */
void processImagePart(  FrameSlider &start, const FrameSlider& end,
                        ImageToTextResult& result);

#endif // __IMAGE_PROCESSOR_H__
