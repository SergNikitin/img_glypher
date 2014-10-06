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
    SymbolMatches(size_t framesQuantity, const std::string& symbolSet);
    SymbolMatches(const SymbolMatches&);

    std::vector<frame_winner> frameWinners;
    std::atomic_size_t progress;
    std::string symbolSet;
};

void processVocabularyPart(const FramedBitmap*, SymbolMatches*);

#endif // __COMPARISON_THREAD_H__
