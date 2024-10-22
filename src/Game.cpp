#define INCLUDE_SDL_IMAGE
#define INCLUDE_SDL_MIXER
#include <string>
#include <iostream>
#include "SDL_include.h"
#include "Game.h"

Game* Game::instance = nullptr;

Game::Game(std::string title, int width, int height) {
    if(this->instance == nullptr) {
        this->instance = this;
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
        IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF);
        Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3);
        Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);
        Mix_AllocateChannels(32);
        this->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
        this->renderer = SDL_CreateRenderer(this->window, -1, (SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE));
        this->state = new State();
    }
}
Game::~Game() {
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}
SDL_Renderer* Game::GetRenderer() {
    return this->renderer;
}
State& Game::GetState() {
    return *state;
}
Game& Game::GetInstance() {
    if(Game::instance == nullptr) {
        new Game("190085312", 1200, 900);
    }
    return *Game::instance;
}
void Game::Run() {
    while(!state->QuitRequested()) {
        state->Update(0);
        state->Render();
        SDL_RenderPresent(renderer);
        SDL_Delay(33);

    }
    return;
}