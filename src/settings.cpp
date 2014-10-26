#include <iostream>
#include <string>
#include <vector>
#include <getopt.h>

#include "settings.h"

Settings::Settings()
    : imagePath("image_unspecified")
    , fontPath("font_unspecified")
    , fontSize(6) {}

enum ArguementCodes {
    IMAGE_ID = 1, FONT_ID, FONTSIZE_ID, HELP_ID
};

static std::vector<option> options = {
    {"image",   required_argument, NULL, IMAGE_ID       },
    {"font",    required_argument, NULL, FONT_ID        },
    {"fontsize",required_argument, NULL, FONTSIZE_ID    },
    {"help",    no_argument,       NULL, HELP_ID        },
    {0,         0,                 NULL, 0              }
};

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

            case HELP_ID: {
                std::cout << "i am help" << std::endl;
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
