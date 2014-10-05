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
    size_t slowest = results.front().progress;

    for (SymbolMatches& partialResult : results) {
        if (partialResult.progress < slowest) {
            slowest = partialResult.progress;
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

static inline uint_fast8_t calcNeededThreads(uint_fast8_t threadContribution) {
    uint_fast8_t symbolsTotal = LAST_ASCII_SYMBOL - FIRST_ASCII_SYMBOL;
    uint_fast8_t threadsTotal = (symbolsTotal % threadContribution != 0)
                                ? symbolsTotal / threadContribution + 1
                                : symbolsTotal / threadContribution;

    return threadsTotal;
}

static void assignSymbolTasksForThreads(std::vector<SymbolMatches>& res) {
    char symbolTask = FIRST_ASCII_SYMBOL;

    for (SymbolMatches& singleThreadRes : res) {
        for (char& symbol : singleThreadRes.symbolSet) {
            symbol = symbolTask <= LAST_ASCII_SYMBOL
                    ? symbolTask++ : LAST_ASCII_SYMBOL;
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
    std::thread threads[threadsTotal];

    std::string dummySymbolSet(THREAD_CONTRIBUTION, ' ');
    SymbolMatches dummy(map.countFrames(), dummySymbolSet);
    std::vector<SymbolMatches> threadResults(threadsTotal, dummy);
    assignSymbolTasksForThreads(threadResults);

    for (uint_fast8_t threadNum = 0; threadNum < threadsTotal; ++threadNum) {
        threads[threadNum] = std::thread(   processVocabularyPart, &map,
                                            &threadResults.at(threadNum));
        threads[threadNum].detach();
    }

    size_t processedFrames = 0;

    while (processedFrames < map.countFrames() - 1) {
        if (slowestThreadProgress(threadResults) > processedFrames) {
            outfile << bestFrameMatchAmongThreads(threadResults, processedFrames);

            if (processedFrames % (map.columns / map.frameWidth) == 0) {
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
