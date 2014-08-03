#include <exception>
#include <sstream>

#include "ft2build.h"
#include FT_FREETYPE_H

#include "freetype_interface.h"
#include "grayscale_bitmap.h"

class FreetypeMaintainer {
public:
    FreetypeMaintainer() {
        library = nullptr;
        fontFace = nullptr;

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

static FreetypeMaintainer maintainer;

#define SAME_AS_NEXT_ARG 0

void setFontFile(const std::string& newFilePath) {
    int error = FT_New_Face(maintainer.library, newFilePath.c_str(),
                            0, &maintainer.fontFace);

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

    error = FT_Set_Char_Size(maintainer.fontFace,   SAME_AS_NEXT_ARG, 16*64,
                                                    SAME_AS_NEXT_ARG, 300);
    if (error) {
        throw std::runtime_error("Error while setting char size");
    }
}

GrayscaleBitmap getBitmapForAsciiSymbol(char symbol) {
    int error = FT_Load_Char(   maintainer.fontFace, (FT_ULong)symbol,
                                FT_LOAD_RENDER);

    if (error) {
        throw std::runtime_error("error while loading char");
    }

    return GrayscaleBitmap(maintainer.fontFace->glyph->bitmap);
}
