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

static char bestFrameMatchAmongThreads( const std::vector<SymbolMatches>& results,
                                        size_t frame) {
    char bestMatch          = results.front().frameWinners.at(frame);

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

size_t slowestThreadProgress(const std::vector<SymbolMatches>& results) {
    size_t slowestThProgress = results.front().progress.load();
    for (const SymbolMatches& result : results) {
        size_t progress = result.progress.load();
        if (progress < slowestThProgress) {
            slowestThProgress = progress;
        }
    }

    return slowestThProgress;
}

uint_fast8_t const THREAD_CONTRIBUTION = LAST_PRINTABLE_ASCII_SYMBOL
                                        - FIRST_PRINTABLE_ASCII_SYMBOL;
void imageToText(const std::string& imgPath, const std::string& fontPath) {
    std::ofstream outfile("test.txt");

    setFontFile(fontPath);
    FramedBitmap map = loadGrayscaleImage(imgPath);
    map.setFrameSize(getFontWidth(), getFontHeight());

    // uint_fast8_t threadsTotal = calcNeededThreads(THREAD_CONTRIBUTION);
    uint_fast8_t threadsTotal = 1;

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

    while (processedFrames < framesTotal) {
        if (slowestThreadProgress(threadResults) > processedFrames) {
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
