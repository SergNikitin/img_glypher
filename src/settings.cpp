#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <getopt.h>

#include "settings.h"

Settings::Settings()
    : imagePath("image_unspecified")
    , fontPath("font_unspecified")
    , fontSize(6)
    , invert(false)
    , abort(false) {}

enum ArguementCodes {
    IMAGE_ID = 1, FONT_ID, FONTSIZE_ID, INVERT_ID, HELP_ID
};

static std::vector<option> options = {
    {"image",   required_argument, NULL, IMAGE_ID       },
    {"font",    required_argument, NULL, FONT_ID        },
    {"fontsize",required_argument, NULL, FONTSIZE_ID    },
    {"invert",  no_argument,       NULL, INVERT_ID      },
    {"help",    no_argument,       NULL, HELP_ID        },
    {0,         0,                 NULL, 0              }
};

static std::map<std::string, std::string> settingsHelp = {
    {"image",   "path to *.bmp image you want to convert"},
    {"font",    "path to font file you want to use as a base for conversion (font must be monospaced)"},
    {"fontsize","defines how detailed the output will be, must be 1 or more"},
    {"invert",  "generate output as if painting with white on black"},
    {"help",    "print help"}
};

static void printHelp() {
    std::cout << "Image glypher accepts the following options:\n";
    for (option& opt : options) {
        if (!opt.name) {
            continue;
        }

        auto helpEntryIter = settingsHelp.find(std::string(opt.name));
        if (helpEntryIter != settingsHelp.end()) {
            std::cout << '\t' << "--" << helpEntryIter->first << "\t- "
                        << helpEntryIter->second << '\n';
        }
    }

    std::cout << "\nReport img_glypher bugs to "
                << "https://github.com/SergNikitin/img_glypher/issues\n";
    std::cout << "Made by Sergey Nikitin - snikitin@outlook.com, 2014" << std::endl;
}


Settings parseArguments(int argc, char* argv[]) {
    Settings settings;

    while (1) {
        int optionIndex = 0;
        int optionCode = getopt_long(argc, argv, "", options.data(), &optionIndex);
        if (optionCode == -1) {
            break;
        }

        switch (optionCode) {
            case IMAGE_ID: {
                if (optarg) {
                    settings.imagePath.assign(optarg);
                }
            }
            break;

            case FONT_ID: {
                if (optarg) {
                    settings.fontPath.assign(optarg);
                }
            }
            break;

            case FONTSIZE_ID: {
                if (optarg) {
                    settings.fontSize = std::stoull(optarg);
                }
            }
            break;

            case INVERT_ID: {
                settings.invert = true;
            }
            break;

            case HELP_ID: {
                printHelp();
                settings.abort = true;
            }
            break;

            case '?': {
                break;
            }

            default:
            {
                std::cout   << "getopt returned unknown code" << optionCode
                            << std::endl;
            }
        }
    }

    return settings;
}
