#include <exception>
#include <sstream>
#include <algorithm>

extern "C" {
    #include "ft2build.h"
    #include FT_FREETYPE_H
}

#include "freetype_interface.h"
#include "grayscale_bitmap.h"

static class FreetypeMaintainer {
public:
    FreetypeMaintainer() : library(nullptr), fontFace(nullptr) {

        int error = FT_Init_FreeType(&library);
        if (error) {
            throw std::runtime_error("Unable to initalize Freetype library");
        }
    };

    ~FreetypeMaintainer() {
        if (fontFace != nullptr) {
            FT_Done_Face(fontFace);
        }

        FT_Done_FreeType(library);
    }

    FT_Library library;
    FT_Face fontFace;
    brihgtness_map brightnessVocab;

private:
    FreetypeMaintainer(const FreetypeMaintainer&);
} ft;

static obj_brightness averageBitmapBrightness(const GrayscaleBitmap& bitmap) {
    uint64_t acc = 0;
    for (obj_brightness brightness : *bitmap.pixels) {
        acc += brightness;
    }

    return acc / bitmap.pixels->size();
}

static void checkGlyphFormat(const FT_GlyphSlot& glyph) {
    if (glyph->format != FT_GLYPH_FORMAT_BITMAP) {
        throw std::runtime_error("Freetype symbol glyph must have bitmap format");
    }

    if (glyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY) {
        throw std::runtime_error("Given Freetype bitmap is not 8-bit grayscale");
    }
}

static GrayscaleBitmap asciiSymbolToBitmap(char symbol) {
    int error = FT_Load_Char(   ft.fontFace, static_cast<FT_ULong>(symbol),
                                FT_LOAD_RENDER);

    if (error) {
        throw std::runtime_error("Error while loading char");
    }

    checkGlyphFormat(ft.fontFace->glyph);

    return GrayscaleBitmap(ft.fontFace);
}

template<typename T> static bool compareSecond(const T& lhs, const T& rhs) {
    return lhs.second < rhs.second;
}

static void expandBrightnessRange(brihgtness_map& brMap) {
    obj_brightness maxBr = std::max_element(brMap.begin(), brMap.end(),
                                compareSecond<symbol_brightness_pair>)->second;

    obj_brightness minBr = std::min_element(brMap.begin(), brMap.end(),
                                compareSecond<symbol_brightness_pair>)->second;

    for (symbol_brightness_pair& entry : brMap) {
        uint16_t correctedBr = entry.second - minBr;
        correctedBr *= MAX_GRAY_LEVELS;
        correctedBr /= maxBr - minBr;
        entry.second = correctedBr;
    }
}

static void initVocabulary() {
    ft.brightnessVocab.clear();

    for (char   symbol = FIRST_PRINTABLE_ASCII_SYMBOL;
                symbol <= LAST_PRINTABLE_ASCII_SYMBOL; ++symbol) {
        GrayscaleBitmap bitmap = asciiSymbolToBitmap(symbol);
        // obj_brightness brightness = MAX_GRAY_LEVELS
                                    // - averageBitmapBrightness(bitmap);
        obj_brightness brightness = averageBitmapBrightness(bitmap);
        symbol_brightness_pair entry(symbol, brightness);
        ft.brightnessVocab.insert(entry);
    }

    expandBrightnessRange(ft.brightnessVocab);
}

static void loadDefaultFaceFromFontFile(const std::string& fontPath,
                                        const FT_Library& freetypeLib,
                                        FT_Face& faceContainer) {
    static const FT_Long INDEX_OF_FIRST_FACE_IN_FONT = 0;

    int error = FT_New_Face(freetypeLib, fontPath.c_str(),
                            INDEX_OF_FIRST_FACE_IN_FONT, &faceContainer);

    if (error == FT_Err_Unknown_File_Format) {
        std::stringstream err;
        err << "The font file '" << fontPath << "' could be opened and read, "
            << "but it appears that its font format is unsupported";
        throw std::runtime_error(err.str());
    } else if (error) {
        std::stringstream err;
        err << "The font file '" << fontPath << "' either could not "
            << "be opened and read, or it is simply broken";
        throw std::runtime_error(err.str());
    }
}

static void checkFontfaceFormat(FT_Face& face) {
    if (!FT_IS_FIXED_WIDTH(face)) {
        throw std::runtime_error("Loaded font face must be monospaced");
    }

    if (!FT_IS_SCALABLE(face)) {
        throw std::runtime_error("Loaded font face must be scaleble");
    }
}

static const uint32_t FIXED_POINT_26_6_COEFF = 1<<6;
static void setCharSizeInPoints(FT_Face& face, uint32_t size,
                                uint16_t horizRes, uint16_t verticalRes) {
    static const uint32_t SAME_AS_NEXT_ARG = 0;

    int error = FT_Set_Char_Size(face,      SAME_AS_NEXT_ARG,
                                            size * FIXED_POINT_26_6_COEFF,
                                            horizRes, verticalRes);

    if (error) {
        throw std::runtime_error("Error while setting char size");
    }
}

void setFont(const std::string& fontpath, uint_fast16_t fontSize) {
    static const FT_UInt DEFAULT_HORIZ_RES          = 72;
    static const FT_UInt DEFAULT_VERTICAL_RES       = DEFAULT_HORIZ_RES;

    loadDefaultFaceFromFontFile(fontpath, ft.library, ft.fontFace);
    checkFontfaceFormat(ft.fontFace);
    setCharSizeInPoints(ft.fontFace, fontSize, DEFAULT_HORIZ_RES,
                        DEFAULT_VERTICAL_RES);

    initVocabulary();
}

uint_fast16_t getFontHeight() {
    return ft.fontFace->size->metrics.height / FIXED_POINT_26_6_COEFF;
}

uint_fast16_t getFontWidth() {
    return ft.fontFace->size->metrics.max_advance / FIXED_POINT_26_6_COEFF;
}

obj_brightness getSymbolBrightness(char symbol) {
    return ft.brightnessVocab.at(symbol);
}

const brihgtness_map& getBrightnessVocabulary() {
    return ft.brightnessVocab;
}

char symbolWithBrightnessClosestTo(obj_brightness targetBrightness) {
    obj_brightness leastBrDiff = MAX_GRAY_LEVELS;
    char bestMatch = ft.brightnessVocab.begin()->first;

    for (const symbol_brightness_pair& entry : ft.brightnessVocab) {
        obj_brightness brDiff = abs(static_cast<int>(targetBrightness)
                                    - entry.second);
        if (brDiff < leastBrDiff) {
            leastBrDiff = brDiff;
            bestMatch = entry.first;
        }
    }

    return bestMatch;
}
