#include <SDL/SDL.h>
#include "game.h"
#include "graphics.h"
#include "player.h"
#include "input.h"

namespace {
const int kFps = 60;
}

//static
int Game::kTileSize = 32;

Game::Game() {
    SDL_Init(SDL_INIT_EVERYTHING);
    eventLoop();
}

Game::~Game() {
    SDL_Quit();
}

void Game::eventLoop() {
    Graphics graphics;
    Input input;
    SDL_Event event;

    player_.reset(new Player(graphics, 320, 240));

    bool running = true;
    int last_update_time = SDL_GetTicks();

    while (running) {

        const int start_time_ms = SDL_GetTicks();
        input.beginNewFrame();
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    input.keyDownEvent(event);
                    break;
                case SDL_KEYUP:
                    input.keyUpEvent(event);
                    break;
                default:
                    break;
            }
        }

        if (input.wasKeyPressed(SDLK_ESCAPE))
            running = false;

        // Player Horizontal Movement
        // if both left and right keys are being pressed, stop moving
        if (input.isKeyHeld(SDLK_LEFT) && input.isKeyHeld(SDLK_RIGHT))
            player_->stopMoving();
        // else if left key is being pressed, move left
        else if (input.isKeyHeld(SDLK_LEFT))
            player_->startMovingLeft();
        // else if left key is being pressed, move right
        else if (input.isKeyHeld(SDLK_RIGHT))
            player_->startMovingRight();
        // otherwise stop moving
        else
            player_->stopMoving();

        // if both up and right keys are being pressed, look 'horizontally'
        if (input.isKeyHeld(SDLK_UP) && input.isKeyHeld(SDLK_DOWN))
            player_->lookHorizontal();
        // else if up key is being pressed, look up
        else if (input.isKeyHeld(SDLK_UP))
            player_->lookUp();
        // else if down key is being pressed, look down
        else if (input.isKeyHeld(SDLK_DOWN))
            player_->lookDown();
        // otherwise look 'horizontally'
        else
            player_->lookHorizontal();


        // Player Jump
        if (input.wasKeyPressed(SDLK_z))
            player_->startJump();
        else if (input.wasKeyReleased(SDLK_z))
            player_->stopJump();

        const int current_time_ms = SDL_GetTicks();
        update(current_time_ms - last_update_time);
        last_update_time = current_time_ms;

        draw(graphics);

        const int ms_per_frame = 1000/*ms*/ / kFps;
        const int elapsed_time_ms = SDL_GetTicks() - start_time_ms;
        if (elapsed_time_ms < ms_per_frame) {
            SDL_Delay(ms_per_frame - elapsed_time_ms);
        }
    }
}

void Game::update(int elapsed_time_ms) {
    player_->update(elapsed_time_ms);
}

void Game::draw(Graphics& graphics) {
    graphics.clear();
    player_->draw(graphics);
    graphics.flip();
}
