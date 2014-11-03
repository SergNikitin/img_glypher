# Image Glypher - ASCII-art generator

Small program for ASCII-art generation from images. Developed not just for the
sake of learning new stuff, but because it was a fun task, too! Currently is in
unfinished state, works best on images with both dark and bright areas.

## Examples

![spacemarine_src]

![spacemarine_ascii]

## How to build

Currently builds only on Linux. g++ compiler is used

1. Run `cmake .` command in the sources directory
2. Run `make` command to build the program as well as automatically download
and build its dependencies

## How to use

Image Glypher accepts the following command-line arguements:
* `--image=<path_to_image>` - path to *.bmp image you want to convert (more image extensions will be available in future)
* `--font=<path_to_font>` - path to font file you want to use as a base for conversion
(font must be monospaced)
* `--fontsize=<size>` - defines how detailed the output will be, must be 1 or greater
* `--oufile=<path_to_file>` - path to the output file; if not specified, image file path will be used
* `--invert` - generate output as if painting with white on black

Example:

```
./img_glypher   --image=./my_image.bmp                                               \
                --font=/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf \
                --fontsize=10
```

## Dependencies

* [FreeType](http://freetype.org/) for retrieving font data
* [SDL2](https://www.libsdl.org/download-2.0.php) and [SDL_Image](https://www.libsdl.org/projects/SDL_image/) for retrieving image data


[spacemarine_src]:http://i.imgur.com/uVqEyLb.jpg
[spacemarine_ascii]:http://i.imgur.com/Z2M6zIO.png
