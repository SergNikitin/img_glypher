#include <exception>

extern "C" {
    #include "SDL.h"
    #include "SDL_image.h"
}

#include "sdl_interface.h"

#define NO_EXTRA_SUBMODULES 0

class SdlMaintainer {
public:
    SdlMaintainer() {
        int error = SDL_Init(NO_EXTRA_SUBMODULES);
        if (error) {
            throw std::runtime_error(SDL_GetError());
        }

        error = IMG_Init(NO_EXTRA_SUBMODULES);
        if (error) {
            SDL_Quit();
            throw std::runtime_error(IMG_GetError());
        }
    };

    virtual ~SdlMaintainer() {
        IMG_Quit();
        SDL_Quit();
    }

private:
    SdlMaintainer(const SdlMaintainer&);
};

static SdlMaintainer sdl;

static void safeLockSurface(SDL_Surface* surface) {
    int error = SDL_LockSurface(surface);

    if (error) {
        throw std::runtime_error("Unable to lock surface");
    }
}

FramedBitmap loadGrayscaleImage(const std::string& filepath) {
    SDL_Surface* source = IMG_Load(filepath.c_str());

    if (source == NULL) {
        throw std::runtime_error(IMG_GetError());
    }

    static const uint32_t UNUSED_FLAGS = 0;
    SDL_Surface* converted = SDL_ConvertSurfaceFormat(  source,
                                                        SDL_PIXELFORMAT_RGB888,
                                                        UNUSED_FLAGS);
    safeLockSurface(converted);
    FramedBitmap bitmap(converted);
    SDL_UnlockSurface(converted);

    SDL_FreeSurface(source);
    SDL_FreeSurface(converted);

    return bitmap;
}
