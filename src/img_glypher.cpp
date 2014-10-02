#include <iostream>
#include <fstream>
#include <cstdlib>
#include <exception>

#include "grayscale_bitmap.h"
#include "freetype_interface.h"
#include "sdl_interface.h"
#include "comparison_thread.h"

// typedef std::unique_ptr<SymbolMatches> unique_res_ptr;

uint_fast8_t const THREAD_CONTRIBUTION = 5;
void imageToText(const std::string& imgPath, const std::string& fontPath) {
    std::ofstream outfile("test.txt");

    setFontFile(fontPath);
    FramedBitmap map = loadGrayscaleImage(imgPath);
    map.frameWidth  = getFontWidth();
    map.frameHeight = getFontHeight();


    size_t framesCount =  (map.columns / map.frameWidth)
                            * (map.rows / map.frameHeight);
    uint_fast8_t totalSymbols = LAST_ASCII_SYMBOL - FIRST_ASCII_SYMBOL;
    uint_fast8_t threadsQuantity = (totalSymbols % THREAD_CONTRIBUTION)
                                ? totalSymbols / THREAD_CONTRIBUTION + 1
                                : totalSymbols / THREAD_CONTRIBUTION;

    std::vector<SymbolMatches> threadResults;
    threadResults.reserve(threadsQuantity);

    uint_fast8_t assignedSymbols = 0;
    for (uint_fast8_t threadNum = 0; threadsQuantity < threadsQuantity; ++threadNum) {
        std::string threadSymbols;
        for (size_t symbolNum = 0; symbolNum < THREAD_CONTRIBUTION; ++symbolNum) {
            if (assignedSymbols < totalSymbols) {
                threadSymbols.push_back(FIRST_ASCII_SYMBOL + assignedSymbols);
                ++assignedSymbols;
            }
        }

        threadResults.emplace_back(framesCount, threadSymbols);
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
