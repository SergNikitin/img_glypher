#include <iostream>
#include <fstream>
#include <cstdlib>
#include <exception>
#include <thread>

#include "grayscale_bitmap.h"
#include "freetype_interface.h"
#include "sdl_interface.h"
#include "comparison_thread.h"

static size_t slowestThreadProgress(std::vector<SymbolMatches>& results) {
    size_t slowest = *results.front().progress;

    for (SymbolMatches& partialResult : results) {
        if (*partialResult.progress < slowest) {
            slowest = *partialResult.progress;
        }
    }

    return slowest;
}

static char bestFrameMatchAmongThreads(std::vector<SymbolMatches>& results,
                                        size_t frame) {
    char bestMatch          = results.front().frameWinners.at(frame).first;
    uint_fast8_t minDiff    = results.front().frameWinners.at(frame).second;

    for (SymbolMatches& partialResult : results) {
        uint_fast8_t diff = partialResult.frameWinners.at(frame).second;

        if (diff < minDiff) {
            bestMatch = partialResult.frameWinners.at(frame).first;
            minDiff = diff;
        }
    }

    return bestMatch;
}

uint_fast8_t const THREAD_CONTRIBUTION = 5;
void imageToText(const std::string& imgPath, const std::string& fontPath) {
    std::ofstream outfile("test.txt");

    setFontFile(fontPath);
    FramedBitmap map = loadGrayscaleImage(imgPath);
    map.frameWidth  = getFontWidth();
    map.frameHeight = getFontHeight();


    size_t framesInRow = map.columns / map.frameWidth;
    size_t framesCount =  framesInRow * (map.rows / map.frameHeight);
    uint_fast8_t totalSymbols = LAST_ASCII_SYMBOL - FIRST_ASCII_SYMBOL;
    uint_fast8_t threadsQuantity = (totalSymbols % THREAD_CONTRIBUTION)
                                ? totalSymbols / THREAD_CONTRIBUTION + 1
                                : totalSymbols / THREAD_CONTRIBUTION;

    std::vector<SymbolMatches> threadResults;
    threadResults.reserve(threadsQuantity);


    uint_fast8_t assignedSymbols = 0;
    for (uint_fast8_t threadNum = 0; threadNum < threadsQuantity; ++threadNum) {
        std::string threadSymbols;
        for (size_t symbolNum = 0; symbolNum < THREAD_CONTRIBUTION; ++symbolNum) {
            if (assignedSymbols < totalSymbols) {
                threadSymbols.push_back(FIRST_ASCII_SYMBOL + assignedSymbols);
                ++assignedSymbols;
            }
        }

        threadResults.emplace_back(framesCount, threadSymbols);
    }

    std::thread threads[threadsQuantity];

    for (uint_fast8_t threadNum = 0; threadNum < threadsQuantity; ++threadNum) {
        threads[threadNum] = std::thread(   processVocabularyPart, &map,
                                            &threadResults.at(threadNum));
        threads[threadNum].detach();
    }

    size_t processedFrames = 0;

    while (processedFrames < framesCount - 1) {
        if (slowestThreadProgress(threadResults) > processedFrames) {
            outfile << bestFrameMatchAmongThreads(threadResults, processedFrames);

            if (processedFrames % framesInRow == 0) {
                outfile << '\n';
            }

            ++processedFrames;
        }
    }

    outfile.close();
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
