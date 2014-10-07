#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <exception>
#include <thread>

#include "grayscale_bitmap.h"
#include "freetype_interface.h"
#include "sdl_interface.h"
#include "comparison_thread.h"

static char bestFrameMatchAmongThreads(const std::vector<SymbolMatches>& results,
                                        size_t frame) {
    char bestMatch          = results.front().frameWinners.at(frame).symbol;
    uint_fast8_t minDiff    = results.front().frameWinners.at(frame).grayLvlDiff;

    for (const SymbolMatches& partialResult : results) {
        uint_fast8_t diff = partialResult.frameWinners.at(frame).grayLvlDiff;

        if (diff < minDiff) {
            bestMatch = partialResult.frameWinners.at(frame).symbol;
            minDiff = diff;
        }
    }

    return bestMatch;
}

static inline uint_fast8_t calcNeededThreads(uint_fast8_t threadContribution) {
    uint_fast8_t symbolsTotal = LAST_PRINTABLE_ASCII_SYMBOL
                                - FIRST_PRINTABLE_ASCII_SYMBOL;
    uint_fast8_t threadsTotal = (symbolsTotal % threadContribution != 0)
                                ? symbolsTotal / threadContribution + 1
                                : symbolsTotal / threadContribution;

    return threadsTotal;
}

static void assignSymbolTasksForThreads(std::vector<SymbolMatches>& res) {
    char symbolTask = FIRST_PRINTABLE_ASCII_SYMBOL;

    for (SymbolMatches& singleThreadRes : res) {
        for (char& symbol : singleThreadRes.symbolSet) {
            symbol = symbolTask <= LAST_PRINTABLE_ASCII_SYMBOL
                    ? symbolTask++ : LAST_PRINTABLE_ASCII_SYMBOL;
        }
    }
}

uint_fast8_t const THREAD_CONTRIBUTION = 20;
void imageToText(const std::string& imgPath, const std::string& fontPath) {
    std::ofstream outfile("test.txt");

    setFontFile(fontPath);
    FramedBitmap map = loadGrayscaleImage(imgPath);
    map.setFrameSize(getFontWidth(), getFontHeight());

    uint_fast8_t threadsTotal = calcNeededThreads(THREAD_CONTRIBUTION);

    std::string dummySymbolSet(THREAD_CONTRIBUTION, ' ');
    SymbolMatches dummy(map.countFrames(), dummySymbolSet);
    std::vector<SymbolMatches> threadResults(threadsTotal, dummy);
    assignSymbolTasksForThreads(threadResults);

    for (SymbolMatches& container : threadResults) {
        std::thread(processVocabularyPart, &map, &container).detach();
    }

    size_t processedFrames = 0;
    size_t framesTotal = map.countFrames();
    size_t framesInRow = map.columns / map.frameWidth;
    auto slowestThreadLamda = [](SymbolMatches& lhs, SymbolMatches& rhs) {
        return lhs.progress.load() < rhs.progress.load();
    };
    while (processedFrames < framesTotal) {
        auto slowest = std::min_element(threadResults.begin(), threadResults.end(),
                                        slowestThreadLamda);

        if (slowest->progress.load() > processedFrames) {
            outfile << bestFrameMatchAmongThreads(threadResults, processedFrames);

            if (processedFrames % framesInRow == 0) {
                outfile << '\n';
            }

            ++processedFrames;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "incorrect number of arguements: " << argc << std::endl;
        return 1;
    }

    std::string imageFilePath(argv[1]);
    std::string fontFilePath(argv[2]);

    try {
        imageToText(imageFilePath, fontFilePath);
    }
    catch (const std::exception& error) {
        std::cerr << error.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }

    return 0;
}
