#include <iostream>

#include "ft2build.h"
#include FT_FREETYPE_H

static void printByte(unsigned char byte) {
    for (int bit = 7; bit >= 0; --bit) {
        std::cout << (byte & (unsigned char)(1 << bit) ? '*' : ' ');
    }
}

static void printMonoBitmap(FT_Bitmap& map) {
    std::cout << map.rows << " rows" << std::endl;
    std::cout << map.width << " width" << std::endl;
    std::cout << map.pitch << " pitch" << std::endl;
    std::cout << (uint)map.pixel_mode << " pixel_mode" << std::endl;

    int bytesInRow = map.pitch;
    for (int rowNum = 0; rowNum < map.rows; ++rowNum) {
        for (int byteNum = 0; byteNum < bytesInRow; ++byteNum) {
            unsigned char byte = map.buffer[rowNum*bytesInRow + byteNum];
            printByte(byte);
        }
        std::cout << std::endl;
    }
}


int main() {
    FT_Library library;
    int error = FT_Init_FreeType(&library);
    if (error) {
        std::cout << "error during library init" << std::endl;
    }

    FT_Face face;
    error = FT_New_Face(library, "/usr/share/fonts/truetype/tlwg/Loma.ttf", 0, &face);
    if (error == FT_Err_Unknown_File_Format) {
        std::cout   << "the font file could be opened and read, but it appears "
                    << "that its font format is unsupported"
                    << std::endl;
    }
    else if (error) {
        std::cout   << "the font file could not"
                    << "be opened or read, or simply that it is broken..."
                    << std::endl;
    }

    error = FT_Set_Char_Size(face, 0, 16*64, 300, 300);
    if (error) {
        std::cout << "error while setting char size" << std::endl;
    }

    error = FT_Load_Char(face, 0x42, FT_LOAD_RENDER | FT_LOAD_MONOCHROME);
    if (error) {
        std::cout << "error while loading char" << std::endl;
    }

    printMonoBitmap(face->glyph->bitmap);

    FT_Done_Face(face);
    std::cout << "exiting now" << std::endl;

    return 0;
}
