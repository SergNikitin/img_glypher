#include <exception>

#include "SDL.h"
#include "SDL_image.h"

#include "sdl_interface.h"

#define NO_EXTRA_SUBMODULES 0

class SdlMaintainer {
public:
    SdlMaintainer() : surface(nullptr) {
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
        if (surface != nullptr) {
            SDL_FreeSurface(surface);
        }

        IMG_Quit();
        SDL_Quit();
    }

    SDL_Surface* surface;

private:
    SdlMaintainer(const SdlMaintainer&);
};

static SdlMaintainer maintainer;

FramedBitmap loadGrayscaleImage(const std::string& filepath) {
    SDL_Surface* surface = IMG_Load(filepath.c_str());

    if (surface == NULL) {
        throw std::runtime_error(IMG_GetError());
    }

    maintainer.surface = surface;
    return FramedBitmap(maintainer.surface);
}
