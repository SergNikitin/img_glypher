#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <exception>
#include <thread>

#include "grayscale_bitmap.h"
#include "freetype_interface.h"
#include "sdl_interface.h"
#include "image_processor.h"

uint_fast8_t const THREADS_TOTAL = 5;
void imageToText(const std::string& imgPath, const std::string& fontPath) {
    std::ofstream outfile("test.txt");

    setFontFile(fontPath);
    FramedBitmap map = loadGrayscaleImage(imgPath);
    map.setFrameSize(getFontWidth(), getFontHeight());

    size_t framesTotal = map.countFrames();
    ImageToTextResult dummy(framesTotal);
    std::vector<ImageToTextResult> threadResults(THREADS_TOTAL, dummy);
    std::vector<std::pair<FrameSlider, FrameSlider>> threadTasks;

    size_t framesPerThread = framesTotal % THREADS_TOTAL == 0
                            ? framesTotal / THREADS_TOTAL
                            : framesTotal / THREADS_TOTAL + 1;

    FrameSlider slider  = map.firstFrame();
    FrameSlider imgEnd  = map.lastFrame();
    for (uint_fast8_t threadNum = 0; threadNum < THREADS_TOTAL; ++threadNum) {
        FrameSlider taskStart = slider;
        for (size_t frame = 0; frame < framesPerThread; ++frame) {
            if (slider == imgEnd) {break;}
            slider.slide();
        }

        threadTasks.emplace_back(taskStart, slider);
        std::thread(processImagePart,
                    std::ref(threadTasks.back().first),
                    std::ref(threadTasks.back().second),
                    std::ref(threadResults.at(threadNum))).detach();

        if (slider != imgEnd) {
            slider.slide();
        }
    }

    size_t processedFrames = 0;
    size_t nextThread = 0;
    size_t framesInRow = map.columns / map.frameWidth;

    while (nextThread < THREADS_TOTAL) {
        if (threadResults.at(nextThread).done) {
            size_t threadFrames = threadResults.at(nextThread).frameMatches.size();
            for (size_t frame = 0; frame < threadFrames; ++frame) {
                outfile << threadResults.at(nextThread).frameMatches.at(frame);

                if (++processedFrames % framesInRow == 0) {
                    outfile << '\n';
                }
            }

            ++nextThread;
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
