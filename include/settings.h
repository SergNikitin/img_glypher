struct Settings {
    Settings();

    std::string imagePath;
    std::string fontPath;
    uint_fast16_t fontSize;
};

Settings parseArguments(int argc, char* argv[]);
