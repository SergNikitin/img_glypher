#ifndef __COMPARISON_THREAD_H__
#define __COMPARISON_THREAD_H__

#include <memory>
#include <vector>
#include <atomic>

#include "grayscale_bitmap.h"

class FrameWinner {
public:
    FrameWinner(char, uint_fast8_t);

    char symbol;
    uint_fast8_t grayLvlDiff;
};

class SymbolMatches {
public:
    SymbolMatches(size_t framesQuantity, const std::string& symbolSet);
    SymbolMatches(const SymbolMatches&);

    std::vector<FrameWinner> frameWinners;
    std::atomic_size_t progress;
    std::string symbolSet;
};

void processVocabularyPart(const FramedBitmap*, SymbolMatches*);

#endif // __COMPARISON_THREAD_H__
