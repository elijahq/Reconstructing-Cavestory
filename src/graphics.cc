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
    SDL_FreeSurface(screen_);
}

void Graphics::blitSurface(
        SDL_Surface* source,
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
