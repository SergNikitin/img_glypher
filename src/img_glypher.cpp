#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <exception>
#include <thread>

#include "settings.h"
#include "grayscale_bitmap.h"
#include "freetype_interface.h"
#include "sdl_interface.h"
#include "image_processor.h"

static void writeThreadsOutputToFile(const std::string& outfilePath,
                                    const std::vector<ImageToTextResult>& thResults,
                                    size_t symbolsInLine) {
    std::ofstream outfile(outfilePath);

    size_t symbolsInFile = 0;
    for (const ImageToTextResult& oneThResult : thResults) {
        while (!oneThResult.done.load());

        for (char frameMatch : oneThResult.frameMatches) {
            outfile << frameMatch;

            if (++symbolsInFile % symbolsInLine == 0) {
                outfile << '\n';
            }
        }
    }
}

typedef std::pair<FrameSlider, FrameSlider> img_data_range;
static void assignThreadTasks(  const FramedBitmap& map,
                                std::vector<img_data_range>& tasks,
                                uint_fast8_t threadsNum) {
    size_t framesTotal = map.countFrames();
    size_t framesPerThread = framesTotal % threadsNum == 0
                            ? framesTotal / threadsNum
                            : framesTotal / threadsNum + 1;

    FrameSlider slider  = map.firstFrame();
    FrameSlider imgEnd  = map.lastFrame();
    for (uint_fast8_t threadNum = 0; threadNum < threadsNum; ++threadNum) {
        FrameSlider taskStart = slider;
        for (size_t frame = 0; frame < framesPerThread; ++frame) {
            if (slider == imgEnd) {
                break;
            }

            slider.slide();
        }

        tasks.emplace_back(taskStart, slider);

        if (slider != imgEnd) {
            slider.slide();
        }
    }
}

uint_fast8_t const THREADS_TOTAL = 4;
void imageToText(const Settings& settings) {
    setupFont(settings.fontPath, settings.fontSize, settings.invert);
    FramedBitmap map = loadGrayscaleImage(settings.imagePath);
    map.setFrameSize(getFontWidth(), getFontHeight());

    ImageToTextResult resDummy(map.countFrames());
    std::vector<ImageToTextResult> threadResults(THREADS_TOTAL, resDummy);
    std::vector<img_data_range> threadTasks;

    assignThreadTasks(map, threadTasks, THREADS_TOTAL);

    for (uint_fast8_t threadNum = 0; threadNum < THREADS_TOTAL; ++threadNum) {
        std::thread(processImagePart,
                    std::ref(threadTasks.at(threadNum).first),
                    std::ref(threadTasks.at(threadNum).second),
                    std::ref(threadResults.at(threadNum))).detach();
    }

    size_t framesInRow = map.columns / map.frameWidth;
    writeThreadsOutputToFile(settings.outfile, threadResults, framesInRow);
}

int main(int argc, char* argv[]) {
    try {
        Settings settings = parseArguments(argc, argv);

        if (settings.abort) {
            std::cerr << "Aborted due to invalid settings" << std::endl;
            return 1;
        }

        std::cout << settings.outfile << std::endl;

        imageToText(settings);

        return 0;
    }
    catch (const std::exception& error) {
        std::cerr << error.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }

    return 1;
}
