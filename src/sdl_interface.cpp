#include <exception>

#include "SDL.h"
#include "SDL_image.h"

#include "sdl_interface.h"

class SdlMaintainer {
public:
    SdlMaintainer() : surface(nullptr) {
        int error = SDL_Init(0);
        if (error) {
            throw std::runtime_error(SDL_GetError());
        }

        error = IMG_Init(0);
        if (error) {
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
    maintainer.surface = IMG_Load(filepath.c_str());
    return FramedBitmap(maintainer.surface);
}
