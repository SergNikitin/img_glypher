#ifndef __SETTINGS_H__
#define __SETTINGS_H__


/**
 * @brief Object for application's general settings storage and transportation
 */
struct Settings {
    Settings();

    std::string imagePath;  /**< Relative or absolute path to the image that
                                will be turned to ASCII art */
    std::string fontPath;   /**< Relative or absolute path to the font that
                                will be used as a reference point to choose
                                best matching symbols for parts of the image */
    std::string outfile;    /**< Relative or absolute path to the output file */
    uint_fast16_t fontSize; /**< Font size that will be used, the smaller it is,
                                the more detailed the result will be */
    bool invert;            /**< Paint in white over black background if true */
    bool abort;             /**< Invalid settings combination detected if true */
};

/**
 * @brief Extract the known options and their corresponding values from the
 * standart option input format
 *
 * @param argc Number of given arguements
 * @param argv Array of char-string options
 *
 * @return Valid Settings object
 */
Settings parseArguments(int argc, char* argv[]);

#endif // __SETTINGS_H__
