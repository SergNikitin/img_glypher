#ifndef __COMPARISON_THREAD_H__
#define __COMPARISON_THREAD_H__

#include <vector>
#include <atomic>

#include "grayscale_bitmap.h"

typedef std::pair<char, uint_fast8_t> frame_winner;

class SymbolMatches {
public:
    SymbolMatches(size_t framesQuantity, std::string& _symbolSet);

    std::vector<frame_winner> frameWinners;
    std::atomic_size_t progress;
    const std::string symbolSet;

private:
    SymbolMatches() = delete;
};

void processVocabularyPart(const FramedBitmap&, SymbolMatches&);

#endif // __COMPARISON_THREAD_H__
