#include <exception>
#include <sstream>

extern "C" {
    #include "ft2build.h"
    #include FT_FREETYPE_H
}

#include "freetype_interface.h"
#include "grayscale_bitmap.h"

class FreetypeMaintainer {
public:
    FreetypeMaintainer() : library(nullptr), fontFace(nullptr) {
        int error = FT_Init_FreeType(&library);
        if (error) {
            throw std::runtime_error("Unable to initalize Freetype library");
        }
    };

    virtual ~FreetypeMaintainer() {
        if (fontFace != nullptr) {
            FT_Done_Face(fontFace);
        }

        FT_Done_FreeType(library);
    }

    FT_Library library;
    FT_Face fontFace;

private:
    FreetypeMaintainer(const FreetypeMaintainer&);
};

static FreetypeMaintainer ft;

#define FIXED_POINT_26_6_COEFF 2^6

void setFontFile(const std::string& newFilePath) {
    #define SAME_AS_NEXT_ARG 0
    static const FT_UInt DEFAULT_HORIZ_RES          = 72;
    static const FT_UInt DEFAULT_VERTICAL_RES       = DEFAULT_HORIZ_RES;
    static const FT_Long FIRST_FACE_IN_FONT_INDEX   = 0;

    int error = FT_New_Face(ft.library, newFilePath.c_str(),
                            FIRST_FACE_IN_FONT_INDEX, &ft.fontFace);

    if (error == FT_Err_Unknown_File_Format) {
        std::stringstream err;
        err << "The font file '" << newFilePath << "' could be opened and read, "
            << "but it appears that its font format is unsupported";
        throw std::runtime_error(err.str());
    }
    else if (error) {
        std::stringstream err;
        err << "The font file '" << newFilePath << "' either could not "
            << "be opened and read, or it is simply broken";
        throw std::runtime_error(err.str());
    }

    if (!FT_IS_FIXED_WIDTH(ft.fontFace)) {
        throw std::runtime_error("Loaded font face must be monospaced");
    }

    if (!FT_IS_SCALABLE(ft.fontFace)) {
        throw std::runtime_error("Loaded font face must be scaleble");
    }

    error = FT_Set_Char_Size(ft.fontFace,   SAME_AS_NEXT_ARG,
                                            16 * FIXED_POINT_26_6_COEFF,
                                            DEFAULT_HORIZ_RES,
                                            DEFAULT_VERTICAL_RES);

    if (error) {
        throw std::runtime_error("Error while setting char size");
    }
}

uint16_t getFontHeight() {
    return ft.fontFace->size->metrics.height / FIXED_POINT_26_6_COEFF;
}

uint16_t getFontWidth() {
    return ft.fontFace->size->metrics.max_advance / FIXED_POINT_26_6_COEFF;
}

GrayscaleBitmap getBitmapForAsciiSymbol(char symbol) {
    int error = FT_Load_Char(   ft.fontFace, static_cast<FT_ULong>(symbol),
                                FT_LOAD_RENDER);

    if (error) {
        throw std::runtime_error("Error while loading char");
    }

    if (ft.fontFace->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
        throw std::runtime_error("Freetype symbol glyph must have bitmap format");
    }

    if (ft.fontFace->glyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY) {
        throw std::runtime_error("Given Freetype bitmap is not 8-bit grayscale");
    }

    return GrayscaleBitmap(ft.fontFace);
}
