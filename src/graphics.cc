#include <SDL/SDL.h>
#include "graphics.h"

namespace {
    // 640 x 480
    const int kScreenWidth = 640;
    const int kScreenHeight = 480;

    // 1680 x 1050
    //const int kScreenWidth = 1680;
    //const int kScreenHeight = 1050;

    // 1920 x 1080
    //const int kScreenWidth = 1920;
    //const int kScreenHeight = 1080;

    const int kBitsPerPixel = 32;
}

Graphics::Graphics() {
    screen_ = SDL_SetVideoMode(
        kScreenWidth,
        kScreenHeight,
        kBitsPerPixel,
        SDL_FULLSCREEN
    );
    SDL_ShowCursor(SDL_DISABLE);
}

Graphics::~Graphics() {
    for (SpriteMap::iterator iter = sprite_sheets_.begin();
        iter != sprite_sheets_.end();
        ++iter) {
        SDL_FreeSurface(iter->second);
    }

    SDL_FreeSurface(screen_);
}

Graphics::SurfaceID Graphics::loadImage(const std::string& file_path) {
    // if we have no loaded in the spritesheet
    if (sprite_sheets_.count(file_path) == 0)
        // load it in now
        sprite_sheets_[file_path] = SDL_LoadBMP(file_path.c_str());

    if (sprite_sheets_[file_path] == NULL) {
            // Could not load the image for whatever reason, print to stderr.
            fprintf(stderr, "ERROR: Could not find image: %s\n", file_path.c_str());
            // Don't try to continue if we couldn't load an image, just exit.
            exit(EXIT_FAILURE);
    }

    return sprite_sheets_[file_path];
}

void Graphics::blitSurface(
        SurfaceID source,
        SDL_Rect* source_rectangle,
        SDL_Rect* destination_rectangle) {

    SDL_BlitSurface(source, source_rectangle, screen_, destination_rectangle);
}

void Graphics::clear() {
    SDL_FillRect(screen_, NULL /*destination_rectangle*/, 0/*black-color*/);
}

void Graphics::flip() {
    SDL_Flip(screen_);
}
