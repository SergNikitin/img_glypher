#include "SDL.h"
#include "SDL_image.h"

#include "sdl_interface.h"

class SdlMaintainer {
public:
    SdlMaintainer() : surface(nullptr){};

    virtual ~SdlMaintainer() {
        if (surface != nullptr) {
            SDL_FreeSurface(surface);
        }
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
