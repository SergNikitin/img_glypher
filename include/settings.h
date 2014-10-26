#ifndef __SETTINGS_H__
#define __SETTINGS_H__

struct Settings {
    Settings();

    std::string imagePath;
    std::string fontPath;
    uint_fast16_t fontSize;
    bool abort;
};

Settings parseArguments(int argc, char* argv[]);

#endif // __SETTINGS_H__
