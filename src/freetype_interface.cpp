#include <exception>
#include <sstream>

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
    brihgtness_map vocabulary;

private:
    FreetypeMaintainer(const FreetypeMaintainer&);
} ft;

static gray_pixel averageSymbolBrightness(const GrayscaleBitmap& bitmap) {
    uint64_t acc = 0;
    for (gray_pixel brightness : *bitmap.pixels) {
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

static void expandBrightnessToFullRange(brihgtness_map& brMap) {
    gray_pixel maxBrightness = brMap.begin()->second;
    gray_pixel minBrightness = brMap.begin()->second;
    for (auto& entry : brMap) {
        if (entry.second > maxBrightness) {
            maxBrightness = entry.second;
        }

        if (entry.second < minBrightness) {
            minBrightness = entry.second;
        }

    }

    for (auto& entry : brMap) {
        uint16_t correctedBrightness = entry.second - minBrightness;
        correctedBrightness *= MAX_GRAY_LEVELS;
        correctedBrightness /= (maxBrightness - minBrightness);
        entry.second = correctedBrightness;
    }
}

static void initVocabulary() {
    ft.vocabulary.clear();

    for (char   symbol = FIRST_PRINTABLE_ASCII_SYMBOL;
                symbol <= LAST_PRINTABLE_ASCII_SYMBOL; ++symbol) {
        GrayscaleBitmap bitmap = asciiSymbolToBitmap(symbol);
        gray_pixel brightness = MAX_GRAY_LEVELS - averageSymbolBrightness(bitmap);
        // gray_pixel brightness = averageSymbolBrightness(bitmap);
        symbol_brightness entry(symbol, brightness);
        ft.vocabulary.insert(entry);
    }

    expandBrightnessToFullRange(ft.vocabulary);
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

void setFontFile(const std::string& filepath) {
    static const FT_UInt DEFAULT_HORIZ_RES          = 72;
    static const FT_UInt DEFAULT_VERTICAL_RES       = DEFAULT_HORIZ_RES;

    loadDefaultFaceFromFontFile(filepath, ft.library, ft.fontFace);
    checkFontfaceFormat(ft.fontFace);
    setCharSizeInPoints(ft.fontFace, 10, DEFAULT_HORIZ_RES,
                        DEFAULT_VERTICAL_RES);

    initVocabulary();
}

uint_fast16_t getFontHeight() {
    return ft.fontFace->size->metrics.height / FIXED_POINT_26_6_COEFF;
}

uint_fast16_t getFontWidth() {
    return ft.fontFace->size->metrics.max_advance / FIXED_POINT_26_6_COEFF;
}

gray_pixel getSymbolBrightness(char symbol) {
    return ft.vocabulary.at(symbol);
}

const brihgtness_map& getBrightnessMap() {
    return ft.vocabulary;
}
