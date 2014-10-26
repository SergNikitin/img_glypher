# IMG_GLYPHER

Small program for generating ASCII-art from images

## Examples

[spacemarine_src]
[spacemarine_ascii]

## How to build

Currently build only under Linux. g++ compiler is used

1. Run `cmake .` command in the sources directory
2. Run `make` command to build the program as well as automatically download
and build its dependencies

## How to use

In this unfinished state the program accepts only two arguements when launched
from the terminal:
* path to .bmp image you want to convert (more image extensions will be available in future)
* path to font file you want to use a base for conversion (font must be monospaced)

Example:

`./img_glypher ./my_image.bmp /usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf`

## Dependencies

* [FreeType](http://freetype.org/) for retrieving font data
* [SDL2](https://www.libsdl.org/download-2.0.php)
+ [SDL_Image](https://www.libsdl.org/projects/SDL_image/) for retrieving image data


[spacemarine_src]:http://i.imgur.com/uVqEyLb.jpg
[spacemarine_ascii]:http//i.imgur.com//Z2M6zIO.png
