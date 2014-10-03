#ifndef __COMPARISON_THREAD_H__
#define __COMPARISON_THREAD_H__

#include <memory>
#include <vector>
#include <atomic>

#include "grayscale_bitmap.h"

typedef std::pair<char, uint_fast8_t> frame_winner;

class SymbolMatches {
public:
    SymbolMatches() = delete;
    SymbolMatches(size_t framesQuantity, std::string& _symbolSet);

    std::vector<frame_winner> frameWinners;
    std::unique_ptr<std::atomic_size_t> progress;
    const std::string symbolSet;
};

void processVocabularyPart(const FramedBitmap*, SymbolMatches*);

#endif // __COMPARISON_THREAD_H__
